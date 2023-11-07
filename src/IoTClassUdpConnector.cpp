#include <IoTClassUdpConnector.h>

#define DEBUG 1 //-----Режим отладки

class IoTClassUdpConnector;

IoTClassUdpConnector::IoTClassUdpConnector(uint16_t port)
{
   localUdpPort = port;
}

void IoTClassUdpConnector::start(unsigned int count_timer_val, unsigned long timer_interval_val, String string_mac_addres){
    mac_addres = string_mac_addres;

    Udp.begin(localUdpPort);
    count_timer = count_timer_val;
    Timer.setValueTime(timer_interval_val);
    #if DEBUG
        Serial.printf("Now listening at IP, UDP port %d\n", localUdpPort);
    #endif
}

void IoTClassUdpConnector::sendReq(IPAddress ip, uint16_t port, const char *packet){
    Udp.beginPacket(ip, port);
    Udp.printf(packet);
    Udp.endPacket();
}

String IoTClassUdpConnector::analyseString(String incStr, String separator, const char* strCheck) {
    int i = 0;
    String val = "";
    bool stringReadFlag = false;
    bool stringReadFlagVal2 = false;
    
    while (incStr[i] != '\0') {
        if((int)incStr[i] == (int)separator[0]) stringReadFlag=true;
        else if(!stringReadFlag) val += incStr[i];
        else if(stringReadFlag && strCheck == "first") return val;
        else if(stringReadFlag && strCheck == "second" && !stringReadFlagVal2) {
            val = "";
            stringReadFlagVal2 = true;
        }
        if(stringReadFlagVal2) val += incStr[i];
        i++;
    };
    return val;
}

void IoTClassUdpConnector::sendInitialQuest(){
    if(flag_count){
        flag_count = false;
    }
    if(flag_timer){
        flag_timer = false;
    }
    if(count_timer > 0){
        if(Timer.startTimer()){}
        else{
            #if DEBUG
                String res = "";
                for(int i = 0; i < sizeof(brodcast); i++){
                    int act = (int) brodcast[i];
                    res += (i == sizeof(brodcast)-1) ? String(act) : String(act) + "."; 
                }
                Serial.println(res);
                Serial.printf("incomingPacket %s\n", String(localUdpPort));
            #endif

            char resp[size_packet] = "";
            strcat(resp, askPacket);
            strcat(resp, (";"+mac_addres).c_str());

            sendReq(brodcast,localUdpPort,resp);
            
            Timer.stopTimer();

            count_timer--;
            }
    }
}

bool IoTClassUdpConnector::checkMAC(JsonArray &arrayNetDevices, String &mac_addres){
    bool check = false;
    for (JsonVariant value : arrayNetDevices) {
        #if DEBUG
            Serial.println(mac_addres);
            Serial.println(value["mac"].as<String>());
        #endif
        if(mac_addres == value["mac"].as<String>()) check = true;
    }
    return check;  
}

void IoTClassUdpConnector::getReq(JsonArray &arrayNetDevices){
    int packetSize = Udp.parsePacket();
    if (packetSize){
        #if DEBUG
            Serial.println("UDP Start parse");
        #endif
        int len = packetSize+1 <= size_packet ? Udp.read(incomingPacket, size_packet) : 0;

        #if DEBUG
            Serial.printf("Incomming massage %s\n", incomingPacket); // На запрос делаем что-то DEBUG
        #endif

        String message = analyseString(incomingPacket,";", "first");
        String str_mac = analyseString(incomingPacket,";", "second");
        if(str_mac.length()){
            if(message == String(askPacket)) { // Отправляем подтверждение MACaddress;SmartESPOk на запрос SmartESPHello
                #if DEBUG
                    Serial.printf("UDP ask Packet, section Ok: %s\n", incomingPacket);
                #endif
                
                if(!checkMAC(arrayNetDevices,str_mac)){ // Проверка, существует ли такой mac адррес
                    JsonObject nested = arrayNetDevices.createNestedObject(); //Создаем вложение в массив JSon
                    nested["mac"] = str_mac; //Добавляем мак
                    nested["ip"] = Udp.remoteIP(); //Добавляем ip
                }

                char resp[size_packet] = "";
                strcat(resp, replyPacket);
                strcat(resp, (";"+mac_addres).c_str());
                sendReq(Udp.remoteIP(), Udp.remotePort(), resp);
            }
            if(message == String(replyPacket)) { // Отправляем подтверждение SmartESPGood на запрос SmartESPOk
                #if DEBUG
                    Serial.printf("UDP too reply Packet, section Good: %s\n", incomingPacket);
                #endif
                
                if(!checkMAC(arrayNetDevices,str_mac)){ // Проверка, существует ли такой mac адррес
                    JsonObject nested = arrayNetDevices.createNestedObject(); //Создаем вложение в массив JSon
                    nested["mac"] = str_mac; //Добавляем мак
                    nested["ip"] = Udp.remoteIP(); //Добавляем ip
                }
                sendReq(Udp.remoteIP(), Udp.remotePort(), tooReplyPacket);
            }
        }
        memset(&incomingPacket, 0, sizeof(incomingPacket));
        Udp.flush();
    }    
}

void IoTClassUdpConnector::loop(JsonArray &arrayNetDevices){
    sendInitialQuest();
    getReq(arrayNetDevices);
}