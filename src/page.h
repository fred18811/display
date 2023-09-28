#include <Arduino.h>

/*
//----------------------------------------------------------------Сохранение настроек MQTT------------------------------------------------------------------------
void handleSaveSettingMQTT(String webPage){
  webPage = "";
  webPage += "<html>\
 <head>\
    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\
   <title>ESP8266 Settings MQTT</title>\
    "+style_css+"\
 </head>";
  webPage += "<body>";
   
 File configFile = SPIFFS.open ("/config.json","w");

      if(server.arg("ip_mqtt")!="")netBuf["ip_mqtt"] = server.arg("ip_mqtt");
      if(server.arg("port_mqtt")!="")netBuf["port_mqtt"] = server.arg("port_mqtt");
      if(server.arg("id_mqtt")!="")netBuf["id_mqtt"] = server.arg("id_mqtt");
      if(server.hasArg("check"))netBuf["mqtton"] = true;
      else netBuf["mqtton"] = false;
  
  serializeJson(netBuf, configFile);
  configFile.close();

  webPage += "<p>Setting MQTT saved.</p>";
  webPage += "<a href='/'>Back</a>";
  webPage += "</body></html>";
  server.send ( 200, "text/html", webPage );
 // ESP.restart();
}
*/
//----------------------------------------------------------------Сохранение настроек Контроллера------------------------------------------------------------------------
// String handleSaveSettingPechka(StaticJsonDocument<400u> pechkaBuf, AsyncWebServerRequest *request){
//   String webPage = "";
//   webPage += "<html>\
//  <head>\
//     <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\
//     <meta http-equiv='Refresh' content='0; url=/'>\
//    <title>ESP8266 Settings ethernet</title>\
//  </head>";
//   webPage += "<body style=\"text-align: center;\">";

//  File pechkaFile = SPIFFS.open ("/configcontroller.json","w");
//       if(request->arg("deltaTempval")!="")pechkaBuf["deltaTempval"] = request->arg("deltaTempval");
//       if(request->arg("timerClear")!="")pechkaBuf["timerClear"] = request->arg("timerClear");
//       if(request->arg("timerShnek")!="")pechkaBuf["timerShnek"] = request->arg("timerShnek");
//       if(request->arg("timerSvecha")!="")pechkaBuf["timerSvecha"] = request->arg("timerSvecha");
//       if(request->arg("timerVent")!="")pechkaBuf["timerVent"] = request->arg("timerVent");
//       if(request->arg("maxTemp")!="")pechkaBuf["maxTemp"] = request->arg("maxTemp");
//   serializeJson(pechkaBuf, pechkaFile);
//   pechkaFile.close();

//   webPage += "<p>Setting Ethernet saved.</p>";
//   webPage += "</body></html>";
//   return webPage;
// }

//----------------------------------------------------------------Сохранение настроек сети------------------------------------------------------------------------
String handleSaveSettingEth(ArduinoNvs NVS, AsyncWebServerRequest *request){
  String webPage = "";
  webPage += "<html>\
 <head>\
    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\
    <meta http-equiv='Refresh' content='0; url=/'>\
   <title>ESP8266 Settings ethernet</title>\
 </head>";
  webPage += "<body style=\"text-align: center;\">";
  //--Принимаем данные из формы и записываем в NVS---
  int params = request->params();
  for(int i=0;i<params;i++){
    AsyncWebParameter* p = request->getParam(i);
    if(p->isPost()){
        NVS.setString(p->name(), p->value());
    }
  }
  //-------------------------------------------------
  webPage += "<p>Setting Ethernet saved.</p>";
  webPage += "</body></html>";
  return webPage;
}

//-----------------------------------------------------------Форматирование файловой системы------------------------------------------------------------------- 
String clearflash(){
  String webPage ="";
  webPage += "<html>\
  <head>\
  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\
  <title>ESP8266 Settings</title>\
  </head>";
  webPage += "<body>"; 
      if(SPIFFS.format()){webPage += "File System is Formated</br>";
       Serial.println("File System is Formated");
      }
      else{webPage += "File System is NOT Formated</br>";
       Serial.println("File System is NOT Formated");
      }
  webPage += "<a href=\"/\">Return to settings page</a></br>";
  webPage += "</body></html>";
  return webPage;
  }