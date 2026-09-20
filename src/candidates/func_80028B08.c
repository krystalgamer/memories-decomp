#include "../types.h"
#include "../game/text_encode_decimal_digits.h"
#include "../game/display_object_projection.h"
#include "../game/duel_effect_resource_record.h"
#include "../psyq/libgs.h"
#include "../game/display_object.h"
#include "../game/card_preview_callbacks.h"
#include "../game/sprite_primitive.h"
#define DISPLAY_OBJECT_PACKET_SUBMIT_CARD_LIST
#include "../game/display_object_packet_submit.h"
#include "../game/card_constants.h"
#include "../ygo_types.h"
/*
 * Current best under gcc_2_8_1_g8_split: 384/384 instructions at exact length,
 * an empty opcode census, 371 of 384 aligned on opcode and registers in 4
 * structural blocks, 15 raw words differing with relocations masked, 7 with
 * the register fields masked as well, and a shift-aware structural distance
 * of 8 (difflib over the register-masked words). The previous state was 384,
 * census 0, 4 blocks, 369 aligned, 17 raw, 7 register-masked, shift-aware 8;
 * the one before it was 384, census 0, 13 structural blocks, 356 aligned, 54
 * raw, 46 register-masked, shift-aware 36 -- and its header said "register
 * choices only". It was not:
 * read by raw words, the residue was seven zones of ORDER, and each fell to a
 * source lever. Read the raw words, not the aligner (#5358 erases a0-a3).
 *
 * Levers, each measured alone against the state below it and then combined:
 *  - the three scratchpad pointers are assigned EXT, PRM, CTX: retail
 *    materialises 0x1F800398 into s7 before 0x1F800344 into s6, and the
 *    order of the three assignments is what decides it (raw 54 -> 50);
 *  - the uv halfword is read as `*(u16 *)((u8 *)obj + 0x5C)` and written
 *    BEFORE the tpage store, and the same for the 0x5E read after the extent
 *    copy: retail never hoists a load from obj above the PRM store that
 *    precedes it, and the plain member read is hoisted the moment the two
 *    statements are swapped (the swap alone is 72 raw and shifts the whole
 *    first block); the byte-address cast is the address gcc cannot
 *    disambiguate, so the load stays where the source puts it. Both together
 *    54 -> 42 raw, shift-aware 36 -> 20;
 *  - in the 0x77 block `PRM->uv.b.lo = 0;` is written FIRST, before cx and
 *    the extent constant (permuter find): 42 -> 34;
 *  - in the last block the cx store is written before cy, with the sum as
 *    `(u8)tile + win->field_40.h.field_40`, and the cx sum reads the byte
 *    back through `PRM->uv.b.lo` instead of casting `tile`: the read-back is
 *    forwarded as retail's `andi 0xff` right after the `sb` (34 -> 27,
 *    shift-aware 16 -> 10). A named `(u8)tile` local at the same point, a
 *    `u8` local, a chained `tile = PRM->uv.b.lo = ...` (which folds to
 *    `andi 0xf0`, a false gain the aligner likes) and `u8 tile` are all
 *    worth nothing or worse;
 *  - `i = rec->field_32 + rec->field_36;` inline after the second submit,
 *    with `wrap` never reassigned: retail reads field_36 AFTER the call
 *    (words 203-204) and the `wrap = rec->field_36;` before it kept the value
 *    in s0 across the call. Alone this costs the 0xF8 constant its s4 (the
 *    counter's pseudo no longer occupies s0 across the call, so 0xF8 takes
 *    s0), and the partner is the next lever;
 *  - `PRM->cxcy.h.cy = 0xF8;` in the second block is written right after
 *    `nye = win->field_30.h.field_32;`, before the w/y/h stores: the 0xF8
 *    pseudo is then born while 0x60 still holds s0, conflicts with it, and
 *    gets s4 as retail has; the store itself is scheduled among the three.
 *    Eight positions were measured: at the block top or after the CTX
 *    read-modify-write it is 16 raw with the store emitted early; after x or
 *    nye it is 7 register-masked words; after w, y, h or the uv.b.hi bump it
 *    is 16-18 masked and worse on every key;
 *  - `hi = PRM->attribute;` read into the dead `hi` (the 0xF0000 name from
 *    the first block) before the uv.b.hi bump, and the attribute mask written
 *    against `hi` (permuter find): shift-aware 11 -> 7 at a cost in
 *    register-only words (the borrowed name rotates t0/t1/t2 and moves the
 *    0xF0000 constant into t2 in the first block). A FRESH name is worth
 *    nothing; `tile` borrows to the same figures; f0c, w14, f4, ny, nab and
 *    ny2 are all worse.
 *
 * MEASURED AND DEAD on this base: `volatile` on PRM (+4), on CTX (nothing),
 * on all three; a `do { } while (0)` pin or a plain local for the 14 or the
 * 0xF8 at the block top (+1/+2 and 200+ raw); the h store before or after
 * the w store, the y store, or first in the block (no change: the scheduler
 * decides the store order, not the source); the 0x90009 extent store as a
 * pinned or plain local, as two halfword member stores (+1), or after cy or
 * y (no change); byte-address casts on the extent store and on the uv.b.lo /
 * uv.b.hi stores of the last block (no change); the x store after y (worse);
 * `PRM->uv.b.hi += 0x60` (no change); a `u8 *` local for the tpage read
 * (worse); the extent copy before the uv copy with a plain member read (-1).
 * From the earlier header, on the aligner and re-read here: the uv store's
 * "monotone gradient" was the aligner counting a shifted block as aligned;
 * shared constants as locals stay worse; the 0xE constant as a local is inert.
 *
 *  - in the last block the cx sum is written `win->field_40.h.field_40 +
 *    PRM->uv.b.lo` (permuter find): the addu takes its operands in retail's
 *    order and the pair lands in t0/v1 (words 363 and 365): 17 -> 15 raw,
 *    369 -> 371 aligned, every other key unchanged.
 *
 * THE SECOND BLOCK, read off the -dS/-dR dumps (tmp/agents dumps of this
 * source, sched1 block 5 = insns 185-381) rather than swept: the three
 * halfword stores (w at 8, h at 10, cy at 18) have equal priority at sched2
 * and are ordered by LUID, which is sched1's output order, which is the
 * source order; writing them w, y, h, cy gives retail's w, h, cy (117-119).
 * The 14 and the 0xF8 are single-set pseudos (the 0xF8 shared by cse with
 * the stores at 180 and 202, so it crosses three calls), and sched1's
 * adjust_priority gives such an insn LAUNCH_PRIORITY when it becomes ready
 * (birthing_insn_p: REG_N_SETS == 1), which glues each `li` directly above
 * its store; at sched2 a `li` floats to the block top only when its hard
 * register is free above it, and retail's t1 and s4 are. With the cy store
 * written last the 0xF8 is born after the w store, no longer conflicts with
 * the 0x60 in s0, takes s0, and `sh s0,8(s1)` pins it (v61: 26 raw, 17
 * masked); with the cy store early (this source) it conflicts and takes s4
 * but the store is early. The 14 takes v0 or v1 because the uv.b.hi load
 * (v0) and the attribute load (v1) are scheduled below it; retail has both
 * above and the 14 in t1. MEASURED AND DEAD on the w,y,h,cy order: the 0xF8
 * in a local born after nye and stored late (+1, the launch glues it); a
 * second live set of that local at the 0xF8 site of the `< 0x14` arm (cse
 * folds a single-use set back into a constant store and the count returns
 * to one; a two-use version re-materialises the 180/202 stores, +2); a named
 * 14 with a second set in the first block (breaks the cse sharing of the
 * 0x60, `li v0,96`, +1); a fresh `attr` for the attribute (v1 as retail, 24)
 * or the read-modify-write inline (24); the attribute read, the uv bump or
 * the whole attribute statement written above the stores (22, 31, 31, 26);
 * decomp-permuter from the fresh-`attr` state (one output, the cx operand
 * swap above) and from this state (nothing in nine minutes).
 *
 * Residual, 15 words: the 14 and the 0xF8 are materialised in the other
 * order at the top of the second block (retail `li t1,14` then `li s4,248`;
 * here 248 first and 14 into v1) and the cy store is emitted before the w
 * and h stores instead of after them (117-119); the 0x90009 `lui` sits one
 * word before the x load instead of after it (302-303); and register-only
 * words: the 0xFEFFFFFF mask in t1 where retail has t2, the attribute in t2
 * where retail has v1, and the 0xF0000 constant in t2 where retail has v0.
 *
 * HOW THE 2026-09 REGRESSION WAS MISSED, kept from the earlier header so it
 * is not repeated: align_functions.py prints `target N instructions,
 * candidate M (+d)` as its FIRST line; a harness that greps only the
 * `aligned on opcode and registers:` line discards it and reports figures
 * computed across a length error. No gate catches this. Read the first line.
 */
