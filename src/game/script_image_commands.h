#ifndef MEMORIES_DECOMP_SCRIPT_IMAGE_COMMANDS_H
#define MEMORIES_DECOMP_SCRIPT_IMAGE_COMMANDS_H

#include "../types.h"

#define SCRIPT_SHOW_IMAGE_CONTROL_TRANSFER_PHASE 0x4000
#define SCRIPT_SHOW_IMAGE_CONTROL_REBUILD_PHASE 0x2000

/* The two adjacent image opcodes. Script_OpStageImage leaves the viewport
 * pair gScript_ViewportTargetX/gScript_ViewportTargetY and state 5 in gScript_wCommandWord; Script_OpShowImage
 * publishes that viewport when it draws. */

/* gScript_apfnCommandHandlers handler: the viewport jump. Clears the viewport pair, takes a
 * 16-bit id from the stream, and with bit 15 set takes two more halfwords as an
 * immediate viewport position -- then leaves gScript_wCommandWord at 5, which is the
 * state the following ticks read. */
void Script_OpStageImage(void);

/* gScript_apfnCommandHandlers handler: draws the script image. Builds the 0x140 by 0xA0 VRAM
 * copy object kept in gScript_TransitionObject, releases the previous image slots, and
 * publishes the viewport from gScript_ViewportTargetX/gScript_ViewportTargetY; a later pass tears the
 * object down again and rebuilds the slots through ScriptImage_RebuildObjects. */
void Script_OpShowImage(void);

#endif
