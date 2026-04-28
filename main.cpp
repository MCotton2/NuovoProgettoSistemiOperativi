/*Codice principale*/

#include <stdio.h>

/*Contiene librerie standard della Pico SDK*/
#include "pico/stdlib.h"

/*Libreria hardware del Pico SDK, serve per implementare il convertitore Analogico-Digitale.*/
#include "hardware/adc.h"

/*Libreria  per segnale PWM*/

#include "hardware/pwm.h"

#define SERVO_PIN 15



int main() {

    /*Inizializza la comunicazione standard. Nel CMakeLists ho abilitato stdio 
    via USB per stampare sulla USB della Pico*/

    stdio_init_all();

    // ================== ADC PER JOYSTICK ====================== 

    /*Inizializzo il blocco interno adc della Pico, così che possa leggere valori analogici.*/

    adc_init();

    /*Abilito il pin GP26 (31) come ingresso analogico, collegandoci la lettura su asse X del joystick*/

    adc_gpio_init(26);

    /*Seleziono il canale 0 (ADC0)*/

    adc_select_input(0);

    // =============================================================

    // ================ PWM SERVO ===============================

    //Imposto il pin del servo come PWM
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);

    //Prendo quale slice PWM è associato al pin

    uint slice = pwm_gpio_to_slice_num(SERVO_PIN);

    // Ottengo canale PWM (A o B)
    uint channel = pwm_gpio_to_channel(SERVO_PIN);

    //Configurazione base del PWM
    pwm_config config = pwm_get_default_config();

    /* Il servo lavora circa a 50 Hz, un periodo di circa 20 millisecondi, rallento il clock PWM*/
    pwm_config_set_clkdiv(&config, 64.0f);

    //Imposto il valore massimo del contatore PWM per la risoluzione del segnale
    pwm_config_set_wrap(&config, 39062);

    //Applico configurazione e attivo PWM
    pwm_init(slice, &config, true);

    // =================================================================



    while(true) {




        // ============== Leggo Joystick ======================

        uint16_t value = adc_read();

        printf("Joystick raw value = %u\n", value);

        // ================== Converto in angolo ========================

        float angle = ((float) value / 4095.0f) * 180.0f;

        // =================== Converto angolo in impulso PWM =============

        float min_us = 500.0f;
        float max_us = 2500.0f;

        /* Prendo l'angolo e lo trasformo nella durata dell impulso*/

        float pulse = min_us + (angle / 180.0f) * (max_us -min_us);

        // ==================== Converto impulso in PWM ================

        uint16_t level = (uint16_t)((pulse / 20000.0f /*periodo di 20 ms*/) * 39062);

        // ==================== Applico PWM al servo ======================

        pwm_set_chan_level(slice, channel, level);

        // ================== Random Debug ================
        printf("raw=%u angolo=%.2f\n", value, angle);


        sleep_ms(200);

    }

    return 0;

}