
cmake_minimum_required(VERSION 3.13)

#Includo file di collegamento alla Pico

include(pico_sdk_import.cmake)

#Nome del progetto e quali linguaggi uso

project(test C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

#Inizializzo il Pico SDK
pico_sdk_init()

#Definisco l'eseguibile finale
add_executable(test main.cpp
                    Joystick.cpp
                    ServoMotor.cpp
                    Qmc5883l.cpp
                    )

#Collego librerie necessarie
target_link_libraries(test pico_stdlib hardware_adc hardware_pwm hardware_i2c)

#Disabilita UART fisica e usiamo l'usb
pico_enable_stdio_usb(test 1)
pico_enable_stdio_uart(test 0)

#Genera file extra da caricare su Pico
pico_add_extra_outputs(test)
