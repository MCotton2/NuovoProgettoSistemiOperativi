#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h" //Libreria per comunicazione I2C
#include "Qmc5883l.h" 

/*Test generale QMC5883L, versione precisa del magnetometro*/


//Pin scelti per I2C (GP4 = SDA, GP5 = SCL)

#define SDA_PIN 4
#define SCL_PIN 5


int main() {
    stdio_init_all();
    sleep_ms(2000);

    printf("Test Magnetometro avviato\n");

    //Inizializzo I2C a 100 kHz (standard e stabile)

    i2c_init(i2c0, 100*1000);

    //Dico alla Pico che GP4 funziona come SDA
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);

    //Dico alla Pico che GP5 funziona come SCL
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);

    //Attivo resistenze di pull-up interne, la linea i2c deve stare alta

    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    //Configuro il magnetometro

    Qmc5883l mag;

    mag.init();

    while(true) {

        int16_t x, y, z;

        mag.readRaw(x, y, z);

        printf("Mag -> X:%d, Y:%d, Z:%d\n", x, y, z);

        sleep_ms(500);

    }


}