#include <Arduino.h>
#include <WiFiUdp.h>
#include <IoTClassTimer.h>
#include <ArduinoJson.h>

#ifndef _IOTCLASSCONNECTOR_H
#define _IOTCLASSCONNECTOR_H

class IoTClassUdpConnector{
    static const int size_packet = 33;

    private:
        WiFiUDP Udp;
        IoTTimer Timer;
        uint8_t brodcast[4] = {255,255,255,255};
        uint16_t localUdpPort = 4210;
        String mac_addres;
        char incomingPacket[size_packet];
        const char askPacket[size_packet] = "SmartESPHello";
        const char replyPacket[size_packet] = "SmartESPOk";
        const char tooReplyPacket[size_packet] = "SmartESPGood";
        bool flag_count = true;
        bool flag_timer = true;
        unsigned int count_timer = 0;

        String analyseString(String incStr, String separator, const char* strCheck);
        bool checkMAC(JsonArray &arrayNetDevices, String &mac_addres); //Проверяем существование мак адреса
        void sendInitialQuest(); //Опрос устройств при стратре контроллера
        void sendReq(IPAddress ip, uint16_t port, const char *packet); //Отправляем сообщения
        void getReq(JsonArray &arrayNetDevices); //Получаем запрос широковещательным сообщением
    public:
        IoTClassUdpConnector(uint16_t port = 4210);
        void start(unsigned int count_timer_val, unsigned long timer_interval_val, String string_mac_addres); //Инициализация в setupe
        void loop(JsonArray &arrayNetDevices);//Прослушивание на порту
};

#endif // _IOTCLASSCONNECTOR_H