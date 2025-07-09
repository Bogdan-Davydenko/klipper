#include "command.h"
#include "basecmd.h"

struct encoder {
    uint8_t oid;
    int32_t position;
};

void
command_config_encoder(uint32_t *args)
{
    struct encoder *enc = oid_alloc(args[0], command_config_encoder, sizeof(*enc));
    enc->position = 0;
    //sendf("helloworld_response");
}
DECL_COMMAND(command_config_encoder, "config_encoder oid=%c");

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