#include "exti.h"
#include "stm32f4xx.h"
#include "sched.h"

#include "board/irq.h"
#include "board/internal.h"

#include "command.h"
#include "basecmd.h"
#define MAX_EXTI_LINES 16

#include "generic/armcm_boot.h" // armcm_enable_irq

static int
gpio_valid(uint32_t pin)
{
    uint32_t port = GPIO2PORT(pin);
    return port < 8;
}

static struct {
    void (*cb)(void *);
    void *ctx;
} exti_callbacks[MAX_EXTI_LINES] = { 0 };

struct gpio_exti
gpio_exti_setup(uint32_t pin, int32_t pull)
{
    armcm_enable_irq(EXTI0_IRQHandler, EXTI0_IRQn, 0); // Добавление handler функции и настройка NVIC

    if (!gpio_valid(pin))
        shutdown("Not a valid input pin");
    GPIO_TypeDef *regs = digital_regs[GPIO2PORT(pin)];
    struct gpio_exti g = { .regs=regs, .bit=GPIO2BIT(pin) };

    // LED Для теста
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // LED
    GPIOD->MODER &= ~GPIO_MODER_MODER13; // LED
    GPIOD->MODER |= GPIO_MODER_MODER13_1; // LED

    // Тест (ТОЛЬКО для PA0)
	// Настройка RCC
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; // Включение GPIO A
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Включение SYSCFG

	// Настройка пина
    GPIOE->MODER &= ~GPIO_MODER_MODER0;
    GPIOE->MODER |= GPIO_MODER_MODER0_0;
	
	// Настройка EXTI
    EXTI->RTSR |= EXTI_RTSR_TR0; // Отслеживание RISING
    EXTI->FTSR |= EXTI_FTSR_TR0; // Отслеживание FALLING
	EXTI->IMR |= EXTI_IMR_MR0; // Разрешение прерываний

	// Подключение порта к EXTI
    SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI0);
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PE; // Подключение пина

    return g;
}

/*
void stm32_exti_attach(uint8_t pin, uint8t edge_type, void (*cb)(void *), void *ctx) {
    
    RCC->AHB1ENR |= (1 << 0); //+
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; //+

    //
    //Настройка GPIO
    //Пин: PA0
    //Режим: Input Pull Up
    //
    //GPIOA->CRL &= ~(GPIO_MODER_MODE0 | GPIO_LCKR_LCK10);
    //GPIOA->CRL |= (0x02 << GPIO_CRL_CNF0_Pos); //Вход Pull Up/Pull Down
    //GPIOA->ODR |= (1 << 0); //Подтяжка вверх

    
    //Настройка EXTI
    
    SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI0); //Нулевой канал EXTI подключен к порту PA0 //+

    EXTI->RTSR |= EXTI_RTSR_TR0; //Прерывание по нарастанию импульса //+
    EXTI->FTSR |= EXTI_FTSR_TR0; //Прерывание по спаду импульса //+

    EXTI->PR = EXTI_PR_PR0;      //Сбрасываем флаг прерывания //+?
                                //перед включением самого прерывания
    EXTI->IMR |= EXTI_IMR_MR0;   //Включаем прерывание 0-го канала EXTI //+

    
    exti_callbacks[0].cb = cb;
    exti_callbacks[0].ctx = ctx;

    NVIC_EnableIRQ(EXTI0_IRQn);  //Разрешаем прерывание в контроллере прерываний //+
    sendf("cb_attached");
}
*/



void EXTI0_IRQHandler(void) {
    EXTI_PR = EXTI_LINE0;
    GPIOD_ODR |= GPIO_ODR_13PIN;
    sendf("exti0");
    //EXTI->PR = EXTI_PR_PR0;
    //if (EXTI->PR & (1 << 0)) {
    //    sendf("exti0");
    //    EXTI->PR = (1 << 0);
    //    if (exti_callbacks[0].cb)
    //        exti_callbacks[0].cb(exti_callbacks[0].ctx);
    //}
}

void EXTI1_IRQHandler(void) {
    if (EXTI->PR & (1 << 1)) {
        sendf("exti1");
        EXTI->PR = (1 << 1);
        if (exti_callbacks[1].cb)
            exti_callbacks[1].cb(exti_callbacks[1].ctx);
    }
}

void EXTI2_IRQHandler(void) {
    if (EXTI->PR & (1 << 2)) {
        sendf("exti2");
        EXTI->PR = (1 << 2);
        if (exti_callbacks[2].cb)
            exti_callbacks[2].cb(exti_callbacks[2].ctx);
    }
}

void EXTI3_IRQHandler(void) {
    if (EXTI->PR & (1 << 3)) {
        sendf("exti3");
        EXTI->PR = (1 << 3);
        if (exti_callbacks[3].cb)
            exti_callbacks[3].cb(exti_callbacks[3].ctx);
    }
}

void EXTI4_IRQHandler(void) {
    if (EXTI->PR & (1 << 4)) {
        sendf("exti4");
        EXTI->PR = (1 << 4);
        if (exti_callbacks[4].cb)
            exti_callbacks[4].cb(exti_callbacks[4].ctx);
    }
}

void EXTI9_5_IRQHandler(void) {
    for (int i = 5; i <= 9; ++i) {
        sendf("exti95");
        if (EXTI->PR & (1 << i)) {
            EXTI->PR = (1 << i);
            if (exti_callbacks[i].cb)
                exti_callbacks[i].cb(exti_callbacks[i].ctx);
        }
    }
}

void EXTI15_10_IRQHandler(void) {
    for (int i = 10; i <= 15; ++i) {
        sendf("exti1510");
        if (EXTI->PR & (1 << i)) {
            EXTI->PR = (1 << i);
            if (exti_callbacks[i].cb)
                exti_callbacks[i].cb(exti_callbacks[i].ctx);
        }
    }
}