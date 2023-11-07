#include <Arduino.h>
#include <future>

class DisplayDwin{
    private:
        bool serialReadFlag = false;
        bool stringReadFlag=false;
        bool switshflag =false;
        HardwareSerial & _serial = Serial;

        String incStr = ""; 
        String data_param="";
        String data_value="";

        bool getDataUart();
        bool AnalyseString(String incStr);

    public:
        DisplayDwin(HardwareSerial & serial);
        bool loop(); //Отслеживаем данные из uart
        String getDataParam(); //Получение параметра
        String getDataValue(); //Получение значения
        void sendDataToDisplayDwinStr(String data,String val); // Отправка данных строковых
        void sendDataToDisplayDwinVal(String data,String val); // Отправка данных интовых
};