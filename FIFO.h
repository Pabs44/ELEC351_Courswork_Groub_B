#ifndef __FIFO__
#define __FIFO__

    #include "mbed.h"

    class FIFO {
    public:
        float temp;
        float pres;
        float lux;
        //Constructor
        FIFO(float t, float p, float l){
            temp = t;
            pres = p;
            lux = l;
        }
    };

#endif