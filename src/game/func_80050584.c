#include "../types.h"
#include "../unmatched.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/rand.h"
#include "card_constants.h"
#include "file_transfer.h"
#include "model.h"
#include "func_8005922C.h"
#include "model_cleanup.h"

void func_80050584(s32 arg0) {
    ModelSlot *p;
    ModelSlot *b;
    ModelSlot *b0;
    ModelSlot *q;
    GsCOORDUNIT *r;
    ModelSlot *s;
    s32 v;
    s32 t;
    s32 a;
    s32 m1;

    b0 = D_800F2C40;
    p = b0 + arg0;
    if (p->field_E1F == 0) {
        if (p->field_E14 == 0xFF) {
            if (((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                 D_8009B134_abs) == 0) {
                do {
                    t = rand() >> 8;
                    v = t % CARD_COUNT;
                } while (v < 0 || v >= CARD_COUNT ||
                         (v >= 0x12C && v < 0x15E) ||
                         (v >= 0x28A && v < 0x2BC) ||
                         v == 0x2D0);
                m1 = -1;
                Model_LoadMonsterMerge(arg0 | 0x80, v, m1, m1, m1, m1, 0);
            }
        } else {
            func_80056828(arg0);
        }

        b = D_800F2C40;
        if ((b + arg0)->field_E1F != 0) {
            q = b + (arg0 ^ 1);
            a = MODEL_ANGLE_QUARTER_TURN;
            if (q->field_E1F != 0) {
                r = q->field_D18;
                if (r != (GsCOORDUNIT *)0) {
                    t = r->rot.vy + MODEL_ANGLE_QUARTER_TURN;
                    a = t / MODEL_ANGLE_FULL_TURN;
                    a = t - a * MODEL_ANGLE_FULL_TURN;
                }
            }
            s = D_800F2C40 + arg0;
            if (s->field_D18 != (GsCOORDUNIT *)0) {
                s->field_D18->rot.vx = 0;
                s->field_D18->rot.vy = a;
                s->field_D18->rot.vz = 0;
                s->field_D18->matrix.t[0] = 0;
                s->field_D18->matrix.t[1] = 0;
                s->field_D18->matrix.t[2] = 0;
            }
            func_8005922C(s->field_D18, 0);
            func_80059DD8(arg0);
            s->field_E15 = 0;
        }
    }
}
