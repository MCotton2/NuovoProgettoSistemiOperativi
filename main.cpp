#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h" //Libreria per comunicazione I2C

/*Test generale QMC5883L, versione precisa del magnetometro*/


//Uso il controller i2c0 della Pico
#define I2C_PORT i2c0

//Pin scelti per I2C (GP4 = SDA, GP5 = SCL)

#define SDA_PIN 4
#define SCL_PIN 5

// Indirizzo I2C del sensore, trovato precedentemente tramite scan 

#define QMC5883L_ADDR 0x0D

//Nel sensore i dati magnetici partono dal registro 0x00:

#define QMC5883L_DATA_REG 0x00

//Registro di controllo principale, server per configurare modalità di lavoro, frequenza di aggiornamento
//fondo scala e oversampling

#define QMC5883L_CONTROL_REG 0x09

//Funzione write_register scrive un valore dentro un registro del sensore. Usa come parametri reg, ovvero
// il registro del sensore, e value, il valore da scrivere nel registro

void write_register(uint8_t reg, uint8_t value) {

    //Creo un array di 2 byte, il primo serve per il registro da modificare, il secondo contiene il valore
    // da scrivere in quel registro

    uint8_t buffer[2] = {reg, value};


    //Invio i 2 byte al sensore tramite funzione di i2c, dove mandiamo porta su cui scrivere, indirizzo del 
    // sensore, dati da inviare, numero di byte da inviare, e false per chiudere la comunicazione dopo la
    // scrittura
    i2c_write_blocking(I2C_PORT, QMC5883L_ADDR, buffer, 2, false);

}


//Leggo più registri consecutivi dal sensore. Passo il primo registro da leggere, il buffer dove salvare i
// dati letti, e il numero di byte da leggere
void read_register(uint8_t start_reg, uint8_t *buffer, uint8_t length) {

    //Dico al sensore da quale registro voglio iniziare a leggere. True significa "non chiudere comunicazione 
    // perché subito dopo rifarò un altra lettura"

    i2c_write_blocking(I2C_PORT, QMC5883L_ADDR, &start_reg, 1, true);

    //Ora leggiamo length byte dal sensore. I dati finiscono in buffer

    i2c_read_blocking(I2C_PORT, QMC5883L_ADDR, buffer, length, false);

}

//Configurazione sensore per iniziare a misurare

void init_qmc5883l() {

    //Scrivo nel registro 0x09 il valore 0x1D, ovvero modalità continua, output data rate alto, 
    //fondo scala 8G e oversampling 512. In sostanza il sensore continua a misurare e leggiamo quando vogliamo
    write_register(QMC5883L_CONTROL_REG, 0x1D);

}


int main() {
    stdio_init_all();
    sleep_ms(2000);

    printf("Test Magnetometro avviato\n");

    //Inizializzo I2C a 100 kHz (standard e stabile)

    i2c_init(I2C_PORT, 100*1000);

    //Dico alla Pico che GP4 funziona come SDA
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);

    //Dico alla Pico che GP5 funziona come SCL
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);

    //Attivo resistenze di pull-up interne, la linea i2c deve stare alta

    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    //Configuro il magnetometro

    init_qmc5883l();

    while(true) {

        //Leggiamo continuamente il sensore e stampiamo i valori
        uint8_t data[6];

        //Leggiamo partendo da 0x00

        read_register(QMC5883L_DATA_REG, data, 6);

        //Ricostruiamo i valori a 16 bit

        int16_t x = (int16_t) ((data[1] << 8) | data[0]);
        int16_t y = (int16_t) ((data[3] << 8) | data[2]);
        int16_t z = (int16_t) ((data[5] << 8) | data[4]);

        //Stampiamo valori grezzi
        printf("Mag raw -> X: %d | Y: %d | Z: %d\n", x, y, z);

        sleep_ms(500);

    }

}