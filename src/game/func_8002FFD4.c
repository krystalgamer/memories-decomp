#include "../types.h"
#include "scene_script.h"
#include "scene_script_record_callbacks.h"

void func_8002FFD4(u8 *arg0) {
    s32 i;
    u8 *p = arg0;
    for (i = 0; i < 3; i++) {
        s32 v = *(s32 *)p;
        if (v != 0) D_80090CAC[p[4]](p, v);
        p += 0x14;
    }
}
