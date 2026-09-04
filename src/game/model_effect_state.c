#include "../types.h"

typedef struct StatRec {
    s16 f0;
    s16 f2;
    s16 f4;
    s16 f6;
} StatRec;

typedef struct Buf {
    char pad[6];
    s16 f6;
} Buf;

extern StatRec D_80091570[];
extern signed char D_8009B07A;
extern unsigned char D_8009B07B;
extern unsigned char D_8009B07C;

extern void func_80059000(s32 a0, Buf *a1);
extern void func_8005D994(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5);

int func_8005F564(void)
{
    if (D_8009B07A < 0) {
        return 0;
    }
    return D_8009B07A++ > 0;
}

void func_8005F588(int value)
{
    if (D_8009B07B != 1 || D_8009B07C != 1) {
        if (value == 0) {
            D_8009B07A = -1;
        } else {
            D_8009B07A = 0;
        }
    }
}

/* If D_8009B07B==1 and D_8009B07C matches it, bail early. Otherwise reads
   D_80091570[arg1].f0 as a base stat value; if arg0 (level?) < 2, scales
   the stat by a growth ratio derived from func_80059000's output clamped
   to >=50 then offset by -300, divided by 750, and adds the delta. Passes
   the (possibly adjusted) value plus f2/f4 on to setup_positional_sfx. */
void func_8005F5C8(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    StatRec *rec = &D_80091570[arg1];
    s32 s0;

    if (D_8009B07B == 1 && D_8009B07C == D_8009B07B) {
        return;
    }

    s0 = rec->f0;

    if (arg0 < 2) {
        Buf buf;
        s32 v1;

        func_80059000(arg0, &buf);
        if (buf.f6 < 0x32) {
            buf.f6 = 0x32;
        }
        buf.f6 -= 300;
        v1 = buf.f6;
        if (v1 != 0) {
            s32 divisor = 750;
            s32 half = (v1 > 0) ? (s0 / 2) : s0;
            s0 += (v1 * half) / divisor;
        }
    }

    func_8005D994(arg0, s0, rec->f2, rec->f4, arg2, arg3);
}
