#include "../types.h"
#include "func_800282E8.h"
#include "graphics_frame.h"
#include "func_80029574.h"
#include "input.h"
#include "fade.h"

#include "display_object_layout.h"
#include "duel_effect.h"
#include "duel_card_viewer.h"
#include "duel_effect_tables.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#include "display_object_api.h"
#include "display_object.h"
#include "display_object_helpers.h"
#include "func_800291E0.h"
#include "duel_effect_resource_setup.h"
#include "func_80043178.h"
#include "display_object_interpolation.h"
#include "file_transfer.h"
#include "text_box_runtime.h"
#include "duel_card.h"
#include "duel_effect_resource_record.h"
#include "card_constants.h"
#include "func_800283F4.h"

extern u8 D_8009B248;
extern u8 D_8009B24A;

s32 DuelEffect_UpdateState(void) {
    u8 v = D_8009B254;
    if (v == 0) return 0;
    if ((v & 0x80) == 0) {
        D_8009B24A = v;
        D_8009B254 = v | 0x80;
        D_8009B248 = 0;
        return 1;
    }
    if (v & 0x40) {
        D_8009B254 = 0;
        return 0;
    }
    gDuelEffect_apfnStateHandler[D_8009B24A]();
    return 1;
}
