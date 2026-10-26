/* Four contiguous display-effect resource functions from
   DisplayEffect_HasResourceEntry (0x8003A198) through DisplayEffect_UpdateResourceAcquisition: the
   three-level halfword offset-table probe, the up-to-three-object builder
   that consults it, the child layer/attribute setup, and the VRAM page swap
   state callback that calls the builder after loading or restoring a page. */

#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA

#include "../types.h"
#include "display_effect_has_resource_entry.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_effect_lifecycle.h"
#include "color_constants.h"
#include "display_object.h"
#include "display_object_core.h"
#include "display_object_layout.h"
#include "display_object_helpers.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "util_memory.h"
#include "display_effect_load_resource_stage.h"
#include "display_effect_build_resource_objects.h"
#include "display_effect_set_object_row_blend.h"
#include "display_effect_update_resource_acquisition.h"
#include "high_memory_addresses.h"
#include "shared_work_base.h"
#include "../unmatched.h"
#include "display_resource_banks.h"

int DisplayEffect_HasResourceEntry(unsigned char*b,int x,int y,int z){u16 *p;p=(u16*)(b+x*2);if(*p){p=(u16*)(b+*p+y*2);if(*p){p=(u16*)(b+*p+z*2);if(*p)return 1;}}return 0;}

/* Regional value: the CLUT rectangles' x (512 -> 640). The European build
 * (src/game/european/) defines its own. */
#ifndef DISPLAY_EFFECT_CLUT_X
#define DISPLAY_EFFECT_CLUT_X 512
#endif

s32 DisplayEffect_BuildResourceObjects(
    MenuRecord *a, DisplayObject **out, s32 c)
{
    DisplayObject *p;
    u8 *tb;
    s32 f;
    s32 g;
    s32 h;
    s32 m;

    if (a->slot_index != 0) {
        f = -0xA;
        tb = gDisplay_abResourceBank1;
        g = 0x1A;
        h = 0x202;
    } else {
        f = -0xE;
        tb = gDisplay_abResourceBank0;
        g = 0x1D;
        h = 0x200;
    }

    if (DisplayEffect_HasResourceEntry(tb, c, 0, 0) == 0) {
        return 0;
    }

    {
        m = GsALON | DISPLAY_OBJECT_ATTRIBUTE_8BPP;
        p = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(p, (s16)a->field_34, (s16)a->field_36, c, 0, 0, g, h,
                      tb);
        DisplayObject_SelectOrderingTable1(p);
        DisplayObject_SetDepthOffset(p, f);
        p->attribute = p->attribute | m;
        p->flags = p->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        out[0] = p;

        if (DisplayEffect_HasResourceEntry(tb, c, 1, 0) != 0) {
            p = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
            DisplayObject_ConfigureSpriteAtPositionWithResource(p, (s16)a->field_34, (s16)a->field_36, c, 1, 0, g,
                          h, tb);
            DisplayObject_SelectOrderingTable1(p);
            DisplayObject_SetDepthOffset(p, f | 1);
            p->attribute = p->attribute | m;
            p->flags = p->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        } else {
            p = (DisplayObject *)0;
        }
        out[1] = p;

        if (DisplayEffect_HasResourceEntry(tb, c, 2, 0) != 0) {
            p = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
            DisplayObject_ConfigureSpriteAtPositionWithResource(p, (s16)a->field_34, (s16)a->field_36, c, 2, 0, g,
                          h, tb);
            DisplayObject_SelectOrderingTable1(p);
            DisplayObject_SetDepthOffset(p, f | 1);
            p->attribute =
                p->attribute | GsALON | DISPLAY_OBJECT_ATTRIBUTE_8BPP;
            p->flags = p->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        } else {
            p = (DisplayObject *)0;
        }
        out[2] = p;
    }

    return 1;
}

void DisplayEffect_SetObjectRowBlend(u8 **arg0, u32 arg1, s32 arg2)
{
    DisplayObject *e;
    s8 c;
    s32 i;

    if (arg1 == 0) {
        for (i = 2; i >= 0; i--) {
            c = (s8)arg2;
            e = (DisplayObject *)arg0[i];
            if (e != 0) {
                e->attribute = e->attribute & ~(GsALON | GsATWO | GsAONE);
                e->attribute = e->attribute | GsALON;
                DisplayObject_SetDepthOffset(e, c);
                e->rgb = COLOR_RGB24_NEUTRAL_GREY;
                e->field_40.h.field_42 -= 1;
            }
        }
    } else {
        for (i = 2; i >= 0; i--) {
            e = (DisplayObject *)arg0[i];
            if (e != 0) {
                e->attribute = (e->attribute & ~(GsALON | GsATWO | GsAONE)) | arg1;
                DisplayObject_SetDepthOffset(e, (s8)arg2);
                if (arg1 == (GsALON | GsATWO)) {
                    e->field_40.h.field_42 = 0xFD;
                } else {
                    e->field_40.h.field_42 += 1;
                }
            }
        }
    }
}

