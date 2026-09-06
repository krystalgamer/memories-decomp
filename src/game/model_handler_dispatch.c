#include "../types.h"
extern u8 D_800F5918[];
u8 *func_80089E20(u8 **arg0);

/* Maps an id to its handler in the second dispatch family: looks the id up
 * in the 0x50-entry table at D_800F5918 (func_80089E20 is the sentinel that
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
    u8 *p;
    s32 n;
    s32 v;
    u32 t;

    p = D_800F5918;
    n = 0;

    if (arg0 == (s32)func_80089E20) {
        v = -1;
    } else {
        do {
            if (*(s32 *)p == arg0) {
                v = *(s32 *)(p + 4);
                goto have;
            }
            n++;
            p += 8;
        } while (n < 0x50);
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
void func_800612C0(void);
void func_8006151C(void);
void func_800617E0(void);
void func_80061A84(void);
void func_80061DDC(void);
void func_80062058(void);
void func_8006233C(void);
void func_80062600(void);
void func_80062978(void);
void func_80062BC0(void);
void func_80062E70(void);
void func_80063100(void);
void func_80063444(void);
void func_800636AC(void);
void func_8006397C(void);
void func_80063C2C(void);
void func_80067354(void);
void func_8006759C(void);
void func_80067858(void);
void func_80067ABC(void);
void func_80067D94(void);
void func_80067FD0(void);
void func_8006825C(void);
void func_800684B4(void);

s32 func_8005FE44(s32 arg0) {
    u8 *p;
    s32 n;
    s32 v;
    u32 t;

    p = D_800F5918;
    n = 0;

    if (arg0 == (s32)func_80089E20) {
        v = -1;
    } else {
        do {
            if (*(s32 *)p == arg0) {
                v = *(s32 *)(p + 4);
                goto have;
            }
            n++;
            p += 8;
        } while (n < 0x50);
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
