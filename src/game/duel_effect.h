#ifndef YUGIOH_GAME_DUEL_EFFECT_H
#define YUGIOH_GAME_DUEL_EFFECT_H

#include "../types.h"

#define DUEL_EFFECT_OFFSET(type, member) ((u32)&(((type *)0)->member))

#define DUEL_EFFECT_CHANNEL_COUNT 4
#define DUEL_EFFECT_ENTRY_COUNT 620
#define DUEL_EFFECT_OCCUPANCY_COUNT 240
#define DUEL_EVENT_SCRIPT_FLAG_DIALOG_ACTIVE 0x4000
#define DUEL_EVENT_SCRIPT_FLAG_STARTED 0x8000
#define TEXT_BOX_FLAG_BUILD_REQUESTED 0x800
#define TEXT_BOX_FLAG_DONE 0x2000
#define DUEL_EFFECT_STATE_FLAG_INITIALIZED 0x80

/* One text/effect entry, 0x1C bytes, the element type of D_800EB288. The
   leading words are unnamed but must stay four-byte aligned: retail copies a
   whole entry with aligned lw/sw pairs during the compaction in
   DuelEffect_ProcessEntries, and a byte-aligned struct turns that into
   lwl/lwr. The password overlay reads two of the fields: code_00 is the
   entry's Shift-JIS glyph code, and the 0x0C pair is its local pixel
   position, which TextBox_GetGlyphAt searches to find the node under a
   coordinate. */
typedef struct {
    u16 code_00;
    u16 pad_02;
    s32 field_04;
    s32 field_08;
    s16 x_0C;
    s16 y_0E;
    u8 field_10;
    u8 flags_11;
    u8 field_12;
    u8 field_13;
    u8 pad_14;
    u8 field_15;
    u8 pad_16[2];
    u8 field_18;
    u8 pad_19[3];
} DuelEffectEntry;

/* One text-box record, 0x64 bytes, the element type of D_800EB0F8. 0x00 is the
   decoded string the record is playing back (TextBox_BuildStep stores it there),
   and 0x20/0x24 bracket the record's slice of D_800EB288: TextBox_BuildStep seeds
   both with &D_800EB288[range_start_5C], DuelEffect_ProcessEntries walks from
   0x24 and moves 0x20 as it compacts. */
typedef struct {
    u8 *text_00;
    u8 pad_04[0x1C];
    DuelEffectEntry *entry_end_20;
    DuelEffectEntry *entry_head_24;
    s32 field_28;
    s32 field_2C;
    void *field_30;
    u16 flags_34;
    u16 field_36;
    u16 field_38;
    u16 field_3A;
    s16 field_3C;
    s16 field_3E;
    s16 field_40;
    s16 field_42;
    u8 pad_44[0x0F];
    u8 field_53;
    u8 field_54;
    u8 pad_55[2];
    u8 index_57;
    u8 pad_58;
    u8 field_59;
    u8 field_5A;
    u8 field_5B;
    u16 range_start_5C;
    u16 range_count_5E;
    u8 field_60;
    u8 field_61;
    u8 pad_62[2];
} DuelEffectChannel;

typedef char DuelEffectChannel_size_must_be_0x64[
    sizeof(DuelEffectChannel) == 0x64 ? 1 : -1
];
typedef char DuelEffectChannel_entry_end_20_offset_must_be_0x20[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, entry_end_20) == 0x20 ? 1 : -1
];
typedef char DuelEffectChannel_entry_head_24_offset_must_be_0x24[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, entry_head_24) == 0x24 ? 1 : -1
];
typedef char DuelEffectChannel_field_28_offset_must_be_0x28[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, field_28) == 0x28 ? 1 : -1
];
typedef char DuelEffectChannel_field_30_offset_must_be_0x30[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, field_30) == 0x30 ? 1 : -1
];
typedef char DuelEffectChannel_flags_34_offset_must_be_0x34[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, flags_34) == 0x34 ? 1 : -1
];
typedef char DuelEffectChannel_field_3C_offset_must_be_0x3C[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, field_3C) == 0x3C ? 1 : -1
];
typedef char DuelEffectChannel_field_53_offset_must_be_0x53[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, field_53) == 0x53 ? 1 : -1
];
typedef char DuelEffectChannel_index_57_offset_must_be_0x57[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, index_57) == 0x57 ? 1 : -1
];
typedef char DuelEffectChannel_field_5A_offset_must_be_0x5A[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, field_5A) == 0x5A ? 1 : -1
];
typedef char DuelEffectChannel_range_start_5C_offset_must_be_0x5C[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, range_start_5C) == 0x5C ? 1 : -1
];
typedef char DuelEffectChannel_field_61_offset_must_be_0x61[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, field_61) == 0x61 ? 1 : -1
];

