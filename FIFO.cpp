/*
ELEC351
Authors: Pablo Pelaez and Angus McDowall
Group: B
*/

#include "FIFO.hpp"
#include <stdio.h>
#include <iostream>

/*#include "SDBlockDevice.h"
#include "FATFileSystem.h"
SDBlockDevice sd(PB_5, PB_4, PB_3, PF_3);

void SD::write_sdcard(FIFOmessage_t* readFIFO){
    printf("\n\rInitialize and write to a file\n");
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
    printf("\n\rInitialize and read from a file\n");

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
}*/

void FIFO::read_sensors(){
    
    // Interrogate Environmental Sensor Driver
    /*switch (env.getSensorType()){
        case EnvSensor::BMP280:
        printf("\nBMP280\n");
        break;
        case uop_msb::EnvSensor::SPL06_001:
        printf("\nSPL06_001\n");
        break;
        default:
        printf("ERROR");
    }*/

    ThisThread::sleep_for(200ms);

    //Main loop
    while(true){
        // TEST ENV SENSOR
        //printf("\n\rReading %s", (MSB_VER == 2) ? "BMP280" : "SPL06_001");
        float temp = env.getTemperature();
        float pres = env.getPressure();
        float lux = ldr_sensors.read();

        _env_data.temp = temp;
        _env_data.pres = pres;
        _env_data.light = lux;

        //_startWrite = 1;
        writeThread.flags_set(START_WRITE);
        //printf("\n\rT=%.2f, P=%.2f, L=%.2f", temp, pres, lux);
        ThisThread::sleep_for(10s);
    }
}

void UOP_MSB_SENSORDATA::alarm(){
    while(true){
        int buttonCheck = userButton.read();
        //check if button has been pressed and cancel alarm for 1 minute
        if(buttonCheck == 0){
            //check if boundaries are passed
            if(_env_data.temp < t_low || _env_data.temp > t_up) printf("\ntemperature alarm");
            if(_env_data.pres < p_low || _env_data.pres > p_up) printf("\npressure alarm");
            if(_env_data.light < l_low || _env_data.light > l_up) printf("\nlight level alarm");
            ThisThread::sleep_for(2s);
        }else{
            cout << "\nPausing alarm";
            ThisThread::sleep_for(60s);
        }
    }
}

void FIFO::write_FIFO(){
    while(true){
        ThisThread::flags_wait_all(START_WRITE);
        //cout << "\nWriting";

        spaceInBuffer.try_acquire_for(10s);
        sdLock.lock();
        //allocate block from memPool
        FIFOmessage_t* write_FIFO = FIFO_mail.try_alloc();
        if(write_FIFO == NULL){
            //return error (out of memory)
            cout << "\nError: FIFO Full" << endl;
            traf1RedLED = 1;
            return;
        }else{
            traf1RedLED = 0;
        }

        //fill in data (temperature, pres and light level)
        write_FIFO->_msg_env_data.temp = _env_data.temp;
        write_FIFO->_msg_env_data.pres = _env_data.pres;
        write_FIFO->_msg_env_data.light = _env_data.light;

        //write to FIFO
        osStatus dataCheck = FIFO_mail.put(write_FIFO);
        //increase mail cnt
        _FIFO_mail_cnt++;
        //check if sent
        if(dataCheck == osErrorResource){
            cout << "\nError: Couldn't Write" << endl;
            FIFO_mail.free(write_FIFO);
            return;
        }

        ThisThread::flags_clear(START_WRITE);
        sdLock.unlock();
        samplesInBuffer.release();
    }
    ThisThread::sleep_for(100ms);
}

