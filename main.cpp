#include "mbed.h"
#include "FIFO.hpp"
#include <cstdint>
#include <cstring>
#include <string.h>

FIFO env_FIFO;

Thread sensorThread(osPriorityHigh);
Thread testThread(osPriorityBelowNormal);
Thread FIFOwrite, FIFOread, alarmThread;

void thread_test(){
    while(true){
        printf("\n\rTesting Threads");
        ThisThread::sleep_for(2500ms);
    }
}

//run the sensor reading
void read_sensor_thread(){
    env_FIFO.FIFO_board.read_sensors();
}
//run alarm check
void alarm_thread(){
    env_FIFO.FIFO_board.alarm();
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
    alarmThread.start(alarm_thread);
    FIFOwrite.start(write_FIFO_thread);
    FIFOread.start(read_FIFO_thread);
    //testThread.start(thread_test);
}
