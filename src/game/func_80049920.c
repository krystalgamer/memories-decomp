#include "../types.h"

extern u8 *D_8009B458;
extern s32 D_80011434[];

void SpuSetKey(long on_off, unsigned long voice_bit);
s32 func_80077090(s32 arg0);

/* Second pass over the 0x28-byte records at D_8009B458: for each record
 * whose +0x18D counter is set and whose channel reports state 3, either
 * bumps the counter or, once it reaches 2, drains the channel through the
 * SpuSetKey/func_80077090 pair, clears the counter and ORs the channel mask
 * into one final SpuSetKey call. */
void func_80049920(void) {
    u8 *base;
    s32 i;
    s32 mask;
    s32 off;
    s32 *tbl;
    s32 *q;
    s32 v;
    s32 two;
    u8 b;
    u8 *p;
    s32 z;
    u8 *b1;
    u8 *b2;

    mask = 0;
    base = D_8009B458;
    i = 0;
    mask = i;
    z = 0;
    if (*(s16 *)(base + 0x510) > z) {
        two = 2;
        off = 0;
        tbl = D_80011434;
        do {
            if (*(base + off + 0x18D) != 0) {
                if (func_80077090(*tbl) == 3) {
                    b1 = D_8009B458;
                    p = b1 + off;
                    b = p[0x18D];
                    if (b >= 2) {
                        mask |= *tbl;
                        q = tbl;
                        do {
                            SpuSetKey(0, *q);
                            v = func_80077090(*q);
                        } while (v != two && v != 0);
                        b2 = D_8009B458;
                        *(b2 + off + 0x18D) = 0;
                    } else {
                        p[0x18D] = b + 1;
                    }
                }
            }
            off += 0x28;
            base = D_8009B458;
            tbl++;
        } while (++i < *(s16 *)(base + 0x510));
    }
    if (mask != 0) {
        SpuSetKey(0, mask);
    }
}
