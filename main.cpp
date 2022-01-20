/*
ELEC351
Authors: Pablo Pelaez and Angus McDowall
Group: B
*/

#include "mbed.h"
#include "FIFO.hpp"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string.h>

#define UART2_TX PD_5
#define UART2_RX PD_6

FIFO env_FIFO;
static BufferedSerial user_in(USBTX, USBRX, 115200);

Thread testThread(osPriorityBelowNormal);
Thread serialThread;

void serial_input_thread(){
    const int max_len = 25;
    char buff[max_len] = {0};
    int len = 0;
    
    printf("\n");
    while (true) {
        char buff_in;
        if(buff[len-1] == ')'){
            if(buff[0] == 'l'){
                cout << "\nrunning latest()";
                for(int idx = 0; idx <= max_len; idx++) buff[idx] = 0;
                ThisThread::sleep_for(100ms);
                len = 0;
                env_FIFO.latest();
            }else if(buff[0] == 'b'){
                cout << "\nrunning buffered()";
                for(int idx = 0; idx <= max_len; idx++) buff[idx] = 0;
                len = 0;
                env_FIFO.buffered();
            }else if(buff[0] == 'f'){
                cout << "\nrunning flushed()";
                for(int idx = 0; idx <= max_len; idx++) buff[idx] = 0;
                len = 0;
                env_FIFO.flush();
            }else if (buff[0] == 's' && buff[4] == 'l'){
                cout << "\nrunning set_low()";
                for(int idx = 0; idx <= max_len; idx++) buff[idx] = 0;
                len = 0;
                //env_FIFO.set_low(buff[10], buff[12], buff[14]);
            }else if (buff[0] == 's' && buff[4] == 'h'){
                cout << "\nrunning set_high()";
                len = 0;
                size_t str_len = len;
                float t_new = 0, p_new = 0, l_new = 0;

                cout << "\nSet upper temperature boundary:" <<endl;
                do{
                    user_in.read(&buff_in, 1);
                    buff[len++] = buff_in;
                    user_in.write(&buff_in, 1);
                    ThisThread::sleep_for(100ms);
                }while(buff[len-1] != '!');
                str_len = len;
                string buff_t(buff);
                t_new = stof(buff_t, &str_len);
                for(int idx = 0; idx <= len; idx++) buff[idx+1] = 0;
                ThisThread::sleep_for(1s);
                len = 0;

                cout << "\nSet upper pressure boundary:" <<endl;
                do{
                    user_in.read(&buff_in, 1);
                    buff[len++] = buff_in;
                    user_in.write(&buff_in, 1);
                    ThisThread::sleep_for(100ms);
                }while(buff[len-1] != '!');
                str_len = len;
                string buff_p(buff);
                p_new = stof(buff_p, &str_len);
                for(int idx = 0; idx <= max_len; idx++) buff[idx+1] = 0;
                ThisThread::sleep_for(1s);
                len = 0;

                cout << "\nSet upper light level boundary:" <<endl;
                do{
                    user_in.read(&buff_in, 1);
                    buff[len++] = buff_in;
                    user_in.write(&buff_in, 1);
                    ThisThread::sleep_for(100ms);
                }while(buff[len-1] != '!');
                str_len = len;
                string buff_l(buff);
                for(int idx = 0; idx <= max_len; idx++) buff[idx+1] = 0;
                ThisThread::sleep_for(1s);
                l_new = stof(buff_l, &str_len);

                for(int idx = 0; idx <= max_len; idx++) buff[idx] = 0;
                len = 0;

                env_FIFO.set_high(t_new, p_new, l_new);
            }
        }else{
            while(user_in.readable()){
                user_in.read(&buff_in, 1);
                buff[len++] = buff_in;
                user_in.write(&buff_in, 1);
            }
        }
    }
}

void thread_test(){
    while(true){
        printf("\n\rTesting Threads");
        ThisThread::sleep_for(2500ms);
    }
}

int main() {
    //testThread.start(thread_test);
    serialThread.start(serial_input_thread);
    //sleep();
}
