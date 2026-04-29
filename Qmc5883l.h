#pragma once 

#include <stdint.h> 

//Struttura per lettura magnetometro

struct QmcData {

    int16_t x;
    int16_t y;
    int16_t z;

};

//Classe per Qmc5883l

class Qmc5883l {

    private:
        uint8_t address;

        // Valori minimi e massimi osservati durante la calibrazione. Stimano l'offset del sensore

        int16_t x_min;
        int16_t x_max;
        int16_t y_min;
        int16_t y_max;

        void writeRegister(uint8_t reg, uint8_t value);
        void readRegisters(uint8_t start_reg, uint8_t *buffer, uint8_t length);

    public:
        Qmc5883l(uint8_t addr = 0x0D);

        void init();

        QmcData readRaw();

        float getHeading();

        void updateCalibration();

        float getCalibrationHeading();

        void printCalibration();

};

