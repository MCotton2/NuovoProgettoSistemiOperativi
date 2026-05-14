#include "LoRaReceiver.h"

#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/uart.h"

//UART0
//GP0 Tx pico -> Rx modulo
//GP1 Px pico -> Tx modulo

#define LORA_UART uart0

#define LORA_TX_PIN 0
#define LORA_RX_PIN 1

LoRaReceiver::LoRaReceiver(uint32_t baud) {

    baudrate = baud;

}

void LoRaReceiver::init() {

    //Inizializzazione uart

    uart_init(LORA_UART, baudrate);

    //Imposta una UART

    gpio_set_function(LORA_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(LORA_RX_PIN, GPIO_FUNC_UART);

    printf("LoRa E220 ricevitore inizializzato\n");

}

bool LoRaReceiver::readLine(char *buffer, int max_length) {

    static int index = 0;

    //Legge tutti i caratteri disponibili su UART

    while(uart_is_readable(LORA_UART)) {

        char c = uart_getc(LORA_UART);


        //Fine del messaggio, consideriamo \n come un pacchetto

        if(c == '\n') {

            buffer[index] = '\0';
            index = 0;
            return true;

        }

        //Salvo tutto nel buffer

        if(index < max_length - 1) {

            buffer[index] = c;
            index++;

        }


    }

    return false;

}