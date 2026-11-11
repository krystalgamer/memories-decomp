#include "../types.h"
#include "../psyq/rand.h"
#include "display_effect_constants.h"
#include "display_object_system.h"
#include "display_effect_update_menu_record.h"
#include "display_effect_process_menu_records.h"
#include "menu_record.h"
#include "display_menu_records.h"
#include "display_object.h"
#include "display_effect_step_table.h"
#include "display_effect_resource_setup.h"
#include "mem_card.h"
#include "display_effect_lifecycle.h"
#include "display_effect_update_callbacks.h"
#include "mem_card_work.h"

/* Initialized data this unit owns at 0x80090F68: the display effect step
 * table and, directly after it, the dialog transition step table.
 *
 * DisplayEffect_ProcessMenuRecords walks the menu records and, for each live
 * one, dispatches `display_effect_step & 0x1F`. The mask permits thirty-two
 * entries and there are eight; the game only stores steps zero through six.
 *
 * The table and dispatcher use MenuRecord *, matching the arena they walk.
 * The two repeated lifecycle entries and the single VRAM entry retain casts
 * because those handlers deliberately expose narrower layout views of the
 * same record.
 * The entries include two that share DisplayEffect_FinishStepWhenIdle.
 *
 * The memory card dialog selects one of the five transition steps with its
 * callback state and passes the root and slot; each step reads the root
 * through its own MenuRecord view and ignores the slot. */

void (*gDisplayEffect_apfnSteps[])(MenuRecord *) = {
    (void (*)(MenuRecord *))DisplayEffect_FinishStepWhenIdle,
    (void (*)(MenuRecord *))DisplayEffect_FinishStepWhenIdle,
    (void (*)(MenuRecord *))DisplayEffect_UpdateResourceAcquisition,
    DisplayEffect_UpdatePositionTween,
    DisplayEffect_CrossfadeResources,
    DisplayEffect_UpdatePortraitTransition,
    DisplayEffect_FadeOutRecord,
    DisplayEffect_FadeInRecord,
};

void (*gDialog_apfnTransitionSteps[MEM_CARD_WORK_CALLBACK_COUNT])(
    MemCardWorkRoot *root, MemCardWorkSlot *slot
) = {
    (void (*)(MemCardWorkRoot *, MemCardWorkSlot *))Dialog_UpdateMessageTransition,
    (void (*)(MemCardWorkRoot *, MemCardWorkSlot *))Dialog_UpdateOpenTransition,
    (void (*)(MemCardWorkRoot *, MemCardWorkSlot *))Dialog_UpdateCloseTransition,
    (void (*)(MemCardWorkRoot *, MemCardWorkSlot *))Dialog_UpdateSpotlightContraction,
    (void (*)(MemCardWorkRoot *, MemCardWorkSlot *))Dialog_UpdateMessageTransition,
};

#define DISPLAY_OBJECT_CONFIG_VIEW(object) \
    ((DisplayObjectConfig *)(object))

void DisplayEffect_UpdateMenuRecord(MenuRecord *p, s32 n) {
    s32 f;
    s32 g;
    s32 c;
    s32 v;

    f = p->variant_update_flags;
    if ((f & 0x10) != 0) {
        p->variant_update_flags = f & 0xFC;
        return;
    }

    if (p->grid[0][1] != 0) {
        if ((f & 1) != 0) {
            if (((DisplayObject *)p->grid[0][1])->field_5A == 0) {
                p->variant_countdown =
                    (rand() & DISPLAY_EFFECT_DELAY_MASK) +
                    DISPLAY_EFFECT_DELAY_BASE;
                p->variant_update_flags = p->variant_update_flags & 0xFE;
                DisplayObject_SetResourceVariant(
                    DISPLAY_OBJECT_CONFIG_VIEW(p->grid[0][1]), 0);
            }
        } else {
            v = p->variant_countdown - 1;
            p->variant_countdown = v;
            if ((s16)v <= 0) {
                p->variant_update_flags = p->variant_update_flags | 1;
                DisplayObject_SetResourceVariant(
                    DISPLAY_OBJECT_CONFIG_VIEW(p->grid[0][1]), 1);
            }
        }
    }

    if (p->grid[0][2] == 0) {
        return;
    }

    g = p->variant_update_flags;
    if ((g & 2) != 0) {
        c = p->variant_update_counter - 1;
        p->variant_update_counter = c;
        if (((DisplayObject *)p->grid[0][2])->field_5A != 0) {
            return;
        }
        if ((s8)c > 0) {
            return;
        }
        if (n <= 0) {
            p->variant_update_counter = 0;
            p->variant_update_flags = p->variant_update_flags & 0xFD;
            return;
        }
    } else {
        if (n < 0) {
            goto slow;
        }
        p->variant_update_flags = g | 2;
    }

    p->variant_update_counter = 6;
    p->requested_variant = n;
    DisplayObject_SetResourceVariant(
        DISPLAY_OBJECT_CONFIG_VIEW(p->grid[0][2]), n);
    return;

slow:
    p->variant_update_counter = p->variant_update_counter + 1;
    if ((s8)p->variant_update_counter < 6) {
        return;
    }
    DisplayObject_UpdateResourceVariant(
        DISPLAY_OBJECT_CONFIG_VIEW(p->grid[0][2]),
        0
    );
}

void DisplayEffect_ProcessMenuRecords(s32 value) {
    MenuRecord *record = gDisplayEffect_aMenuRecords;
    s32 i = MENU_RECORD_COUNT - 1;
    void (**t)(MenuRecord *) = gDisplayEffect_apfnSteps;

    do {
        if (record->effect_id >= 0) {
            if (record->variant_update_flags & 0x40) {
                DisplayEffect_UpdateMenuRecord(record, value);
            }

            if (record->display_effect_step != 0) {
                t[record->display_effect_step & 0x1F](record);
            }
        }
        record++;
        i--;
    } while (i >= 0);
}
