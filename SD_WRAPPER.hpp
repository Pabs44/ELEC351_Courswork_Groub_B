#ifndef __SD_WRAPPER__
#define __SD_WRAPPER__

#include <vector>
#include "mbed.h"
typedef struct {
    float temp;
    float pres;
    float light;
    long long time;
} ENVDATA;

class SD_WRAPPER {
public:
    private:
    std::vector<ENVDATA> _buffer;
    bool _mounted = false;
    public:
    SD_WRAPPER() {
        _mounted = false;
    }
    // Mount the storage device ready for read/write
    // Returns false if the device cannot be mounted
    bool mount() {
        //If already mounted, quit
        if (_mounted) return true;
        //Assume it works (read devices can fail)
        _mounted = true;
        //Simulate file open
        ThisThread::sleep_for(500ms);
        return true;
    }
    // Unmount the storage device (typically close the file)
    void umount() {
        //Already unmounted?
        if (!_mounted) return;
        //Simulate file closed
        _mounted = false;
        ThisThread::sleep_for(500ms);
    }
    // Writes a block of samples to the storage device
    //
    //  dat is an array of samples (type ENVDATA)
    //  NumberOfSamples is the number of samples in the array
    //  Returns true is the write operation succeeded
    bool write_samples(const ENVDATA dat[], uint16_t NumberOfSamples)
    {
        if (!_mounted) return false;
        ThisThread::sleep_for(250ms);
        for (uint16_t n=0; n<NumberOfSamples; n++) {
            _buffer.push_back(dat[n]);
            ThisThread::sleep_for(25ms);
        }
        //Assume it worked
        return true;
    }
    // Erase all data on the storage device
    // Returns true is successful
    bool wipe()
    {
        if (!_mounted) return false;
        _buffer.clear();
        return true;
    }
    // Write all samples from the storage device to the terminal
    void dump_samples()
    {
        if (!_mounted) return;
        for (ENVDATA rec : _buffer) {
            printf("\nTime: %lld, Temp: %5.2f, Press: %6.2f, Light: %3.2f", rec.time, rec.temp, rec.pres, rec.light);
            //Simulate read delay
            ThisThread::sleep_for(10ms);
        }
    }
};

#endif