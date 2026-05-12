#include "SDLogger.h"

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "f_util.h"
#include "hw_config.h"

//Costruttore

SDLogger::SDLogger() {

    mounted = false;
    file_open = false;

}

// Monta Sched SD

bool SDLogger::init() {

    FRESULT fr = f_mount(&fs, "0:", 1);

    if(fr != FR_OK) {

        printf("Errore mount SD: %s\n", FRESULT_str(fr));
        mounted = false;
        return false;

    }

    mounted = true;

    printf("SD Montata correttamente\n");

    return true;

}

bool SDLogger::openFile(const char *filename) {

    if(!mounted) {

        printf("Errore: SD non montata\n");
        return false;

    }

    FRESULT fr = f_open(
        &file, 
        filename, 
        FA_WRITE | FA_OPEN_APPEND
    );

    if(fr != FR_OK) {

        printf("Errore apertura file\n");
        file_open = false;
        return false;

    }

    file_open = true;

    printf("File aperto correttamente: %s\n", filename);

    return true;

}

bool SDLogger::writeLine(const char *line) {

    if(!file_open) {

        printf("Errore: file non aperto\n");
        return false;

    }

    UINT bytes_written;

    FRESULT fr = f_write(
        &file,
        line,
        strlen(line),
        &bytes_written
    );

    if( fr != FR_OK) {

        printf("Errore scrittura SD: %s\n", FRESULT_str(fr));
        return false;

    }

    //Forza salvataggio fisico
    f_sync(&file);

    return true;

}

//Scrivo dati testi su csv

bool SDLogger::writeTestData() {

    if(!file_open) {

        printf("Erore: file non aperto\n");
        return false;

    }

    char buffer[128];

    writeLine("Tempo ms, valore\n");

    for(int i=0; i<10; i++) {

        snprintf(
            buffer, 
            sizeof(buffer), 
            "%d, %d\n", 
            i*1000, 
            i*10
        );

        if(!writeLine(buffer)) {

            return false;

        }

        printf("Riga %d scritta\n", i);

        sleep_ms(100);
    }

    return true;

}

void SDLogger::close() {

    if(file_open) {

        f_close(&file);
        file_open = false;
        printf("File chiuso\n");

    }

    if(mounted) {

        f_unmount("0:");
        mounted = false;
        printf("SD Smontata\n");

    }

}