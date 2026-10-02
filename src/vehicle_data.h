#ifndef VEHICLE_DATA_H
#define VEHICLE_DATA_H

#include <stdint.h>
#include <stdbool.h>
typedef struct {
    float speed_kmh;      
    float rpm;            
    float fuel_level;      
    float coolant_temp;    
    float battery_voltage; 
    bool  connected;       
    float latitude;       
    float longitude;      
} vehicle_data_t;
typedef struct __attribute__((packed)) {
    uint32_t magic;          
    float    speed_kmh;
    float    rpm;
    float    fuel_level;
    float    coolant_temp;
    float    battery_voltage;
    float    latitude;
    float    longitude;
} telemetry_packet_t;

#define TELEMETRY_MAGIC   0x41564431u
#define TELEMETRY_SERIAL_DEVICE "COM3"
#define TELEMETRY_BAUD_RATE      115200  
bool vehicle_data_init(const char *device);

void vehicle_data_deinit(void);
void vehicle_data_get(vehicle_data_t *out);

#endif 