void func_80028B08(DisplayObject *obj, s32 arg1) {
    u8 buf1[5];
    u8 buf2[5];
    Func80028B08Extra *EXT;
    SpritePrim *PRM;
    Func80028B08Ctx *CTX;
    DisplayObject *win;
    DuelEffectResourceRecord *rec;
    s32 arg;
    s32 i;
    u16 flags;
    u32 f4;
    u32 tile;
    s32 wrap;
    s32 ny2;
    s32 nab;
    s32 hi;
    s32 ny;
    s32 nye;
    u32 f0c;
    s32 w14;

    wrap = 0xFFFF;
    win = (DisplayObject *)obj->field_54;
    if ((obj->attribute & GsDOFF) != 0) {
        return;
    }
    flags = win->flags;
    if ((flags & 0x40) == 0) {
        return;
    }
    EXT = (Func80028B08Extra *)0x1F800398;
    PRM = (SpritePrim *)0x1F800320;
    CTX = (Func80028B08Ctx *)0x1F800344;
    arg = (((s16)win->field_14 - 1) & 0xFFFF) | 0x10000;
    if (flags & 0x4) {
        obj->field_20.word = win->field_20.word;
        f4 = obj->attribute & ~GsROTOFF;
        obj->field_44.word = win->field_44.word;
        obj->attribute = f4;
        f4 = f4 | (win->attribute & 0x08000000);
        obj->attribute = f4;
        if (func_80041F90(
                (struct DisplayObject *)obj, (s16)win->field_30.h.field_30 + (s16)win->field_18,
                (s16)win->field_30.h.field_32 + (s16)win->field_1A, (struct ProjectionOut *)EXT
            ) <= 0) {
            return;
        }
        w14 = (s16)win->field_14;
        f0c = win->field_0C;
        do {
            CTX->field_3 = 9;
        } while (0);
        hi = 0xF0000;
        arg = ((w14 - 1) & 0xFFFF) | hi;
        *(u32 *)&CTX->field_4 = f0c;
        CTX->field_7 = 0x2C;
    }

    PRM->attribute = obj->attribute;
    PRM->xy.h.x = win->field_30.h.field_30 + 0x13;
    ny = win->field_30.h.field_32;
    PRM->extent.wh.w.word = 0x66;
    PRM->extent.wh.h = 0x60;
    PRM->xy.h.y = ny + 0x32;
    PRM->rgb = win->field_0C;
    *(u32 *)&PRM->cxcy = obj->field_40.word;
    PRM->uv.word = *(u16 *)((u8 *)obj + 0x5C);
    PRM->tpage = obj->field_66;
    DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);

    CTX->field_7 = CTX->field_7 | 2;
    PRM->xy.h.x = win->field_30.h.field_30 + 0xC;
    nye = win->field_30.h.field_32;
    PRM->cxcy.h.cy = 0xF8;
    PRM->extent.wh.w.word = 0x60;
    PRM->xy.h.y = nye + 0xE;
    PRM->extent.wh.h = 0xE;
    hi = PRM->attribute;
    PRM->uv.b.hi = PRM->uv.b.hi + 0x60;
    PRM->attribute = (hi & 0xFEFFFFFF) | 0x60000000;
    PRM->cxcy.h.cx = 0x1E0;
    DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);

    EXT->field_4 = 0;
    rec = &D_800EA0E8[obj->field_67];
    PRM->tpage = 0x1F;
    PRM->cxcy.h.cx = PRM->cxcy.h.cx + 0x10;
    PRM->xy.h.x = win->field_30.h.field_30 + obj->field_30.h.field_30;
    PRM->xy.h.y = win->field_30.h.field_32 + obj->field_30.h.field_32;
    *(u32 *)&PRM->extent = obj->field_3C.word;
    PRM->uv.word = *(u16 *)((u8 *)obj + 0x5E);
    if (obj->field_68 < 0x14) {
        if (rec->field_3C & 0x80) {
            PRM->cxcy.h.cy = PRM->cxcy.h.cy + 1;
        }
        DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);
        PRM->cxcy.h.cy = 0xF8;
        PRM->uv.b.hi = PRM->uv.b.hi + *(u8 *)&PRM->extent.wh.h;
        PRM->xy.h.y = PRM->xy.h.y + (PRM->extent.wh.h + wrap);
        if (rec->field_3C & 0x40) {
            PRM->cxcy.h.cy = 0xF9;
        }
        DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);
        PRM->cxcy.h.cy = 0xF8;

        i = rec->field_32 + rec->field_36;
        if (i > CARD_STAT_MAX) {
            i = CARD_STAT_MAX;
        }
        Text_EncodeDecimalDigits(i, 4, buf1);
        i = rec->field_34 + rec->field_38;
        if (i > CARD_STAT_MAX) {
            i = CARD_STAT_MAX;
        }
        Text_EncodeDecimalDigits(i, 4, buf2);

        PRM->uv.b.hi = (PRM->uv.b.hi & 0x80) + 0x10;
        PRM->xy.h.x = win->field_30.h.field_30 + 0x61;
        PRM->xy.h.y = win->field_30.h.field_32 + 0x9D;
        *(u32 *)&PRM->extent = 0x000D0006;
        if (rec->field_3C & 0x80) {
            PRM->cxcy.h.cy = 0xF9;
        }
        i = 3;
        do {
            PRM->uv.b.lo = buf1[i] * 6 + 0x10;
            DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);
            PRM->xy.h.x = PRM->xy.h.x + 6;
            i--;
        } while (i >= 0);

        PRM->xy.h.x = win->field_30.h.field_30 + 0x61;
        nab = win->field_30.h.field_32;
        PRM->cxcy.h.cy = 0xF8;
        PRM->xy.h.y = nab + 0xAB;
        if (rec->field_3C & 0x40) {
            PRM->cxcy.h.cy = 0xF9;
        }
        i = 3;
        do {
            PRM->uv.b.lo = buf2[i] * 6 + 0x10;
            DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);
            PRM->xy.h.x = PRM->xy.h.x + 6;
            i--;
        } while (i >= 0);

        PRM->xy.h.x = win->field_30.h.field_30 + 0x77;
        ny2 = win->field_30.h.field_32 + 0x20;
        PRM->uv.b.lo = 0;
        PRM->cxcy.h.cx = 0x1C0;
        *(u32 *)&PRM->extent = 0x00090009;
        PRM->cxcy.h.cy = 0xF8;
        PRM->xy.h.y = ny2;
        if (rec->field_3A != 0) {
            i = 0;
            do {
                DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);
                PRM->xy.h.x = PRM->xy.h.x - 9;
                i++;
            } while (i < (s32)rec->field_3A);
        }
    } else {
        DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);
    }

    PRM->xy.h.x = win->field_30.h.field_30 + 0x6E;
    *(u32 *)&PRM->extent = 0x00100010;
    PRM->xy.h.y = win->field_30.h.field_32 + 0xD;
    tile = rec->field_3B << 4;
    PRM->uv.b.lo = tile;
    PRM->uv.b.hi = PRM->uv.b.hi & 0x80;
    PRM->cxcy.h.cx = win->field_40.h.field_40 + PRM->uv.b.lo;
    PRM->cxcy.h.cy = 0xFF;
    DisplayObject_SubmitPacket(PRM, CTX, arg1, arg, EXT);
}
