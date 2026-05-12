#include "Bmp280.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include <math.h>


//========== Porta i2c usata =================

#define I2C_PORT i2c0

//==========================================

//=========== Registri BMP/BME280 ================

#define REG_ID  0xD0
#define REG_CALIB   0x88
#define REG_CTRL_MEAS   0xF4
#define REG_CONFIG  0xF5
#define REG_PRESS_MSB   0xF7

//================================================

//============= Costruttore =====================

Bmp280::Bmp280(uint8_t addr) {

    //Salvo indirizzo i2c del sensore
    address = addr;

    

    //t_fine parte da 0, verrà poi aggiornato
    t_fine = 0;


}

//==============================================

//=================== Scrittura registro ==========================

void Bmp280::writeRegister(uint8_t reg, uint8_t value) {

    //Mando 2 byte per scrivere su registro

    uint8_t buffer[2] = {reg, value};

    i2c_write_blocking(I2C_PORT, address, buffer, 2, false);

}

//===================================================================


//==================== Lettura registro =============================

void Bmp280::readRegisters(uint8_t start_reg, uint8_t *buffer, uint8_t length) {

    //Mando prima il registro da cui voglio leggere, poi inizio a leggere

    i2c_write_blocking(I2C_PORT, address, &start_reg, 1, true);

    i2c_read_blocking(I2C_PORT, address, buffer, length, false);


}

//====================================================================

//================== Lettura chip_id =================================

uint8_t Bmp280::readChipId() {

    uint8_t chip_id;

    readRegisters(REG_ID, &chip_id, 1);

    return chip_id;

}

//====================================================================

//================= Lettura calibrazione ================================

void Bmp280::readCalibrationData() {

    //Servono 24 byte

    uint8_t calib[24];

    readRegisters(REG_CALIB, calib, 24);

    //Ricostruisco un valore a 16 bit (byte_alto << 8) | byte_basso

    dig_T1 = (uint16_t) ((calib[1] << 8) | calib[0]);
    dig_T2 = (int16_t) ((calib[3] << 8) | calib[2]);
    dig_T3 = (int16_t) ((calib[5] << 8) | calib[4]);

    dig_P1 = (uint16_t) ((calib[7] << 8) | calib[6]);
    dig_P2 = (int16_t) ((calib[9] << 8) | calib[8]);
    dig_P3 = (int16_t) ((calib[11] << 8) | calib[10]);
    dig_P4 = (int16_t) ((calib[13] << 8) | calib[12]);
    dig_P5 = (int16_t) ((calib[15] << 8) | calib[14]);
    dig_P6 = (int16_t) ((calib[17] << 8) | calib[16]);
    dig_P7 = (int16_t) ((calib[19] << 8) | calib[18]);
    dig_P8 = (int16_t) ((calib[21] << 8) | calib[20]);
    dig_P9 = (int16_t) ((calib[23] << 8) | calib[22]);

}

//==========================================================================

//================ Inizializzazionw sensore ====================0

void Bmp280::init() {

    uint8_t chip_id = readChipId();

    printf("BMP/BME Chip ID: 0x%02X\n", chip_id);

    if(chip_id == 0x58) {

        printf("Sensore rilevatop: BMP280\n");

    } else if (chip_id == 0x60) {

        printf("Sensore rilevato: BME280\n");

    } else {

        printf("Attenzione: chip ID non riconosciuto\n");

    }

    //Leggiamo coefficienti di calibrazione

    readCalibrationData();

    writeRegister(REG_CTRL_MEAS, 0x27);

    writeRegister(REG_CONFIG, 0xA0);

    sleep_ms(1000);


    

}

//===============================================================

//================= Lettura dati grezzi =========================

void Bmp280::readRawData(int32_t *adc_T, int32_t *adc_P) {

    uint8_t data[6];

    readRegisters(REG_PRESS_MSB, data, 6);

    *adc_P = ((int32_t)data[0]<<12 | (int32_t) data[1] << 4 | (int32_t) data[2] >> 4);

    *adc_T = ((int32_t)data[3] << 12 | (int32_t) data[4] << 4 | (int32_t) data[5] >> 4);


}

//==================================================================0

//================= Compensazione temperatura =============================

float Bmp280::compensateTemperature(int32_t adc_T) {

    int32_t var1;
    int32_t var2;

    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t) dig_T2)) >> 11;

    var2 = (((((adc_T >> 4) - ((int32_t) dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;

    t_fine = var1 + var2;

    float temperature = (t_fine * 5 + 128) >> 8;

    return temperature / 100.0f;



}

//==============================================================================

//===================== Compenazione Pressione ================================

float Bmp280::compensatePressure(uint32_t adc_P) {

    int64_t var1;
    int64_t var2;
    int64_t p;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t) dig_P6;
    var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
    var2 = var2 + (((int64_t) dig_P4) << 35);

    // REG_CTRL_MEAS
    // bits 7-5 = oversampling temperatura
    // bits 4-2 = oversampling pressione
    //bits 1-0 = modalità

    writeRegister(REG_CTRL_MEAS, 0x27);

    //REG_CONFIG
    //0xA0 = tempo standby stabile, filtro spento o leggero


    writeRegister(REG_CONFIG, 0xA0);
    var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) +((var1 * (int64_t) dig_P2) << 12);

    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t) dig_P1) >> 33;

    if(var1 == 0) {

        return 0;
    
    }

    p = 1048576 - adc_P;

    p = (((p << 31) - var2) * 3125) / var1;

    var1 = (((int64_t)dig_P9) * (p >> 13) * (p >>13)) >> 25;

    var2 = (((int64_t)dig_P8) * p) >> 19;

    p = ((p +  var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);

    //Pressione in Pascal
    return (float) p / 256.0f;

}

//=======================================================================

//============= Lettura temperatura ======================0

float Bmp280::readTemperature() {

    int32_t adc_T;
    int32_t adc_P;

    readRawData(&adc_T, &adc_P);

    return compensateTemperature(adc_T);


}

//=============================================================

//============== Lettura Pressione ==========================

float Bmp280::readPressure() {

    int32_t adc_T;
    int32_t adc_P;

    readRawData(&adc_T, &adc_P);

    //Leggiamo entrambi perché per la pressione serve ad aggiorna t_fine tramite temperatura
    compensateTemperature(adc_T);

    return compensatePressure(adc_P);


}

//=========================================================

//================ Lettura altezza ===============================

float Bmp280::readAltitude() {

    float pressure = readPressure();

    return 44330.0f * (1.0f - powf(pressure / 100800.0f, 0.1903f));



}

//===================================================================

void Bmp280::setSeaLevelPressure(float pressure) {

    //Permette di cambiare la pressione di riferimento. La useremo per settare l'altezza relativa.

    sea_level_pressure = pressure;

}