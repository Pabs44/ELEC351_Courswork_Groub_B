#include "UOP_MSB_SENSORDATA.h"
using namespace uop_msb;

void UOP_MSB_SENSORDATA::read_sensors(){
    // Interrogate Environmental Sensor Driver
    switch (env.getSensorType())
    {
        case EnvSensor::BMP280:
        printf("BMP280\n");
        break;
        case uop_msb::EnvSensor::SPL06_001:
        printf("SPL06_001\n");
        break;
        default:
        printf("ERROR");
    }

    wait_us(2000000);

    //Main loop
    while (true) {     
        // LDR
        for (unsigned int n=0; n<10; n++) {
            float lightLevel = ldr_sensors;
            printf("\n\rLIGHT: %4.2f", lightLevel);
            wait_us(250000);
        }
        // TEST ENV SENSOR
        printf("\n\rTesting %s", (MSB_VER == 2) ? "BMP280" : "SPL06_001");
        for (uint16_t n = 0; n < 20; n++) {
            float temp = env.getTemperature();
            float pres = env.getPressure();
            float hum = env.getHumidity();
            float lux = ldr_sensors.read();
            printf("\n\rT=%.2f, P=%.2f, H=%.2f, L=%.2f\n", temp, pres, hum, lux);   
            wait_us(500000);         
        }
    }            
}