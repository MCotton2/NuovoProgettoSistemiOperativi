// Test BMP/BME280

#include <stdio.h>
#include<math.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

#include "Bmp280.h"
#include "Qmc5883l.h"
#include "Joystick.h"
#include "ServoMotor.h"


//=========== Configurazione I2C =================

#define I2C_PORT i2c0

#define SDA_PIN 4 //GP4 = SDA
#define SCL_PIN 5 //GP5 = SCL

//===================================================

//============ Servo ====================

#define SERVO_PIN 15
#define PWM_WRAP 20000

//=======================================

//============ Joystick ==================

#define JOYSTICK_PIN 26
#define JOYSTICK_ADC_CHANNEL 0

//========================================

//================ Main =============================

int main() {

    stdio_init_all();
    sleep_ms(5000);

    printf("======= Avvio Sistema =========\n");

    //Inizializzazione i2c

    i2c_init(I2C_PORT, 400000);

    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);

    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    printf("I2C inizializzato\n");

    //Inizializzazione Joystick

    
    //Creazione oggetti

    Joystick joystick(JOYSTICK_PIN, JOYSTICK_ADC_CHANNEL);

    ServoMotor servo(SERVO_PIN);

    joystick.init();

    printf("Joystick inizializzato\n");

    servo.init();

    printf("Servo inizializzato\n");

    //Inizializzazione magnetometro

    Qmc5883l magnetometer(0x2C);

    magnetometer.init();

    printf("Magnetometro inizializzato\n");

    //Inizializzazione BMP280

    Bmp280 bmp(0x76);

    bmp.init();

    printf("Inizializzazione BMP completata\n");

    //Ciclo principale

    while(true) {

        //Joystick -> Servo

        float servo_angle = joystick.getAngle();

        servo.setAngle(servo_angle);

        //Magnetometro

        magnetometer.updateCalibration();

        QmcData mag = magnetometer.readRaw();

        float heading = magnetometer.getCalibrationHeading();

        //BMP280

        float temperature = bmp.readTemperature();

        float pressure = bmp.readPressure();

        float altitude = bmp.readAltitude();

        //Stampa

        printf("Servo: %.2f gradi\n Mag X:%d Y:%d Z:%d\n Heading: %.2f gradi\n Temperatura: %.2f C\n Pressione: %.2f Pa\n Altitudine: %.2f m\n", servo_angle, mag.x, mag.y, mag.z, heading, temperature, pressure, altitude);
        
        magnetometer.printCalibration();
        printf("\n");
        sleep_ms(100);


    }

}
