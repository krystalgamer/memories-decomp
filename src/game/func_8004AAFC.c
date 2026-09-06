#include "../types.h"

extern u8 *D_8009B458;
extern s32 D_80011434[];

void SpuGetAllKeysStatus(char *status);
void func_8004A43C(u8 *arg0, s32 arg1);
void func_8004A7C0(s32 arg0, s32 arg1);
void SpuSetKey(long on_off, unsigned long voice_bit);
s32 func_80077090(s32 arg0);

/* Per-entry sweep over the 0x28-byte records at D_8009B458 + 0x180: sends the
 * under-0x10 ones through func_8004A43C, resets a record whose lane at sp10
 * is idle, advances the +0xD counter in lane 3, drains lane 1/2 records through
 * the SpuSetKey/func_80077090 pair, and ORs the touched masks into one final
 * SpuSetKey call. */

void func_8004AAFC(void) {
    u8 sp10[0x18];
    u8 *p;
    u8 *e;
    u8 *q;
    u8 *r;
    s32 *t;
    s32 *u;
    s32 i;
    s32 m;
    s32 o;
    s32 two;
    s32 a;
    s32 b;
    u32 c;
    s32 v;

    SpuGetAllKeysStatus((char *)sp10);
    p = D_8009B458;
    i = 0;
    m = i;

    if (*(s16 *)(p + 0x510) > 0) {
        two = 2;
        t = D_80011434;
        q = sp10;
        o = 0x180;
        do {
            e = p + o;
            if (e[3] < 0x10) {
                func_8004A43C(e, 0);
            }
            if (*q == 0) {
                if (e[0xD] == 0) {
                    goto next;
                }
                r = D_8009B458 + e[3] * 0x18;
                a = r[6];
                if ((a & 0xF) != 0) {
                    r[6] = a - 1;
                }
                e[0xD] = 0;
                *(s16 *)(e + 0x1E) = 0;
                e[3] = 0x63;
            }

            b = e[0xD];
            c = b & 0xFF;
            if (c != 0 && *q == 3) {
                if (c >= 2) {
                    m = m | *t;
                    func_8004A7C0(i, b);
                } else {
                    e[0xD] = b + 1;
                }
            }

next:
            if (e[0xF] == 0 && (u32)(*q - 1) < 2) {
                u = t;
                while (1) {
                    SpuSetKey(0, *u);
                    v = func_80077090(*u);
                    if (v == two) {
                        break;
                    }
                    if (v == 0) {
                        break;
                    }
                }
                m = m | *u;
            }

            t++;
            q++;
            p = D_8009B458;
            o += 0x28;
            i++;
        } while (i < *(s16 *)(p + 0x510));
    }

    if (m != 0) {
        SpuSetKey(0, m);
    }
}
