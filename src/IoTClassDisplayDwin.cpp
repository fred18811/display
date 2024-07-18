#include <IoTClassDisplayDwin.h>

#define DEBUG 0 //-----Режим отладки

class DisplayDwin;

void DisplayDwin::setHTTPClientWork (bool val){ 
    http_client_work = val;
    }
bool DisplayDwin::getHTTPClientWork (){
    return http_client_work;
    }
    
bool DisplayDwin::sendGetRequest () {
    if(getHTTPClientWork()){
        String url_link = linkParam["urllink"];
        String ports_data = linkParam["portsData"];
        linkParam.clear();
        String link = url_link + ports_data;

        HTTPClient http;
        http.begin(link.c_str());
        int httpResponseCode = http.GET();
        if (httpResponseCode<0){
            Serial.println("notConnect");
            setHTTPClientWork(false);
        }
        else {
            setHTTPClientWork(false);
            Serial.println("Connect");
            return true;
        }
        http.end();
    }
    return false;
}
bool DisplayDwin::getDataFromController () {
}
bool DisplayDwin::getDataFromDwin (String address, int lastByte) {
    for(int i = 0; i<dwinBuf["elements"].size(); i++){
        String str_address = dwinBuf["elements"][i]["address"];
        if(str_address == address){
            String url_link = dwinBuf["elements"][i]["urllink"];
            String ports = dwinBuf["elements"][i]["ports"];

            Serial.println(url_link);
            Serial.println(ports);

            String command_data = "";
            String ports_data = "";
            String data = "";
            int data_int = 0;

            for(int j=0; j<ports.length(); j++){

                    if(ports.charAt(j) != ';' && ports.charAt(j) != '=' ){
                        data = data + ports[j];
                    }
                    else if (ports.charAt(j) == ';') {
                        data_int = data.toInt();
                        ports_data = ports_data + (String(data_int, DEC) + ":" + String(lastByte, DEC) + ";");
                        data = "";
                    }
                    else if (ports.charAt(j) == '=') {
                        ports_data = data + "=";
                        data = "";
                        Serial.println(ports_data);
                    }
            }

            Serial.println(ports_data);
            String link = url_link + ports_data;
            //Serial.println(url_link);
            Serial.println(link);

            HTTPClient http;
            http.begin(link.c_str());
            int httpResponseCode = http.GET();
            if (httpResponseCode<0){
                //Если контроллер не доступен или что то пошло не так возвращаем обратно положение кнопки---------------------------------
                Serial.println("notConnect");
                setHTTPClientWork(false);
            }
            else {
                setHTTPClientWork(false);
                //Serial.println("Connect");
            }
            http.end();
        }
    }
    return false;
}

void DisplayDwin::setDwinBuf (const char* address, const char* reqData){
    for(int i = 0; i< dwinBuf["elements"].size(); i++){
        String str_address_buf = dwinBuf["elements"][i]["address"];
        if(str_address_buf == address){
            String data = reqData;
            dwinBuf["elements"][i]["tempValue"] = data;
        }
    }
}

String DisplayDwin::setDwinBuf(const char* address){
    for(int i = 0; i<dwinBuf["elements"].size(); i++){
        String str_address_buf = dwinBuf["elements"][i]["address"];
        if(str_address_buf == address){
            return dwinBuf["elements"][i]["tempValue"];
        }
    }
    return "null";
}

String DisplayDwin::getLinkWithoutParameters (const char* str) {
    String _str = str;
    _str = _str.substring(0,_str.indexOf("?")+1);
    return _str;
}

short int DisplayDwin::getControllerResponseValue (String resp) {
    if(resp=="ON") return 1;
    if(resp=="OFF") return 0;
    return -1;
}

void DisplayDwin::timerUpdate(){
    int timer = dwinBuf["timer"];
    polling_timer.setValueTime(timer);   
}

void DisplayDwin::dataControllerUpdate(AsyncWebSocket* handler){
    if(polling_timer.startTimer()){}
    else{
        for(int i = 0; i<dwinBuf["elements"].size(); i++){
            String state_port = dwinBuf["elements"][i]["statePort"];
            if(state_port.length() != 0){
                String str_address_buf = dwinBuf["elements"][i]["address"];
                String urllink = getLinkWithoutParameters(dwinBuf["elements"][i]["urllink"]);

                HTTPClient http;
                http.begin((urllink+state_port).c_str());
                int httpResponseCode = http.GET();
                if (httpResponseCode<0){
                    #if DEBUG
                    Serial.println("dataControllerUpdate notConnect http");
                    #endif
                }
                else {
                    #if DEBUG
                    Serial.println("dataControllerUpdate connect http");
                    #endif

                    String payload = http.getString();
                    payload = (getControllerResponseValue(payload) != -1) ? String(getControllerResponseValue(payload)) : payload;

                    #if DEBUG
                    Serial.println((urllink+state_port) + " = " + payload);
                    #endif

                    if(dwinBuf["elements"][i]["tempValue"] != payload){
                        dwinBuf["elements"][i]["tempValue"] = payload;
                        long address_get = (long) strtol(str_address_buf.c_str(), 0, 16);
                        byte data_get = byte(atoi(payload.c_str()));
                        setVP(address_get,data_get);
                        handler->textAll("{\"address\":" + str_address_buf + ",\"data\":" + payload  +"}" );

                        #if DEBUG
                        Serial.println(address_get);
                        Serial.println(data_get);
                        #endif     
                    }
                }
                http.end();
            }
        }
        polling_timer.stopTimer();
    }    
}

//Запучкается в области setup
void DisplayDwin::setup(){
    echoEnabled(false);
    timerUpdate();
}

//Запучкается в области loop
void DisplayDwin::loop(AsyncWebSocket* handler){
    sendGetRequest(); //Отправка запроса на контроллер - потом перенести в listen !!!!!!!!!!!!
    dataControllerUpdate(handler);
}