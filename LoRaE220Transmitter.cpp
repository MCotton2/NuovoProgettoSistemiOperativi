#include "LoRaE220Transmitter.h"

#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/uart.h"

//Usiamo Uart0
//GP0 = TX
//GP1 = RX

#define LORA_UART uart0
#define LORA_TX_PIN 0
#define LORA_RX_PIN 1

LoRaTransmitter::LoRaTransmitter(uint32_t baud) {

    baudrate = baud;

}

void LoRaTransmitter::init() {

    //Inizializza UART a velocità scelta

    uart_init(LORA_UART, baudrate);

    //Imposto GP0 e GP1 come pin UART

    gpio_set_function(LORA_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(LORA_RX_PIN, GPIO_FUNC_UART);

    printf("LoRa E220 trasmettitore inizializzato\n");

}

void LoRaTransmitter::sendMessage(const char *message) {

    //Invia stringa tramite uart al modulo ricevente

    uart_puts(LORA_UART, message);


}