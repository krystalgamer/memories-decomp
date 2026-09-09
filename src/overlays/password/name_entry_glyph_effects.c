#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../game/display_object_api.h"
#include "../../game/display_object_lifecycle.h"
#include "../../game/display_object_helpers.h"
#include "name_entry_keyboard.h"
#include "name_entry_state.h"

/* The sprite every function here works on. NameEntry_SpawnGlyphSprite makes
 * it and the four update callbacks step it, each installed into the object's
 * +0x24 slot; the fields below are the ones this screen reaches by name.
 *
 * +0x44/+0x46 carry two different meanings and the callback installed at
 * +0x24 decides which: NameEntry_UpdateGlyphPulse writes them as the sprite's
 * x and y scale, while the two tween callbacks read them as the destination
 * the sprite is sliding to. Spelled as offsets here rather than as a union
 * because nothing yet distinguishes the two uses at the type level. */
typedef struct {
    u8 pad_00[0x4];
    u32 flags;                    /* 0x04 */
    u8 pad_08[0x3C];
    s16 scale_x;                  /* 0x44, target x in the tween callbacks */
    s16 scale_y;                  /* 0x46, target y in the tween callbacks */
    u32 field_48;                 /* 0x48 */
    DuelEffectEntry *sourceGlyph; /* 0x4C, the node the sprite was made from */
    u8 pad_50[0xA];
    s16 savedSourceX;             /* 0x5A */
    u8 pad_5C[0x4];
    s16 frame;                    /* 0x60, also the tween callbacks' timer */
    u8 pad_62[0x5];
    u8 textBoxSlot;               /* 0x67 */
    u8 pad_68[0x2];
    u8 sequence;                  /* 0x6A */
} GlyphSprite;

/* The origin the cursor sprite is placed at when there is no glyph node. */
typedef struct {
    u8 pad0[48];
    s16 x;
    s16 y;
} Fixed;

extern Fixed *D_8016D404;
extern s32 func_800358FC(s32);
extern void func_80040510(void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);

/* One frame of the flash a glyph makes when it is picked on the keyboard: the
 * source node is parked off-screen for the twelve frames the sprite scales up
 * and back down, then restored. */
void NameEntry_UpdateGlyphPulse(u8 *sprite)
{
    GlyphSprite *obj = (GlyphSprite *)sprite;
    DuelEffectEntry *source;
    s32 step;
    s32 value;
    s16 frame;

    source = obj->sourceGlyph;
    if (func_80042B98((DisplayObjectLifecycle *)sprite) == 0) {
        if (source != 0) {
            obj->savedSourceX = source->x_0C;
            source->x_0C = 0x400;
        }
        obj->frame = 0;
        obj->flags &= ~GsROTOFF;
    }
    value = obj->frame;
    if (value >= 12) {
        func_8004036C(obj);
        return;
    }
    step = value;
    if (step >= 6) {
        step = 11 - step;
    }
    obj->scale_y = 4096 - step * 341;
    obj->scale_x = 4096 - step * 341;
    frame = obj->frame + 1;
    obj->frame = frame;
    if (frame >= 12) {
        if (source != 0) {
            source->x_0C = obj->savedSourceX;
        }
    }
}

/* One frame of a single shard: seeded once with a random sideways kick and an
 * upward one, then thrown until it falls off the bottom of the screen. */
void NameEntry_UpdateGlyphFragment(u8 *object)
{
    u8 flags;

    flags = object[0x6C];
    if ((flags & 0x80) == 0) {
        object[0x6C] = flags | 0x80;
        func_800429D8(object);
        *(s16 *)(object + 0x36) = func_800358FC(0x200) - 0x100;
        *(s16 *)(object + 0x38) = -func_800358FC(0x180);
    }
    *(s16 *)(object + 0x36) = func_80042B08(*(s16 *)(object + 0x36), 8);
    *(s16 *)(object + 0x38) = func_80042AD8(*(s16 *)(object + 0x38), 0x800, 0x40);
    func_80042A78(object);
    if (*(s16 *)(object + 0x32) >= 0xF0) {
        func_8004036C(object);
    }
}

/* Breaks one 16x16 glyph into sixteen 4x4 shards, each on
 * NameEntry_UpdateGlyphFragment, and retires the glyph itself. */
