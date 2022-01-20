/*Each point relates to the requirements respectively:
1. 
FIFO.CPP
LINE 61
function read_sensors takes data from the environmental sensor and samples them at a rate of 1 every 10 seconds.
Sampling jitter has been minimised by making T1 time as small as possible while keeping T2 at 10 seconds

2. 
SD_WRAPPER.hpp
Virtual SD used as we couldn’t get the SD card working.
Data is written in human readable format to SD.

3.
FIFO.CPP
read_FIFO and write_FIFO functions read and write data to the FIFO in their own threads
Writing to the FIFO doesn’t block any other tasks
LINE 158 Blocks if the FIFO is empty

4.
Temperature, pressure and light max and min values have been implemented and can be changed by typing set_low() or set_high()
The console will ask for each value in turn
Every value must end in a “!” to move to the next value setting

5.
Treads used:
alarmThread
sensorThread
writeThread
readThread
serialThread
No operations interfere

6.
To minimise power consumption treads sleep when not being used
CPU is set to sleep when not used

7.
Typable Functions:
latest()
buffered()
flush()
set_low() - explained in 4.
set_high() - explained in 4.
All functions should work as intended 

8.
N/A

9.
Locks and Semaphores use so critical processes don’t become interrupted 

10.
N/A

*/
