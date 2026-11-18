#ifndef MEMORIES_DECOMP_MODEL_SLOT_SETUP_H
#define MEMORIES_DECOMP_MODEL_SLOT_SETUP_H

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
    OFFSETOF(ModelPrimitiveBlockHeader, record_count) == 0x2 ? 1 : -1
];
typedef char ModelPrimitiveBlockHeader_word_offset_must_be_0x4[
    OFFSETOF(ModelPrimitiveBlockHeader, word_offset) == 0x4 ? 1 : -1
];
typedef char ModelPrimitiveContexts_share_primtop_offset_0x14[
    OFFSETOF(GsARGUNIT_NORMAL, primtop) == 0x14 &&
    OFFSETOF(GsARGUNIT_SHARED, primtop) == 0x14 ? 1 : -1
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

/* One record of any of those kinds, as the rebase pass reads it. */
typedef union {
    ModelPrimFT3 ft3;
    ModelPrimGT3 gt3;
    ModelPrimFT4 ft4;
    ModelPrimGT4 gt4;
    ModelPrimFT3C ft3c;
    ModelPrimGT3C gt3c;
    ModelPrimFT4C ft4c;
    ModelPrimGT4C gt4c;
} ModelPrimRecord;

typedef char ModelPrimFT3_size_must_be_0x14[sizeof(ModelPrimFT3) == 0x14 ? 1 : -1];
typedef char ModelPrimGT3_size_must_be_0x18[sizeof(ModelPrimGT3) == 0x18 ? 1 : -1];
typedef char ModelPrimFT4_size_must_be_0x18[sizeof(ModelPrimFT4) == 0x18 ? 1 : -1];
typedef char ModelPrimGT4_size_must_be_0x1C[sizeof(ModelPrimGT4) == 0x1C ? 1 : -1];
typedef char ModelPrimFT3C_size_must_be_0x18[sizeof(ModelPrimFT3C) == 0x18 ? 1 : -1];
typedef char ModelPrimGT3C_size_must_be_0x1C[sizeof(ModelPrimGT3C) == 0x1C ? 1 : -1];
typedef char ModelPrimFT4C_size_must_be_0x1C[sizeof(ModelPrimFT4C) == 0x1C ? 1 : -1];
typedef char ModelPrimGT4C_size_must_be_0x20[sizeof(ModelPrimGT4C) == 0x20 ? 1 : -1];
typedef char ModelPrim_cba_tsb_must_be_0x2_0x6[
    OFFSETOF(ModelPrimFT3, cba) == 0x2 && OFFSETOF(ModelPrimFT3, tsb) == 0x6 &&
    OFFSETOF(ModelPrimGT4C, prim.cba) == 0x6 && OFFSETOF(ModelPrimGT4C, prim.tsb) == 0xA ? 1 : -1
];
typedef char ModelPrimFT3_norm0_must_be_0xC[OFFSETOF(ModelPrimFT3, norm0) == 0xC ? 1 : -1];
typedef char ModelPrimGT3_norms_must_be_0xC_0x10_0x14[
    OFFSETOF(ModelPrimGT3, norm0) == 0xC && OFFSETOF(ModelPrimGT3, norm1) == 0x10 &&
    OFFSETOF(ModelPrimGT3, norm2) == 0x14 ? 1 : -1
];
typedef char ModelPrimFT4_norm0_must_be_0xE[OFFSETOF(ModelPrimFT4, norm0) == 0xE ? 1 : -1];
typedef char ModelPrimGT4_norms_must_be_0xA_0x10_0x14_0x18[
    OFFSETOF(ModelPrimGT4, norm0) == 0xA && OFFSETOF(ModelPrimGT4, norm1) == 0x10 &&
    OFFSETOF(ModelPrimGT4, norm2) == 0x14 && OFFSETOF(ModelPrimGT4, norm3) == 0x18 ? 1 : -1
];
typedef char ModelPrimGT4C_norm3_must_be_0x1C[OFFSETOF(ModelPrimGT4C, prim.norm3) == 0x1C ? 1 : -1];

/*
 * The polygon drawers read each record a word at a time and copy its
 * u/v/CLUT/tpage words straight into the GPU packet, so to them a record is
 * also its word array. Their window-record kinds are the 0x200 kinds: the
 * four-byte prefix is the texture-window (E2) command word.
 */
typedef union {
    u32 words[5];
    ModelPrimFT3 prim;
} ModelDrawFt3Record;

typedef union {
    u32 words[6];
    ModelPrimGT3 prim;
} ModelDrawGt3Record;

typedef union {
    u32 words[6];
    ModelPrimFT4 prim;
} ModelDrawFt4Record;

typedef union {
    u32 words[7];
    ModelPrimGT4 prim;
} ModelDrawGt4Record;

typedef union {
    u32 words[6];
    ModelPrimFT3C prim;
} ModelDrawFt3WindowRecord;

typedef union {
    u32 words[7];
    ModelPrimGT3C prim;
} ModelDrawGt3WindowRecord;

typedef union {
    u32 words[7];
    ModelPrimFT4C prim;
} ModelDrawFt4WindowRecord;

typedef union {
    u32 words[8];
    ModelPrimGT4C prim;
} ModelDrawGt4WindowRecord;

s32 tent_Model_RebaseTexturedPrimitives(
    s32 slot_index, const GsTYPEUNIT *unit, GsARGUNIT *ctx,
    s32 *max_lit_color_index, s32 *packet_bytes
);

/* One block of a mapped HMD's primitive-header chain, as
   Model_ResetSlotAnimationTables walks it from the pointer at hmd + 0x10:
   the next block (-1 ends the chain), the block's header-section pointer,
   its type count, and the category byte at the top of the first type word
   (2 and 3 select the payloads the slot imports). */
typedef struct ModelHmdPrimitiveBlock {
    struct ModelHmdPrimitiveBlock *next;
    void **header;
    s32 type_count;
    u8 type_low[3];
    u8 category;
} ModelHmdPrimitiveBlock;

typedef char ModelHmdPrimitiveBlock_category_must_be_0xF[
    OFFSETOF(ModelHmdPrimitiveBlock, category) == 0xF ? 1 : -1
];

/* Clears animation keys/durations and imports the mapped owner-block payloads. */
void Model_ResetSlotAnimationTables(s32 slot_index, u8 *hmd);

/* Indexes each part's animation rows and computes their duration maxima. */
void Model_BuildSlotAnimationTables(s32 slot_index);

/* Relinks the command list of one model slot's animation row tables.
 *
 * It returns immediately unless the slot has both a command list in
 * animation_commands and a nonzero channel count in animation_sequence_count, then walks that
 * count of channels by MODEL_SLOT_ROW_COUNT rows, rewriting halfword links
 * in the list so entries point at one another rather than at a key.
 *
 * The reset fills the keys with 0xFFFF and imports the command list in
 * animation_commands. Model_BuildSlotAnimationTables then claims the keys and
 * accumulates row durations. This relink pass follows it in model-load
 * phase 3. */
void Model_LinkSlotAnimationRows(s32 slot_index);

#endif
