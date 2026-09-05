#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "model.h"

typedef float f32;
typedef double f64;
typedef char M2C_UNK;
typedef struct { s32 words[9]; } Mtx32;
typedef u8 State;
typedef u8 Record;
typedef u8 Pair;
typedef u8 Controller;
typedef u8 Entry;
typedef u8 Rec;
typedef u8 Block;
typedef struct { u32 words[2]; } Blk8;
typedef u8 *(*ModelHandler)(u8 **);

extern s32 func_80089E20[];
extern s32 (*func_800603DC())();

/* Finds `key` in the table; if absent and there is a free slot, claims it
   with (key, val). No-op once all 80 slots are taken and no match exists. */
void Model_RegisterHandlerKey(s32 key, s32 val) {
    s32 i;
    ModelHandlerRegistryEntry *e = D_800F5918;
    for (i = 0; i < MODEL_HANDLER_REGISTRY_COUNT; i++, e++) {
        if (e->key == key) {
            return;
        }
        if (e->key != 0) {
            continue;
        }
        if (e->handler_value != 0) {
            continue;
        }
        e->key = key;
        e->handler_value = val;
        return;
    }
}

/* Reverse lookup: finds the entry whose val matches, returns its key (or -1
   if val is the sentinel, or if no entry matches after scanning all 80). */
s32 Model_FindHandlerKey(s32 val) {
    ModelHandlerRegistryEntry *e = D_800F5918;
    s32 i;
    if (val == (s32) func_80089E20) {
        return -1;
    }
    for (i = 0; i < MODEL_HANDLER_REGISTRY_COUNT; i++, e++) {
        if (e->handler_value == val) {
            return e->key;
        }
    }
    return -1;
}

void func_80060220(s32 arg0, u8 *arg1, u8 *arg2) {
    u8 *p;
    u8 *q;
    u8 *b;
    u16 *e;
    u8 *r;
    u8 *rec;
    s32 n;
    s32 i;
    s32 j;
    s32 k;
    s32 f;
    s32 sh;
    s32 v;
    s32 w;
    s32 m;

    m = 0x2000001;
    i = 0;
    q = *(u8 **)arg2;
    r = arg2;
    n = *(u16 *)(q + 2);
    q += 4;

    if (n != 0) {
        f = arg0 < 2;
        sh = arg0 << 8;
        p = q;

        do {
            q = p + 0xC;
            if (*(s32 *)arg1 == m) {
                b = *(u8 **)(r + 0x18);
                if (b != (u8 *)0) {
                    j = 0;
                    e = (u16 *)(b + *(s32 *)(q + 8) * 4);
                    rec = q;
                    if (*(s16 *)(q + 6) > 0) {
                        do {
                            if (f) {
                                *e = 0;
                            }
                            e++;
                            for (k = 1; k < *(s16 *)(rec + 4); k++) {
                                *e |= 0x8000;
                                e++;
                            }
                            j++;
                        } while (j < *(s16 *)(rec + 6));
                    }
                    if (f) {
                        v = *(u16 *)rec;
                        if (*(s16 *)rec >= 0x280) {
                            *(u16 *)rec = (v - 0x280) + sh;
                        } else {
                            *(u16 *)rec = sh;
                        }
                        v = *(s16 *)(rec + 2);
                        if (v >= 8) {
                            *(s16 *)(rec + 2) = v % 8;
                        }
                        *(u16 *)(rec + 2) = *(u16 *)(rec + 2) + 0xF8;
                    }
                    q += 0xC;
                }
            }
            if (f) {
                w = *(u16 *)p;
                *(u16 *)p = (w - 0x280) + sh;
            }
            i++;
            p = q;

        } while (i < n);
    }

    func_800603DC(*(s32 *)arg1)(arg2);
    DrawSync(0);
    *(ModelHandler *)(*(s32 *)(arg1 + 4)) =
        (ModelHandler)func_80089E20;
}
