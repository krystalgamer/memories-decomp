#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model_packet_handlers.h"

extern void func_8005C7BC(void);
extern void func_8005CEF0(void);
extern void func_8005D378(void);

void func_8005C6A0(s32 *object, u8 *entry)
{
    int local[2];
    int count;
    void *handler;

    if (object[0] != 0x03800000) {
        handler = (void *)GsU_00000000;
        *(void **)object[1] = handler;
        return;
    }
    handler = func_8005C7BC;
    *(void **)object[1] = handler;
    count = GsLinkAnim(
        (GsSEQ **)(entry + 0x1E0 + entry[0xE1B] * 4),
        (u32 *)object[1]
    );
    if (GsScanAnim((u32 *)object[1], 0)) {
        while (GsScanAnim(0, (GsTYPEUNIT *)local)) {
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
    return (void *)GsU_00000000;
}
