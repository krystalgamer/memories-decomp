#include "../types.h"
#include "scene_script.h"
#include "scene_script_record_callbacks.h"

void SceneScript_UpdateSlots(SceneScriptSlot *records) {
    s32 i;
    for (i = 0; i < 3; i++) {
        s32 value = records->unk00;
        if (value != 0) {
            D_80090CAC[*(u8 *)&records->unk04](records, value);
        }
        records++;
    }
}
