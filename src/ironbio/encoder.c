#include <stdlib.h>
#include "command.h"
#include "sched.h"
#include "basecmd.h"

struct encoder {
    uint8_t oid;
    uint32_t position;
    // ... другие поля
};

void
command_config_encoder(uint32_t *args)
{
    struct encoder *enc = oid_alloc(args[0], command_config_encoder, sizeof(*enc));
    enc->position = 0;
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
    struct encoder *enc = encoder_oid_lookup(args[0]);
    sendf("encoder_%c: pos=%u", args[0], enc->position);
    enc->position ++;
}
DECL_COMMAND(command_check_encoder, "check_encoder oid=%c");
