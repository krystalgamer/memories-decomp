#ifndef MEMORIES_DECOMP_SOUND_OUTPUT_H
#define MEMORIES_DECOMP_SOUND_OUTPUT_H

#include "../types.h"

/* The sound output unit's exported entry points.
 *
 * SD_SetOutputType is deliberately absent: sound.h already declares it, and
 * one function belongs in one header.
 *
 * SD_GetStatusFlags returns the driver's flag word sign-extended from a
 * halfword, so it is s32 rather than the u32 its callers assumed. Every
 * observed use masks the result, so the two spellings agree in practice --
 * but only one of them matches the definition. */
s32 SD_GetStatusFlags(void);
void SD_Shutdown(void);
void SD_VSync(void);
/* Both fade the CD out and queue file-transfer command 32 with a sector
   offset and a source selector (0x10, 0x20 or 0x40). SD_StepActiveCommand
   copies the tail into the FileRequestSlot that SD_ConfigureTransferPhase
   consumes: an SPU phase (destination, encoded size) written through SpuWrite
   one staged sector at a time, then a RAM phase read directly to its
   destination. SD_EnqueueRamTransfer(ram, offset, ram_size, source) queues
   only the RAM phase; SD_EnqueueSpuRamTransfer(spu, ram, offset, spu_size,
   ram_size, source) queues both. Size parameters remain positive byte
   counts or negated sector counts; zero skips that phase. */
void SD_EnqueueRamTransfer(
    s32 ram_destination, s32 sector_offset, s32 ram_size, s32 source_selector
);
void SD_EnqueueSpuRamTransfer(
    s32 spu_destination, s32 ram_destination, s32 sector_offset,
    s32 spu_size, s32 ram_size, s32 source_selector
);
/* Prepares a BGM command without starting it: bit-15 ids queue a CD seek,
   other ids queue their music package load. */
void SD_BGMPreload(s32 arg0);
void SD_BGMPlayDefaultStep(u32 value);
void SD_BGMPlayWithStep(s32 arg0, s32 arg1);
void SD_BGMStop(u32 value);
void SD_BGMStopWithStep(u16 id, s32 step);
void SD_BGMFade(s32 step, s32 target);

#endif
