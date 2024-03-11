//Добавить возможностьизменнения MAC
//HostName не понятно нужен ли

// Дописать analyseString
//Сделать StaticJsonDocument и добавлять туда новые устройста
//Организовать ппооверку при получении запроса добавленных устройств

//Из веб в разделе dwin сделать кнопку отправки комманд на экран
//Из веб в разделе dwin сделать кнопку сохранения настроек
//Пережавать жанные их dwin на веб

#include <Arduino.h>
#include "ArduinoNvs.h"
#include <math.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <WiFiUdp.h>
#include <ESPAsyncWebServer.h>
#include <ESP32httpUpdate.h>        //!!!!! под вопросом
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <IoTFunction.h>
#include <settings.h>
#include <Bounce2.h>
#include <WebServerFunc.h>
#include <IoTClassDisplayDwin.h>
#include <IoTClassWachDog.h>
#include <IoTClassTimer.h>
#include <IoTClassUdpConnector.h>

#define DEBUG 1 //-----Режим отладки

String version_prosh ="0.1b";//------Версия прошивки

IoTTimer mytime(2); //Для теста чего-то (так не нужен)
//--------------------------------------------------------------WachDog--------------------------------------------------------------------------------------------
MyWachDog wachdog(whatchdog);
//--------------------------------------------------------------Определение кнопки---------------------------------------------------------------------------------
Bounce debouncer = Bounce();
//--------------------------------------------------------------Хранение данных------------------------------------------------------------------------------------
IoTTimer rest_esp(2);
//--------------------------------------------------------------Сетевые настройки WIFI-----------------------------------------------------------------------------
struct
{
String str_soft_ap = "On";
uint8_t ip[4] = {192,168,1,2};
uint8_t gateway[4] = {192,168,1,1};
uint8_t subnet[4] = {255,255,255,0};
String host_name = "Display";
String str_ssid = "None";
String str_dhsp = "On";
} wifi_settings;
uint8_t newMACAddress[] = {0x32, 0xAE, 0xA4, 0x07, 0x0D, 0x60};
//--------------------------------------------------------------Настройка UDP-------------------------------------------------------------------------------------
IoTClassUdpConnector UdpConnector(4210); 
//--------------------------------------------------------------переменные для MQTT--------------------------------------------------------------------------------
const char* ipmqtt = "0.0.0.0";
const char* CLIENT_ID = "Display";
//--------------------------------------------------------------Счетчик попыток подключения к wifi
int count_WIFI = 0;
//--------------------------------------------------------------Настройка дисплея----------------------------------------------------------------------------------
DisplayDwin hmi(DGUS_SERIAL, rx2, tx2, DGUS_BAUD);
//--------------------------------------------------------------настройки WEB Client-------------------------------------------------------------------------------
bool http_client_work = false; 
//--------------------------------------------------------------настройки WEB Server-------------------------------------------------------------------------------
WiFiClient espClient;
PubSubClient client(espClient);
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
//AsyncEventSource events("/events");

StaticJsonDocument<400> netBuf; //Буфер хранения настроек сети
StaticJsonDocument<8192> dwinBuf; //Буфер хранения настроек элементов экрана dwin
StaticJsonDocument<400> linkParam; //Буфер хранения временных настроек для url контроллера

DynamicJsonDocument netDevices(2048);
JsonArray arrayNetDevices = netDevices.to<JsonArray>(); //Массив Хранения опрошенных сетевых устройств

