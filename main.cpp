#include "mbed.h"
#include "UOP_MSB_SENSORDATA.h"
#include "FIFO_message.h"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string.h>
#include <stdio.h>
using namespace uop_msb;

UOP_MSB_SENSORDATA board;

Mutex sdLock;
Semaphore samplesInBuffer;
Semaphore spaceInBuffer(FIFO_size);

Queue<FIFOmessage_t, FIFO_size> FIFO;
MemoryPool<FIFOmessage_t, FIFO_size> memPool;

void read_sensor_thread();
void write_FIFO_thread();
void read_FIFO_thread();
void thread_test();

Thread sensorThread(osPriorityHigh);
Thread testThread(osPriorityBelowNormal);
Thread FIFOwrite, FIFOread;

void thread_test(){
    while(true){
        uint32_t count = FIFO.count();
        printf("\nFIFO size: %u", count);
        printf("\n\rTesting Threads");
        ThisThread::sleep_for(2500ms);
    }
}

void read_sensor_thread(){
    board.read_sensors();
}

//producer
void write_FIFO_thread(){
    while(true){
        if(startWrite == 1){
            spaceInBuffer.try_acquire_for(20ms);
            sdLock.lock();

            //allocate block from memPool
            FIFOmessage_t* write_FIFO = memPool.try_alloc();
            if(write_FIFO == NULL){
                //return error (out of memory)
                cout << "\nError: FIFO Full" << endl;
            }
            //fill in data (temperature, pressure and light level)
            write_FIFO->temp = datain[0];
            write_FIFO->pres = datain[1];
            write_FIFO->lux = datain[2];
            //write to FIFO
            bool dataCheck = FIFO.try_put(write_FIFO);
            //check if sent
            if(!dataCheck){
                cout << "\nError: Couldn't Write" << endl;
                memPool.free(write_FIFO);
            }

            startWrite = 0;

            sdLock.unlock();
            samplesInBuffer.release();
        }
        ThisThread::sleep_for(100ms);
    }
}

//consumer
void read_FIFO_thread(){
    while(true){
        if(FIFO.full()){
            cout << "\nreading" << endl;
            samplesInBuffer.try_acquire_for(20ms);
            sdLock.lock();

            FIFOmessage_t* read_FIFO;
            //block in case FIFO is empty
            bool dataCheck = FIFO.try_get_for(5s, &read_FIFO);
            if(dataCheck){
                for(int idx = 0; idx <= FIFO_size; idx++){
                    board.write_sdcard(read_FIFO);
                    //ThisThread::sleep_for(1s);
                }
                board.read_sdcard();
                memPool.free(read_FIFO);
            }else{
                cout << "\nError: Couldn't Read" << endl;
            }
            sdLock.unlock();
            spaceInBuffer.release();
        }
        ThisThread::sleep_for(100ms);
    }
}


int main() {

//    Ticker timer;
//    timer.attach(&write_FIFO_thread, 100ms);
    
    sensorThread.start(read_sensor_thread);
    FIFOwrite.start(write_FIFO_thread);
    FIFOread.start(read_FIFO_thread);
    //testThread.start(thread_test);

}