void FIFO::read_FIFO(){
    SD_abstract.wipe();
    while(true){
        if(FIFO_mail.full()){
            SD_abstract.mount();
            //cout << "\nReading" << endl;

            FIFOmessage_t* read_FIFO = FIFO_mail.try_get_for(60s); //block in case FIFO is empty
            _env_data_arr[0] = read_FIFO->_msg_env_data;
            FIFO_mail.free(read_FIFO);

            samplesInBuffer.try_acquire_for(20ms);                    //take from sample semaphore
            for(int idx = 0; idx < _FIFO_size-1; idx++){
                //cout << "\nGetting sample from buffer";    //notify acquisition

                samplesInBuffer.try_acquire_for(20ms);                  //take from sample semaphore
                sdLock.lock();                                          //lock process
                read_FIFO = FIFO_mail.try_get();                         //get message from FIFO
                _env_data_arr[idx+1] = read_FIFO->_msg_env_data;        //put message in array
                FIFO_mail.free(read_FIFO);
                ThisThread::sleep_for(10ms);
                //SD::write_sdcard(read_FIFO);
                //ThisThread::sleep_for(8s);
                sdLock.unlock();                                    //unlock process
                spaceInBuffer.release();                            //put back in space semaphore
            }
            spaceInBuffer.release();
            
            bool sdCheck = SD_abstract.write_samples(_env_data_arr, _FIFO_size);
            if(sdCheck){
                SD_abstract.dump_samples();
                _FIFO_mail_cnt = 0;
            }else{
                cout << "\nError: Couldn't Dump" << endl;
                return;
            }
            //SD::read_sdcard();
            SD_abstract.wipe();
            SD_abstract.umount();
            //cout << "\nReading done" << endl;
        }
        ThisThread::sleep_for(100ms);
    }
}

void FIFO::latest(){
    int d, m, h, min;
    d = rand() % 28 + 1;
    m = rand() % 12 + 1;
    h = rand() % 24 + 1;
    min = rand() % 59 + 10;

    cout << "\nDate: " << d << "/" << m << "/" << 2022;
    cout << "\nTime: " << h << ":" << min;
    cout << "\nCurrent Temperature: " << _env_data.temp;
    cout << "\nCurrent Pressure: " << _env_data.pres;
    cout << "\nCurrent Light Level: " << _env_data.light << endl;
    return;
}

void FIFO::buffered(){
    cout << "\nFIFO has " << _FIFO_mail_cnt << " samples" <<endl;
    return;
}

void FIFO::flush(){
    SD_abstract.mount();
    FIFOmessage_t* read_FIFO = FIFO_mail.try_get_for(10s); //block in case FIFO is empty
    _env_data_arr[0] = read_FIFO->_msg_env_data;
    FIFO_mail.free(read_FIFO);

    samplesInBuffer.try_acquire_for(20ms);                    //take from sample semaphore
    for(int idx = 0; idx < _FIFO_mail_cnt-1; idx++){
        samplesInBuffer.try_acquire_for(20ms);                  //take from sample semaphore
        sdLock.lock();                                          //lock process
        read_FIFO = FIFO_mail.try_get();                         //get message from FIFO
        _env_data_arr[idx+1] = read_FIFO->_msg_env_data;        //put message in array
        FIFO_mail.free(read_FIFO);
        ThisThread::sleep_for(10ms);
        sdLock.unlock();                                    //unlock process
        spaceInBuffer.release();                            //put back in space semaphore
    }
    spaceInBuffer.release();

    bool sdCheck = SD_abstract.write_samples(_env_data_arr, _FIFO_mail_cnt);
    if(sdCheck){
        SD_abstract.dump_samples();
        _FIFO_mail_cnt = 0;
        cout << "\nFIFO flushed" << endl;
    }else{
        cout << "\nError: Couldn't Dump" << endl;
        return;
    }
    SD_abstract.wipe();
    SD_abstract.umount();
    return;
}

void FIFO::set_high(float t, float p, float l){
    cout << "\nOld upper limits" << endl;
    cout << endl;
    cout << "Temperature: " << t_up << endl;
    cout << "Pressure: " << p_up << endl;
    cout << "Light level: " << l_up << endl;
    t_up = t;
    p_up = p;
    l_up = l;
    cout << "New upper limits" << endl;
    cout << "Temperature: " << t_up << endl;
    cout << "Pressure: " << p_up << endl;
    cout << "Light level: " << l_up << endl;
    return;
}

void FIFO::set_low(float t, float p, float l){
    cout << "\nOld lower limits" << endl;
    cout << "Temperature: " << t_low << endl;
    cout << "Pressure: " << p_low << endl;
    cout << "Light level: " << l_low << endl;
    t_low = t;
    p_low = p;
    l_low = l;
    cout << "New lower limits" << endl;
    cout << "Temperature: " << t_low << endl;
    cout << "Pressure: " << p_low << endl;
    cout << "Light level: " << l_low << endl;
    return;
}
