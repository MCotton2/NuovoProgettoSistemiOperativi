#include <stdio.h>

#include "pico/stdlib.h"
#include "LoRaReceiver.h"
#include "ServoMotor.h"

#define SERVO_PIN 15

int main() {

    stdio_init_all();
    sleep_ms(3000);

    printf("======== Pico Ricevente E200 =========\n");

    LoRaReceiver receiver;

    receiver.init();

    ServoMotor servo(SERVO_PIN);

    servo.init();

    char message[256];

    while(true) {

        if(receiver.readLine(message, sizeof(message))) {

            printf("\n ================================\n\n");
            printf("Valori: Tempo s | Servo | Mag X Y Z | Heading | Heading Calibrato | Temp C | Press Pa | Alt m\n");
            printf("Ricevuto: %s\n", message);
            printf("\n ================================\n");

        }

    

        float servo_angle;

        sscanf(
            message,
            "%*f,%f",
            &servo_angle
        );

        servo.setAngle(servo_angle);

        sleep_ms(10);

    }

    return 0;

}