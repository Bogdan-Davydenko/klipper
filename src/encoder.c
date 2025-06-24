#include "encoder.h"
#include "command.h"
#include "gpio.h"
#include "alloc.h"

// Максимум 4 энкодера
#define MAX_ENCODERS 4

static struct encoder *encoder_list[MAX_ENCODERS];
static uint8_t encoder_count = 0;

// Считать текущее состояние пинов (A и B) как 2-бит число
static inline uint8_t read_encoder_state(struct encoder *e) {
    uint8_t state = 0;
    if (gpio_in_read(&e->pin_a))
        state |= 0x01;
    if (gpio_in_read(&e->pin_b))
        state |= 0x02;
    return state;
}

// Прерывание — обновляем позицию энкодера
static void encoder_irq_handler(void *data) {
    struct encoder *e = (struct encoder *)data;
    uint8_t state = read_encoder_state(e);

    uint8_t delta = (e->last_state << 2) | state;
    switch (delta) {
        case 0b0001: case 0b0111: case 0b1110: case 0b1000:
            e->position++;
            break;
        case 0b0010: case 0b0100: case 0b1101: case 0b1011:
            e->position--;
            break;
        default:
            break;
    }

    e->last_state = state;
}

// Создание энкодера — настраиваем пины и прерывания
void encoder_create(uint8_t pin_a, uint8_t pin_b) {
    if (encoder_count >= MAX_ENCODERS)
        return;

    struct encoder *e = alloc_struct(encoder);
    gpio_in_setup(&e->pin_a, pin_a, GPIO_PULLUP);
    gpio_in_setup(&e->pin_b, pin_b, GPIO_PULLUP);

    e->position = 0;
    e->last_state = read_encoder_state(e);

    gpio_set_interrupt(&e->pin_a, GPIO_INT_EDGE_BOTH, encoder_irq_handler, e);
    gpio_set_interrupt(&e->pin_b, GPIO_INT_EDGE_BOTH, encoder_irq_handler, e);

    encoder_list[encoder_count++] = e;
}

// Команда MCU: получить позицию энкодера
static void command_get_encoder_position(uint32_t *args) {
    uint8_t id = args[0];
    if (id >= encoder_count)
        return;
    struct encoder *e = encoder_list[id];
    sendf("encoder_position id=%u pos=%ld", id, e->position);
}

// Команда MCU: установить позицию энкодера (например обнулить)
static void command_set_encoder_position(uint32_t *args) {
    uint8_t id = args[0];
    int32_t pos = (int32_t)args[1];
    if (id >= encoder_count)
        return;
    struct encoder *e = encoder_list[id];
    e->position = pos;
}

// Регистрация команд
void encoder_setup_commands(void) {
    register_command("get_encoder_position", command_get_encoder_position);
    register_command("set_encoder_position", command_set_encoder_position);
}
