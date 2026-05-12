#pragma once
#include "ff.h"

class SDLogger {

    private:
        FATFS fs;
        FIL file;
        bool mounted;
        bool file_open;

    public:
        SDLogger();

        bool init();
        bool openFile(const char *filename);
        bool writeLine(const char *line);
        bool writeTestData();
        void close();

};