/* The scalars carry section(".data") so -G8 keeps them off $gp and they take
   the assembler macro form, while the one-byte gDisplayEffect_ResourceCacheWriteSlot stays gp-relative. */
/* Which effect each VRAM slot holds, and the first sector of the effect
   packages. The Japanese build keeps the slot ids inside the gRuntime_pSharedWorkBase arena
   at +0x7FFF0 (as its DisplayEffect_ResetMenuRecords resets them) and its packages start five
   sectors lower, so a regional build supplies its own. */
#ifndef DISPLAY_EFFECT_SLOT_ID
#define DISPLAY_EFFECT_SLOT_ID(i) (gDisplayEffect_CachedResourceIds[i])
#define DISPLAY_EFFECT_RESOURCE_FIRST_SECTOR 15182
#endif

void DisplayEffect_UpdateResourceAcquisition(DisplayEffectVramState *a)
{
    DisplayEffectVramSlot *slots;
    DisplayEffectVramSlot *slot;
    FileTransferDescriptor *req;
    s32 i;

    if (DisplayEffect_MarkInitialized((DisplayEffectState *)a) == 0) {
        slots = (DisplayEffectVramSlot *)gRuntime_pSharedWorkBase;
        gDisplayEffect_ResourceCacheWriteSlot = 0;
        /* The slot cursor has to be an explicit walking pointer decremented at
           the end of the body: spelling it &slots[i] leaves the strength-reduced
           induction value in a call-clobbered register and costs a per-iteration
           move plus the fifth callee-saved register. */
        slot = &slots[DISPLAY_EFFECT_VRAM_SLOT_COUNT - 1];
        for (i = DISPLAY_EFFECT_VRAM_SLOT_COUNT - 1; i >= 0; i--) {
            if (DISPLAY_EFFECT_SLOT_ID(i) < 0) {
                gDisplayEffect_ResourceCacheWriteSlot = i;
            }
            if (DISPLAY_EFFECT_SLOT_ID(i) == a->effect_id) {
                while (IsIdleGPU(10) != 0) {
                    ;
                }
                slot->image_rect.x = 832 - a->slot_index * 192;
                slot->image_rect.y = 256;
                slot->image_rect.w = 192;
                slot->image_rect.h = 256;
                LoadImage(&slot->image_rect, (u32 *)slot->image);
                slot->clut_rect.x = DISPLAY_EFFECT_CLUT_X;
                slot->clut_rect.y = a->slot_index * 2 + 240;
                slot->clut_rect.w = 256;
                slot->clut_rect.h = 2;
                LoadImage(&slot->clut_rect, (u32 *)slot->clut);
                if (a->slot_index != 0) {
                    Util_CopyWords(gDisplay_abResourceBank1, slot->extra, 0x800);
                } else {
                    Util_CopyWords(gDisplay_abResourceBank0, slot->extra, 0x800);
                }
                a->state |= 0x40;
                return;
            }
            slot--;
        }
        if (((gFile_dwTransferState_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             gFile_dwCancelRequestState_abs) != 0) {
            a->state &= 0x7F;
            return;
        }
        req = File_TryRequestAsyncTransfer(
            0, 0, a->effect_id * 50 + DISPLAY_EFFECT_RESOURCE_FIRST_SECTOR, 50,
            DisplayEffect_LoadResourceStage, 0, 0
        );
        req->callback_data = gDisplay_abResourceBank0;
        req->position = a->slot_index;
        if (a->slot_index != 0) {
            req->callback_data = gDisplay_abResourceBank1;
        }
        gFile_dwTransferState_abs = req->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
    } else if ((a->state & 0x40) == 0) {
        if (((gFile_dwTransferState_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             gFile_dwCancelRequestState_abs) != 0) {
            return;
        }
        a->state |= 0x40;
        DISPLAY_EFFECT_SLOT_ID(gDisplayEffect_ResourceCacheWriteSlot) = a->effect_id;
        while (IsIdleGPU(10) != 0) {
            ;
        }
        slot = &((DisplayEffectVramSlot *)gRuntime_pSharedWorkBase)[gDisplayEffect_ResourceCacheWriteSlot];
        slot->image_rect.x = 832 - a->slot_index * 192;
        slot->image_rect.y = 256;
        slot->image_rect.w = 192;
        slot->image_rect.h = 256;
        StoreImage(&slot->image_rect, (u32 *)slot->image);
        slot->clut_rect.x = DISPLAY_EFFECT_CLUT_X;
        slot->clut_rect.y = a->slot_index * 2 + 240;
        slot->clut_rect.w = 256;
        slot->clut_rect.h = 2;
        StoreImage(&slot->clut_rect, (u32 *)slot->clut);
        if (a->slot_index != 0) {
            Util_CopyWords(slot->extra, gDisplay_abResourceBank1, 0x800);
        } else {
            Util_CopyWords(slot->extra, gDisplay_abResourceBank0, 0x800);
        }
    } else {
        DisplayEffect_BuildResourceObjects(
            (MenuRecord *)a, (DisplayObject **)a, a->resource_entry_index);
        a->state = 0;
        a->field_32 |= 0x40;
    }
}
