#include "../types.h"
#include "model.h"
#include "model_graphics_state.h"
#include "func_8004E7B0.h"

/* These tentative definitions are a codegen input: changing them to extern
 * shortens this function by four bytes. model_graphics_state.c supplies the
 * strong .sdata definitions, so the common symbols allocate no storage. */
u8 *D_8009AF88;
s16 D_8009AF8E;
s16 D_8009AF90;

ModelBytes8 D_8009B478;
s16 D_8009B47A;
s16 D_8009B47C;
ModelBytes8 D_8009B480;
s16 D_8009B482;
s16 D_8009B484;

void func_8004E7B0(s32 force)
{
    register s32 dy asm("$5");
    s32 dz;
    register s32 t asm("$2");
    s32 a;
    s32 b;
    s32 u;
    s32 r;
    s32 m;
    register s32 previous_pitch asm("$2");
    s32 previous_yaw;
    register s32 current_pitch asm("$4");

    dy = D_8009B47A;
    previous_yaw = D_8009B482;
    current_pitch = D_8009B47C;
    previous_pitch = D_8009B484;
    current_pitch = current_pitch - previous_pitch;
    previous_yaw = dy - previous_yaw;
    dy = previous_yaw;
    dz = current_pitch;
    t = dy;
    if (dy < 0) {
        t = -t;
    }
    if (t >= 0x801) {
        if (dy > 0) {
            dy -= MODEL_ANGLE_FULL_TURN;
        } else {
            dy += MODEL_ANGLE_FULL_TURN;
        }
    }
    if (force != 0 || dy != 0) {
        m = *(u16 *)(D_8009AF88 + 0xA6);
        D_8009AF8E = (dy * 1280 / 384 + D_8009AF8E + m) % m;
    }
    if (force != 0 || dz != 0) {
        a = D_8009B47C;
        if (a < 0x801) {
            goto low;
        }
        t = a - MODEL_ANGLE_FULL_TURN;
        if (t < 0) {
            goto neg_side;
        }
        r = -0x139;
        if (t < MODEL_ANGLE_QUARTER_TURN) {
            goto compute;
        }
        goto store;
neg_side:
        r = -0x139;
        if (MODEL_ANGLE_FULL_TURN - a >= MODEL_ANGLE_QUARTER_TURN) {
            goto store;
        }
compute:
        b = D_8009B47C;
        u = b - MODEL_ANGLE_FULL_TURN;
        if (u < 0) {
            goto sub_form;
        }
        r = -(u * 26) / 85;
        goto store;
sub_form:
        r = -((MODEL_ANGLE_FULL_TURN - b) * 26) / 85;
        goto store;
low:
        if (a >= MODEL_ANGLE_QUARTER_TURN) {
            goto clamp_pos;
        }
        r = (a * 26) / 85;
        goto store;
clamp_pos:
        r = 0x139;
store:
        D_8009AF90 = r;
    }
    D_8009B480 = D_8009B478;
}
