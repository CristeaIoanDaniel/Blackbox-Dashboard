#include "vehicle_data.h"

#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#ifdef _WIN32
    #include <windows.h>
    typedef HANDLE serial_handle_t;
    #define SERIAL_INVALID INVALID_HANDLE_VALUE
#else
    #include <unistd.h>
    #include <fcntl.h>
    #include <termios.h>
    typedef int serial_handle_t;
    #define SERIAL_INVALID (-1)
#endif

static vehicle_data_t g_data;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
static time_t g_last_packet_time = 0;
static bool g_running = false;
static pthread_t g_thread;
static char g_preferred_port[64] = {0};

#define TELEMETRY_TIMEOUT_SEC 2

static void reset_to_zero(void) {
    pthread_mutex_lock(&g_lock);
    memset(&g_data, 0, sizeof(g_data));
    g_data.connected = false;
    pthread_mutex_unlock(&g_lock);
}

static void apply_packet(const telemetry_packet_t *pkt) {
    pthread_mutex_lock(&g_lock);
    g_data.speed_kmh       = pkt->speed_kmh;
    g_data.rpm             = pkt->rpm;
    g_data.fuel_level      = pkt->fuel_level;
    g_data.coolant_temp    = pkt->coolant_temp;
    g_data.battery_voltage = pkt->battery_voltage;
    g_data.latitude        = pkt->latitude;
    g_data.longitude       = pkt->longitude;
    g_data.connected       = true;
    pthread_mutex_unlock(&g_lock);
    g_last_packet_time = time(NULL);
}
#ifdef _WIN32
static void set_streaming_timeouts(serial_handle_t h) {
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout        = 50;
    timeouts.ReadTotalTimeoutConstant   = 300;
    timeouts.ReadTotalTimeoutMultiplier = 5;
    SetCommTimeouts(h, &timeouts);
}

static serial_handle_t open_serial_port(const char *device) {
    char full_name[160];
    snprintf(full_name, sizeof(full_name), "\\\\.\\%s", device);

    HANDLE h = CreateFileA(full_name, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                            OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        return SERIAL_INVALID;
    }

    DCB dcb = {0};
    dcb.DCBlength = sizeof(dcb);
    if (!GetCommState(h, &dcb)) { CloseHandle(h); return SERIAL_INVALID; }

    dcb.BaudRate = TELEMETRY_BAUD_RATE;
    dcb.ByteSize = 8;
    dcb.Parity   = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fBinary  = TRUE;
    dcb.fParity  = FALSE;
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fDtrControl  = DTR_CONTROL_ENABLE;
    dcb.fRtsControl  = RTS_CONTROL_ENABLE;

    if (!SetCommState(h, &dcb)) { CloseHandle(h); return SERIAL_INVALID; }

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout        = 50;
    timeouts.ReadTotalTimeoutConstant   = 300;
    timeouts.ReadTotalTimeoutMultiplier = 5;
    SetCommTimeouts(h, &timeouts);

    return h;
}

static bool read_exact(serial_handle_t h, void *buf, size_t len) {
    uint8_t *p = (uint8_t *)buf;
    size_t got = 0;
    while (got < len) {
        DWORD n = 0;
        if (!ReadFile(h, p + got, (DWORD)(len - got), &n, NULL) || n == 0) {
            return false;
        }
        got += n;
    }
    return true;
}

static void close_serial_port(serial_handle_t h) { CloseHandle(h); }

#else

static serial_handle_t open_serial_port(const char *device) {
    int fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) return SERIAL_INVALID;

    fcntl(fd, F_SETFL, 0);

    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(fd, &tty) != 0) { close(fd); return SERIAL_INVALID; }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ISIG;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(ICRNL | INLCR);
    tty.c_oflag &= ~OPOST;

    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 3; 

    if (tcsetattr(fd, TCSANOW, &tty) != 0) { close(fd); return SERIAL_INVALID; }
    return fd;
}

static bool read_exact(serial_handle_t fd, void *buf, size_t len) {
    uint8_t *p = (uint8_t *)buf;
    size_t got = 0;
    while (got < len) {
        ssize_t n = read(fd, p + got, len - got);
        if (n <= 0) return false;
        got += (size_t)n;
    }
    return true;
}

static void close_serial_port(serial_handle_t fd) { close(fd); }

