#include "Qmc5883l.h"

#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include <cmath>
#include <stdio.h>

// Usiamo i2c0 della Raspberry Pi Pico
#define I2C_PORT i2c0

// ==========================
// Registri del magnetometro
// ==========================

// Registro da cui iniziano i dati X,Y,Z
#define QMC_DATA_REG     0x01

// Registro di stato del sensore
#define QMC_STATUS_REG   0x09

// Registro di controllo principale
#define QMC_CONTROL_REG  0x0A

// Registro configurazione/reset
#define QMC_CONFIG_REG   0x0B

// Registro "signature" richiesto da questa variante del chip
#define QMC_SIGN_REG     0x29


// ==========================
// Costruttore classe
// ==========================

Qmc5883l::Qmc5883l(uint8_t addr) {

    // Salvo indirizzo I2C del sensore
    address = addr;

    // Inizializzo valori minimi e massimi
    // usati nella calibrazione.
    // Partiamo da estremi molto grandi/piccoli
    // così il primo valore letto li sostituisce.

    x_min = 32767;
    x_max = -32768;

    y_min = 32767;
    y_max = -32768;
}


// ==========================
// Scrittura singolo registro
// ==========================

void Qmc5883l::writeRegister(uint8_t reg, uint8_t value) {

    // Buffer:
    // posizione 0 = registro
    // posizione 1 = valore da scrivere

    uint8_t buffer[2] = {reg, value};

    // Invio via I2C:
    // - porta I2C
    // - indirizzo sensore
    // - buffer
    // - lunghezza 2 byte
    // - false = stop alla fine

    i2c_write_blocking(
        I2C_PORT,
        address,
        buffer,
        2,
        false
    );
}


// ======================================
// Lettura di più registri consecutivi
// ======================================

void Qmc5883l::readRegisters(uint8_t start_reg,
                             uint8_t *buffer,
                             uint8_t length) {

    // Prima diciamo al sensore
    // da quale registro iniziare a leggere

    i2c_write_blocking(
        I2C_PORT,
        address,
        &start_reg,
        1,
        true
    );

    // Poi leggiamo i byte richiesti

    i2c_read_blocking(
        I2C_PORT,
        address,
        buffer,
        length,
        false
    );
}


// ==========================
// Inizializzazione sensore
// ==========================

void Qmc5883l::init() {

    /*
     * Questa versione del sensore trovata a 0x2C
     * richiede una inizializzazione diversa
     * rispetto al vecchio QMC5883L.
     */

    // Scrittura registro firma

    writeRegister(QMC_SIGN_REG, 0x06);

    sleep_ms(10);

    // Configurazione sensore

    writeRegister(QMC_CONFIG_REG, 0x08);

    sleep_ms(10);

    // Modalità continua
    // e attivazione misura

    writeRegister(QMC_CONTROL_REG, 0xC3);

    sleep_ms(20);
}


// ==========================
// Lettura dati grezzi
// ==========================

QmcData Qmc5883l::readRaw() {

    // Array che contiene:
    // X_LSB X_MSB
    // Y_LSB Y_MSB
    // Z_LSB Z_MSB

    uint8_t data[6];

    // Leggo 6 byte consecutivi

    readRegisters(QMC_DATA_REG, data, 6);

    QmcData result;

    // Ricostruisco valori a 16 bit
    // combinando byte basso e alto

    result.x = (int16_t)((data[1] << 8) | data[0]);

    result.y = (int16_t)((data[3] << 8) | data[2]);

    result.z = (int16_t)((data[5] << 8) | data[4]);

    return result;
}


// ==========================
// Heading NON calibrato
// ==========================

float Qmc5883l::getHeading() {

    // Leggo valori grezzi

    QmcData data = readRaw();

    /*
     * atan2(y,x)
     * calcola angolo del vettore magnetico
     * rispetto all'asse X.
     *
     * Il risultato è in radianti.
     */

    float heading = std::atan2(
        (float)data.y,
        (float)data.x
    );

    // Conversione radianti -> gradi

    heading = heading * 180.0f / 3.14159265f;

    /*
     * atan2 può restituire valori negativi.
     * Noi vogliamo un angolo 0-360.
     */

    if (heading < 0.0f) {
        heading += 360.0f;
    }

    return heading;
}


// ==========================
// Aggiornamento calibrazione
// ==========================

void Qmc5883l::updateCalibration() {

    // Leggo dati grezzi

    QmcData data = readRaw();

    // Aggiorno minimi e massimi
    // osservati durante il movimento

    if (data.x < x_min) {
        x_min = data.x;
    }

    if (data.x > x_max) {
        x_max = data.x;
    }

    if (data.y < y_min) {
        y_min = data.y;
    }

    if (data.y > y_max) {
        y_max = data.y;
    }
}


// ==========================
// Heading calibrato
// ==========================

float Qmc5883l::getCalibrationHeading() {

    // Leggo dati grezzi

    QmcData data = readRaw();

    /*
     * Calcolo offset.
     *
     * L'offset è il centro
     * tra minimo e massimo.
     */

    float x_offset =
        ((float)x_max + (float)x_min) / 2.0f;

    float y_offset =
        ((float)y_max + (float)y_min) / 2.0f;

    // Tolgo offset dai valori grezzi

    float x_corr =
        (float)data.x - x_offset;

    float y_corr =
        (float)data.y - y_offset;

    // Calcolo heading corretto

    float heading =
        std::atan2(y_corr, x_corr);

    // Radianti -> gradi

    heading =
        heading * 180.0f / 3.14159265f;

    // Normalizzo tra 0 e 360

    if (heading < 0.0f) {
        heading += 360.0f;
    }

    return heading;
}


// ==========================
// Stampa dati calibrazione
// ==========================

void Qmc5883l::printCalibration() {

    printf(
        "CALIB | Xmin:%d Xmax:%d | Ymin:%d Ymax:%d\n",
        x_min,
        x_max,
        y_min,
        y_max
    );
}