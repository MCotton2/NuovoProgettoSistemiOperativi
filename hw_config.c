#include "hw_config.h"
#include "sd_card.h"

//Configurazione SPI per MicroSD.

static spi_t spis[] = {
    {
        .hw_inst = spi0,
        .miso_gpio = 16,
        .mosi_gpio = 19,
        .sck_gpio = 18,
        .baud_rate = 1000 * 1000
    }
};

static sd_card_t sd_cards[] = {
    {
        .pcName = "0:",
        .spi = &spis[0],
        .ss_gpio = 17,
        .use_card_detect = false
    }
};

//Dice alla libreria quante interfacce SPI abbiamo configurato.
 
size_t spi_get_num() {
    return count_of(spis);
}

//Restituisce la configurazione SPI numero num.

spi_t *spi_get_by_num(size_t num) {
    if (num >= count_of(spis)) {
        return NULL;
    }

    return &spis[num];
}

//Dice alla libreria quante SD abbiamo configurato.
size_t sd_get_num() {
    return count_of(sd_cards);
}

//Restituisce la SD numero num.
sd_card_t *sd_get_by_num(size_t num) {
    if (num >= count_of(sd_cards)) {
        return NULL;
    }

    return &sd_cards[num];
}