#include "../types.h"
#include "scene_script_record_callbacks.h"
#include "scene_script_noop_record_callback.h"
#include "scene_script_color_pulse.h"

/* Initialized data at 0x80090CAC: the two callbacks SceneScript_UpdateSlots selects
 * from each live scene-script record's byte at +4. Their native prototypes
 * differ, so the table keeps the generic two-argument call shape used by its
 * sole consumer and casts at the initializer boundary.
 */

SceneScriptRecordCallback gSceneScript_RecordCallbacks[] = {
    (SceneScriptRecordCallback)SceneScript_NoOpRecordCallback,
    (SceneScriptRecordCallback)SceneScript_UpdateColorPulse,
};
