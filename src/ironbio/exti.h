#ifndef __EXTI_H
#define __EXTI_H

#define EDGE_FALLING -1
#define EDGE_RISING 1
#define EDGE_CHANGE 0

#include <stdint.h> 

struct gpio_exti{
    void *regs;
    uint32_t bit;
};

struct gpio_exti gpio_exti_setup(uint32_t pin, int32_t pull_up);

#endif // exti.h