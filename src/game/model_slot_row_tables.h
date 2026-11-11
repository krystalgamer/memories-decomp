#ifndef MEMORIES_DECOMP_MODEL_SLOT_ROW_TABLES_H
#define MEMORIES_DECOMP_MODEL_SLOT_ROW_TABLES_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"

/* Eight-byte prefix reached through the primitive context's primp pointer. */
typedef struct {
    u8 descriptor_word_count_le[2];
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

/* Psy-Q HMD textured polygon records selected by unit->type's low
   halfword: u/v pairs interleaved with the CLUT (cba) and texture page
   (tsb) halfwords, then normal and vertex indices. The 0x200 kinds add a
   four-byte colour/code prefix. */
typedef struct {
    u8 u0, v0;
    u16 cba;
    u8 u1, v1;
    u16 tsb;
    u8 u2, v2;
    u16 pad;
    u16 norm0;
    u16 vert0, vert1, vert2;
} ModelPrimFT3;

typedef struct {
    u8 u0, v0;
    u16 cba;
    u8 u1, v1;
    u16 tsb;
    u8 u2, v2;
    u16 pad;
    u16 norm0, vert0;
    u16 norm1, vert1;
    u16 norm2, vert2;
} ModelPrimGT3;

typedef struct {
    u8 u0, v0;
    u16 cba;
    u8 u1, v1;
    u16 tsb;
    u8 u2, v2;
    u16 pad;
    u8 u3, v3;
    u16 norm0;
    u16 vert0, vert1, vert2, vert3;
} ModelPrimFT4;

typedef struct {
    u8 u0, v0;
    u16 cba;
    u8 u1, v1;
    u16 tsb;
    u8 u2, v2;
    u16 norm0;
    u8 u3, v3;
    u16 vert0;
    u16 norm1, vert1;
    u16 norm2, vert2;
    u16 norm3, vert3;
} ModelPrimGT4;

typedef struct { u8 r, g, b, code; ModelPrimFT3 prim; } ModelPrimFT3C;
typedef struct { u8 r, g, b, code; ModelPrimGT3 prim; } ModelPrimGT3C;
typedef struct { u8 r, g, b, code; ModelPrimFT4 prim; } ModelPrimFT4C;
typedef struct { u8 r, g, b, code; ModelPrimGT4 prim; } ModelPrimGT4C;

#define PRIM_OFFSET(type, member) ((u32)&((type *)0)->member)

typedef char ModelPrimFT3_size_must_be_0x14[sizeof(ModelPrimFT3) == 0x14 ? 1 : -1];
typedef char ModelPrimGT3_size_must_be_0x18[sizeof(ModelPrimGT3) == 0x18 ? 1 : -1];
typedef char ModelPrimFT4_size_must_be_0x18[sizeof(ModelPrimFT4) == 0x18 ? 1 : -1];
typedef char ModelPrimGT4_size_must_be_0x1C[sizeof(ModelPrimGT4) == 0x1C ? 1 : -1];
typedef char ModelPrimFT3C_size_must_be_0x18[sizeof(ModelPrimFT3C) == 0x18 ? 1 : -1];
typedef char ModelPrimGT3C_size_must_be_0x1C[sizeof(ModelPrimGT3C) == 0x1C ? 1 : -1];
typedef char ModelPrimFT4C_size_must_be_0x1C[sizeof(ModelPrimFT4C) == 0x1C ? 1 : -1];
typedef char ModelPrimGT4C_size_must_be_0x20[sizeof(ModelPrimGT4C) == 0x20 ? 1 : -1];
typedef char ModelPrim_cba_tsb_must_be_0x2_0x6[
    PRIM_OFFSET(ModelPrimFT3, cba) == 0x2 && PRIM_OFFSET(ModelPrimFT3, tsb) == 0x6 &&
    PRIM_OFFSET(ModelPrimGT4C, prim.cba) == 0x6 && PRIM_OFFSET(ModelPrimGT4C, prim.tsb) == 0xA ? 1 : -1
];
typedef char ModelPrimFT3_norm0_must_be_0xC[PRIM_OFFSET(ModelPrimFT3, norm0) == 0xC ? 1 : -1];
typedef char ModelPrimGT3_norms_must_be_0xC_0x10_0x14[
    PRIM_OFFSET(ModelPrimGT3, norm0) == 0xC && PRIM_OFFSET(ModelPrimGT3, norm1) == 0x10 &&
    PRIM_OFFSET(ModelPrimGT3, norm2) == 0x14 ? 1 : -1
];
typedef char ModelPrimFT4_norm0_must_be_0xE[PRIM_OFFSET(ModelPrimFT4, norm0) == 0xE ? 1 : -1];
typedef char ModelPrimGT4_norms_must_be_0xA_0x10_0x14_0x18[
    PRIM_OFFSET(ModelPrimGT4, norm0) == 0xA && PRIM_OFFSET(ModelPrimGT4, norm1) == 0x10 &&
    PRIM_OFFSET(ModelPrimGT4, norm2) == 0x14 && PRIM_OFFSET(ModelPrimGT4, norm3) == 0x18 ? 1 : -1
];
typedef char ModelPrimGT4C_norm3_must_be_0x1C[PRIM_OFFSET(ModelPrimGT4C, prim.norm3) == 0x1C ? 1 : -1];

s32 tent_Model_RebaseTexturedPrimitives(
    s32 slot_index, const GsTYPEUNIT *unit, GsARGUNIT *ctx,
    s32 *max_lit_color_index, s32 *packet_bytes
);

/* Clears animation keys/durations and imports the mapped owner-block payloads. */
void Model_ResetSlotAnimationTables(s32 slot_index, u8 *hmd);

/* Indexes each part's animation rows and computes their duration maxima. */
void Model_BuildSlotAnimationTables(s32 slot_index);

#endif