typedef char DuelEffectEntry_size_must_be_0x1C[
    sizeof(DuelEffectEntry) == 0x1C ? 1 : -1
];
typedef char DuelEffectEntry_must_be_four_byte_aligned[
    sizeof(struct { u8 lead; DuelEffectEntry entry; }) == 0x20 ? 1 : -1
];
typedef char DuelEffectEntry_code_00_offset_must_be_0[
    DUEL_EFFECT_OFFSET(DuelEffectEntry, code_00) == 0 ? 1 : -1
];
typedef char DuelEffectEntry_x_0C_offset_must_be_0x0C[
    DUEL_EFFECT_OFFSET(DuelEffectEntry, x_0C) == 0x0C ? 1 : -1
];
typedef char DuelEffectEntry_field_10_offset_must_be_0x10[
    DUEL_EFFECT_OFFSET(DuelEffectEntry, field_10) == 0x10 ? 1 : -1
];
typedef char DuelEffectEntry_flags_11_offset_must_be_0x11[
    DUEL_EFFECT_OFFSET(DuelEffectEntry, flags_11) == 0x11 ? 1 : -1
];
typedef char DuelEffectEntry_field_12_offset_must_be_0x12[
    DUEL_EFFECT_OFFSET(DuelEffectEntry, field_12) == 0x12 ? 1 : -1
];
typedef char DuelEffectEntry_field_13_offset_must_be_0x13[
    DUEL_EFFECT_OFFSET(DuelEffectEntry, field_13) == 0x13 ? 1 : -1
];
typedef char DuelEffectEntry_field_15_offset_must_be_0x15[
    DUEL_EFFECT_OFFSET(DuelEffectEntry, field_15) == 0x15 ? 1 : -1
];
typedef char DuelEffectEntry_field_18_offset_must_be_0x18[
    DUEL_EFFECT_OFFSET(DuelEffectEntry, field_18) == 0x18 ? 1 : -1
];

#undef DUEL_EFFECT_OFFSET

extern DuelEffectChannel D_800EB0F8[DUEL_EFFECT_CHANNEL_COUNT];
extern DuelEffectEntry D_800EB288[DUEL_EFFECT_ENTRY_COUNT];

/* The effect/text advance flag. TextBox_BuildStep is the only reader: it
 * clears the flag to 0, calls the opcode handler, and then tests the result
 * twice -- `>= 0` and then `== 1`. Everything else only ever writes it, and
 * almost always writes 1.
 *
 * Signed, because that `>= 0` test is only meaningful on a signed value.
 *
 * volatile, because the two tests have no intervening call, so without it
 * GCC folds them onto a single load: dropping the qualifier shrinks the
 * executable by 12 bytes. It is a property of the object, so it belongs on
 * the shared declaration rather than on the one file that happens to read it.
 */
extern volatile s32 D_8009B350;

/* Per-scene state flags. Bit 0x80 is the run-once latch: every scene entry
 * point in this subsystem opens with the same
 * `if ((D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0)` test and
 * sets the bit inside, so the body runs on the first tick of a scene only.
 * dialog_transition.c and func_8003DA40.c additionally raise 0x20 and 0x40,
 * and the byte is cleared back to 0 when a scene is torn down. */
extern u8 D_8009B3C1;

/* A 0x1C-byte effect object, the record func_80025D30 and func_800260D0 both
 * walk. Both files described the same layout: the first named the halfword
 * at 0x12 and padded 0x06..0x11, the second padded straight across
 * 0x06..0x13. This carries the union of what each knew. */
typedef struct {
    u16 x;             /* 0x00 */
    u16 y;             /* 0x02 */
    u16 field_04;      /* 0x04 */
    u8 pad_06[0xC];    /* 0x06 */
    s16 field_12;      /* 0x12 */
    s32 field_14;      /* 0x14 */
    u8 pad_18[2];      /* 0x18 */
    s16 field_1A;      /* 0x1A */
} DuelEffectObject;

typedef char DuelEffectObject_size_must_be_0x1C[
    sizeof(DuelEffectObject) == 0x1C ? 1 : -1
];

s32 DuelEffect_UpdateState(void);

#endif
