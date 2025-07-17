#include "command.h"
#include "basecmd.h"
#include "exti.h"
#include "board/gpio.h"

struct encoder {
    uint8_t oid;
    struct gpio_exti pina;
    struct gpio_out pinb;
    volatile int32_t position;
};
 
void
callback(void *ctx)
{
    sendf("cb");
    volatile int32_t *pos = (volatile int32_t *)ctx;
    (*pos)++;
}

void
command_config_encoder(uint32_t *args)
{
    struct encoder *enc = oid_alloc(args[0], command_config_encoder, sizeof(*enc));
    enc->position = 0;
    enc->pina = gpio_exti_setup(args[1], 1);
    enc->pinb = gpio_out_setup(args[2], 1);
    gpio_out_write(enc->pinb, 1);
    //stm32_exti_attach(enc->pina.bit, EDGE_CHANGE, callback, &enc->position);
    //sendf("helloworld_response");
}
DECL_COMMAND(command_config_encoder, "config_encoder oid=%c a_pin=%c b_pin=%c");

static struct encoder *
encoder_oid_lookup(uint8_t oid)
{
    return oid_lookup(oid, command_config_encoder);
}

void
command_check_encoder(uint32_t *args)
{
    uint8_t oid = args[0];
    struct encoder *enc = encoder_oid_lookup(oid);
    sendf("encoder_position oid=%c pos=%i", oid, enc->position);
    //struct gpio_in pina = enc->pina;
    //struct gpio_in pinb = enc->pinb;
    //sendf("encoder_pins pina=%c pinb=%c", pina.bit, pinb.bit);
    //if((enc->position %2) == 0){
    //    gpio_out_write(enc->pinb, 0);
    //}else{
    //    gpio_out_write(enc->pinb, 1);
    //}
    //callback(&enc->position);
}
DECL_COMMAND(command_check_encoder, "check_encoder oid=%c");

void
command_reset_encoder(uint32_t *args)
{
    uint8_t oid = args[0];
    struct encoder *enc = encoder_oid_lookup(oid);
    enc->position = args[1];
}
DECL_COMMAND(command_reset_encoder, "reset_encoder oid=%c pos=%i");