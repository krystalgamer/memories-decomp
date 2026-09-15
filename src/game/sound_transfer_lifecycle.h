#ifndef MEMORIES_DECOMP_SOUND_TRANSFER_LIFECYCLE_H
#define MEMORIES_DECOMP_SOUND_TRANSFER_LIFECYCLE_H

#include "../ygo_types.h"

/* SD_InitState points this at 0x801E1650; func_80045514 fills the request
   passed directly to func_80014C40. Its loads retain the absolute arm. */
#ifdef SOUND_TRANSFER_REQUEST_IN_DATA
extern FileRequestSlot *D_8009B460 __attribute__((section(".data")));
#else
extern FileRequestSlot *D_8009B460;
#endif

void func_80049640(void);
void SD_Term(void);
s32 SD_VabOpenHead(u8 *vab, s16 vab_id, s32 spu_addr);
s32 SD_VabTransBody(s32 value, s16 expected);

#endif
