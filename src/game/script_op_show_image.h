#ifndef MEMORIES_DECOMP_SCRIPT_OP_SHOW_IMAGE_H
#define MEMORIES_DECOMP_SCRIPT_OP_SHOW_IMAGE_H

#include "../types.h"

/* D_80090C50 handler: draws the script image. Builds the 0x140 by 0xA0 VRAM
 * copy object kept in D_8009B280, releases the previous image slots, and
 * publishes the viewport from D_8009B2A8/D_8009B2AA; a later pass tears the
 * object down again and rebuilds the slots through func_8002E128. */
void Script_OpShowImage(void);

#endif
