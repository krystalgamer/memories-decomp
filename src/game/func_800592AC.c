#include "../types.h"
#include "../psyq/libgte.h"
#include "model.h"

/* arg2's transform block: unk0 flag, a 0x20-byte MATRIX at +4, then the
   computed rotation angles at +0x44/+0x46/+0x48, and an id/handle at +0x4C. */
typedef struct {
    s32 f0;
    MATRIX f4;
    char pad24[0x44 - 0x24];
    s16 f44;
    s16 f46;
    s16 f48;
    char pad4A[0x4C - 0x4A];
    s32 f4C;
} Obj;

extern MATRIX *func_800889C0(SVECTOR *r, MATRIX *m);
extern void func_80088E50(SVECTOR *r, MATRIX *m);
extern void *func_8008E3D0(void *p, s32 v, u32 n);
extern u16 D_8009B47A;
extern u16 D_8009B47C;

/* Builds arg2's rotation matrix from a camera-relative direction vector
   (derived from D_8009B47A/D_8009B47C) combined with the per-slot base
   angles at D_800F2C40[arg0]+0xD18, then stamps arg2's id field with arg1. */
void func_800592AC(s32 arg0, s32 arg1, Obj *arg2) {
    SVECTOR packed;
    SVECTOR raw;
    MATRIX sp20;
    ModelSlot *slot;

    func_8008E3D0(&raw, 0, 8);
    raw.vy = (s16)(0x1000 - D_8009B47A);
    raw.vz = (s16)D_8009B47C;

    slot = &D_800F2C40[arg0];

    packed = raw;

    arg2->f0 = 0;
    arg2->f44 = (s16)(0x1000 - slot->field_D18->field_44);
    arg2->f46 = (s16)(0x1000 - slot->field_D18->field_46);
    arg2->f48 = (s16)(0x1000 - slot->field_D18->field_48);

    func_800889C0((SVECTOR *)&arg2->f44, &arg2->f4);
    func_80088E50(&packed, &sp20);
    MulMatrix(&arg2->f4, &sp20);

    arg2->f4.t[2] = 0;
    arg2->f4.t[1] = 0;
    arg2->f4.t[0] = 0;
    arg2->f4C = arg1;
}
