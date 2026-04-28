#pragma once

#include <cstdint>
/*

Classe ServoMotor 

Inizializza PWM, converte angolo in segnale e muove il servo

*/


class ServoMotor {

    private:
        uint32_t pin;
        uint32_t slice;
        uint32_t channel;

    public:
        ServoMotor(uint32_t gpio);

        void init();

        void setAngle(float angle);


};