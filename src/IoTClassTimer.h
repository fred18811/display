
#include <Arduino.h>

#ifndef _IOTCLASSTIMER_H
#define _IOTCLASSTIMER_H

class IoTTimer {
    private:
        unsigned long timing = 0;
        unsigned long value = 1;
        unsigned int count = 0;
        bool timer_work = true;
        bool timer_initialization = true;
        bool flag_count = true;
        bool flag_timer = true;

    public:
        IoTTimer(unsigned long val = 1); //По умолчанию таймер на 1сек
        void setValueTime(unsigned long val); //Установить колличество секунд таймера
        bool startTimer(); //Запускаем таймер, пока идет отчет возвращает false, по завершению true
        void stopTimer(); //Сбрасываем таймер
        void loop(void f()); //вызываем функцию по истечению времени и сбрасываем таймер
        void loop(void f(), unsigned int cnt); //вызываем функцию по истечению времени и сбрасываем таймер, выставляем число срабатываний
        void loop(void f(), unsigned int cnt, unsigned long val); //вызываем функцию по истечению времени и сбрасываем таймер, выставляем число срабатываний, выставляем время
};

#endif // _IOTCLASSTIMER_H