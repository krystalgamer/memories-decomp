#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model.h"
#include "model_packet_handlers.h"
#include "../unmatched.h"

#define MODEL_ANIMATION_SEQUENCE_SLOT(entry) \
    ((GsSEQ **)&(entry)->field_1E0[(entry)->field_E1B])
#define MODEL_ANIMATION_WORDS(object) ((u32 *)(object)[1])

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MODEL_ANIMATION_PACKET_HANDLER)
void func_8005C6A0(s32 *object, ModelSlot *entry)
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
        MODEL_ANIMATION_SEQUENCE_SLOT(entry),
        MODEL_ANIMATION_WORDS(object)
    );
    if (GsScanAnim(MODEL_ANIMATION_WORDS(object), 0)) {
        while (GsScanAnim(0, (GsTYPEUNIT *)local)) {
            *(void **)local[1] = func_8005C768(local[0]);
        }
        entry->field_E1B += count;
    }
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MODEL_PACKET_HANDLER_LOOKUP)
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
#endif
