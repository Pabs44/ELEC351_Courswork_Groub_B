#include "mbed.h"
#include "FIFO.hpp"
#include <cstdint>
#include <cstring>
#include <string.h>

FIFO env_FIFO;

void read_sensor_thread();
void write_FIFO_thread();
void read_FIFO_thread();
void thread_test();

Thread sensorThread(osPriorityHigh);
Thread testThread(osPriorityBelowNormal);
Thread FIFOwrite, FIFOread;

void thread_test(){
    while(true){
        printf("\n\rTesting Threads");
        ThisThread::sleep_for(2500ms);
    }
}

void read_sensor_thread(){
    env_FIFO.FIFO_board.read_sensors();
}

//producer
void write_FIFO_thread(){
    env_FIFO.write_FIFO();
}

//consumer
void read_FIFO_thread(){
    env_FIFO.read_FIFO();
}


int main() {
    
    sensorThread.start(read_sensor_thread);
    FIFOwrite.start(write_FIFO_thread);
    FIFOread.start(read_FIFO_thread);
    //testThread.start(thread_test);

}
