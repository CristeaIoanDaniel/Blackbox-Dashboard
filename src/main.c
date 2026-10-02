#include "lvgl/lvgl.h"
#include "hal/hal.h"
#include "gui_dashbord.h"
#include "vehicle_data.h"
#include <unistd.h> 
#define MY_DISP_HOR_RES  800
#define MY_DISP_VER_RES  480

static void data_update_timer_cb(lv_timer_t *timer) {
    (void)timer;
    dashboard_update_from_vehicle_data();
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    lv_init();
    sdl_hal_init(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    vehicle_data_init(TELEMETRY_SERIAL_DEVICE);
    dashboard_init();
    lv_timer_create(data_update_timer_cb, 100, NULL);
    while (1) {
        lv_timer_handler();
        usleep(5000);
    }
    vehicle_data_deinit();

    return 0;
}