#include "../types.h"

/* The display-effect menu-record unit: lifecycle helpers, the resource
 * file-transfer stage, resource object setup, the step update callbacks and
 * the per-frame menu-record dispatcher with its step tables. */

#include "../psyq/rand.h"
#include "display_object_system.h"
#include "menu_record.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "../psyq/libgs.h"
#include "color_constants.h"
#include "display_object.h"
#include "display_object_layout.h"
#include "util.h"
#include "high_memory_addresses.h"
#include "shared_work_base.h"
#include "display_asset_banks.h"
#include "script_scene.h"
#include "main.h"
#include "trig_constants.h"
#include "graphics_frame_controls.h"
#include "mem_card.h"
#include "display_effect.h"

#define DISPLAY_EFFECT_DELAY_BASE 60
#define DISPLAY_EFFECT_DELAY_MASK 0xFF

s32 DisplayEffect_MarkInitialized(MenuRecord *record)
{
    u8 state = record->display_effect_step;

    if ((state & 0x80) == 0) {
        record->display_effect_step = state | 0x80;
        return 0;
    }
    return 1;
}

void DisplayEffect_ResetRecordState(MenuRecord *record)
{
    record->x = 0x68;
    record->variant_update_flags = 0;
    record->display_effect_step = 0;
    record->resource_entry_index = 0;
    record->y = 0xB2;
    record->variant_countdown =
        (rand() & DISPLAY_EFFECT_DELAY_MASK) + DISPLAY_EFFECT_DELAY_BASE;
}

void DisplayEffect_ReleaseObjectRow(DisplayObject **objects)
{
    s32 i;

    for (i = 2; i >= 0; i--) {
        DisplayObject_ReleaseIfPresent(objects[i]);
        objects[i] = 0;
    }
}

void DisplayEffect_ReleaseRecord(MenuRecord *record)
{
    record->effect_id = -1;
    DisplayEffect_ReleaseObjectRow(record->grid[0]);
}

void DisplayEffect_FinishStepWhenIdle(MenuRecord *record)
{
    u8 flags = record->variant_update_flags;

    if ((flags & 3) == 0) {
        record->variant_update_flags = flags | 0x10;
        record->display_effect_step = 0;
    }
}

/* Regional value: the load descriptor's x (0x200 -> 0x280). The European
 * build (src/game/european/) defines its own. */
#ifndef DISPLAY_EFFECT_RESOURCE_LOAD_X
#define DISPLAY_EFFECT_RESOURCE_LOAD_X 0x200
#endif

void DisplayEffect_LoadResourceStage(FileTransferDescriptor *p, s32 mode)
{
    s32 v_0;
    s32 v_1;
    s32 v_2;
    s32 w;
    s32 n;
    s32 b;
    s32 t;
    s32 c;
    s32 k;
    s32 m;
    s32 m2;
    u32 *phase_size;

    switch (mode) {
    case 0:
        m = 0xFFDDFFFF;
        v_0 = gFile_dwTransferState;
        n = p->position;
        p->field_30.h.field_32 = 0x100;
        p->w = 0x40;
        p->phase_size = 48 * FILE_SECTOR_SIZE;
        p->h = 0x10;
        v_0 &= m;
        gFile_dwTransferState = v_0;
        w = gFile_dwTransferState;
        w |= 0x10000;
        gFile_dwTransferState = w;
        p->transfer_mode = 2;
        b = gFile_TransferBufferBase;
        p->field_30.h.counter = 0x340 - n * 192;
        p->sector_destination = b;
        p->alternate_sector_destination = b + FILE_SECTOR_SIZE;
        break;

    case 1:
        m2 = 0xFFDCFFFF;
        phase_size = &p->phase_size;
        *phase_size = FILE_SECTOR_SIZE;
        v_1 = gFile_dwTransferState;
        t = gFile_TransferBufferBase;
        v_1 &= m2;
        gFile_dwTransferState = v_1;
        p->alternate_sector_destination = t;
        p->sector_destination = t;
        p->transfer_mode = 1;
        break;

    case 2:
        p->x = DISPLAY_EFFECT_RESOURCE_LOAD_X;
        k = p->position;
        p->h = 2;
        c = gFile_TransferBufferBase;
        p->w = 0x100;
        p->y = k * 2 + 0xF0;
        LoadImage2((RECT *)p, (u32 *)c);
        m2 = 0xFFDCFFFF;
        phase_size = &p->phase_size;
        *phase_size = FILE_SECTOR_SIZE;
        v_2 = gFile_dwTransferState;
        t = (s32)p->callback_data;
        v_2 &= m2;
        gFile_dwTransferState = v_2;
        p->alternate_sector_destination = t;
        p->sector_destination = t;
        p->transfer_mode = 1;
        break;
    }
}

