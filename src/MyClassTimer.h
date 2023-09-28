
#include <Arduino.h>

class MyTimer{
    private:
        unsigned long timing = 0;
        unsigned long value = 1;
        bool timer_work = true;
        bool timer_initialization = true;

    public:
        MyTimer(unsigned long val = 1); //По умолчанию таймер на 1сек
        void setValueTime(unsigned long val); //Установить колличество секунд таймера
        bool startTimer(); //Запускаем таймер, пока идет отчет возвращает false, по завершению true
        void stopTimer(); //Сбрасываем таймер
};