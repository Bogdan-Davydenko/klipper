#include "command.h"
#include "sched.h"

void
command_helloworld(uint32_t *args)
{
    sendf("helloworld_response");
}
DECL_COMMAND(command_helloworld, "helloworld");  // Декларация команды
