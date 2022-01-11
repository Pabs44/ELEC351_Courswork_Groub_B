#include "UOP_MSB_SENSORDATA.h"
using namespace uop_msb;

void UOP_MSB_SENSORDATA::read_sensors(){
    
    // Interrogate Environmental Sensor Driver
    switch (env.getSensorType()){
        case EnvSensor::BMP280:
        printf("BMP280\n");
        break;
        case uop_msb::EnvSensor::SPL06_001:
        printf("SPL06_001\n");
        break;
        default:
        printf("ERROR");
    }

    ThisThread::sleep_for(200ms);

    //Main loop
    while(true){
        // TEST ENV SENSOR
        printf("\n\rTesting %s", (MSB_VER == 2) ? "BMP280" : "SPL06_001");
        for (uint16_t n = 0; n < 20; n++) {
            float temp = env.getTemperature();
            float pres = env.getPressure();
            float lux = ldr_sensors.read();
            printf("\n\rT=%.2f, P=%.2f, L=%.2f", temp, pres, lux);   
            ThisThread::sleep_for(9800ms);
        }
    }            
}