#ifndef YUGIOH_GAME_DUEL_EFFECT_H
#define YUGIOH_GAME_DUEL_EFFECT_H

#include "../types.h"
#include "../ygo_types.h"

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
/* DuelEffectEntry is defined in ygo_types.h. func_80036C14 writes 0x16 on
   its 0x100 path and 0x17 on its 0x80 path, with no resident readers; those
   fields therefore retain offset names in the central definition. */

/* One text-box record, 0x64 bytes, the element type of D_800EB0F8. 0x00 is the
   decoded string the record is playing back (TextBox_BuildStep stores it there),
   and 0x20/0x24 bracket the record's slice of D_800EB288: TextBox_BuildStep seeds
   both with &D_800EB288[range_start_5C], DuelEffect_ProcessEntries walks from
   0x24 and moves 0x20 as it compacts. */
struct DisplayObject;

typedef struct DuelEffectChannel {
    u8 *text_00;
    /* The fade callbacks in D_80090EAC reach this block. They are
       reached as bytes at 0x04-0x0A, as halfwords at 0x0C and 0x0E,
       and as single bytes at 0x13-0x15; 0x04 is additionally written
       as one word (0 and 0x80808080), which those sites spell as a
       width over field_04 rather than a separate member.

       0x13 is the byte DisplayObjectFade_MarkInitialized tests and
       sets, and the one carrying DISPLAY_OBJECT_FADE_FLAG_*. The
       rest keep field_NN: 0x04-0x07 and 0x08-0x0A are each written
       as a run of equal bytes stepped together, which says component
       groups without saying which component is which. */
    u8 field_04;
    u8 field_05;
    u8 field_06;
    u8 field_07;
    u8 field_08;
    u8 field_09;
    u8 field_0A;
    u8 pad_0B;
    u16 field_0C;
    u16 field_0E;
    u8 pad_10[3];
    u8 field_13;
    u8 field_14;
    u8 field_15;
    u8 pad_16[10];
    DuelEffectEntry *entry_end_20;
    DuelEffectEntry *entry_head_24;
    /* The display object this record owns, stored as a pointer.
     *
     * Every consumer already asserted that: func_800391E4.c and
     * func_8002EE94 (now a candidate) cast it to DisplayObject *,
     * card_list_text_boxes.c reaches ->flags through it, func_8002EE94 also
     * takes the DisplayObjectSnapshot view, and Dialog_UpdateChoice read it
     * through `*(u8 **)&`. The s32 spelling made every one of those a cast. */
    struct DisplayObject *field_28;
    /* The second display object this record owns, on the same evidence as
     * field_28 above: func_800391E4.c releases it through func_8004036C and
     * then stores a DisplayObject * into it, func_800610E0.c and
     * func_800611D0.c both store the object they just built, and
     * text_box_build_step.c releases it the same way. Every other site
     * assigns 0.
     *
     * The s32 spelling survived field_28's retype because
     * `b->field_2C = 0x200` in func_8002BFCC looked like an integer use of
     * this field. It is not: that b is a DuelEffectResourceRecord *, which
     * has its own field_2C at its own offset. The candidate func_800179F4's
     * `pane->field_2C = 0` is that same other record. */
    struct DisplayObject *field_2C;
    void *field_30;
    u16 flags_34;
    u16 field_36;
    u16 field_38;
    u16 field_3A;
    s16 field_3C;
    s16 field_3E;
    s16 field_40;
    s16 field_42;
    u8 pad_44[0x0D];
    /* The record's state byte. TextBox_BuildStep dispatches on its low five
       bits through D_80090E64, and 0x80 is a latch every one of those
       callbacks sets on entry: Dialog_UpdateChoice and the contiguous
       callbacks in duel_effect_state_callbacks.c open with the same
       `if ((state & 0x80) == 0) state |= 0x80;` and then write a new state
       number into it. */
    u8 state_51;
    /* A per-tick countdown. TextBox_BuildStep reloads it from field_53,
       decrements it once per call and returns while it is still nonzero;
       func_80037B40 does the same with 0xFF as its reload. */
    u8 delay_52;
    u8 field_53;
    u8 field_54;
    u8 pad_55;
    u8 field_56;
    u8 index_57;
    /* Which of the record's leading pointer words is the live byte stream,
       as a word index rather than an offset: every reader scales it by four.
       TextBox_BuildStep advances the selected pointer past each opcode it
       consumes, and duel_effect_object_commands.c and
       duel_effect_command.c reach the same slot the same way. Signed
       because all three read it through an s8. */
    s8 stream_58;
    u8 field_59;
    u8 field_5A;
    u8 field_5B;
    u16 range_start_5C;
    u16 range_count_5E;
    u8 field_60;
    u8 field_61;
    /* Written by func_80037DA4 (with a type value), Text_CloseChoice and
       duel_effect_entry_control.c, and read back by func_80036C14.
       Named rather than described: the writes and the read prove a
       byte is here, not what it carries. */
    u8 field_62;
    u8 pad_63;
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
typedef char DuelEffectChannel_state_51_offset_must_be_0x51[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, state_51) == 0x51 ? 1 : -1
];
typedef char DuelEffectChannel_field_56_offset_must_be_0x56[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, field_56) == 0x56 ? 1 : -1
];
typedef char DuelEffectChannel_stream_58_offset_must_be_0x58[
    DUEL_EFFECT_OFFSET(DuelEffectChannel, stream_58) == 0x58 ? 1 : -1
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

#undef DUEL_EFFECT_OFFSET

/* The entry occupancy table: one byte per DUEL_EFFECT_OCCUPANCY_COUNT entry,
 * declared with that bound because both consumers already wrote it and the
 * constant is defined above. DuelEffect_ResetOccupancy walks it from the base,
 * func_80035D10 scans for the first zero, and func_80039AD4 clears the slot an
 * object names at its offset 0x10.
 *
 * Two hundred and forty bytes is well over the -G8 threshold, so unlike the
 * small arrays elsewhere in this tree the bound here costs nothing: it does
 * not move the object between sections. */
extern u8 D_800EAF08[DUEL_EFFECT_OCCUPANCY_COUNT];

extern DuelEffectChannel D_800EB0F8[DUEL_EFFECT_CHANNEL_COUNT];
/* D_800EB15C is D_800EB0F8[1] under its own name: 0x800EB0F8 + 0x64, one
 * DuelEffectChannel in. It stays a second name because how this address is
 * spelled is a codegen input, and two matched functions record the shape:
 * func_80031084.c has to hold the array base in a local, since writing
 * `&D_800EB0F8[1]` inline folds the record offset into the address and
 * drops the `addiu` retail keeps for the call argument; and
 * FreeDuel_UpdateScreen has to hold this symbol in a local, since writing
 * `D_800EB15C + 0x34` inline folds the offset into the relocation and the
 * base instruction disappears (free_duel_functions.csv,
 * notes/overlays/matching-patterns.md). Neither of those measures the name
 * against `&D_800EB0F8[1]` -- Main_RunTwoPlayerDuelSetup passes that form directly and
 * matches -- so what is established is that the four files using this name
 * match as written; whether they would also match through the array index
 * is not measured. */
extern DuelEffectChannel D_800EB15C;
/* D_800EB12C is the word at D_800EB0F8 + 0x34: the flags_34 (:82) and
 * field_36 (:83) halfwords of record 0 under one name. Script_RunTick
 * (script_run_tick.c:23) and Password_UpdateShopScreen (now a stored
 * candidate, src/candidates/password/func_8016A37C.c) each load it as a
 * word and test
 * `& 0x2008` against 0x2000, TEXT_BOX_FLAG_DONE (:14) in the first and the
 * literal in the second; retail loads it lui/lw (func_8002FA54.s:15-16).
 * func_8002EE94 (src/candidates/func_8002EE94.c) reads the low halfword
 * as `D_800EB0F8[0].flags_34`, and password README.md:146-147 calls the word
 * the slot-0 text flags. Both units also matched when the read was spelled
 * `*(u32 *)&D_800EB0F8[0].flags_34` (measured, one build each), so the name
 * is kept for the listings, the notes and the generated symbol lists that
 * carry it, not for codegen. The two units used to declare it `s32 []` and
 * `u32`; the `[]` was the -G8 placement device, which the .data arm does
 * now. */
#ifdef D_800EB12C_IN_DATA
extern u32 D_800EB12C __attribute__((section(".data")));
#else
extern u32 D_800EB12C;
#endif
extern DuelEffectEntry D_800EB288[DUEL_EFFECT_ENTRY_COUNT];

/* The selected card's guardian-star text id. func_80023144 stores guardian
 * star 1 or 2 plus 0x17 while it prepares the field-card display, and
 * func_80038070 forwards the byte to the shared text-command renderer.
 * Every access is sb or lbu. Retail addresses the candidate writer through
 * $at, while the matching reader is gp-relative, so the writer selects the
 * .data declaration and the reader takes the plain declaration. */
#ifdef D_8009B344_IN_DATA
extern u8 D_8009B344 __attribute__((section(".data")));
#else
extern u8 D_8009B344;
#endif

/* The field-card text selector. func_80023144 clears it on entry, sets 1
 * for an occupied record, 2 or 3 (3 when the record is face-down) for a
 * record in its side's second row (index within the side at or past
 * DUEL_FIELD_ROW_SIZE) and 0 again for a flagged first-row one;
 * func_800610E0 stores its mode argument into it before building a text
 * box; func_800389D8 adds twice its value to the word the object's +0x58
 * index selects; and func_800218F0 (still assembly) stores it twice.
 * notes/duel-card-record.md calls it a text-selector override. Values
 * 0..3, every access sb or lbu.
 *
 * Retail reaches it through $at in func_80023144 (six stores) with a
 * lui/lbu read in the same function, and gp-relative in func_800389D8, so
 * src/candidates/func_80023144.c defines the .data arm below;
 * duel_effect_object_commands.c takes the plain byte, and so does
 * func_800610E0.c, whose -G0 unit stores through $at either way. */
#ifdef D_8009B34E_IN_DATA
extern u8 D_8009B34E __attribute__((section(".data")));
#else
extern u8 D_8009B34E;
#endif

/* Its companion, set by the same constructor: 1 when the rank it computed
 * is non-zero, or-ed with 2 when D_8009B34E is non-zero, and read by
 * func_80038A44, which adds twice its value to the word the object's +0x58
 * index selects.
 * Same two units, same forms (four $at stores and one lui/lbu read in
 * func_80023144, gp-relative in func_80038A44), same arms. */
#ifdef D_8009B355_IN_DATA
extern u8 D_8009B355 __attribute__((section(".data")));
#else
extern u8 D_8009B355;
#endif

/* Stored by two functions and read by one. func_80023144 stores
 * func_80023090's result -- 4, 1 or 6 -- under
 * `if (D_8009B34E != 0)` in src/candidates/func_80023144.c;
 * func_8002A2F4 stores the byte at +0x54 of the object
 * TextBox_Create returned (`o` is u8 *) and then 4 when
 * `*(p + (n << 2) + 0x56) & 1` (src/candidates/func_8002A2F4.c:41-44);
 * func_80037DA4 stores it into `object[0x54]` when the opcode byte has bit
 * 0x10 (src/candidates/func_80037DA4.c). notes/duel-card-record.md:173-176
 * glosses the three results. No unit defines it; the address comes from the
 * generated tmp/splat/undefined_syms_auto.txt, and the nearest named symbol
 * above it is D_8009B322, at +2 (nothing is named at +1 there, in symbols.txt
 * or in c_symbols.ld).
 *
 * u8 because the one function that loads it, func_80037DA4, already declared
 * it u8 and matched, and the load is lbu (func_80037DA4.s:20, gp-relative).
 * Retail stores it through $at in func_80023144 (func_80023144.s:162-163,
 * gcc_2_8_1_g8_split), so src/candidates/func_80023144.c defines the .data
 * arm; func_8002A2F4's unit assembled at -G0 (gcc_2_8_1_cc_g8_as_g0_split:
 * compiler -G8, maspsx -G0), so its plain declaration was expanded through
 * $at by the assembler either way (func_8002A2F4.s:39-40, :48-49) -- #3859
 * moved it to src/candidates/ for exactly that profile; and func_80037DA4's
 * candidate takes the plain byte. Initial value not read. */
#ifdef D_8009B320_IN_DATA
extern u8 D_8009B320 __attribute__((section(".data")));
#else
extern u8 D_8009B320;
#endif

/* Stored by five C functions and read by one. func_80031CD4 stores the
 * list entry's id (card_list_text_boxes.c:19); func_80023144 stores
 * `id = (s16)record->card_id;` under `if (record->flags & 0x8000)`,
 * immediately after `D_8009B34E = 1;` (src/candidates/func_80023144.c:46);
 * func_800283F4 stores `id = gDuel_wViewerCardID;` (its candidate, :111);
 * func_8002A2F4 stores func_8002A6B8's result
 * (src/candidates/func_8002A2F4.c:27) and then, under `if (n != 0)`, stores
 * 0 when func_80029EB0's result `r` (:33) has `(r & 0x80) == 0` (:35-36);
 * func_80060E70 stores `id` (in its candidate). func_80037DA4 reads it,
 * plain and as the index in `gDuel_adwCardStats[gDuel_wSelectedCardID - 1]`
 * (in its candidate). Four functions still in assembly also store it:
 * func_8001B170.s:140-141, func_800218F0.s:202-203 and :235-236,
 * func_800262D4.s:379-380, func_8002ACA4.s:311-312.
 *
 * s16 because func_80037DA4, the only function that loads it, declared it s16
 * when it matched; the five loads are lh (func_80037DA4.s:27/:34/:56/:68/
 * :87). Two bytes at 0x8009B338 (symbols.txt:27), bounded above by
 * D_8009B33A at +2.
 *
 * Those five loads are %gp_rel and every C writer's sh goes through $at
 * (func_80031CD4.s:20-21, func_80023144.s:36-37, func_800283F4.s:102-103,
 * func_8002A2F4.s:13-14 and :26-27, func_80060E70.s:61-62), so the three
 * units whose profiles are -G8 at both the compiler and maspsx --
 * the src/candidates/ files for func_80023144, func_800283F4 and
 * func_80060E70 -- define the .data arm, and card_list_text_boxes.c, whose
 * unit assembles at -G0, takes the plain declaration, as the func_8002A2F4
 * candidate does.
 * Initial value not read. */
#ifdef GDUEL_WSELECTEDCARDID_IN_DATA
extern s16 gDuel_wSelectedCardID __attribute__((section(".data")));
#else
extern s16 gDuel_wSelectedCardID;
#endif

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

/* The entry-list rebuild flag. func_80039AD4 raises it -- its own header
 * calls it "the D_8009B330 rebuild flag so the next pass" picks the change up
 * -- and duel_effect_process_entries.c is the other end: it clears the flag
 * before walking the entries and tests `D_8009B330 != 0` to decide whether
 * the walk has to start again. */
extern s32 D_8009B330;

/* The repeat count for an effect run. duel_effect_play_sound_command.c
 * latches it from func_80036D3C's result, and duel_effect_state_callbacks.c
 * spends it: it tests the count, decrements it at the end of a run, and
 * repeats while the result is still `> 0`.
 *
 * Signed, because that is a post-decrement test. On a u16 `> 0` would be the
 * same test as the `!= 0` above it and the underflow would run away; the
 * signed compare is what makes the two tests differ. */
extern s16 D_8009B33C;

/* Per-scene state flags. Bit 0x80 is the run-once latch: every scene entry
 * point in this subsystem opens with the same
 * `if ((D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0)` test and
 * sets the bit inside, so the body runs on the first tick of a scene only.
 * dialog_transition.c and func_8003DA40.c additionally raise 0x20 and 0x40,
 * and the byte is cleared back to 0 when a scene is torn down. */
extern u8 D_8009B3C1;

/* The dialog mode byte, captured beside those flags. func_8003D300 is the
 * dialog-open initialiser: in one run it clears D_8009B3C1 above, zeroes
 * D_8009B3CF/D_8009B3DD, sets D_8009B3CC to 0x8000, and stores its own `value`
 * parameter here. Only bit 0 is established -- dialog_transition.c spends it
 * as `DuelEffect_CreateChannel((a & 1) | 0xD0, 0)`, so it selects the channel
 * flavour; the other seven bits have no reader in matched code.
 *
 * Plain, with no .data arm: the one remaining assembly reader, func_8003DC1C,
 * reaches it as `lbu %gp_rel(D_8009B3C7)($gp)`, so retail addresses it through
 * $gp like the small-data neighbours here. */
extern u8 D_8009B3C7;

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

#define DUEL_EFFECT_DIALOG_FLAG_CHOICE_OPEN 0x40
#define DUEL_EFFECT_DIALOG_FLAG_CREATED 0x80

/* The dialog request consumed by func_80028310, its two-stage latch, and the
 * active handler index captured by DuelEffect_UpdateState. These are separate
 * scalar symbols: the gaps between them remain unmodeled. */
extern u16 D_8009B244;
extern u8 D_8009B248;
extern u8 D_8009B24A;

/* The pending duel-effect request. DuelEffect_UpdateState reads it each
 * tick: zero is idle; otherwise the low bits are the effect id, which it
 * copies into D_8009B24A and then marks with 0x80 as started; func_800283F4
 * and func_80028310 raise 0x40 when the effect is finished, and the next
 * tick clears the byte back to 0. Stored 2 by build_deck_pane_input.c,
 * duel_update_card_pick_cursor.c and the main_menu overlay's
 * MainMenu_UpdateTradeScreen (now a build-integrated candidate,
 * src/candidates/main_menu/func_801821DC.c), 3 and 4 by func_80030E30.c,
 * and cleared by
 * Main_ResetFrontendRuntime and Main_RunCampaign. One byte, read lbu; the
 * next named byte is gCardGrid_bCursorColumn at 0x8009B258.
 * Retail reaches it through $gp in func_8002892C, func_800283F4 and
 * func_80028310, and through %hi/%lo everywhere else, including
 * func_8001BD88 and func_8001D670 (still assembly). func_80030E30.c,
 * main_run_campaign.c and duel_update_card_pick_cursor.c define the .data
 * arm below for that; build_deck_pane_input.c, src/candidates/func_800283F4.c,
 * func_80028310.c, main_reset_frontend_runtime.c and the build-integrated
 * MainMenu_UpdateTradeScreen candidate take the plain arm. */
#ifdef D_8009B254_IN_DATA
extern u8 D_8009B254 __attribute__((section(".data")));
#else
extern u8 D_8009B254;
#endif

s32 DuelEffect_UpdateState(void);

#endif
