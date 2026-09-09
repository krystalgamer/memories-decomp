#include "../types.h"
#include "scene_script_record_callbacks.h"

/* Initialized data at 0x80090CAC: the two callbacks func_8002FFD4 selects
 * from each live scene-script record's byte at +4. Their native prototypes
 * differ, so the table keeps the generic two-argument call shape used by its
 * sole consumer and casts at the initializer boundary.
 */

void func_8002FED0(void);
void func_8002FED8(u8 *, u8 *);

SceneScriptRecordCallback D_80090CAC[] = {
    (SceneScriptRecordCallback)func_8002FED0,
    (SceneScriptRecordCallback)func_8002FED8,
};
