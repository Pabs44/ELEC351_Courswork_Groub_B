#include "mbed.h"
#include "UOP_MSB_SENSORDATA.h"
#include <cstring>
#include <string.h>
using namespace uop_msb;

void read_sensor_thread();
void thread_test();

Thread sensorThread(osPriorityHigh);
Thread testThread;

int main() {
    
    sensorThread.start(read_sensor_thread);
    testThread.start(thread_test);

}

void read_sensor_thread(){
    // Testing the sd card and checking sensors
    UOP_MSB_SENSORDATA board;
    board.read_sensors();
}

void thread_test(){
    while(true){
        printf("\n\rTesting Threads");
        ThisThread::sleep_for(5000ms);
    }
}
