#include <Arduino.h>
#include <future>
#include <DWIN.h>

class DisplayDwin : public DWIN{
    public:
        DisplayDwin(HardwareSerial& port, uint8_t receivePin, uint8_t transmitPin, long baud=DWIN_DEFAULT_BAUD_RATE) : DWIN(port, receivePin, transmitPin, baud){};    
};