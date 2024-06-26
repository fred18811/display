#include <IoTClassDisplayDwin.h>

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

bool DisplayDwin::getDataFromDwin (String address, int lastByte) {
    for(int i = 0; i<dwinBuf.size(); i++){
        String str_address = dwinBuf[i]["address"];
        if(str_address == address){
            String url_link = dwinBuf[i]["urllink"];
            String ports = dwinBuf[i]["ports"];

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
                //Если контроллер не доступен или что то пощло не так возвращаем обратно положение кнопки---------------------------------
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
    for(int i = 0; i< dwinBuf.size(); i++){
        String str_address_buf = dwinBuf[i]["address"];
        if(str_address_buf == address){
            String data = reqData;
            dwinBuf[i]["tempValue"] = data;
        }
    }
}

String DisplayDwin::setDwinBuf(const char* address){
    for(int i = 0; i<dwinBuf.size(); i++){
        String str_address_buf = dwinBuf[i]["address"];
        if(str_address_buf == address){
            return dwinBuf[i]["tempValue"];
        }
    }
    return "null";
}

//Запучкается в области setup
void DisplayDwin::setup(){
    echoEnabled(false);
}

//Запучкается в области loop
void DisplayDwin::loop(){
    sendGetRequest(); //Отправка запроса на контроллер - потом перенести в listen !!!!!!!!!!!!
}