#include "../types.h"

extern u8 *D_8009B290;
extern s32 D_8009B400[];
extern s32 D_8009B404 __attribute__((section(".data")));
extern s32 D_8009B0F4[];
extern s32 D_8009B134[];
extern u16 D_8009B27C;

extern s32 func_8002E3B4(void);
extern void SD_SEPlayFull(s32);
extern void func_8003FF08(s32);

void func_8002EC74(void)
{
    u8 *q;
    s32 c;
    s32 f;

    if (func_8002E3B4() == 0) {
        q = D_8009B290;
        c = *q;
        q++;
        D_8009B290 = q;
        f = c;

        if ((f & 0x3F) == 0) {
            if (f & 0x40) {
                D_8009B290 = q + 2;
                SD_SEPlayFull(q[0] | (q[1] << 8));
            } else {
                D_8009B290 = q + 2;
                func_8003FF08(q[0] | (q[1] << 8));
            }
        } else {
            if (f & 1) {
                func_8003FF08(D_8009B404);
            }
            if (f & 2) {
                q = D_8009B290;
                D_8009B290 = q + 2;
                D_8009B404 = q[0] | (q[1] << 8);
            }
            if (f & 4) {
                D_8009B404 = D_8009B400[0];
            }
        }
        if (f & 0x80) {
            return;
        }
    } else if ((D_8009B0F4[0] & 0x2000030) | D_8009B134[0]) {
        return;
    }
    D_8009B27C = 0;
}
