

//================ Librerie Standard ==========================

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h" //Libreria per comunicazione I2C

//=============================================================


// ======================= Sensori ============================

#include "Qmc5883l.h" 
#include "ServoMotor.h"
#include "Joystick.h"

// =============================================================


//================== Pin scelti per I2C (GP4 = SDA, GP5 = SCL) =======================

#define SDA_PIN 4       //GP4 = SDA
#define SCL_PIN 5       //GP5 = SCL

//=====================================================================================

//=================== Pin per il Joystick ============================

#define JOYSTICK_PIN 26                     //Pin 26
#define JOYSTICK_ADC_CHANNEL 0              //Canale 0

//====================================================================


//================== Pin per il servo ===============================

#define SERVO_PIN 15            //GP15 usato come PWM

//===================================================================



int main() {

    // ============== Inizio comunicazione seriale con Pico ===================

    stdio_init_all();
    sleep_ms(2000);

    printf("Avvio Sistema...\n");

    //=========================================================================

    

    //======================= Inizializzo I2C a 100 kHz (standard e stabile) =========================

    i2c_init(i2c0, 100*1000);   

    //Dico alla Pico che GP4 funziona come SDA
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);

    //Dico alla Pico che GP5 funziona come SCL
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);

    //Attivo resistenze di pull-up interne, la linea i2c deve stare alta

    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    //==================================================================================================

    //========================== Creazione Oggetti ===================================

    //Creazione Joystick

    Joystick joystick(JOYSTICK_PIN, JOYSTICK_ADC_CHANNEL);

    //Servo

    ServoMotor servo(SERVO_PIN);

    //Magnetometro

    Qmc5883l magnetometer;

    //==================================================================================

    //========================= Inizializzazione Moduli ================================

    //Inizializzazione ADC del joystick

    joystick.init();

    //Inizializzazione PWM del servo

    servo.init();

    //Inizializzazione del magnetometro

    magnetometer.init();

    //===================================================================================

    printf("Sistema inizializzato correttamente\n");

    //==================== LOOP PRINCIPALE =======================================

    while(true) {

        //Legge joystick e riceve un angolo tra 0 e 360

        float servo_angle = joystick.getAngle();

        //Manda l'angolo al servo

        servo.setAngle(servo_angle);

        //=============== Lettura Magnetometro =======================

        //Leggo valori grezzi

        QmcData mag = magnetometer.readRaw();

        //Aggiorno valori min/max per la calibrazione. RUOTARE il sensore durante i primi secondi

        magnetometer.updateCalibration();

        //Calcolo heading non calibrato

        float heading_raw = magnetometer.getHeading();

        //Calcolo heading calibrato usando i min/max

        float heading_calibrated = magnetometer.getCalibrationHeading();

        //============================================================

        //==================== STAMPA DEBUG ==================================0

        printf("Servo: %.2f gradi | Magnetometro X:%d Y:%d Z:%d | Heading raw: %.2f gradi | Heading calibrato: %.2f gardi\n",servo_angle, mag.x, mag.y, mag.z, heading_raw, heading_calibrated);

        //Stampo i valori di calibrazione

        magnetometer.printCalibration();

        sleep_ms(300);

    }


}