#ifndef __UOP_MSB_SENSORDATA__
#define __UOP_MSB_SENSORDATA__

    #include "mbed.h"
    #include "uop_msb.h"

    class UOP_MSB_SENSORDATA {
    private:
        
        //Light Levels
        AnalogIn ldr_sensors;
        //Environmental Sensor
        uop_msb::EnvSensor env;
        // LED Matrix
        SPI matrix_spi;   // MOSI, MISO, SCLK
        DigitalOut matrix_spi_cs;           //Chip Select ACTIVE LOW
        DigitalOut matrix_spi_oe;           //Output Enable ACTIVE LOW
   
        public:
        //Constructor
        UOP_MSB_SENSORDATA() : ldr_sensors(AN_LDR_PIN), matrix_spi(PC_12, PC_11, PC_10), matrix_spi_cs(PB_6), matrix_spi_oe(PB_12){}

        void read_sensors();

        private:

    };

#endif