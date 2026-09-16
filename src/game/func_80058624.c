#include "../types.h"
#include "camera_view.h"
#include "../psyq/libgte.h"
#include "sound_voice_data.h"
#include "sound.h"
#include "positional_sound.h"

void SD_ApplyPositionalSound(
    s32 sound_id, s32 play, s32 world_x, s32 world_z)
{
    u8 st[2];
    s8 *pp;
    s32 dx;
    s32 dz;
    s32 a;
    s32 b;
    s32 c;
    s32 d;
    s32 ex;
    s32 ez;
    s32 n;
    s32 r;
    s32 dd;
    s32 v;
    s32 w;

    dx = D_800F56F0.vpx - world_x;
    dz = D_800F56F0.vpz - world_z;
    pp = (s8 *)&st[1];
    st[0] = 0;
    st[1] = 0;
    r = SquareRoot0(dx * dx + dz * dz);
    b = D_800F56F0.vpz;
    c = D_800F56F0.vrx;
    d = D_800F56F0.vrz;
    ez = d - b;
    a = D_800F56F0.vpx;
    ex = a - c;
    n = c * b - a * d;
    dd = SquareRoot0(ez * ez + ex * ex);
    r -= 0x2BC;

    st[0] = 0xFF;
    *pp = 0;
    if (r > 0) {
        v = 0xFF - r / 12;
        if (v < 0x10) {
            v = 0x10;
        }
        st[0] = v;
    }

    if (dd != 0) {
        r = (world_x * ez + world_z * ex + n) / dd;
        w = r / 16;
        /* The conditional form negates w's register instead of v in place. */
        v = __builtin_abs(w);
        if (v >= 0x80) {
            v = 0x7F;
        }
        w = v;
        if (r <= 0) {
            w = -w;
        }
        *pp = w;
    }

    if (play != 0) {
        SD_SEPlay(sound_id & 0xFFFF, st[0], *(s8 *)&st[1]);
    } else {
        func_80048A28(sound_id & 0xFFFF, st[0], *(s8 *)&st[1]);
    }
}
