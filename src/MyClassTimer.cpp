#include <MyClassTimer.h>

class MyTimer;

MyTimer::MyTimer(unsigned long val)
{
    value = val;
}
void MyTimer::setValueTime(unsigned long val){
    value = val;
}
bool MyTimer::startTimer(){
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
void MyTimer::stopTimer(){
            timer_work = true;
            timer_initialization = true;
        }