#endif
static serial_handle_t try_find_and_open_port(char *out_port_name, size_t max_len) {
#ifdef _WIN32
    for (int i = 1; i <= 32; i++) {
        snprintf(out_port_name, max_len, "COM%d", i);
        serial_handle_t h = open_serial_port(out_port_name);
        if (h != SERIAL_INVALID) {
            return h;
        }
    }
#else
    for (int i = 0; i < 10; i++) {
        snprintf(out_port_name, max_len, "/dev/ttyACM%d", i);
        serial_handle_t h = open_serial_port(out_port_name);
        if (h != SERIAL_INVALID) return h;

        snprintf(out_port_name, max_len, "/dev/ttyUSB%d", i);
        h = open_serial_port(out_port_name);
        if (h != SERIAL_INVALID) return h;
    }
#endif
    return SERIAL_INVALID;
}
static void *serial_thread_fn(void *arg) {
    (void)arg;
    char current_port[64] = {0};

    while (g_running) {
        serial_handle_t h = SERIAL_INVALID;
        bool tried_preferred = false;

        if (g_preferred_port[0] != '\0') {
            tried_preferred = true;
            snprintf(current_port, sizeof(current_port), "%s", g_preferred_port);
            h = open_serial_port(current_port);
        }

        if (h == SERIAL_INVALID && !tried_preferred) {
            h = try_find_and_open_port(current_port, sizeof(current_port));
        }

        if (h == SERIAL_INVALID) {
            reset_to_zero();
#ifdef _WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
            continue;
        }
        telemetry_packet_t pkt;
        bool synced = false;

        for (int attempts = 0; attempts < 100 && g_running; attempts++) {
            uint8_t b;
            if (!read_exact(h, &b, 1)) break;

            uint8_t *pkt_ptr = (uint8_t *)&pkt;
            pkt_ptr[0] = b;

            bool packet_complete = true;
            for (size_t i = 1; i < sizeof(telemetry_packet_t); i++) {
                if (!read_exact(h, &pkt_ptr[i], 1)) {
                    packet_complete = false;
                    break;
                }
            }

            if (packet_complete && pkt.magic == TELEMETRY_MAGIC) {
                synced = true;
                apply_packet(&pkt);
                break;
            }
        }

        if (!synced) {
            printf("[vehicle_data] Port %s deschis, dar sincronizarea magic number a esuat.\n", current_port);
            close_serial_port(h);
#ifdef _WIN32
            Sleep(100);
#else
            usleep(100000);
#endif
            continue;
        }
        printf("[vehicle_data] Pico 2W conectat pe portul: %s\n", current_port);
        #ifdef _WIN32
        set_streaming_timeouts(h);
        #endif

        uint8_t stream_buf[sizeof(telemetry_packet_t)];
        size_t stream_len = 0;
        int missed_reads = 0;

        while (g_running) {
            uint8_t b;
            if (!read_exact(h, &b, 1)) {
                missed_reads++;
                if (missed_reads > 20) {
                    goto disconnected;
                }
#ifdef _WIN32
                Sleep(50);
#else
                usleep(50000);
#endif
                continue;
            }
            missed_reads = 0;

            stream_buf[stream_len++] = b;

            if (stream_len >= sizeof(telemetry_packet_t)) {
                memcpy(&pkt, stream_buf, sizeof(telemetry_packet_t));

                if (pkt.magic == TELEMETRY_MAGIC) {
                    apply_packet(&pkt);
                    stream_len = 0;
                    continue;
                }
                memmove(stream_buf, stream_buf + 1, stream_len - 1);
                stream_len--;
            }
        }

    disconnected:
        close_serial_port(h);
        printf("[vehicle_data] Pico 2W deconectat (%s). Caut din nou portul...\n", current_port);
        reset_to_zero();
#ifdef _WIN32
        Sleep(500);
#else
        usleep(500000);
#endif
    }

    return NULL;
}

static void *timeout_watcher_fn(void *arg) {
    (void)arg;
    while (g_running) {
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
        if (g_last_packet_time != 0 &&
            (time(NULL) - g_last_packet_time) > TELEMETRY_TIMEOUT_SEC) {
            reset_to_zero();
            g_last_packet_time = 0;
        }
    }
    return NULL;
}

bool vehicle_data_init(const char *device) {
    memset(g_preferred_port, 0, sizeof(g_preferred_port));
    if (device != NULL && device[0] != '\0') {
        snprintf(g_preferred_port, sizeof(g_preferred_port), "%s", device);
    } else {
        snprintf(g_preferred_port, sizeof(g_preferred_port), "%s", TELEMETRY_SERIAL_DEVICE);
    }

    memset(&g_data, 0, sizeof(g_data));

    g_running = true;
    pthread_create(&g_thread, NULL, serial_thread_fn, NULL);

    pthread_t watcher;
    pthread_create(&watcher, NULL, timeout_watcher_fn, NULL);
    pthread_detach(watcher);

    return true;
}

void vehicle_data_deinit(void) {
    g_running = false;
    pthread_join(g_thread, NULL);
}

void vehicle_data_get(vehicle_data_t *out) {
    pthread_mutex_lock(&g_lock);
    *out = g_data;
    pthread_mutex_unlock(&g_lock);
}