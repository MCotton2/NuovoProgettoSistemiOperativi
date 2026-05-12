#include <stdio.h>

#include "pico/stdlib.h"
#include "LoRaReceiver.h"
#include "ServoMotor.h"

#define SERVO_PIN 15

int main() {

    stdio_init_all();
    sleep_ms(3000);

    printf("======== Pico Ricevente E200 =========");

    LoRaReceiver receiver;

    receiver.init();

    ServoMotor servo(SERVO_PIN);

    servo.init();

    char message[256];

    while(true) {

        if(receiver.readLine(message, sizeof(message))) {

            printf("Ricevuto: %s\n", message);

        }

        float servo_angle;

        sscanf(
            message,
            "%*u,%f",
            &servo_angle
        );

        servo.setAngle(servo_angle);

        sleep_ms(10);

    }

    return 0;

}