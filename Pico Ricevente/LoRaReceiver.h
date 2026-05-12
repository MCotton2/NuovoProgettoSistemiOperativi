#pragma once

#include <stdint.h>

//Classe LoRaReceiver
//Gestisce dati ricevuti dal sensore tramite UART

class LoRaReceiver {

    private:
        uint32_t baudrate;

    public:

        LoRaReceiver(uint32_t baud = 9600);

        void init();

        bool readLine(char *buffer, int max_length);

};