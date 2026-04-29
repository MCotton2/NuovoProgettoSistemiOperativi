#include "Qmc5883l.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0

Qmc5883l::Qmc5883l(uint8_t addr) {

    address = addr;

}

//Scrive un registro del sensore

void Qmc5883l::writeRegister(uint8_t reg, uint8_t value) {

    uint8_t buffer[2] = {reg, value};
    i2c_write_blocking(I2C_PORT, address, buffer, 2 , false);

}

//Legge più registri consecutivi

void Qmc5883l::readRegisters(uint8_t reg, uint8_t *buffer, uint8_t len) {

    i2c_write_blocking(I2C_PORT, address, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, address, buffer, len, false);


}

//Inizializza il sensore

void Qmc5883l::init() {

    // 0x09 registro del controllo
    // 0x1D Modo continuo
    writeRegister(0x09, 0x1D);

}

//Lettura dati grezzi
void Qmc5883l::readRaw(int16_t &x, int16_t &y, int16_t &z) {

    uint8_t data[6];

    readRegisters(0x00, data, 6);

    x = (int16_t) ((data[1] << 8) | data[0]);
    y = (int16_t) ((data[3] << 8 | data[2]));
    z = (int16_t) ((data[5] << 8 | data[4]));

}