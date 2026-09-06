#include "../types.h"
#include "../psyq/libgte.h"

/* Clears D_8009B260's bit 0x1, then walks all 8 D_800EAD88[] records. For
   each record whose f1C flags byte has 0x80 set: raises D_8009B260's bit 0
   unless f1C also has 0x20 set; clears D_8009B261; if f1C's 0x40 bit isn't
   set yet, sets it and resets f1A to -1; copies D_800E9D90.f4/f8 into
   f18record's fC/f8, stashes the record pointer at D_8009B264, and calls
   func_801462B0(f18, the ORIGINAL f1A read before the possible reset,
   f14, record) followed by SetGeomOffset(0,0) and SetGeomScreen using
   D_800F2848's projection field; if D_8009B261 ends up 1, clears f1C back
   to 0. Returns D_8009B260's bit 0.

   savedF1A must be s32, not the field's true s16 type -- declaring it s16
   makes gcc emit an unsigned reload + manual sign-extend at the call site
   instead of the target's plain signed halfword load. */
struct Rec {
    char pad0[0x8];
    s32 f8;
    s32 fC;
    char pad1[0x14 - 0x10];
    s32 f14;
    s16 f18;
    s16 f1A;
    u8 f1C;
    char pad2[0x20 - 0x1D];
};

struct D800E9D90Type {
    char pad[0x4];
    s32 f4;
    s32 f8;
};

struct D800F2848Type {
    char pad[0xE];
    s16 fE;
};

extern struct Rec D_800EAD88[8];
extern struct D800E9D90Type D_800E9D90;
extern struct D800F2848Type D_800F2848;
extern u8 D_8009B260;
extern u8 D_8009B261;
extern struct Rec *D_8009B264;
extern void func_801462B0(s16 a0, s16 a1, s32 a2, struct Rec *a3);

s32 func_8002C6C8(void) {
    struct Rec *rec;
    s32 i;

    D_8009B260 = D_8009B260 & 0xFE;

    rec = &D_800EAD88[0];
    for (i = 8; i != 0; i--) {
        u8 flags1 = rec->f1C;
        if (flags1 & 0x80) {
            u8 flags2;
            s32 savedF1A;

            if (!(flags1 & 0x20)) {
                D_8009B260 = D_8009B260 | 1;
            }
            D_8009B261 = 0;
            flags2 = rec->f1C;
            savedF1A = rec->f1A;
            if (!(flags2 & 0x40)) {
                rec->f1C = flags2 | 0x40;
                rec->f1A = -1;
            }
            rec->fC = D_800E9D90.f4;
            D_8009B264 = rec;
            rec->f8 = D_800E9D90.f8;
            func_801462B0(rec->f18, savedF1A, rec->f14, rec);
            SetGeomOffset(0, 0);
            SetGeomScreen(D_800F2848.fE);
            if (D_8009B261 == 1) {
                rec->f1C = 0;
            }
        }
        rec++;
    }

    return D_8009B260 & 1;
}
