#pragma once
#include <cstdint>
/*

Classe Joystick
Inizializza ADC, legge valore analogico, lo converte in angolo

*/

class Joystick {

    private:
        int adc_channel; //Canale ADC (GP26 = 0)
        int gpio_pin;    //Pin fisico

    public:
        //Costruttore
        Joystick(int pin, int channel);

        //Inizializza ADC
        void init();

        //Legge Valore grezzo tra 0  e 4095, tra 0 e 3.3V
        uint16_t readRaw();

        //Converte in angolo (0-180)
        float getAngle();

};
