#ifndef MEMORIES_DECOMP_GRAPHICS_PACKET_SUBMISSION_H
#define MEMORIES_DECOMP_GRAPHICS_PACKET_SUBMISSION_H

/* gpu_packets.h is a frozen dependency of historical overlay attempts. */
#include "gpu_packets.h"

void Graphics_SubmitDrawModePacket(u32 *src, GsOT *ot, s32 idx, s32 flags);
void Graphics_SubmitMaskWritePacket(u32 *src, GsOT *ot, s32 idx, s32 flags);

#endif