void NameEntry_UpdateGlyphShatter(u8 *object)
{
    u8 *piece;
    s32 dx;
    s32 dy;

    if (func_80042B98((DisplayObjectLifecycle *)object) == 0) {
        for (dy = 0; dy < 0x10; dy += 4) {
            for (dx = 0; dx < 0x10; dx += 4) {
                piece = func_800400AC(func_8004002C(), 1);
                if (piece != 0) {
                    func_80040510(piece,
                                  *(s16 *)(object + 0x30) + dx,
                                  *(s16 *)(object + 0x32) + dy,
                                  4, 4,
                                  object[0x5C] + dx,
                                  object[0x5D] + dy,
                                  object[0x66],
                                  *(u16 *)(object + 0x40),
                                  *(u16 *)(object + 0x42));
                    piece[0x6C] = 3;
                    *(NameEntryGlyphUpdate *)(piece + 0x24) =
                        NameEntry_UpdateGlyphFragment;
                }
            }
        }
    } else {
        func_8004036C(object);
    }
}

/* Slides the caret to +0x44/+0x46 over the +0x60 frames it was given, then
 * snaps to the target and uninstalls itself. */
void NameEntry_UpdateCaretTween(u8 *object)
{
    u8 flags;
    s16 remaining;

    flags = object[0x6C];
    if ((flags & 0x80) == 0) {
        object[0x6C] = flags | 0x80;
        func_800429D8(object);
        *(s16 *)(object + 0x36) =
            ((*(s16 *)(object + 0x44) - *(s16 *)(object + 0x30)) << 8) /
            *(s16 *)(object + 0x60);
        *(s16 *)(object + 0x38) =
            ((*(s16 *)(object + 0x46) - *(s16 *)(object + 0x32)) << 8) /
            *(s16 *)(object + 0x60);
    }
    func_80042A78(object);
    remaining = *(u16 *)(object + 0x60) - 1;
    *(s16 *)(object + 0x60) = remaining;
    if (remaining <= 0) {
        *(s32 *)(object + 0x24) = 0;
        object[0x6C] = 0;
        *(s32 *)(object + 0x30) = *(s32 *)(object + 0x44);
    }
}

/* Flies an accepted glyph from the keyboard to the name field on the same
 * tween prologue as the caret, dropping a fading copy of itself each frame.
 * On arrival it looks the destination glyph up and shatters it. */
void NameEntry_UpdateGlyphTransfer(u8 *w)
{
    u8 *o;
    DuelEffectEntry *node;
    s32 n;

    if ((w[0x6C] & 0x80) == 0) {
        w[0x6C] |= 0x80;
        func_800429D8(w);
        *(s16 *)(w + 0x36) =
            ((*(s16 *)(w + 0x44) - *(s16 *)(w + 0x30)) << 8) / *(s16 *)(w + 0x60);
        *(s16 *)(w + 0x38) =
            ((*(s16 *)(w + 0x46) - *(s16 *)(w + 0x32)) << 8) / *(s16 *)(w + 0x60);
    }
    if ((w[0x6C] & 0x40) != 0) {
        n = *(u16 *)(w + 0x60) - 1;
        *(s16 *)(w + 0x60) = n;
        if ((s16)n <= 0) {
            func_8004036C(w);
        }
        return;
    }
    func_80042A78(w);
    n = *(u16 *)(w + 0x60) - 1;
    *(s16 *)(w + 0x60) = n;
    if ((s16)n > 0) {
        o = func_800400AC(func_8004002C(), 1);
        func_80040510(o, *(s16 *)(w + 0x30), *(s16 *)(w + 0x32), 16, 16,
                      w[0x5C], w[0x5D], w[0x66], *(u16 *)(w + 0x40),
                      *(u16 *)(w + 0x42));
        *(u32 *)(o + 0x0C) = 0x606060;
        *(s16 *)(o + 0x60) = 6;
        *(void **)(o + 0x24) = func_80042BC0;
        *(u32 *)(o + 4) |= (GsALON | GsAONE);
        return;
    }
    w[0x6C] |= 0x40;
    *(u32 *)(w + 0x30) = *(u32 *)(w + 0x44);
    /* The reload of the timer is written inside the last argument, not as the
       statement before the call.  A statement is emitted ahead of every
       argument, which leaves the two instructions for it at the head of the
       block and pushes the zero for a2 to the end; carried with the argument
       they are emitted after a1 is computed, which is the order the target
       has.  See notes/overlays/README.md. */
    node = TextBox_GetGlyphAt(3, D_8016D42C << 4, (*(s16 *)(w + 0x60) = 2, 0));
    if (node == 0) {
        return;
    }
    o = NameEntry_SpawnGlyphSprite(3, node);
    *(NameEntryGlyphUpdate *)(o + 0x24) = NameEntry_UpdateGlyphShatter;
    o[0x6C] = 5;
}

