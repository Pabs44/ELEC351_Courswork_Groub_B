#ifndef __FIFO__
#define __FIFO__

#include "uop_msb.h"
#include "SD_WRAPPER.hpp"
using namespace uop_msb;

    const int _FIFO_size = 6;
    static int _startWrite;
    static ENVDATA _env_data;
    static ENVDATA _env_data_arr[_FIFO_size];
    
    class FIFOmessage_t {
    public:
        ENVDATA _msg_env_data;

        //Constructor
        FIFOmessage_t(float t, float p, float l){
            _msg_env_data.temp = t;
            _msg_env_data.pres = p;
            _msg_env_data.light = l;
            _msg_env_data.time = 1200;
        }
    };

    class UOP_MSB_SENSORDATA {
    private:
        //Light Levels
        AnalogIn ldr_sensors;
        //Environmental Sensor
        uop_msb::EnvSensor env;
        //LED Matrix
        SPI matrix_spi;   // MOSI, MISO, SCLK
        DigitalOut matrix_spi_cs;           //Chip Select ACTIVE LOW
        DigitalOut matrix_spi_oe;           //Output Enable ACTIVE LOW
        //User Switch
        DigitalIn userButton;

    public:
        //Constructor
        UOP_MSB_SENSORDATA() : 
        ldr_sensors(AN_LDR_PIN), matrix_spi(PC_12, PC_11, PC_10), matrix_spi_cs(PB_6), matrix_spi_oe(PB_12), userButton(USER_BUTTON){}

        void read_sensors();
        void alarm();
    };

    class FIFO : public UOP_MSB_SENSORDATA {
    public:
        Mutex sdLock;
        Semaphore samplesInBuffer;
        Semaphore spaceInBuffer;

        Queue<FIFOmessage_t, _FIFO_size> FIFO_queue;
        MemoryPool<FIFOmessage_t, _FIFO_size> memPool;

        UOP_MSB_SENSORDATA FIFO_board;

        SD_WRAPPER SD_abstract;

        //Constructor
        FIFO() : 
        sdLock(), samplesInBuffer(), spaceInBuffer(_FIFO_size), FIFO_queue(), memPool(), FIFO_board(){}

        void write_FIFO();
        void read_FIFO();
    };

    /*class SD {
    public:
        void static write_sdcard(FIFOmessage_t* readFIFO);
        void static read_sdcard();
    };*/

#endif