void setup() {

  pinMode(btn_reset, INPUT_PULLUP); //???????????????????????
  debouncer.attach(btn_reset); //????????????????????????????
  debouncer.interval(50); //?????????????????????????????????
  wachdog.start();   //-----Start WachDog
  NVS.begin();       //-----Start ArduinoNvs

  delay(1000);
  Serial.begin(115200);

  wifi_settings.str_soft_ap = NVS.getString("wifimode") == "Off" ? "Off" : "On";  //-----Проверяем наличие кюча wifimode
  wifi_settings.str_dhsp = NVS.getString("dhsp") == "Off" ? "Off" : "On";         //---------------Чтение параметра dhcp
    //----------------------------------------------
  const String str_host_name = NVS.getString("host_name");
  if(str_host_name.length() && str_host_name != wifi_settings.host_name) wifi_settings.host_name = str_host_name; //---------------Чтение имя WIFI
  //----------------------------------------------
  const String const_str_ssid = NVS.getString("ssid");
  if(const_str_ssid.length() && const_str_ssid != wifi_settings.str_ssid) wifi_settings.str_ssid = const_str_ssid;
  const char* ssid = wifi_settings.str_ssid.c_str(); //---------------Чтение логина WIFI
  //----------------------------------------------
  const String const_str_pass = NVS.getString("pswd");
  const char* pass = const_str_pass.c_str();         //---------------Чтение пароля WIFI
  //----------------------------------------------
  const String str_ip = NVS.getString("ip");              
  if(str_ip.length())writeNetworkSetting(str_ip.c_str(),wifi_settings.ip);      //---------------Чтение пароля ip
  //----------------------------------------------
  const String str_gw = NVS.getString("gateway");              
  if(str_gw.length())writeNetworkSetting(str_gw.c_str(),wifi_settings.gateway); //---------------Чтение пароля gateway
  //----------------------------------------------
  const String str_subnet = NVS.getString("subnet");              
  if(str_subnet.length())writeNetworkSetting(str_subnet.c_str(),wifi_settings.subnet); //---------------Чтение пароля subnet

  if(wifi_settings.str_soft_ap == "Off"){                                              //-----Проверяем состояние параметра
    if((digitalRead(btn_reset) == LOW)){                            //-----Сбрасываем параметр wifimode для перехода точки в режим AP
      NVS.setString("wifimode", "On");
      #if DEBUG
        Serial.println("Please reboot module for coniguration");
      #endif
      ESP.restart();        
    }
    else{
      WiFi.mode(WIFI_STA);
      #if DEBUG
        //esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);
        Serial.println();
        Serial.println("Connecting to ");
        Serial.println(ssid);
        Serial.println(pass);
      #endif
      WiFi.setHostname(wifi_settings.host_name.c_str());
      WiFi.begin(ssid, pass);

      if(wifi_settings.str_dhsp=="Off"){                  //--------Проверяем состояние флага dhsp
          WiFi.config(wifi_settings.ip, wifi_settings.gateway, wifi_settings.subnet);
        }
      while (WiFi.status() != WL_CONNECTED) {
        if(digitalRead(btn_reset) != LOW){
          delay(1000);
          count_WIFI++;
          if(count_WIFI>=60){ESP.restart();}
          else{
            #if DEBUG
              Serial.print(".");
            #endif
            }
        }
        else{
          NVS.setString("wifimode", "On");
          #if DEBUG
            Serial.println("reboot awp");
            Serial.println("Reboot");
          #endif
          ESP.restart();
        }
      }

      #if DEBUG
        Serial.println("");
        Serial.println("WiFi connected");

        Serial.print("hostname ");
        Serial.println(WiFi.getHostname());

        Serial.print("ip addres: ");  
        Serial.println(WiFi.localIP());

        Serial.print("gateway addres: ");
        Serial.println(WiFi.gatewayIP());

        Serial.print("subnet addres: ");
        Serial.println(WiFi.subnetMask());

        Serial.print("mac addres: ");
        Serial.println(WiFi.macAddress());
      #endif
    }
  }
  else {SoftAP_init();}

  #if DEBUG
    Serial.println("softAP " + wifi_settings.str_soft_ap);
    Serial.println("DHSP " + wifi_settings.str_dhsp);
  #endif
//--------------------------------------------------------Инициалтзация UDP-----------------------------------------------------------------------------------------------
  // Udp.begin(udp_settings.localUdpPort);
  // #if DEBUG
  //   Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), udp_settings.localUdpPort);
  // #endif
  UdpConnector.start(5,5,WiFi.macAddress());

// --------------------------------------------------------------Настройка WEB--------------------------------------------------------------------------------------------
  if(SPIFFS.begin()){    //-----Mount FileSystem

    File json_setting_dwin = SPIFFS.open("/data/dwin.json", FILE_READ); //Загружаем настройки dwin
    if(json_setting_dwin && json_setting_dwin.size()){
        deserializeJson(dwinBuf, json_setting_dwin);
    }

    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html").setCacheControl("max-age=10");
    server.on("/setting", HTTP_ANY, [](AsyncWebServerRequest *request){request->send(SPIFFS, "/setting.html", "text/html");});
    server.on("/settingmqtt", HTTP_ANY, [](AsyncWebServerRequest *request){request->send(SPIFFS, "/settingmqtt.html", "text/html");});
    server.on("/settingdwin", HTTP_ANY, [](AsyncWebServerRequest *request){request->send(SPIFFS, "/settingdwin.html", "text/html");});
    server.on("/data/dwin.json", HTTP_ANY, [](AsyncWebServerRequest *request){request->send(SPIFFS, "/data/dwin.json", "text/json");});
    server.on("/getethsetting", HTTP_ANY, [](AsyncWebServerRequest *request){ //-----------Отправляем данные настройки сети
      String str_json = "";
      netBuf["ip"]= NVS.getString("ip").length() ? NVS.getString("ip") : getStringNetworAddress(wifi_settings.ip);
      netBuf["gateway"]= NVS.getString("gateway").length() ? NVS.getString("gateway") : getStringNetworAddress(wifi_settings.gateway);
      netBuf["subnet"]= NVS.getString("subnet").length() ? NVS.getString("subnet") : getStringNetworAddress(wifi_settings.subnet);
      netBuf["wifimode"]= wifi_settings.str_soft_ap;
      netBuf["dhsp"]= wifi_settings.str_dhsp;
      netBuf["host_name"]= wifi_settings.host_name;
      netBuf["ssid"]= wifi_settings.str_ssid;
      netBuf["sofApIp"] = WiFi.softAPIP().toString();
      netBuf["versionProsh"] = version_prosh;
      serializeJsonPretty(netBuf,str_json);
      request->send(200, "text/html", str_json);
    });
    //-------------------------------------------
    server.on("/saveether", HTTP_ANY, [](AsyncWebServerRequest *request){     //-----------Сохранение настроек сети
      request->send(200, "text/html", handleSaveSettingEth(NVS, request));
      ESP.restart();
    });
    //-------------------------------------------
    server.on("/getdwinsetting", HTTP_ANY, [](AsyncWebServerRequest *request){ //-----------Отправляем клиенту данные настройки dwin
      String str_json = "";
      serializeJsonPretty(dwinBuf,str_json);
      request->send(200, "text/html", str_json);
    });
    //-------------------------------------------
    server.on("/savedwinsetting", HTTP_PUT, [](AsyncWebServerRequest *request){}, NULL,//-----------Сохраняем клиентские настройки dwin
    [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
      String req = String((char *)data);
      //Пишем данные в буфер
      dwinBuf.clear();
      deserializeJson(dwinBuf,req);
      //Пишем данные в файл
      File fileDwin = SPIFFS.open ("/data/dwin.json",FILE_WRITE);
      if(fileDwin) serializeJson(dwinBuf, fileDwin);
      fileDwin.close();
      request->send(200, "text/html", "Ok");
    });
    //-------------------------------------------
     server.on("/sendlinkfromdwin", HTTP_POST, [](AsyncWebServerRequest *request){}, NULL,//-----------Получаем данные с dwin для отправки на контроллер
    [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
      String req = String((char *)data);
      deserializeJson(linkParam,req);
      http_client_work = true;
      request->send(200, "text/html", "Ok");
    });
    //-------------------------------------------
    server.on("/getdwinreq", HTTP_GET, [](AsyncWebServerRequest *request){ //-----------Отправляем данные по запросу get dwin
      if(request->hasArg("address") && request->hasArg("data")) {
        long address_get = (long) strtol(request->arg(0u).c_str(), 0, 16);
        byte data_get = byte(atoi(request->arg(1).c_str()));
        hmi.setVP(address_get,data_get);

        //-------------вынести в отдельную функцию-----
        //---------------------------------------------
        //---------------------------------------------
        for(int i = 0; i<dwinBuf.size(); i++){
          String str_address_buf = dwinBuf[i]["address"];
          if(str_address_buf == request->arg(0u).c_str()){
            String data = request->arg(1).c_str();
            Serial.println(data);
            dwinBuf[i]["tempValue"] = data;
          }
        }
        //---------------------------------------------
        //---------------------------------------------
        //---------------------------------------------


        request->send(200, "text/html", "Ok");
      }
      else if(request->hasArg("address")){
        //-------------вынести в отдельную функцию-----
        //---------------------------------------------
        //---------------------------------------------
        for(int i = 0; i<dwinBuf.size(); i++){
          String str_address_buf = dwinBuf[i]["address"];
          if(str_address_buf == request->arg(0u).c_str()){
            String a = dwinBuf[i]["tempValue"];
            Serial.println(a);
            request->send(200, "text/html", dwinBuf[i]["tempValue"]);
          }
        }
        //---------------------------------------------
        //---------------------------------------------
        //---------------------------------------------
        request->send(200, "text/html", "null");
        // Получаем состояние элемента из буфера
      }
      else{
        request->send(200, "text/html", "Bad");
      }
      // int args = request->args();
      // for(int i=0;i<args;i++){
      //     Serial.println(request->argName(i));
      //     if(request->argName(i) == "page"){
      //       byte page = hmi.getPage();
      //       request->send(200, "text/html", "{\"page\":" + String(page) + "}");
      //     }
      // }
      request->send(404);
    });
   //-------------------------------------------
    server.on("/getdata", HTTP_ANY, [](AsyncWebServerRequest *request){
        int args = request->args();
        for(int i=0;i<args;i++){
          if(request->argName(i) == "controlsetting"){
            AsyncResponseStream *response = request->beginResponseStream("application/json");
            request->send(response);
          }
        }
        request->send(404);
    });
    //-------------------------------------------
    server.on("/sentdata", HTTP_ANY, [](AsyncWebServerRequest *request){ // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!???????????????????
      int args = request->args();
      if(request->argName(0) == "controlsetting"){
        for(int i=1;i<args;i++){
          // pechkaBuf[request->argName(i)] = request->arg(request->argName(i));
        }
      }
      request->send(200);
    });
    //-------------------------------------------
    server.onNotFound(onRequest);   
    /*server.on("/savemqtt", handleSaveSettingMQTT);*/
  }
// // ---------------------------------- Настройки возврата MQTT-------------------------------------------------------------------------------------------------------------
//         // if (!netBuf["mqtton"]){  
//         //   CLIENT_ID = netBuf["id_mqtt"];
//         //   int port_mqtt = netBuf["port_mqtt"];
//         //   ipmqtt = netBuf["ip_mqtt"];

//         //   client.setServer(ipmqtt,port_mqtt);
//         //   Serial.println("Connecting to MQTT server");
//         //   client.connect(CLIENT_ID);
//         //   Serial.println("connect mqtt...");
//         //   client.connected();
//         //   delay(10);
//         //   Serial.print("IP сервера MQTT: ");
//         //   Serial.println(ipmqtt);
//         //   Serial.println("ID клиенты MQTT: " + String(CLIENT_ID));
//         //   Serial.print("Port сервера MQTT: ");
//         //   Serial.println(port_mqtt);
//         //   client.setCallback(getData);
//         // }

//--------------------------------------HTTP server подключение----------------------------------------------------------------------------------------------------------- 
  server.begin();
  #if DEBUG
    Serial.println("HTTP server started");
  #else
    Serial.println("System start");
  #endif

  //-----------------------------------------Настраиваем дисплей-------------------------------------------------------
  hmi.echoEnabled(false);
}
void loop() {
//-------------------------------------------------Перевод модуля в режим конфигурации путем замыкания GPIO0 на массу-----------------------------------------------------
  // if((digitalRead(btn_reset) == LOW)){
  //   File netFile = SPIFFS.open ("/config.json","r+");
  //   if(netFile && netFile.size()){
  //     // netBuf["wifimode"] = "On";
  //     serializeJson(netBuf, netFile);
  //     netFile.close();
  //     Serial.println("Please reboot module for coniguration");
  //     ESP.restart();
  //   }
  // }
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------ 

  client.loop();
  wachdog.loop();
  //UdpConnector.loop(arrayNetDevices); // Опрос устройст широковещанием и прием запросов

  hmi.hmiCallBack([](String address, int lastByte, String message, String response){
    if(address.toInt() < 10000){ //Может задублироваться и быть не верный address
      Serial.println("OnEvent : [ A : " + address + " | D : "+ String(lastByte, DEC) + " R: " + response +" ]");
      ws.textAll("{\"address\":" + address + ",\"data\":" + String(lastByte, DEC)  +"}" );

    //----------------------------------------------------
      for(int i = 0; i<dwinBuf.size(); i++){
        String str_address = dwinBuf[i]["address"];
        if(str_address == address){
          String url_link = dwinBuf[i]["urllink"];
          String ports = dwinBuf[i]["ports"];
          String ports_data = "";
          String data = "";
          int data_int = 0;

          for(int j=0; j<ports.length(); j++){
            if(ports.charAt(j) != ';'){
              data = data + ports[j];
            }
            else {
              data_int = data.toInt();
              ports_data = String(data_int, DEC) + ":" + String(lastByte, DEC) + ";";
              data = "";
            }
          }
          Serial.println(ports_data);
          String link = url_link + ports_data;
          Serial.println(url_link);
          Serial.println(link);

          HTTPClient http;
          http.begin(link.c_str());
          int httpResponseCode = http.GET();
          if (httpResponseCode<0){
          //Если контроллер не доступен или что то пощло не так возвращаем обратно положение кнопки---------------------------------
          Serial.println("notConnect");
          http_client_work = false;
          }
          else {
            http_client_work = false;
            Serial.println("Connect");
          }
          http.end();
        }
      }
    //----------------------------------------------------
    }
  });
  hmi.listen();

  if(http_client_work){
    String url_link = linkParam["urllink"];
    String ports_data = linkParam["portsData"];

    String link = url_link + ports_data;
    Serial.println(link);


    //String serverPath = "http://192.168.44.10/sec/?cmd=11:2";
    HTTPClient http;
    http.begin(link.c_str());
    int httpResponseCode = http.GET();
    if (httpResponseCode<0){
    Serial.println("notConnect");
    http_client_work = false;
    }
    else {
      http_client_work = false;
      Serial.println("Connect");
    }
    http.end();
  }
  // mytime.loop([](){
  //   Serial.println("timer");
  //   hmi.setVP(0x5001,0x82);
  //   hmi.setVP(0x5000,0xff);
  // });
}