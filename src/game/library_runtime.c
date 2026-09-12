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

/* The first three functions of the Library screen: the trivial state handlers
   for states 0 and 3, followed by the per-frame dispatcher func_8002BAB4.
   The package-transfer phase callback, owned-card pass and entry
   func_8002BFCC are in func_8002BD0C.c.

   The state handlers were recorded at gcc_2_8_1_g8. They compile to
   identical objects at gcc_2_8_1_g8_split, which the dispatcher and entry
   need, so they build there with the rest of the screen. */

void func_8002BAA0(u8 *value)
{
    *value = 1;
}

void func_8002BAAC(u8 *value)
{
}

void func_8002BAB4(void)
{
    u8 *state;
    u8 *model;
    s32 one;
    s32 r;
    s32 a;
    s32 step;
    s32 cur;
    s32 target;
    s32 count;
    s32 mode;
    u8 *dst;
    u8 *p;

    state = D_800EA1E8;
    one = 1;
    mode = D_800EA1E8[0];
    mode &= 0xF;
    switch (mode) {
    case 0:
        func_8002BAA0(state);
        break;
    case 1:
        func_8002A788(state);
        break;
    case 2:
        func_8002ACA4(state);
        model = (u8 *)&D_800F2848;
        r = func_80058DD8(0);
        if (r == one) {
            if (func_80058E68(0) != r) {
                Model_CopySlotU16Values(0, (u16 *)0x80181010);
                r = D_80181012 - D_80181002;
                a = r >= 0 ? r : -r;
                step = a / 24;
                step += 1;
                cur = *(s32 *)(model + 0x20);
                if (D_80181012 < cur) {
                    *(s32 *)(model + 0x20) = cur - step;
                    cur = *(volatile s32 *)(model + 0x20);
                }
                if (cur < D_80181012) {
                    *(s32 *)(model + 0x20) = cur + step;
                }
            } else {
                r = *(s32 *)(model + 0x20);
                target = D_80181002;
                if (r != target) {
                    if (target < r) {
                        r -= 8;
                        if (r < target) {
                            r = target;
                        }
                    } else {
                        r += 8;
                        if (target < r) {
                            r = target;
                        }
                    }
                    *(s32 *)(model + 0x20) = r;
                }
                count = *(s32 *)(state + 0x20) - 1;
                *(s32 *)(state + 0x20) = count;
                if (count <= 0) {
                    *(s32 *)(state + 0x20) = 0x12C;
                    r = 2;
                    if (rand() & 1) {
                        r = 7;
                    }
                    func_80057AF4(0, r, 1);
                }
            }
        }
        p = (u8 *)&D_800F2848;
        *(u16 *)(p + 2) += 0xC;
        func_8001352C();
        dst = func_800591FC();
        ((LibraryViewQuad *)dst)[0] = *(LibraryViewQuad *)(model + 0x10);
        ((LibraryViewQuad *)dst)[1] = *(LibraryViewQuad *)(model + 0x20);
        Model_UpdateViewMetrics(0);
        break;
    case 3:
        func_8002BAAC(state);
        break;
    }
}
