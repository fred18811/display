#include <Arduino.h>

class TempSensor
{
    private:
        const double R1 = 12000.0;   // voltage divider resistor value
        const double To = 298.15;    // Temperature in Kelvin for 25 degree Celsius
        const double adcMax = 4095.0; // ADC resolution 12-bit (0-4095)
        double samples[10];
        double average;
        double Vout, Rt, T, Tc, adc = 0;
        double Vs = 3.3;        // supply voltage
        double Beta = 3950.0;  // Beta value
        double Ro = 10000.0;   // Resistance of Thermistor at 25 degree Celsius

        double GetTempFromTermistor(int termistor) {
            adc = termistor;
            Vout = Beta / adc -1;
            Vout = R1/Vout;
            Rt = Vout / Ro;
            Rt = log(Rt);
            Rt /= Beta;
            Rt += 1.0 /(25 + 273.15);
            Rt = 1.0/Rt;
            Rt -= 273.15;
            return Rt;
        }
    
    public:
        double GetAverageTemp(int temp){
            for (int i=0; i< 10; i++) {
                samples[i] = GetTempFromTermistor(temp);
            }
            average = 0;
            for (int i=0; i< 10; i++) {
            average += samples[i];
            }
            average = average/10;
            return average;

        }
};