# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/alex/pico/pico-sdk/tools/pioasm"
  "/home/alex/Scrivania/Progetto Sistemi Operativi/NuovoProgettoSistemiOperativi/build/pioasm"
  "/home/alex/Scrivania/Progetto Sistemi Operativi/NuovoProgettoSistemiOperativi/build/pioasm-install"
  "/home/alex/Scrivania/Progetto Sistemi Operativi/NuovoProgettoSistemiOperativi/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/tmp"
  "/home/alex/Scrivania/Progetto Sistemi Operativi/NuovoProgettoSistemiOperativi/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp"
  "/home/alex/Scrivania/Progetto Sistemi Operativi/NuovoProgettoSistemiOperativi/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src"
  "/home/alex/Scrivania/Progetto Sistemi Operativi/NuovoProgettoSistemiOperativi/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/alex/Scrivania/Progetto Sistemi Operativi/NuovoProgettoSistemiOperativi/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/alex/Scrivania/Progetto Sistemi Operativi/NuovoProgettoSistemiOperativi/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
