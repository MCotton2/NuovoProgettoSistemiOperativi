#include <stdio.h>

#include "pico/stdlib.h"
#include "LoRaReceiver.h"

int main() {

    stdio_init_all();
    sleep_ms(3000);

    printf("======== Pico Ricevente E200 =========");

    LoRaReceiver receiver;

    receiver.init();

    char message[256];

    while(true) {

        if(receiver.readLine(message, sizeof(message))) {

            printf("Ricevuto: %s\n", message);

        }

        sleep_ms(10);

    }

    return 0;

}