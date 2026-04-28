/*Codice principale*/

#include <stdio.h>

/*Contiene librerie standard della Pico SDK*/
#include "pico/stdlib.h"

#include "Joystick.h"
#include "ServoMotor.h"





int main() {

    /*Inizializza la comunicazione standard. Nel CMakeLists ho abilitato stdio 
    via USB per stampare sulla USB della Pico*/

    stdio_init_all();

    sleep_ms(2000);

    //Creazione oggetti con le varie classi

    Joystick joystick(26, 0);       //GP26 - ADC0
    ServoMotor servo (15);          //GP15 - PWM

    joystick.init();
    servo.init();

    //Loop principale

    while(true) {

        //Legge angolo dal joystick
        float angle = joystick.getAngle();

        //Muove il servo
        servo.setAngle(angle);

        //Debug
        printf("Angolo: %.2f\n", angle);

        sleep_ms(200);


    }
}