#include "mbed.h"
#include "UOP_MSB_SENSORDATA.h"
#include <cstring>
#include <string.h>
using namespace uop_msb;

int main() {

    // Testing the sd card and checking sensors
    UOP_MSB_SENSORDATA board;
    board.read_sensors();

    return 0;
}
