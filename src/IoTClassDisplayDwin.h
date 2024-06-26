#include <Arduino.h>
#include <future>
#include <DWIN.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

class DisplayDwin : public DWIN{

    private:
        bool http_client_work = false;

    public:
        StaticJsonDocument<400> linkParam; //Буфер хранения временных настроек для url контроллера
        StaticJsonDocument<8192> dwinBuf; //Буфер хранения настроек элементов экрана dwin
        
        DisplayDwin(HardwareSerial& port, uint8_t receivePin, uint8_t transmitPin, long baud=DWIN_DEFAULT_BAUD_RATE) : DWIN(port, receivePin, transmitPin, baud){}; 
        void setHTTPClientWork (bool val);
        bool getHTTPClientWork ();

        void setDwinBuf(const char* address, const char* reqData);
        String setDwinBuf(const char* address);

        bool sendGetRequest ();
        bool getDataFromDwin (String address, int lastByte);
        void setup();
        void loop();
};