/* Makes the sprite for one glyph node: maps the node's Shift-JIS code to a
 * cell in the 16x16 font page, places it at the text box's origin plus the
 * node's local position, and stamps the spawn order into +0x6A. With no node
 * it makes the cursor sprite at D_8016D404's position instead. */
void *NameEntry_SpawnGlyphSprite(s32 slot, DuelEffectEntry *w)
{
    /* The nudge is s16 rather than s32 on purpose.  Its only values are 0 and
       plus or minus two, so an (s16) cast written on an s32 would be proved
       redundant and deleted; declared s16, the widening at the use survives
       and picks up the register already holding the sprite width. */
    s16 shift;
    /* Texture origin, a pair of byte coordinates.  Declared u8 they are passed
       without a mask; declared wider they would need one. */
    u8 u;
    u8 v;
    s32 code;
    GlyphSprite *obj;
    DuelEffectChannel *p;
    s32 i;

    shift = 0;
    u = 0;
    v = 0;
    /* Preserve the pre-null-test read; this is not a null-safe C contract. */
    code = w->code_00;
    obj = func_800400AC(func_8004002C(), 1);
    if (w != 0) {
        p = &D_800EB0F8[slot];
        switch (code) {
        case 0x8171: case 0x8173: code = 0x8183; break;
        case 0x8172: case 0x8174: code = 0x8184; break;
        }
        if ((u32)(code - 0x824F) < 76) {
            if ((u32)(code - 0x8259) < 7 || (u32)(code - 0x827A) < 7) { u = 0; v = 120; }
            else {
                u = (code & 0xF) << 4;
                v = (((code - 0x8240) >> 4) << 4) + 72;
            }
        } else {
            s32 tbl[30] = {
                0x8149, 0x8168, 0x8194, 0x8190, 0x8193, 0x8195, 0x8166, 0x8169,
                0x816A, 0x8196, 0x817B, 0x8143, 0x817C, 0x8144, 0x815E, 0x8146,
                0x8147, 0x8183, 0x8181, 0x8184, 0x8148, 0x8140, 0x83BF, 0x83C0,
                0x81C1, 0x81A9, 0x81A8, 0x81BC, 0x81BD, -1,
            };
            i = 0;
            for (;;) {
                if (code == tbl[i]) {
                    if (i < 15) { u = i << 4; v = 72; }
                    else if (i < 22) { u = (i << 4) - 96; v = 88; }
                    else {
                        switch (code) {
                        case 0x83BF: u = 208; v = 152; break;
                        case 0x83C0: u = 224; v = 152; break;
                        case 0x81C1: u = 240; v = 152; break;
                        case 0x81A9: u = 176; v = 120; break;
                        case 0x81A8: u = 192; v = 120; break;
                        case 0x81BC: u = 224; v = 120; shift = 2; break;
                        case 0x81BD: u = 240; v = 120; shift = -2; break;
                        }
                    }
                    goto draw;
                }
                if (tbl[i] < 0) { break; }
                i++;
            }
            u = 0;
            v = 120;
        }
draw:
        func_80040510(obj, p->field_3C + w->x_0C + shift, p->field_40 + w->y_0E,
                      16, 16, u, v, 10, 640, p->field_54 + 232);
        obj->sequence = D_8016D408;
        D_8016D408 = D_8016D408 + 1;
    } else {
        func_80040510(obj, D_8016D404->x, D_8016D404->y, 16, 16, 128, 128, 23,
                      256, 240);
    }
    obj->field_48 = 0x80008;
    obj->textBoxSlot = slot;
    obj->sourceGlyph = w;
    return obj;
}
