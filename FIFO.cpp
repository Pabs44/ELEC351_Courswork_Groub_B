#include "FIFO.hpp"
#include <stdio.h>
#include <iostream>

#include "SDBlockDevice.h"
#include "FATFileSystem.h"
SDBlockDevice sd(PB_5, PB_4, PB_3, PF_3);

void SD::write_sdcard(FIFOmessage_t* readFIFO){
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
        fprintf(fp, "\n\rT=%.2f, P=%.2f, L=%.2f", readFIFO->_msg_env_data.temp, readFIFO->_msg_env_data.pres, readFIFO->_msg_env_data.light);
        //Tidy up here
        fclose(fp);
        printf("\nSD Write done...\n");
        sd.deinit();
    }
}

void SD::read_sdcard(){
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
        if(18<temp && temp<27) printf("\ntemperature alarm");
        if(900<pres && pres<1200) printf("\npressure alarm");
        if(0.1<lux && lux<0.4) printf("\nlight level alarm");

        _env_data.temp = temp;
        _env_data.pres = pres;
        _env_data.light = lux;

        _startWrite = 1;
        printf("\n\rT=%.2f, P=%.2f, L=%.2f", temp, pres, lux);
        ThisThread::sleep_for(10s);
    }
}

void UOP_MSB_SENSORDATA::alarm(){
    //while(!switchp)
}

void FIFO::write_FIFO(){
    while(true){
        if(_startWrite == 1){
            cout << "\nwriting";
            spaceInBuffer.try_acquire_for(20ms);
            sdLock.lock();

            //allocate block from memPool
            FIFOmessage_t* write_FIFO = memPool.try_alloc();
            if(write_FIFO == NULL){
                //return error (out of memory)
                cout << "\nError: FIFO Full" << endl;
            }
            //fill in data (temperature, pres and light level)
            write_FIFO->_msg_env_data.temp = _env_data.temp;
            write_FIFO->_msg_env_data.pres = _env_data.pres;
            write_FIFO->_msg_env_data.light = _env_data.light;
            //write to FIFO
            bool dataCheck = FIFO_queue.try_put(write_FIFO);
            //check if sent
            if(!dataCheck){
                cout << "\nError: Couldn't Write" << endl;
                memPool.free(write_FIFO);
            }

            _startWrite = 0;

            sdLock.unlock();
            samplesInBuffer.release();
        }
        ThisThread::sleep_for(50ms);
    }
}

void FIFO::read_FIFO(){
    while(true){
        if(FIFO_queue.full()){
            cout << "\nreading" << endl;
            samplesInBuffer.try_acquire_for(20ms);
            sdLock.lock();

            FIFOmessage_t* read_FIFO;
            //block in case FIFO is empty
            bool dataCheck = FIFO_queue.try_get_for(5s, &read_FIFO);
            //check if data sent
            if(dataCheck){
                for(int idx = 0; idx <= _FIFO_size-1; idx++){
                    cout << "\ngetting sample from buffer" << endl;
                    samplesInBuffer.try_acquire_for(20ms);
                    SD::write_sdcard(read_FIFO);
                    ThisThread::sleep_for(8s);
                }
                SD::read_sdcard();
                memPool.free(read_FIFO);
            }else{
                cout << "\nError: Couldn't Read" << endl;
            }
            sdLock.unlock();
            spaceInBuffer.release();
        }
        ThisThread::sleep_for(10ms);
    }
}

