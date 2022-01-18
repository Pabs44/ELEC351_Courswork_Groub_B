#include "mbed.h"
#include "FIFO.hpp"
#include <cstdint>
#include <cstring>
#include <string.h>

FIFO env_FIFO;

Thread testThread(osPriorityBelowNormal);

void thread_test(){
    while(true){
        printf("\n\rTesting Threads");
        ThisThread::sleep_for(2500ms);
    }
}

int main() {
    testThread.start(thread_test);
    sleep();
}
