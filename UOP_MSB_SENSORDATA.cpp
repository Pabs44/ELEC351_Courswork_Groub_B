#include "UOP_MSB_SENSORDATA.h"
using namespace uop_msb;

int startWrite;
float datain[3];

#include "SDBlockDevice.h"
#include "FATFileSystem.h"
// Instantiate the SDBlockDevice by specifying the SPI pins connected to the SDCard socket.
//The PINS are: (This can also be done in the JSON file see mbed.org Docs API Storage SDBlockDevice)
// PB_5    MOSI (Master Out Slave In)
// PB_4    MISO (Master In Slave Out)
// PB_3    SCLK (Serial Clock)
// PF_3    CS (Chip Select)
// and there is a Card Detect CD on PF_4 ! (NB this is an Interrupt capable pin..)
SDBlockDevice sd(PB_5, PB_4, PB_3, PF_3);

void UOP_MSB_SENSORDATA::write_sdcard(FIFOmessage_t* readFIFO){
    printf("\n\rInitialise and write to a file\n");
    int err;
    // call the SDBlockDevice instance initialisation method.

    err=sd.init();
    if ( 0 != err) {
        printf("\nInit failed %d\n",err);
    }
    
    FATFileSystem fs("sd", &sd);
    FILE *fp = fopen("/sd/test1.txt","w");
    if(fp == NULL) {
        error("Could not open file for write\n");
        sd.deinit();
    } else {
        //Put some text in the file...
        fprintf(fp, "\n\rT=%.2f, P=%.2f, L=%.2f", readFIFO->temp, readFIFO->pres, readFIFO->lux);
        //Tidy up here
        fclose(fp);
        printf("\nSD Write done...\n");
        sd.deinit();
    }
}

void UOP_MSB_SENSORDATA::read_sdcard(){
    printf("\n\rInitialise and read from a file\n");

    // call the SDBlockDevice instance initialisation method.
    if ( 0 != sd.init()) {
        printf("\nInit failed\n");
    }
    
    FATFileSystem fs("sd", &sd);
    FILE *fp = fopen("/sd/test.txt","r");
    if(fp == NULL) {
        error("\nCould not open or find file for read\n");
        sd.deinit();
    } else {
        //Put some text in the file...
        char buff[64]; buff[63] = 0;
        while (!feof(fp)) {
            fgets(buff, 63, fp);
            printf("\n%s", buff);
        }
        //Tidy up here
        fclose(fp);
        printf("\nSD Write done...");
        sd.deinit();
    }
}

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
        printf("\n\rReading %s", (MSB_VER == 2) ? "BMP280" : "SPL06_001");
        float temp = env.getTemperature();
        float pres = env.getPressure();
        float lux = ldr_sensors.read();
        datain[0] = temp;
        datain[1] = pres;
        datain[2] = lux;
        startWrite = 1;
        printf("\n\rT=%.2f, P=%.2f, L=%.2f", temp, pres, lux);   
        ThisThread::sleep_for(10000ms);
    }
}