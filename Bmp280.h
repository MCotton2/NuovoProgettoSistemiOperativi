#pragma once
#include <stdint.h>

//Classe BMP280

class Bmp280 {

    private:
        //Indirizzo i2c del sensore
        uint8_t address;

        //Coefficienti di calibrazione di temperatura usciti dal sensore
        uint16_t dig_T1;
        int16_t dig_T2;
        int16_t dig_T3;

        //Coefficienti di calibrazione pressione
        uint16_t dig_P1;
        int16_t dig_P2;
        int16_t dig_P3;
        int16_t dig_P4;
        int16_t dig_P5;
        int16_t dig_P6;
        int16_t dig_P7;
        int16_t dig_P8;
        int16_t dig_P9;

        //Variabile interna Bosch, serve per calcolare correttamente la pressione
        int32_t t_fine;

        //Pressione di riferimento. La settiamo inizialmente standard al mare

        float sea_level_pressure;

        //Scrittura dentro un registro

        void writeRegister(uint8_t reg, uint8_t value);

        //Legge più registri consecutivi dal BMP

        void readRegisters(uint8_t start_reg, uint8_t *buffer, uint8_t length);

        //Legge valori di calibrazioni interni al sensore

        void readCalibrationData();

        //Legge valori grezzi di ADC di temperatura

        void readRawData(int32_t *adc_T, int32_t *adc_P);

        //Converte temperatura grezza in gradi

        float compensateTemperature(int32_t adc_T);

        //Converte pressione grezza in Pascal

        float compensatePressure(uint32_t adc_P);

    public:

        //Costruttore

        Bmp280(uint8_t addr = 0x76);

        //Inizializzo sensore

        void init();

        //Leggo temperatura in gradi celsius

        float readTemperature();

        //Leggo pressione in Pascal

        float readPressure();

        //Calcola altitudine arrotondata in metri

        float readAltitude();

        //Cambio altitudine di riferimento

        void setSeaLevelPressure(float pressure);

        //Leggo e stampo chip

        uint8_t readChipId();


};