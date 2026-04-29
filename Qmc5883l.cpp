#include "Qmc5883l.h"
#include "hardware/i2c.h"
#include <cmath>
#include <stdio.h>


#define I2C_PORT i2c0

#define QMC_DATA_REG 0x00
#define QMC_CONTROL_REG 0x09



Qmc5883l::Qmc5883l(uint8_t addr) {

    address = addr;

    x_min = 32767;
    x_max = -32768;

    y_min = 32767;
    y_max = -32768;

};

//Scrive un registro del sensore

void Qmc5883l::writeRegister(uint8_t reg, uint8_t value) {

    uint8_t buffer[2] = {reg, value};
    i2c_write_blocking(I2C_PORT, address, buffer, 2 , false);

}

//Legge più registri consecutivi

void Qmc5883l::readRegisters(uint8_t start_reg, uint8_t *buffer, uint8_t length) {

    i2c_write_blocking(I2C_PORT, address, &start_reg, 1, true);
    i2c_read_blocking(I2C_PORT, address, buffer, length, false);


}

//Inizializza il sensore

void Qmc5883l::init() {

    // 0x09 registro del controllo
    // 0x1D Modo continuo
    writeRegister(QMC_CONTROL_REG, 0x1D);

}

//Lettura dati grezzi
QmcData Qmc5883l::readRaw() {

    uint8_t data[6];

    readRegisters(QMC_DATA_REG, data, 6);

    QmcData result;

    result.x = (int16_t) ((data[1] << 8) | data[0]);
    result.y = (int16_t) ((data[3] << 8) | data[2]);
    result.z = (int16_t) ((data[5] << 8) | data[4]);

    return result;

}

float Qmc5883l::getHeading() {

    

    //Leggo valori grezzi da magnetometro

    QmcData data = readRaw();

    //Uso atan2(y, x) per calcolare l'angolo del vettore magnetico sul piano x,y. Il risultato
    //è inizialmente in radianti

    float heading = std::atan2((float)data.y, (float)data.x);

    //Converto in angoli

    heading = heading * 180.0f / 3.14159265f;

    // atan2 può restituire valori negativi, noi vogliamo solo un angolo tra 0 e 360

    if(heading < 0.0f) {

        heading += 360.0f;

    }

    return heading;

}

void Qmc5883l::updateCalibration() {

    QmcData data = readRaw();

    //Aggiorno min/max

    if(data.x < x_min) {

        x_min = data.x;

    }

    if(data.x > x_max) {

        x_max = data.x;

    }

    if(data.y < y_min) {

        y_min = data.y;

    }
 
    if(data.y > y_max) {

        y_max = data.y;

    }
}

//Calcolo heading calibrato

float Qmc5883l::getCalibrationHeading() {

    QmcData data = readRaw();

    //Calcolo offset come differenza tra minimo e massimo
    float x_offset = ((float) x_max + (float) x_min) / 2.0f;

    float y_offset = ((float) y_max + (float) y_min) / 2.0f;

    //Tolgo l offset

    float x_corr = (float) data.x - x_offset;
    float y_corr = (float) data.y - y_offset;

    //Calcolo heading

    float heading = std::atan2(y_corr, x_corr);

    //Converto radianti in gradi

    heading = heading * 180.0f / 3.12159265;

    //Normalizzo tra 0 e 360

    if(heading < 0.0f) {

        heading += 360.0f;

    }

    return heading;

}

void Qmc5883l::printCalibration() {

    printf("CALIB | Xmin:%d Xmax:%d | Ymin:%d Ymax:%d\n", x_min, x_max, y_min, y_max);

}