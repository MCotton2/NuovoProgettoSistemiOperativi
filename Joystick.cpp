 #include "Joystick.h"
 #include "hardware/adc.h"
 

 /*
 Costruttore: salva pin e canale ADC
 
 */

 Joystick::Joystick(int pin, int channel) {

    gpio_pin = pin;
    adc_channel = channel;

 }

 /*Inizializza ADC: attiva il modulo ADC e abilita il PIN*/

 void Joystick::init() {

    adc_init();                         //attiva ADC Globale
    adc_gpio_init(gpio_pin);            //Imposta pin come analogico
    adc_select_input(adc_channel);      //Seleziona il canale

 }

 /*Legge il valore analogico, restituisce 0-4095*/

 uint16_t Joystick::readRaw() {

    return adc_read();

 }

 /*Converte valore in angolo*/

 float Joystick::getAngle() {

    uint16_t raw = readRaw();

    //Mapping lineare
    return ((float) raw / 4095.0f) * 180.0f;

 }