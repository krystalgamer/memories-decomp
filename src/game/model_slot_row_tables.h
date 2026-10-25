#ifndef MEMORIES_DECOMP_MODEL_SLOT_ROW_TABLES_H
#define MEMORIES_DECOMP_MODEL_SLOT_ROW_TABLES_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"

/* Eight-byte prefix reached through the primitive context's primp pointer. */
typedef struct {
    u8 pad_00[2];
    u16 record_count;
    s32 word_offset;
} ModelPrimitiveBlockHeader;

typedef char ModelPrimitiveBlockHeader_size_must_be_0x8[
    sizeof(ModelPrimitiveBlockHeader) == 0x8 ? 1 : -1
];
typedef char ModelPrimitiveBlockHeader_record_count_offset_must_be_0x2[
    (u32)&((ModelPrimitiveBlockHeader *)0)->record_count == 0x2 ? 1 : -1
];
typedef char ModelPrimitiveBlockHeader_word_offset_must_be_0x4[
    (u32)&((ModelPrimitiveBlockHeader *)0)->word_offset == 0x4 ? 1 : -1
];
typedef char ModelPrimitiveContexts_share_primtop_offset_0x14[
    (u32)&((GsARGUNIT_NORMAL *)0)->primtop == 0x14 &&
    (u32)&((GsARGUNIT_SHARED *)0)->primtop == 0x14 ? 1 : -1
];

#ifdef VERSION_EUROPE
extern s32 tent_Model_RebaseTexturedPrimitives(
    s32 mode, const GsTYPEUNIT *unit, GsARGUNIT *ctx, s32 *best, s32 *total
) asm("func_80051610");
#else
s32 tent_Model_RebaseTexturedPrimitives(
    s32 mode, const GsTYPEUNIT *unit, GsARGUNIT *ctx, s32 *best, s32 *total
);
#endif

/* Clears animation keys/durations and imports the mapped owner-block payloads. */
void Model_ResetSlotAnimationTables(s32 slot, u8 *commands);

/* Indexes each part's animation rows and computes their duration maxima. */
void Model_BuildSlotAnimationTables(s32 slot);

#endif
