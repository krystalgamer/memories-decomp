#include "../types.h"

extern void func_80089E20(void);
extern void func_8005C7BC(void);
extern void func_8005CEF0(void);
extern void func_8005D378(void);
extern int func_80089DD8(void *, void *);
extern int func_80089D30(int, void *);
extern void *func_8005C768(u32);

void func_8005C6A0(int *object, u8 *entry)
{
    int local[2];
    int count;
    void *handler;

    if (object[0] != 0x03800000) {
        handler = func_80089E20;
        *(void **)object[1] = handler;
        return;
    }
    handler = func_8005C7BC;
    *(void **)object[1] = handler;
    count = func_80089DD8(
        entry + 0x1E0 + entry[0xE1B] * 4,
        (void *)object[1]
    );
    if (func_80089D30(object[1], 0)) {
        while (func_80089D30(0, local)) {
            *(void **)local[1] = func_8005C768(local[0]);
        }
        entry[0xE1B] += count;
    }
}

void *func_8005C768(u32 value)
{
    if ((value & 0xFFFF0000) != 0x03000000) {
        goto default_case;
    }
    value &= 0xFFFF;
    if (value == 0x2019) {
        goto case_2019;
    }
    if (value == 0x2119) {
        goto case_2119;
    }
    goto default_case;

case_2019:
    return func_8005CEF0;
case_2119:
    return func_8005D378;
default_case:
    return func_80089E20;
}
