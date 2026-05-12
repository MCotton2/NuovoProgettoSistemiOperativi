#include <stdio.h>
#include <math.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

#include "Bmp280.h"
#include "Qmc5883l.h"
#include "Joystick.h"
#include "LoRaE220Transmitter.h"

//=========== SD Card ====================

#include "SDLogger.h"

//=========================================


//=========== Configurazione I2C =================

#define I2C_PORT i2c0

#define SDA_PIN 4 //GP4 = SDA
#define SCL_PIN 5 //GP5 = SCL

//===================================================


//============ Joystick ==================

#define JOYSTICK_PIN 26
#define JOYSTICK_ADC_CHANNEL 0

//========================================

//=======================================

//============ Joystick ==================

#define JOYSTICK_PIN 26
#define JOYSTICK_ADC_CHANNEL 0

//========================================


//================ Main =============================

int main() {

    stdio_init_all();
    sleep_ms(5000);

    printf("============== AVVIO SISTEMA COMPLETO ===================\n");

    //Inizializzazione I2C

    i2c_init(I2C_PORT, 100*1000);

    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);

    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    printf("Inizializzazione completata di I2C\n");

    //Creazione oggetti

    Joystick joystick(JOYSTICK_PIN, JOYSTICK_ADC_CHANNEL);
    

    Qmc5883l magnetometer(0x2C);

    Bmp280 bmp(0x76);

    SDLogger logger;

    LoRaTransmitter lora;

    //Inizializzazione moduli

    joystick.init();
    printf("Inizializzato joystick\n");

    magnetometer.init();
    printf("Magnetometro inizializzato\n");

    bmp.init();
    printf("BMP/BME inizializzato\n");

    lora.init();
    printf("LoRa inizializzata\n");

    if(!logger.init()) {

        printf("Errore inizializzazione SD\n");

        while(true) {

            sleep_ms(1000);

        }

    }

    if(!logger.openFile("Flight_log.csv")) {

        printf("Errore apertura file csv\n");

        while(true) {

            sleep_ms(1000);

        }

    }

    //Header CSV

    logger.writeLine("Tempo ms, Servo Gradi, Mag X, Mag Y, Mag Z, Heading Raw, Heading calibrato, Temperatura C, Pressione Pa, Altitudine m\n");

    printf("Logger SD Pronto\n");

    printf("=========================== Sistema pronto ===============================\n");

    absolute_time_t last_log_time = get_absolute_time();

    float temperature = bmp.readTemperature();

    float pressure = bmp.readPressure();


    //Loop principale

    while(true) {

        float servo_angle = joystick.getAngle();

        //Sensori e logging ogni 500 ms

        if(absolute_time_diff_us(last_log_time, get_absolute_time()) > 500000) {

    printf("=========================== Sistema pronto ===============================\n");
            last_log_time = get_absolute_time();

            //Magnetometro

            QmcData mag = magnetometer.readRaw();

            magnetometer.updateCalibration();

            float heading_raw = magnetometer.getHeading();

            float heading_calibrated = magnetometer.getCalibrationHeading();

            //BMP/BME280

            temperature = bmp.readTemperature();

            pressure = bmp.readPressure();


            float altitude = bmp.readAltitude();


            //Tempo

            uint32_t time_ms = to_ms_since_boot(get_absolute_time());

            //Stampa

            printf(
                "Tempo:%lu ms | Servo: %.2f | Mag X:%d Y:%d Z:%d | Heading: %.2f | Heading Calibrato: %.2f | Temp: %.2f C | Press: %.2f Pa | Alt: %.2f m\n", 
                time_ms,
                servo_angle,
                mag.x,
                mag.y,
                mag.z,
                heading_raw,
                heading_calibrated,
                temperature,
                pressure,
                altitude
            );

            //Scrittura su CSV

            char line[256];

            snprintf(
                line,
                sizeof(line),
                "%lu, %.2f, %d, %d, %d, %.2f, %.2f, %.2f, %.2f, %.2f\n",
                time_ms,
                servo_angle,
                mag.x,
                mag.y,
                mag.z,
                heading_raw,
                heading_calibrated,
                temperature,
                pressure,
                altitude
            );

            logger.writeLine(line);
            lora.sendMessage(line);


        }

        sleep_ms(10);


    }

    return 0;

}