#include "../../types.h"
#include "name_entry_keyboard.h"

typedef struct {
    u8 pad0[60];
    s16 ox;
    s16 unk3E;
    s16 oy;
    u8 pad42[18];
    u8 pal;
    u8 pad55[15];
} Panel;

typedef struct {
    u8 pad0[72];
    u32 f72;
    void *f76;
    u8 pad80[23];
    u8 f103;
    u8 pad104[2];
    u8 f106;
} Obj;

typedef struct {
    u8 pad0[48];
    s16 x;
    s16 y;
} Fixed;

extern Panel D_800EB0F8[];
extern Fixed *D_8016D404;
extern u8 D_8016D408;
extern void *func_8004002C(void);
extern Obj *func_800400AC(void *, s32);
extern void func_80040510(Obj *, s32, s32, s32, s32, s32, s32, s32, s32, s32);

void *NameEntry_SpawnGlyphSprite(s32 slot, NameEntryGlyphNodeView *w)
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
    Obj *obj;
    Panel *p;
    s32 i;

    shift = 0;
    u = 0;
    v = 0;
    /* Preserve the pre-null-test read; this is not a null-safe C contract. */
    code = w->code;
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
        func_80040510(obj, p->ox + w->x + shift, p->oy + w->y,
                      16, 16, u, v, 10, 640, p->pal + 232);
        obj->f106 = D_8016D408;
        D_8016D408 = D_8016D408 + 1;
    } else {
        func_80040510(obj, D_8016D404->x, D_8016D404->y, 16, 16, 128, 128, 23,
                      256, 240);
    }
    obj->f72 = 0x80008;
    obj->f103 = slot;
    obj->f76 = w;
    return obj;
}
