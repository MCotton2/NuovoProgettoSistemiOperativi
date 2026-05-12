#include "ServoMotor.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"

/*

    Costruttore

*/

ServoMotor::ServoMotor(uint32_t gpio) {

    pin = gpio;
    slice = 0;
    channel = 0;

}


/*

    Inizializzazione PWM

*/

void ServoMotor::init() {

    //Imposto Pin come PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);

    //Ottiene slice e canale
    slice = pwm_gpio_to_slice_num(pin);

    //Ogni slice ha due canali
    channel = pwm_gpio_to_channel(pin);

    //Config base
    pwm_config config = pwm_get_default_config();

    //Riduciamo frequenza per ottenere 50 Hz
    pwm_config_set_clkdiv(&config, 64.0f);

    //Wrap = Periodo
    pwm_config_set_wrap(&config, 39062);

    //Attiva PWM
    pwm_init(slice, &config, true);

}

void ServoMotor::setAngle(float angle) {

    if(angle < 0) angle = 0;
    if(angle > 180) angle = 180;

    /*
    
    Conversione angoli in microsecondi

    */

    float min_us = 500.0f;
    float max_us = 2500.0f;

    float pulse = min_us + (angle / 180.0f ) * (max_us - min_us);

    //Conversione in livello PWM

    uint16_t level = (uint16_t) ((pulse / 20000.0f) * 39062);

    pwm_set_chan_level(slice, channel, level);

}