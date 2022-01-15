#ifndef __FIFO_message__
#define __FIFO_message__

    #include "mbed.h"

    class FIFOmessage_t {
    public:
        float temp;
        float pres;
        float lux;
        //Constructor
        FIFOmessage_t(float t, float p, float l){
            temp = t;
            pres = p;
            lux = l;
        }
    };

#endif