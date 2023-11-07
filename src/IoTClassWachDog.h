
#include <Arduino.h>

class MyWachDog{
    private:
        int pin;
        unsigned long timing;
        bool flag1 = true;
        bool flag2 = true;
    public:
        MyWachDog(int set_pin){
            pin = set_pin;
        }
        void loop(){
            if(flag1){
                digitalWrite(pin, HIGH);
                flag1 = false;
            }
            else if(!flag1 && flag2){
                digitalWrite(pin, LOW);
                flag2 = false;
            }

            if (millis() - timing > 30000){
                digitalWrite(pin, HIGH); 
            }
            if (millis() - timing > 34000){
                timing = millis(); 
                digitalWrite(pin, LOW);
            }
        }
        void start(){
            pinMode(pin, OUTPUT);
            digitalWrite(pin, LOW);
        }
};