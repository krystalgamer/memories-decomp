#ifndef MEMORIES_DECOMP_SCRIPT_OP_STAGE_IMAGE_H
#define MEMORIES_DECOMP_SCRIPT_OP_STAGE_IMAGE_H

#include "../types.h"

/* D_80090C50 handler: the viewport jump. Clears the viewport pair, takes a
 * 16-bit id from the stream, and with bit 15 set takes two more halfwords as an
 * immediate viewport position -- then leaves D_8009B27C at 5, which is the
 * state the following ticks read. */
void Script_OpStageImage(void);

#endif
