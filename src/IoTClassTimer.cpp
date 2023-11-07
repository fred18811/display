#include <IoTClassTimer.h>

class IoTTimer;

IoTTimer::IoTTimer(unsigned long val)
{
    value = val;
}
void IoTTimer::setValueTime(unsigned long val){
    value = val;
}
bool IoTTimer::startTimer(){
    if(timer_initialization){
        timing = millis();
        timer_initialization = false;
    }
    if(timer_work)
    {
        if (millis() - timing > value*1000){
            timer_work = false;
            return false;
        }
        else return true;
    }
    else return false;
}
void IoTTimer::stopTimer(){
    timer_work = true;
    timer_initialization = true;
}

void IoTTimer::loop(void f()){
    if(startTimer()){}
    else{
        f();
        stopTimer();
    }
}
void IoTTimer::loop(void f(), unsigned int cnt){
    if(flag_count){
        count = cnt;
        flag_count = false;
    }
    if(count > 0){
        if(startTimer()){}
        else{
            f();
            stopTimer();
            count--;
        }
    }
}
void IoTTimer::loop(void f(), unsigned int cnt, unsigned long val){
    if(flag_count){
        count = cnt;
        flag_count = false;
    }
    if(flag_timer){
        value = val;
        flag_timer = false;
    }
    if(count > 0){
        if(startTimer()){}
        else{
            f();
            stopTimer();
            count--;
        }
    }
}