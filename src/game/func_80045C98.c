#include "../types.h"
#include "sound.h"

typedef struct {
    u8 pad000[0x44];
    u16 field_0044;
    u8 pad046[3];
    u8 field_0049;
    u8 pad04A[0x510 - 0x4A];
    s16 cd_volume;
    s16 field_0512;
    u8 pad514[0x1580 - 0x514];
    s16 field_1580;
    s16 field_1582;
    u8 field_1584;
    u8 pad1585;
    s16 field_1586;
    s16 field_1588;
    u8 field_158A;
} SDFade;

#define SD ((SDFade *)g_SDValue)

extern void func_80049F10(s16, s16);
extern void func_80044DC0(s16);

void func_80045C98(void) {
    u8 changed;
    s16 y;

    changed = 0;
    if (SD->field_1588 != 0) {
        SD->field_1586 += SD->field_1588;
        changed = 1;
        if (SD->field_1586 <= SD->field_158A && SD->field_1588 < 0) {
            SD->field_1588 = 0;
            SD->field_1586 = SD->field_158A;
        }
        if (SD->field_1586 >= SD->field_158A && SD->field_1588 > 0) {
            SD->field_1588 = 0;
            SD->field_1586 = SD->field_158A;
        }
    }
    if (SD->field_1582 != 0) {
        SD->field_1580 += SD->field_1582;
        changed = 1;
        if (SD->field_1580 <= SD->field_1584 && SD->field_1582 < 0) {
            SD->field_1582 = 0;
            SD->field_1580 = SD->field_1584;
        }
        if (SD->field_1580 >= SD->field_1584 && SD->field_1582 > 0) {
            SD->field_1582 = 0;
            SD->field_1580 = SD->field_1584;
        }
    }
    if (changed) {
        u16 v;

        v = SD->field_0044;
        if (v) {
            register u16 p1 asm("$2") = (u16)(v * (SD->field_1580 + 1));

            v = p1 >> 8;
        } else {
            v = 0;
        }
        if (v) {
            register u16 p2 asm("$2") = (u16)(v * (SD->field_1586 + 1));

            v = p2 >> 8;
        } else {
            v = 0;
        }
        y = v;
        func_80049F10(y, y);
    }
    if (SD->field_0512 != 0) {
        SD->cd_volume += SD->field_0512;
        if (SD->cd_volume <= SD->field_0049 && SD->field_0512 < 0) {
            SD->field_0512 = 0;
            SD->cd_volume = SD->field_0049;
        }
        if (SD->cd_volume >= SD->field_0049 && SD->field_0512 > 0) {
            SD->field_0512 = 0;
            SD->cd_volume = SD->field_0049;
        }
        y = SD->cd_volume;
        func_80044DC0(y);
    }
}
