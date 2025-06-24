#ifndef __ENCODER_H
#define __ENCODER_H

#include "board/gpio.h"
#include <stdint.h>

// Структура энкодера (если понадобится доступ из других файлов)
struct encoder {
    struct gpio_in pin_a;
    struct gpio_in pin_b;
    int32_t position;
    uint8_t last_state;
};

// Функции для создания и настройки команд
void encoder_create(uint8_t pin_a, uint8_t pin_b);
void encoder_setup_commands(void);

#endif // __ENCODER_H
