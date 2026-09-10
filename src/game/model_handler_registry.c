/* The model handler registry and the two maps that search it.
 *
 * D_800F5918 is the eighty-entry table; Model_RegisterHandlerKey fills it and
 * Model_FindHandlerKey reads it back. func_8005FC1C and func_8005FE44 open-code
 * that same reverse lookup -- same sentinel, same eighty-entry scan, same -1 --
 * and then dispatch on the packed id it returns, which is why they belong here
 * rather than beside the handlers they name. */
#include "../types.h"
#include "../ygo_types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model.h"
#include "model_handler_registry.h"
#include "../unmatched.h"

extern s32 (*func_800603DC())();

/* Maps an id to its handler in the second dispatch family: looks the id up
 * in the handler registry at D_800F5918 (GsU_00000000 is the sentinel that
 * skips the search), then dispatches on the high halfword's group and the
 * low halfword's kind. Returns arg0 unchanged when nothing matches. */

void func_80069E44(void);
void func_80069F94(void);
void func_8006A0E8(void);
void func_8006A268(void);
void func_8006A3F0(void);
void func_8006A540(void);
void func_8006A694(void);
void func_8006A814(void);
void func_8006A99C(void);
void func_8006AAFC(void);
void func_8006AC88(void);
void func_8006ADE8(void);

s32 func_8005FC1C(s32 arg0) {
    ModelHandlerRegistryEntry *e;
    s32 n;
    s32 v;
    u32 t;

    e = D_800F5918;
    n = 0;

    if (arg0 == (s32)GsU_00000000) {
        v = -1;
    } else {
        do {
            if (e->handler_value == arg0) {
                v = e->key;
                goto have;
            }
            n++;
            e++;
        } while (n < MODEL_HANDLER_REGISTRY_COUNT);
        v = -1;
    }

have:
    t = v & 0xFFFF0000;

    switch (t) {
    case 0:
    case 0x100000:
    case 0x200000:
    case 0x300000:
        switch (v & 0xFFFF) {
        case 9:
            return (s32)func_80069E44;
        case 0x209:
            return (s32)func_8006A3F0;
        case 0x11:
            return (s32)func_8006A0E8;
        case 0x211:
            return (s32)func_8006A694;
        case 0xD:
            return (s32)func_80069F94;
        case 0x20D:
            return (s32)func_8006A540;
        case 0x15:
            return (s32)func_8006A268;
        case 0x215:
            return (s32)func_8006A814;
        }
        break;
    case 0x1000000:
    case 0x1100000:
    case 0x1200000:
    case 0x1300000:
        switch (v & 0xFFFF) {
        case 0xD:
            return (s32)func_8006A99C;
        case 0x20D:
            return (s32)func_8006AC88;
        case 0x15:
            return (s32)func_8006AAFC;
        case 0x215:
            return (s32)func_8006ADE8;
        }
        break;
    }
    return arg0;
}

/* Maps the registered callback to the handler family selected by the packed
 * registry id. The case order is retail's own arm order in memory. */

s32 func_8005FE44(s32 arg0) {
    ModelHandlerRegistryEntry *e;
    s32 n;
    s32 v;
    u32 t;

    e = D_800F5918;
    n = 0;

    if (arg0 == (s32)GsU_00000000) {
        v = -1;
    } else {
        do {
            if (e->handler_value == arg0) {
                v = e->key;
                goto have;
            }
            n++;
            e++;
        } while (n < MODEL_HANDLER_REGISTRY_COUNT);
        v = -1;
    }

have:
    t = v & 0xFFFF0000;

    switch (t) {
    case 0x200000:
        switch (v & 0xFFFF) {
        case 0x9:
            return (s32)func_800612C0;
        case 0x209:
            return (s32)func_80061DDC;
        case 0x11:
            return (s32)func_800617E0;
        case 0x211:
            return (s32)func_8006233C;
        case 0xD:
            return (s32)func_8006151C;
        case 0x20D:
            return (s32)func_80062058;
        case 0x15:
            return (s32)func_80061A84;
        case 0x215:
            return (s32)func_80062600;
        }
        break;
    case 0x300000:
        switch (v & 0xFFFF) {
        case 0x9:
            return (s32)func_80062978;
        case 0x209:
            return (s32)func_80063444;
        case 0x11:
            return (s32)func_80062E70;
        case 0x211:
            return (s32)func_8006397C;
        case 0xD:
            return (s32)func_80062BC0;
        case 0x20D:
            return (s32)func_800636AC;
        case 0x15:
            return (s32)func_80063100;
        case 0x215:
            return (s32)func_80063C2C;
        }
        break;
    case 0x1200000:
        switch (v & 0xFFFF) {
        case 0xD:
            return (s32)func_80067354;
        case 0x20D:
            return (s32)func_80067858;
        case 0x15:
            return (s32)func_8006759C;
        case 0x215:
            return (s32)func_80067ABC;
        }
        break;
    case 0x1300000:
        switch (v & 0xFFFF) {
        case 0xD:
            return (s32)func_80067D94;
        case 0x20D:
            return (s32)func_8006825C;
        case 0x15:
            return (s32)func_80067FD0;
        case 0x215:
            return (s32)func_800684B4;
        }
        break;
    }
    return arg0;
}

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
    if (val == (s32) GsU_00000000) {
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
        (ModelHandler)GsU_00000000;
}
