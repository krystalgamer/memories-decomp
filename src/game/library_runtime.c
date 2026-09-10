#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "func_80058DD8.h"
#include "camera_view.h"
#include "file_transfer.h"
#include "graphics_frame.h"
#include "library_runtime.h"
#include "model_copy_slot_u16_values.h"
#include "model_update_view_metrics.h"
#include "sound_voice_data.h"
#include "view_state.h"
#include "../psyq/rand.h"
#include "model_slot_queries.h"
#include "../unmatched.h"
#include "func_80057AF4.h"
#include "func_8002A788.h"
#include "save_data.h"
#include "card_constants.h"
#include "campaign_flags.h"
#include "display_object_api.h"
#include "card_grid.h"
#include "display_object_layout.h"
#include "func_8003B6AC.h"
#include "main_services.h"
#include "text_box_lifecycle.h"
#include "duel_card.h"
#include "duel_effect_resource_record.h"
#include "display_object_helpers.h"
#include "duel_deck_lookup.h"
#include "func_80029574.h"
#include "func_80029590.h"
#include "sound.h"
#include "text_render_state.h"
#include "text_staging.h"

/* The first two functions of the Library screen: the trivial state handlers
   the dispatcher calls for states 0 and 3. The per-frame dispatcher that
   follows them, func_8002BAB4, is now a candidate in
   src/candidates/func_8002BAB4.c; the package-transfer phase callback, the
   owned-card pass and the entry func_8002BFCC are in func_8002BD0C.c.

   The state handlers were recorded at gcc_2_8_1_g8. They compile to
   identical objects at gcc_2_8_1_g8_split, which the dispatcher and the
   entry need, so they build there with the rest of the screen. */

void func_8002BAA0(u8 *value)
{
    *value = 1;
}

void func_8002BAAC(u8 *value)
{
}