/* Four contiguous display-effect resource functions from
   DisplayEffect_HasResourceEntry (0x8003A198) through DisplayEffect_UpdateResourceAcquisition: the
   three-level halfword offset-table probe, the up-to-three-object builder
   that consults it, the child layer/attribute setup, and the VRAM page swap
   state callback that calls the builder after loading or restoring a page. */

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
s8 gDisplayEffect_ResourceCacheWriteSlot;

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
        DisplayObject_ConfigureSpriteAtPositionWithResource(p, (s16)a->x, (s16)a->y, c, 0, 0, g, h,
                      tb);
        DisplayObject_SelectOrderingTable1(p);
        DisplayObject_SetDepthOffset(p, f);
        p->attribute = p->attribute | m;
        p->flags = p->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        out[0] = p;

        if (DisplayEffect_HasResourceEntry(tb, c, 1, 0) != 0) {
            p = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
            DisplayObject_ConfigureSpriteAtPositionWithResource(p, (s16)a->x, (s16)a->y, c, 1, 0, g,
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
            DisplayObject_ConfigureSpriteAtPositionWithResource(p, (s16)a->x, (s16)a->y, c, 2, 0, g,
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

void DisplayEffect_SetObjectRowBlend(DisplayObject **objects, u32 attribute, s32 depth_offset)
{
    DisplayObject *e;
    s8 c;
    s32 i;

    if (attribute == 0) {
        for (i = 2; i >= 0; i--) {
            c = (s8)depth_offset;
            e = objects[i];
            if (e != 0) {
                e->attribute = e->attribute & ~(GsALON | GsATWO | GsAONE);
                e->attribute = e->attribute | GsALON;
                DisplayObject_SetDepthOffset(e, c);
                e->rgb.word = COLOR_RGB24_NEUTRAL_GREY;
                e->field_40.clut.y -= 1;
            }
        }
    } else {
        for (i = 2; i >= 0; i--) {
            e = objects[i];
            if (e != 0) {
                e->attribute = (e->attribute & ~(GsALON | GsATWO | GsAONE)) | attribute;
                DisplayObject_SetDepthOffset(e, (s8)depth_offset);
                if (attribute == (GsALON | GsATWO)) {
                    e->field_40.clut.y = 0xFD;
                } else {
                    e->field_40.clut.y += 1;
                }
            }
        }
    }
}

/* Which effect each VRAM slot holds, and the first sector of the effect
   packages. The Japanese build keeps the slot ids inside the gRuntime_pSharedWorkBase arena
   at +0x7FFF0 (as its DisplayEffect_ResetMenuRecords resets them) and its packages start five
   sectors lower, so a regional build supplies its own. */
#ifndef DISPLAY_EFFECT_SLOT_ID
#define DISPLAY_EFFECT_SLOT_ID(i) (gDisplayEffect_CachedResourceIds[i])
#endif
#ifndef DISPLAY_EFFECT_RESOURCE_FIRST_SECTOR
#define DISPLAY_EFFECT_RESOURCE_FIRST_SECTOR 15182
#endif

void DisplayEffect_UpdateResourceAcquisition(DisplayEffectVramState *a)
{
    DisplayEffectVramSlot *slots;
    DisplayEffectVramSlot *slot;
    FileTransferDescriptor *req;
    s32 i;

    if (DisplayEffect_MarkInitialized((MenuRecord *)a) == 0) {
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
        if (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             gFile_dwCancelRequestState) != 0) {
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
        gFile_dwTransferState = req->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
    } else if ((a->state & 0x40) == 0) {
        if (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             gFile_dwCancelRequestState) != 0) {
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
        a->variant_update_flags |= 0x40;
    }
}

/* Seven contiguous display-effect position and update callbacks on the same
 * MenuRecord storage.
 *
 * The final four scale a motion delta by the per-frame step multiplier and do
 * not agree on its width. graphics_frame.h declares gGraphics_nBaseFrameStep once, as s32,
 * and the disagreement is spelled at the use sites: DisplayEffect_FadeInRecord,
 * DisplayEffect_FadeOutRecord and DisplayEffect_CrossfadeResources read it plain, and DisplayEffect_UpdatePortraitTransition reads it
 * `(u16)` twice, which is the halfword load its retail code does. */

/* Regional value: the screen sprite's field_40, its CLUT x (0x200 -> 0x280).
 * The European build (src/game/european/) defines its own. */
#ifndef DISPLAY_EFFECT_SPRITE_CX
#define DISPLAY_EFFECT_SPRITE_CX 0x200
#endif

void DisplayPositionGroup_SetChildPositions(
    DisplayObject **children,
    s16 x,
    s16 y
)
{
    s32 i;

    for (i = 2; i >= 0; i--) {
        if (children[i] != 0) {
            children[i]->xy.s.x = x;
            children[i]->xy.s.y = y;
        }
    }
}

void DisplayPositionGroup_SetPosition(MenuRecord *record, s32 x, s32 y)
{
    record->x = x;
    record->y = y;
    DisplayPositionGroup_SetChildPositions(record->grid[0], (s16)x, (s16)y);
}

/* Eases one display-effect record from its 0x34/0x36 position to the
   0x40/0x42 destination over a quarter turn of cosine, then clears the step
   byte. The record is a MenuRecord, the element type of gDisplayEffect_aMenuRecords and the
   callback argument type stored by gDisplayEffect_apfnSteps.

   Every use goes through that local, including the two calls that want a
   u8 * again. That is not tidiness: leaving `p` live alongside `record` makes
   GCC 2.8.1 hold both in callee-saved registers, which grows the frame by
   eight bytes and the function with it. One name, one register. */
void DisplayEffect_UpdatePositionTween(MenuRecord *p)
{
    MenuRecord *record = p;
    s32 angle_step;
    s32 angle;
    s32 cosine;
    s32 dx;
    s32 dy;

    if (DisplayEffect_MarkInitialized(record) == 0) {
        record->tween_angle = TRIG_ANGLE_QUARTER_TURN;
        angle_step = TRIG_ANGLE_QUARTER_TURN / record->tween_arg.tween_duration;
        record->tween_angle_step = angle_step;
        if (angle_step >= 0) {
            record->tween_angle = 0;
        }
        record->tween_arg.tween_delta_x = *(u16 *)&record->step_arg.tween_target_x - record->x;
        record->tween_delta_y = *(u16 *)&record->tween_target_y - record->y;
    }

    angle = *(u16 *)&record->tween_angle + *(u16 *)&record->tween_angle_step;
    record->tween_angle = angle;

    if ((u16)(angle - 1) >= TRIG_ANGLE_QUARTER_TURN - 1) {
        DisplayPositionGroup_SetPosition(
            record,
            record->step_arg.tween_target_x,
            record->tween_target_y
        );
        record->display_effect_step = 0;
    } else {
        cosine = rcos((s16)angle);
        dx = cosine * record->tween_arg.tween_delta_x / ONE;
        dy = cosine * record->tween_delta_y / ONE;
        if (record->tween_angle_step < 0) {
            dx = record->tween_arg.tween_delta_x - dx;
            dy = record->tween_delta_y - dy;
        }
        DisplayPositionGroup_SetPosition(
            record,
            (s16)(*(u16 *)&record->step_arg.tween_target_x - dx),
            (s16)(*(u16 *)&record->tween_target_y - dy)
        );
    }
}

void DisplayEffect_FadeInRecord(MenuRecord *p) {
    DisplayObject *first;
    DisplayObject **row;
    DisplayObject **cell;
    s32 depth;
    s32 resource_index;
    s32 level;
    s32 color;
    s32 i;

    if (DisplayEffect_MarkInitialized(p) == 0) {
        *(s16 *)&p->x = 0x68;
        p->variant_update_flags |= 0x10;
        if (p->slot_index != 0) {
            *(s16 *)&p->x = 0xD8;
        }
        DisplayPositionGroup_SetChildPositions(
            p->grid[0],
            *(s16 *)&p->x,
            *(s16 *)&p->y
        );
        first = p->grid[0][0];
        depth = first->depth_offset;
        resource_index = first->resource_root_index;
        DisplayEffect_SetObjectRowBlend(p->grid[0], (GsALON | GsAONE), depth);
        row = p->grid[1];
        DisplayEffect_BuildResourceObjects(p, row, resource_index);
        DisplayEffect_SetObjectRowBlend(row, (GsALON | GsATWO), depth - 1);
        p->step_arg.fade_level = 0;
    }

    level = (u16)p->step_arg.fade_level + gGraphics_nBaseFrameStep * 8;
    p->step_arg.fade_level = level;

    if (p->step_arg.fade_level >= 0x80) {
        p->display_effect_step = 0;
        DisplayEffect_SetObjectRowBlend(p->grid[0], 0,
                      p->grid[0][0]->depth_offset);
        DisplayEffect_ReleaseObjectRow(p->grid[1]);
        p->variant_update_flags &= 0xEF;
    } else {
        color = p->step_arg.fade_level;
        color |= (color << 8) | (color << 16);
        for (i = 2, cell = &p->grid[0][2]; i >= 0; i--, cell--) {
            if (cell[0] != 0) {
                cell[0]->rgb.word = color;
            }
            if (cell[3] != 0) {
                cell[3]->rgb.word = color;
            }
        }
    }
}

void DisplayEffect_FadeOutRecord(MenuRecord *p)
{
    DisplayObject *first;
    DisplayObject **row;
    DisplayObject *object;
    s32 depth;
    s32 resource_index;
    s32 level;
    s32 u;
    s32 color;
    s32 i;

    if (DisplayEffect_MarkInitialized(p) == 0) {
        p->variant_update_flags |= 0x10;
        first = p->grid[0][0];
        depth = first->depth_offset;
        resource_index = first->resource_root_index;
        DisplayEffect_SetObjectRowBlend(p->grid[0], (GsALON | GsAONE), depth);
        row = p->grid[1];
        DisplayEffect_BuildResourceObjects(p, row, resource_index);
        DisplayEffect_SetObjectRowBlend(row, (GsALON | GsATWO), depth - 1);
        p->step_arg.fade_level = 0x80;
    }

    level = *(u16 *)&p->step_arg.fade_level - (gGraphics_nBaseFrameStep << 3);
    p->step_arg.fade_level = level;
    u = (s16)level;
    if (u <= 0) {
        p->display_effect_step = 0;
        DisplayEffect_ReleaseObjectRow(p->grid[1]);
        DisplayEffect_ReleaseRecord(p);
    } else {
        color = u;
        color |= (color << 8) | (color << 16);
        for (i = 2; i >= 0; i--) {
            object = p->grid[0][i];
            if (object != 0) {
                object->rgb.word = color;
            }
            object = p->grid[1][i];
            if (object != 0) {
                object->rgb.word = color;
            }
        }
    }
}

void DisplayEffect_CrossfadeResources(MenuRecord *p)
{
    s32 dd[2];
    DisplayObject *first;
    s32 resource_index;
    s32 depth;
    s32 lower_depth;
    s32 m;
    s16 level;
    s32 color;
    s32 w;
    s16 y;
    s32 x;
    DisplayObject *object;

    if (DisplayEffect_MarkInitialized(p) == 0) {
        p->variant_update_flags |= 0x10;
        first = p->grid[0][0];
        resource_index = first->resource_root_index;
        depth = first->depth_offset;
        DisplayEffect_ReleaseObjectRow(p->grid[0]);
        DisplayEffect_BuildResourceObjects(
            p, p->grid[0], p->resource_entry_index);
        DisplayEffect_SetObjectRowBlend(p->grid[0], (GsALON | GsAONE), depth);
        DisplayEffect_BuildResourceObjects(
            p, p->grid[1], p->resource_entry_index);
        lower_depth = depth - 1;
        DisplayEffect_SetObjectRowBlend(p->grid[1], (GsALON | GsATWO), lower_depth);
        DisplayEffect_BuildResourceObjects(
            p, p->grid[2], resource_index);
        DisplayEffect_SetObjectRowBlend(p->grid[2], (GsALON | GsAONE), depth);
        DisplayEffect_BuildResourceObjects(
            p, p->grid[3], resource_index);
        DisplayEffect_SetObjectRowBlend(p->grid[3], (GsALON | GsATWO), lower_depth);
        p->step_arg.fade_level = 0x80;
    }
    level = (u16)p->step_arg.fade_level - gGraphics_nBaseFrameStep * 8;
    p->step_arg.fade_level = level;
    if (level <= 0) {
        p->display_effect_step = 0;
        DisplayEffect_SetObjectRowBlend(p->grid[0], 0,
                      p->grid[0][0]->depth_offset);
        DisplayPositionGroup_SetChildPositions(
            p->grid[0],
            *(s16 *)&p->x,
            *(s16 *)&p->y
        );
        DisplayEffect_ReleaseObjectRow(p->grid[1]);
        DisplayEffect_ReleaseObjectRow(p->grid[2]);
        DisplayEffect_ReleaseObjectRow(p->grid[3]);
        p->variant_update_flags &= 0xEF;
        return;
    }
    color = level;
    color |= (color << 8) | (color << 0x10);
    for (m = 2; m >= 0; m--) {
        object = p->grid[2][m];
        if (object != 0) {
            object->rgb.word = color;
        }
        object = p->grid[3][m];
        if (object != 0) {
            object->rgb.word = color;
        }
    }
    color = 0x80 - p->step_arg.fade_level;
    color |= (color << 8) | (color << 0x10);
    for (m = 2; m >= 0; m--) {
        object = p->grid[0][m];
        if (object != 0) {
            object->rgb.word = color;
        }
        object = p->grid[1][m];
        if (object != 0) {
            object->rgb.word = color;
        }
    }
    dd[0] = p->step_arg.fade_level / 8;
    w = (0x80 - p->step_arg.fade_level) / 8;
    dd[1] = -w;
    if (p->slot_index == 0) {
        dd[0] = -dd[0];
        dd[1] = w;
    }
    y = *(s16 *)&p->y;
    x = *(s16 *)&p->x + dd[0];
    x = (s16)x;
    DisplayPositionGroup_SetChildPositions(p->grid[0], x, y);
    DisplayPositionGroup_SetChildPositions(p->grid[1], x, y);
    x = *(s16 *)&p->x - dd[1];
    x = (s16)x;
    DisplayPositionGroup_SetChildPositions(p->grid[2], x, y);
    DisplayPositionGroup_SetChildPositions(p->grid[3], x, y);
}

void DisplayEffect_UpdatePortraitTransition(MenuRecord *record)
{
    DisplayObject *o;
    DisplayObject *o2;
    s32 id;
    s32 idx;
    s32 shade;
    s32 scale_x;

    /* A portrait record keeps its two display objects in grid[0][0] and
       grid[0][1]. */
    if (DisplayEffect_MarkInitialized(record) == 0) {
        if (record->step_arg.closing != 0) {
            o = record->grid[0][0];
            *(u16 *)&o->field_60 = 0;
        } else {
            id = record->effect_id;
            idx = id - CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID;
            o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
            DisplayObject_ConfigureScreenSprite(o, (s16)record->x, (s16)record->y, 0x30, 0x30, 0, 0, 0xE, 0x380, 0xF0);
            *(u16 *)&o->field_40.clut.x += (idx >> 4) << 6;
            o->texture_uv.b.u =
                (idx % CAMPAIGN_DIALOG_PORTRAIT_GRID_COLUMN_COUNT) *
                CAMPAIGN_DIALOG_PORTRAIT_IMAGE_WIDTH;
            o->texture_uv.b.v =
                (idx / CAMPAIGN_DIALOG_PORTRAIT_GRID_COLUMN_COUNT) *
                CAMPAIGN_DIALOG_PORTRAIT_IMAGE_HEIGHT;
            *(u16 *)&o->field_40.clut.y +=
                idx % CAMPAIGN_DIALOG_PORTRAIT_CLUT_ROWS_PER_COLUMN;
            *(u16 *)&o->field_44.scale.y = 0;
            o->rgb.word = 0;
            o->attribute |=
                GsALON | GsAONE | DISPLAY_OBJECT_ATTRIBUTE_8BPP;
            DisplayObject_SelectOrderingTable1(o);
            DisplayObject_SetDepthOffset(o, -8);
            record->grid[0][0] = o;
            *(u16 *)&o->field_60 = 0x14;
        }
        o->attribute = (o->attribute | (GsALON | GsAONE)) & ~GsROTOFF;
        *(u16 *)&o->field_48.pivot.y = 0;
        o2 = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
        DisplayObject_ConfigureScreenSprite(o2, *(s16 *)&o->xy.h.x, *(s16 *)&o->xy.h.y, 0x30, 0x30, o->texture_uv.b.u, o->texture_uv.b.v, 0xE, DISPLAY_EFFECT_SPRITE_CX, 0xFD);
        o2->attribute = (o2->attribute |
            GsALON | GsATWO | DISPLAY_OBJECT_ATTRIBUTE_8BPP) & ~GsROTOFF;
        *(u16 *)&o2->field_44.scale.y = *(u16 *)&o->field_44.scale.y;
        *(u16 *)&o2->field_48.pivot.y = 0;
        DisplayObject_SelectOrderingTable1(o2);
        DisplayObject_SetDepthOffset(o2, -9);
        record->grid[0][1] = o2;
    }
    o = record->grid[0][0];
    o2 = record->grid[0][1];
    if (record->step_arg.closing != 0) {
        *(u16 *)&o->field_60 += (u16)gGraphics_nBaseFrameStep;
    } else {
        *(u16 *)&o->field_60 -= (u16)gGraphics_nBaseFrameStep;
    }
    /* The faded-out arm sits first and joins the release arm at the step
       clear; the in-range fade continues after the switch. */
    switch (o->field_60 > 0) {
    case 0:
        o->attribute = (o->attribute & ~(GsALON | GsATWO | GsAONE)) | GsROTOFF;
        o->rgb.word = COLOR_RGB24_NEUTRAL_GREY;
        *(u16 *)&o->field_44.scale.y = 0x1000;
        DisplayObject_ReleaseIfPresent(o2);
        record->grid[0][1] = 0;
        if (0) {
        default:
            if (o->field_60 < 0x14) {
                break;
            }
            DisplayEffect_ReleaseRecord(record);
        }
        record->display_effect_step = 0;
        return;
    }
    shade = -0x80 - o->field_60 * 6;
    o2->rgb.c.b = shade;
    o2->rgb.c.g = shade;
    o2->rgb.c.r = shade;
    o->rgb.c.b = shade;
    o->rgb.c.g = shade;
    o->rgb.c.r = shade;
    idx = o->field_60 * 204 + 0x1000;
    *(u16 *)&o2->field_44.scale.y = idx;
    *(u16 *)&o->field_44.scale.y = idx;
    scale_x = o->rgb.c.r << 5;
    *(u16 *)&o2->field_44.scale.x = scale_x;
    *(u16 *)&o->field_44.scale.x = scale_x;
}

/* Initialized data this unit owns at 0x80090F68: the display effect step
 * table and, directly after it, the dialog transition step table.
 *
 * DisplayEffect_ProcessMenuRecords walks the menu records and, for each live
 * one, dispatches `display_effect_step & 0x1F`. The mask permits thirty-two
 * entries and there are eight; the game only stores steps zero through six.
 *
 * The table and dispatcher use MenuRecord *, matching the arena they walk.
 * The single VRAM entry retains its cast because that handler takes the
 * DisplayEffectVramState layout view of the same record.
 * The entries include two that share DisplayEffect_FinishStepWhenIdle.
 *
 * The memory card dialog selects one of the five transition steps with its
 * callback state and passes the root and slot; each step reads the root
 * through its own MenuRecord view and ignores the slot. */

void (*gDisplayEffect_apfnSteps[])(MenuRecord *) = {
    DisplayEffect_FinishStepWhenIdle,
    DisplayEffect_FinishStepWhenIdle,
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
            if ((p->grid[0][1])->field_5A == 0) {
                p->variant_countdown =
                    (rand() & DISPLAY_EFFECT_DELAY_MASK) +
                    DISPLAY_EFFECT_DELAY_BASE;
                p->variant_update_flags = p->variant_update_flags & 0xFE;
                DisplayObject_SetResourceVariant(
                    p->grid[0][1], 0);
            }
        } else {
            v = p->variant_countdown - 1;
            p->variant_countdown = v;
            if ((s16)v <= 0) {
                p->variant_update_flags = p->variant_update_flags | 1;
                DisplayObject_SetResourceVariant(
                    p->grid[0][1], 1);
            }
        }
    }

    if (p->grid[0][2] == 0) {
        return;
    }

    g = p->variant_update_flags;
    /* The slow countdown sits after the shared variant request; the switch
       lets the clear-flag arm reach it with a break. */
    switch (g & 2) {
    default:
        c = p->variant_update_counter - 1;
        p->variant_update_counter = c;
        if ((p->grid[0][2])->field_5A != 0) {
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
        if (0) {
        case 0:
            if (n < 0) {
                break;
            }
            p->variant_update_flags = g | 2;
        }
        p->variant_update_counter = 6;
        p->requested_variant = n;
        DisplayObject_SetResourceVariant(
            p->grid[0][2], n);
        return;
    }

    p->variant_update_counter = p->variant_update_counter + 1;
    if ((s8)p->variant_update_counter < 6) {
        return;
    }
    DisplayObject_UpdateResourceVariant(
        p->grid[0][2],
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
