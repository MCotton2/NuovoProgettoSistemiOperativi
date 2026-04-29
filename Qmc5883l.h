#pragma once 

#include <stdint.h> 

//Classe per Qmc5883l

class Qmc5883l {

    private:
        uint8_t address;

        void writeRegister(uint8_t reg, uint8_t value);
        void readRegisters(uint8_t reg, uint8_t *buffer, uint8_t len);

    public:
        Qmc5883l(uint8_t addr = 0x0D);

        void init();

        void readRaw(int16_t &x, int16_t &y, int16_t &z);

};

