#define FUNC_80049F50_RETURNS_S16
#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
#include "../types.h"
#include "../game/func_80044DC0.h"
#include "../psyq/libspu.h"
#include "../game/sound.h"
#include "../game/sound_voice_constants.h"
#include "../unmatched.h"

void SD_UpdateFades(void) {
    u8 changed;
    s16 y;

    changed = 0;
    if (g_SDValue->field_1588 != 0) {
        g_SDValue->field_1586 += g_SDValue->field_1588;
        changed = 1;
        if (g_SDValue->field_1586 <= g_SDValue->field_158A &&
            g_SDValue->field_1588 < 0) {
            g_SDValue->field_1588 = 0;
            g_SDValue->field_1586 = g_SDValue->field_158A;
        }
        if (g_SDValue->field_1586 >= g_SDValue->field_158A &&
            g_SDValue->field_1588 > 0) {
            g_SDValue->field_1588 = 0;
            g_SDValue->field_1586 = g_SDValue->field_158A;
        }
    }
    if (g_SDValue->field_1582 != 0) {
        g_SDValue->field_1580 += g_SDValue->field_1582;
        changed = 1;
        if (g_SDValue->field_1580 <= g_SDValue->field_1584 &&
            g_SDValue->field_1582 < 0) {
            g_SDValue->field_1582 = 0;
            g_SDValue->field_1580 = g_SDValue->field_1584;
        }
        if (g_SDValue->field_1580 >= g_SDValue->field_1584 &&
            g_SDValue->field_1582 > 0) {
            g_SDValue->field_1582 = 0;
            g_SDValue->field_1580 = g_SDValue->field_1584;
        }
    }
    if (changed) {
        u16 v;

        v = g_SDValue->field_0044;
        if (v) {
            v *= g_SDValue->field_1580 + 1;
            v >>= 8;
        } else {
            v = 0;
        }
        if (v) {
            v *= g_SDValue->field_1586 + 1;
            v >>= 8;
        } else {
            v = 0;
        }
        y = v;
        func_80049F10(y, y);
    }
    if (g_SDValue->field_0512 != 0) {
        g_SDValue->cd_volume += g_SDValue->field_0512;
        if (g_SDValue->cd_volume <= g_SDValue->field_0049 &&
            g_SDValue->field_0512 < 0) {
            g_SDValue->field_0512 = 0;
            g_SDValue->cd_volume = g_SDValue->field_0049;
        }
        if (g_SDValue->cd_volume >= g_SDValue->field_0049 &&
            g_SDValue->field_0512 > 0) {
            g_SDValue->field_0512 = 0;
            g_SDValue->cd_volume = g_SDValue->field_0049;
        }
        y = g_SDValue->cd_volume;
        func_80044DC0(y);
    }
}
