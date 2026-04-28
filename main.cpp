/*Codice principale*/

#include <stdio.h>

/*Contiene librerie standard della Pico SDK*/
#include "pico/stdlib.h"

/*Libreria hardware del Pico SDK, serve per implementare il convertitore Analogico-Digitale.*/
#include "hardware/adc.h"

int main() {

    /*Inizializza la comunicazione standard. Nel CMakeLists ho abilitato stdio 
    via USB per stampare sulla USB della Pico*/

    stdio_init_all();
    sleep_ms(2000);

    /*Inizializzo il blocco interno adc della Pico, così che possa leggere valori analogici.*/

    adc_init();

    /*Abilito il pin GP26 (31) come ingresso analogico, collegandoci la lettura su asse X del joystick*/

    adc_gpio_init(26);

    /*Seleziono il canale 0 (ADC0)*/

    adc_select_input(0);

    while(true) {


        /*Leggo il valore analogico del canale ADC (va da 0 a 4095-3.3V)*/

        uint16_t value = adc_read();

        printf("Joystick raw value = %u\n", value);

        sleep_ms(200);

    }

    return 0;

}