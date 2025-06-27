#include "helloworld.h"
#include "command.h"
#include "sched.h"

// Простейшая команда без параметров
void
command_helloworld(uint32_t *args)
{
    sendf("helloworld_response");
    output("helloworld_response debug");
    //int i = 0;
    //for(;;){
    //    i = i+1;
    //}
}
DECL_COMMAND(command_helloworld, "helloworld");  // Фиксированное имя команды

// Инициализация (обязательная)
void
helloworld_init(void)
{
    // Пустая функция, но должна существовать
}