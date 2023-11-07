#include <Arduino.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include "page.h"

#ifndef _IOTFUNCTIONS_H
#define _IOTFUNCTIONS_H
//-------------------------------------------------------------Получение температуры--------------------------------------------------------------------------------------
double GetTempFromTermistor(int termistor) {
  double Vout, Rt = 0;
  double T, Tc = 0;
  double adcMax = 4095.0; // ADC resolution 12-bit (0-4095)
  double Vs = 3.3;        // supply voltage
  double adc = 0;
  double R1 = 12000.0;   // voltage divider resistor value
  double Beta = 3950.0;  // Beta value
  double To = 298.15;    // Temperature in Kelvin for 25 degree Celsius
  double Ro = 10000.0;   // Resistance of Thermistor at 25 degree Celsius

  adc = termistor;
  Vout = adc * Vs/adcMax;
  Rt = R1 * Vout / (Vs - Vout);
  T = 1/(1/To + log(Rt/Ro)/Beta);
  Tc = T - 273.15; 
  return Tc;
  }

//-------------------------------------------------------------Приходящие данные с сервера MQTT---------------------------------------------------------------------------
void getData(char* topic, byte* payload, unsigned int length)
{ 
  payload[length] = '\0';
  String strTopic = String(topic);
  String strPayload = String((char*)payload);
  String data = "";
  bool flag = false;
  for(int i=1; i<strTopic.length(); i++){if(strTopic[i] == 47){flag =true;}
  else{if(flag && strTopic[i] != 47){data += strTopic[i];}}}
  
  Serial.print("#" + data + "=" + strPayload + ";");
}
 //---------------------------------------------------------------запись данных из json в ip\net\mask ---------------------------------------------------------------------
void writeNetworkSetting (const char* JsonData,byte* NetData) {
  
   String data = JsonData;
   String str ="";
   int count =0;

   for(int i = 0; i < data.length(); i++){
          if(data[i] != 46)str += data[i];
          else{
              NetData[count] = byte(str.toInt());
              count++;
              str="";
              }   
      }
   NetData[3] = byte(str.toInt());
}

//-----------------------------------------------------------Конвертация uint8_t в String---------------------------------------------------------------------------------
String getStringNetworAddress (uint8_t *addr){
  String res = "";
  for(int i = 0; i < sizeof(addr); i++){
    int act = (int) addr[i];
    res += (i == sizeof(addr)-1) ? String(act) : String(act) + "."; 
  }
  return res;
}

 // ---------------------------------------------------------Режим SoftAP--------------------------------------------------------------------------------------------------
void SoftAP_init(void){
        const char *ssid_ap = "ESPap";
        WiFi.mode(WIFI_AP);
        Serial.print("Configuring access point...");
        WiFi.softAP(ssid_ap);
        delay(2000);
        Serial.println("done");
        IPAddress myIP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(myIP);
     }

#endif // _IOTFUNCTIONS_H