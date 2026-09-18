#ifndef MEMORIES_DECOMP_SCRIPT_IMAGE_COMMANDS_H
#define MEMORIES_DECOMP_SCRIPT_IMAGE_COMMANDS_H

#include "../types.h"

/* The two adjacent image opcodes. Script_OpStageImage leaves the viewport
 * pair D_8009B2A8/D_8009B2AA and state 5 in D_8009B27C; Script_OpShowImage
 * publishes that viewport when it draws. */

/* D_80090C50 handler: the viewport jump. Clears the viewport pair, takes a
 * 16-bit id from the stream, and with bit 15 set takes two more halfwords as an
 * immediate viewport position -- then leaves D_8009B27C at 5, which is the
 * state the following ticks read. */
void Script_OpStageImage(void);

/* D_80090C50 handler: draws the script image. Builds the 0x140 by 0xA0 VRAM
 * copy object kept in D_8009B280, releases the previous image slots, and
 * publishes the viewport from D_8009B2A8/D_8009B2AA; a later pass tears the
 * object down again and rebuilds the slots through ScriptImage_RebuildObjects. */
void Script_OpShowImage(void);

#endif
