#pragma once

#include <stdint.h>

//Classe LoRa Transmitter, gestsce il sensore tramite UART

class LoRaTransmitter {

    private:
        uint32_t baudrate;

    public: 
        LoRaTransmitter(uint32_t baud = 9600);

        void init();

        void sendMessage(const char *message);


};

