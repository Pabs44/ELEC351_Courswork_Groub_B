    #ifndef __UOP_MSB_SENSORDATA__
    #define __UOP_MSB_SENSORDATA__
    #include "mbed.h"
    #include "uop_msb.h"


    class UOP_MSB_SENSORDATA {
    private:
        //Light Levels
        AnalogIn ldr;
        //Environmental Sensor
        EnvSensor env;   
        // LED Matrix 
        SPI matrix_spi;   // MOSI, MISO, SCLK
        DigitalOut matrix_spi_cs;            //Chip Select ACTIVE LOW
        DigitalOut matrix_spi_oe;           //Output Enable ACTIVE LOW   
   

        public:
        UOP_MSB_SENSORDATA() : ldr(AN_LDR_PIN), matrix_spi(PC_12, PC_11, PC_10), matrix_spi_cs(PB_6), matrix_spi_oe(PB_12)
        {
            //Constructor
        }

        //void test();
        // Interrogate Environmental Sensor Driver
        switch (env.getSensorType())
        {
        case EnvSensor::BMP280:
        disp.printf("BMP280\n");
        break;
        case uop_msb::EnvSensor::SPL06_001:
        disp.printf("SPL06_001\n");
        break;
        default:
        disp.printf("ERROR");
        }
        // LDR
        for (unsigned int n=0; n<10; n++) {
            float lightLevel = ldr;
            disp.cls(); disp.locate(0, 0);
            disp.printf("LIGHT: %4.2f", lightLevel);
            wait_us(250000);
        }
        // TEST ENV SENSOR
        disp.cls();
        disp.locate(0,0);
        disp.printf("Testing %s", (MSB_VER == 2) ? "BMP280" : "SPL06_001");
        for (uint16_t n = 0; n < 20; n++) {
            float temp = env.getTemperature();
            float pres = env.getPressure();
            float hum = env.getHumidity();
            float lux = ldr.read();
            printf("T=%.2f, P=%.2f, H=%.2f, L=%.2f\n", temp, pres, hum, lux);   
            wait_us(500000);         
        }


        private:
        void clearMatrix(void);
        void matrix_scan(void);

    };    