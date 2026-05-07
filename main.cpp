// Test BMP/BME280

#include <stdio.h>
#include<math.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

//=========== Configurazione I2C =================
#define I2C_PORT i2c0

#define SDA_PIN 4 //GP4 = SDA
#define SCL_PIN 5 //GP5 = SCL

#define BMP280_ADDRESS 0x76 //Indirizzo del sensore

//===================================================

//========== REGISTRI BMP/BME280 ====================

#define REG_ID 0xD0 //Registro identificativo del chip

#define REG_RESET 0x0E //Registro reset

#define REG_CALIB 0x88 //Registro calibrazione temperatura/pressione

#define REG_CTRL_MEAS 0xF4 //Registro controllo misura

#define  REG_CONFIG 0xF5 //Registro configurazione

#define REG_PRESS_MSB 0xF7 //Registro dati pressione/temperatura

//==================================================================

//============= Variabili di calibrazione ===========================

uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;

uint16_t dig_P1;
int16_t dig_P2;
int16_t dig_P3;
int16_t dig_P4;
int16_t dig_P5;
int16_t dig_P6;
int16_t dig_P7;
int16_t dig_P8;
int16_t dig_P9;

//========================================================================


//============== Variabile interna per compensare pressione =============0

int32_t t_fine;

//========================================================================

//=============== Pressione di riferimento al livello del mare ==========

float sea_level_pressure = 101325.0f;

//=======================================================================

//=============== Funzioni I2C base ========================

void writeRegister(uint8_t reg, uint8_t value) {

    //Primo byte = registro
    //Secondo byte = valore da scrivere

    uint8_t buffer[2] = {reg, value};

    i2c_write_blocking(I2C_PORT, BMP280_ADDRESS, buffer, 2, false);

}

void readRegisters(uint8_t start_reg, uint8_t *buffer, uint8_t length) {

    //Dico al sensore dove leggere
    i2c_write_blocking(I2C_PORT, BMP280_ADDRESS, &start_reg, 1, true);

    //Leggo length byte consecutivi
    i2c_read_blocking(I2C_PORT, BMP280_ADDRESS, buffer, length, false);

}

//============================================================0

//================ Lettura calibrazione ======================

void readCalibrationData() {

    uint8_t calib[24];
    
    //Leggo 24 byte da 0x88

    readRegisters(REG_CALIB, calib, 24);

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

// =============================================================00

//================ Inizializzazionw sensore ====================0

void bmp280Init() {


    // REG_CTRL_MEAS
    // bits 7-5 = oversampling temperatura
    // bits 4-2 = oversampling pressione
    //bits 1-0 = modalità

    writeRegister(REG_CTRL_MEAS, 0x27);

    //REG_CONFIG
    //0xA0 = tempo standby stabile, filtro spento o leggero


    writeRegister(REG_CONFIG, 0xA0);

}

//===============================================================

//=============== Lettura temperatura =========================

float compensateTemperature(int32_t adc_T) {

    int32_t var1;

    int32_t var2;

    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t) dig_T2)) >> 11;

    var2 = (((((adc_T >> 4) - ((int32_t) dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;

    t_fine = var1 + var2;

    float temperature = (t_fine * 5 + 128) >> 8;

    return temperature / 100.0f;

}

//=================================================================

//================= Lettura pressione ================================

float compensatePressure(int32_t adc_P) {

    int64_t var1;
    int64_t var2;
    int64_t p;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t) dig_P6;
    var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
    var2 = var2 + (((int64_t) dig_P4) << 35);

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

//==============================================================================

//================== Lettura dati grezzi ============================0

void readRawData(int32_t *adc_T, int32_t *adc_P) {

    uint8_t data[6];

    //Da 0xF7 leggo pressione MSB, LSB, XLSB, e temperatura dagli stessi

    readRegisters(REG_PRESS_MSB, data, 6);

    *adc_P = ((int32_t)data[0]<<12 | (int32_t) data[1] << 4 | (int32_t) data[2] >> 4);

    *adc_T = ((int32_t)data[3] << 12 | (int32_t) data[4] << 4 | (int32_t) data[5] >> 4);

}

//========================================================================

//================ Altezza partendo da pressione ===========================

float calculateAltitude(float pressure) {

    //Formula barometrica approssimata

    return 44330.0f * (1.0f - powf(pressure /sea_level_pressure, 0.1903f));

}

//==============================================================================

//================ Loop Principale ==============================

int main() {

    stdio_init_all();
    sleep_ms(2000);

    printf("Test temperatura, pressione ed altezza\n");

    //Inizializzo I2C

    i2c_init(I2C_PORT, 100*1000);

    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);

    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    //Leggo ID sensore

    uint8_t chip_id;

    readRegisters(REG_ID, &chip_id, 1);

    printf("Chip ID: 0x%02X\n", chip_id);

    //BMP280/BME280 di solito 0x58 o 0x60

    if(chip_id == 0x58) {

        printf("Sensore rilevato: BMP280\n");

    } else if (chip_id == 0x60) {

        printf("Sensore rilebato: BME280\n");

    } else printf("ID Sensore non riconosciuto\n");

    //Leggo coefficenti di calibrazione
    readCalibrationData();

    //Inizializzo modalità misura
    bmp280Init();

    sleep_ms(500);

    while(true) {

        int32_t adc_T;

        int32_t adc_P;

        readRawData(&adc_T, &adc_P);

        float temperature = compensateTemperature(adc_T);

        float pressure = compensatePressure(adc_P);

        float altitude = calculateAltitude(pressure);

        printf("Temperatura: %.2f C | Pressione: %.2f Pa | Altezza Stimata: %.2f m\n", temperature, pressure, altitude);

        sleep_ms(1000);

    }

    return 0;


}
