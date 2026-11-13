/* Seven contiguous display-effect position and update callbacks on the same
 * MenuRecord / DisplayEffectState storage.
 *
 * The final four scale a motion delta by the per-frame step multiplier and do
 * not agree on its width. graphics_frame.h declares gGraphics_nBaseFrameStep once, as s32,
 * and the disagreement is spelled at the use sites: DisplayEffect_FadeInRecord,
 * DisplayEffect_FadeOutRecord and DisplayEffect_CrossfadeResources read it plain, and DisplayEffect_UpdatePortraitTransition reads it
 * `(u16)` twice, which is the halfword load its retail code does. */
#include "../types.h"
#include "display_object_system.h"
#include "display_effect_lifecycle.h"
#include "script_scene.h"
#include "color_constants.h"
#include "main.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_effect_resource_setup.h"
#include "display_effect_update_callbacks.h"
#include "menu_record.h"
#include "trig_constants.h"
#include "graphics_frame_controls.h"

/* Regional value: the screen sprite's field_40, its CLUT x (0x200 -> 0x280).
 * The European build (src/game/european/) defines its own. */
#ifndef DISPLAY_EFFECT_SPRITE_CX
#define DISPLAY_EFFECT_SPRITE_CX 0x200
#endif

#define DISPLAY_EFFECT_STATE_VIEW(record) \
    ((DisplayEffectState *)(record))
#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))
#define DISPLAY_OBJECT_COLOR_BYTES(object) ((u8 *)&(object)->rgb)
#define DISPLAY_POSITION_GROUP_VIEW(record) \
    ((DisplayPositionGroup *)(record))

void DisplayPositionGroup_SetChildPositions(
    DisplayPositionGroup *group,
    s16 x,
    s16 y
)
{
    s32 i;

    for (i = 2; i >= 0; i--) {
        if (group->children[i] != 0) {
            group->children[i]->x = x;
            group->children[i]->y = y;
        }
    }
}

void DisplayPositionGroup_SetPosition(
    DisplayPositionGroup *group,
    s32 x,
    s32 y
)
{
    group->x = x;
    group->y = y;
    DisplayPositionGroup_SetChildPositions(group, (s16)x, (s16)y);
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

    if (DisplayEffect_MarkInitialized(DISPLAY_EFFECT_STATE_VIEW(record)) == 0) {
        record->tween_angle = TRIG_ANGLE_QUARTER_TURN;
        angle_step = TRIG_ANGLE_QUARTER_TURN / record->field_44;
        record->tween_angle_step = angle_step;
        if (angle_step >= 0) {
            record->tween_angle = 0;
        }
        record->field_44 = *(u16 *)&record->field_40 - record->x;
        record->tween_delta_y = *(u16 *)&record->tween_target_y - record->y;
    }

    angle = *(u16 *)&record->tween_angle + *(u16 *)&record->tween_angle_step;
    record->tween_angle = angle;

    if ((u16)(angle - 1) >= TRIG_ANGLE_QUARTER_TURN - 1) {
        DisplayPositionGroup_SetPosition(
            DISPLAY_POSITION_GROUP_VIEW(record),
            record->field_40,
            record->tween_target_y
        );
        record->display_effect_step = 0;
    } else {
        cosine = rcos((s16)angle);
        dx = cosine * record->field_44 / ONE;
        dy = cosine * record->tween_delta_y / ONE;
        if (record->tween_angle_step < 0) {
            dx = record->field_44 - dx;
            dy = record->tween_delta_y - dy;
        }
        DisplayPositionGroup_SetPosition(
            DISPLAY_POSITION_GROUP_VIEW(record),
            (s16)(*(u16 *)&record->field_40 - dx),
            (s16)(*(u16 *)&record->tween_target_y - dy)
        );
    }
}

void DisplayEffect_FadeInRecord(MenuRecord *p) {
    DisplayObject *first;
    s32 *row;
    s32 *cell;
    s32 depth;
    s32 resource_index;
    s32 level;
    s32 color;
    s32 i;

    if (DisplayEffect_MarkInitialized(DISPLAY_EFFECT_STATE_VIEW(p)) == 0) {
        *(s16 *)&p->x = 0x68;
        p->variant_update_flags |= 0x10;
        if (p->slot_index != 0) {
            *(s16 *)&p->x = 0xD8;
        }
        DisplayPositionGroup_SetChildPositions(
            DISPLAY_POSITION_GROUP_VIEW(p),
            *(s16 *)&p->x,
            *(s16 *)&p->y
        );
        first = DISPLAY_OBJECT_VIEW(p->grid[0][0]);
        depth = first->depth_offset;
        resource_index = first->resource_root_index;
        DisplayEffect_SetObjectRowBlend((u8 **)p->grid[0], (GsALON | GsAONE), depth);
        row = p->grid[1];
        DisplayEffect_BuildResourceObjects(p, (DisplayObject **)row, resource_index);
        DisplayEffect_SetObjectRowBlend((u8 **)row, (GsALON | GsATWO), depth - 1);
        p->field_40 = 0;
    }

    level = (u16)p->field_40 + gGraphics_nBaseFrameStep * 8;
    p->field_40 = level;

    if (p->field_40 >= 0x80) {
        p->display_effect_step = 0;
        DisplayEffect_SetObjectRowBlend((u8 **)p->grid[0], 0,
                      DISPLAY_OBJECT_VIEW(p->grid[0][0])->depth_offset);
        DisplayEffect_ReleaseObjectRow((void **)p->grid[1]);
        p->variant_update_flags &= 0xEF;
    } else {
        color = p->field_40;
        color |= (color << 8) | (color << 16);
        for (i = 2, cell = &p->grid[0][2]; i >= 0; i--, cell--) {
            if (cell[0] != 0) {
                DISPLAY_OBJECT_VIEW(cell[0])->rgb = color;
            }
            if (cell[3] != 0) {
                DISPLAY_OBJECT_VIEW(cell[3])->rgb = color;
            }
        }
    }
}

void DisplayEffect_FadeOutRecord(MenuRecord *p)
{
    DisplayObject *first;
    u8 **row;
    DisplayObject *object;
    s32 depth;
    s32 resource_index;
    s32 level;
    s32 u;
    s32 color;
    s32 i;

    if (DisplayEffect_MarkInitialized(DISPLAY_EFFECT_STATE_VIEW(p)) == 0) {
        p->variant_update_flags |= 0x10;
        first = DISPLAY_OBJECT_VIEW(p->grid[0][0]);
        depth = first->depth_offset;
        resource_index = first->resource_root_index;
        DisplayEffect_SetObjectRowBlend((u8 **)p->grid[0], (GsALON | GsAONE), depth);
        row = (u8 **)p->grid[1];
        DisplayEffect_BuildResourceObjects(p, (DisplayObject **)row, resource_index);
        DisplayEffect_SetObjectRowBlend(row, (GsALON | GsATWO), depth - 1);
        p->field_40 = 0x80;
    }

    level = *(u16 *)&p->field_40 - (gGraphics_nBaseFrameStep << 3);
    p->field_40 = level;
    u = (s16)level;
    if (u <= 0) {
        p->display_effect_step = 0;
        DisplayEffect_ReleaseObjectRow((void **)p->grid[1]);
        DisplayEffect_ReleaseRecord(p);
    } else {
        color = u;
        color |= (color << 8) | (color << 16);
        for (i = 2; i >= 0; i--) {
            object = DISPLAY_OBJECT_VIEW(p->grid[0][i]);
            if (object != 0) {
                object->rgb = color;
            }
            object = DISPLAY_OBJECT_VIEW(p->grid[1][i]);
            if (object != 0) {
                object->rgb = color;
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

    if (DisplayEffect_MarkInitialized(DISPLAY_EFFECT_STATE_VIEW(p)) == 0) {
        p->variant_update_flags |= 0x10;
        first = DISPLAY_OBJECT_VIEW(p->grid[0][0]);
        resource_index = first->resource_root_index;
        depth = first->depth_offset;
        DisplayEffect_ReleaseObjectRow((void **)p->grid[0]);
        DisplayEffect_BuildResourceObjects(
            p, (DisplayObject **)p->grid[0], p->resource_entry_index);
        DisplayEffect_SetObjectRowBlend((u8 **)p->grid[0], (GsALON | GsAONE), depth);
        DisplayEffect_BuildResourceObjects(
            p, (DisplayObject **)p->grid[1], p->resource_entry_index);
        lower_depth = depth - 1;
        DisplayEffect_SetObjectRowBlend((u8 **)p->grid[1], (GsALON | GsATWO), lower_depth);
        DisplayEffect_BuildResourceObjects(
            p, (DisplayObject **)p->grid[2], resource_index);
        DisplayEffect_SetObjectRowBlend((u8 **)p->grid[2], (GsALON | GsAONE), depth);
        DisplayEffect_BuildResourceObjects(
            p, (DisplayObject **)p->grid[3], resource_index);
        DisplayEffect_SetObjectRowBlend((u8 **)p->grid[3], (GsALON | GsATWO), lower_depth);
        p->field_40 = 0x80;
    }
    level = (u16)p->field_40 - gGraphics_nBaseFrameStep * 8;
    p->field_40 = level;
    if (level <= 0) {
        p->display_effect_step = 0;
        DisplayEffect_SetObjectRowBlend((u8 **)p->grid[0], 0,
                      DISPLAY_OBJECT_VIEW(p->grid[0][0])->depth_offset);
        DisplayPositionGroup_SetChildPositions(
            DISPLAY_POSITION_GROUP_VIEW(p),
            *(s16 *)&p->x,
            *(s16 *)&p->y
        );
        DisplayEffect_ReleaseObjectRow((void **)p->grid[1]);
        DisplayEffect_ReleaseObjectRow((void **)p->grid[2]);
        DisplayEffect_ReleaseObjectRow((void **)p->grid[3]);
        p->variant_update_flags &= 0xEF;
        return;
    }
    color = level;
    color |= (color << 8) | (color << 0x10);
    for (m = 2; m >= 0; m--) {
        object = *(DisplayObject **)&p->grid[2][m];
        if (object != 0) {
            object->rgb = color;
        }
        object = *(DisplayObject **)&p->grid[3][m];
        if (object != 0) {
            object->rgb = color;
        }
    }
    color = 0x80 - p->field_40;
    color |= (color << 8) | (color << 0x10);
    for (m = 2; m >= 0; m--) {
        object = *(DisplayObject **)&p->grid[0][m];
        if (object != 0) {
            object->rgb = color;
        }
        object = *(DisplayObject **)&p->grid[1][m];
        if (object != 0) {
            object->rgb = color;
        }
    }
    dd[0] = p->field_40 / 8;
    w = (0x80 - p->field_40) / 8;
    dd[1] = -w;
    if (p->slot_index == 0) {
        dd[0] = -dd[0];
        dd[1] = w;
    }
    y = *(s16 *)&p->y;
    x = *(s16 *)&p->x + dd[0];
    x = (s16)x;
    DisplayPositionGroup_SetChildPositions(DISPLAY_POSITION_GROUP_VIEW(p), x, y);
    DisplayPositionGroup_SetChildPositions(
        DISPLAY_POSITION_GROUP_VIEW(p->grid[1]), x, y
    );
    x = *(s16 *)&p->x - dd[1];
    x = (s16)x;
    DisplayPositionGroup_SetChildPositions(
        DISPLAY_POSITION_GROUP_VIEW(p->grid[2]), x, y
    );
    DisplayPositionGroup_SetChildPositions(
        DISPLAY_POSITION_GROUP_VIEW(p->grid[3]), x, y
    );
}

void DisplayEffect_UpdatePortraitTransition(MenuRecord *record)
{
    DisplayObject *o;
    DisplayObject *o2;
    s32 id;
    s32 idx;
    s32 shade;
    s32 scale_x;

    /* A portrait record keeps its two display objects in the first two
       words of grid, as integers like the rest of that array. */
    if (DisplayEffect_MarkInitialized(DISPLAY_EFFECT_STATE_VIEW(record)) == 0) {
        if (record->field_40 != 0) {
            o = DISPLAY_OBJECT_VIEW(record->grid[0][0]);
            *(u16 *)&o->field_60 = 0;
        } else {
            id = record->effect_id;
            idx = id - CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID;
            o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
            DisplayObject_ConfigureScreenSprite(o, (s16)record->x, (s16)record->y, 0x30, 0x30, 0, 0, 0xE, 0x380, 0xF0);
            *(u16 *)&o->field_40.clut.x += (idx >> 4) << 6;
            *(u8 *)&o->texture_uv =
                (idx % CAMPAIGN_DIALOG_PORTRAIT_GRID_COLUMN_COUNT) *
                CAMPAIGN_DIALOG_PORTRAIT_IMAGE_WIDTH;
            ((u8 *)&o->texture_uv)[1] =
                (idx / CAMPAIGN_DIALOG_PORTRAIT_GRID_COLUMN_COUNT) *
                CAMPAIGN_DIALOG_PORTRAIT_IMAGE_HEIGHT;
            *(u16 *)&o->field_40.clut.y +=
                idx % CAMPAIGN_DIALOG_PORTRAIT_CLUT_ROWS_PER_COLUMN;
            *(u16 *)&o->field_44.scale.y = 0;
            o->rgb = 0;
            o->attribute |=
                GsALON | GsAONE | DISPLAY_OBJECT_ATTRIBUTE_8BPP;
            DisplayObject_SelectOrderingTable1(o);
            DisplayObject_SetDepthOffset(o, -8);
            record->grid[0][0] = (s32)o;
            *(u16 *)&o->field_60 = 0x14;
        }
        o->attribute = (o->attribute | (GsALON | GsAONE)) & ~GsROTOFF;
        *(u16 *)&o->field_48.pivot.y = 0;
        o2 = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
        DisplayObject_ConfigureScreenSprite(o2, *(s16 *)&o->xy.h.x, *(s16 *)&o->xy.h.y, 0x30, 0x30, *(u8 *)&o->texture_uv, ((u8 *)&o->texture_uv)[1], 0xE, DISPLAY_EFFECT_SPRITE_CX, 0xFD);
        o2->attribute = (o2->attribute |
            GsALON | GsATWO | DISPLAY_OBJECT_ATTRIBUTE_8BPP) & ~GsROTOFF;
        *(u16 *)&o2->field_44.scale.y = *(u16 *)&o->field_44.scale.y;
        *(u16 *)&o2->field_48.pivot.y = 0;
        DisplayObject_SelectOrderingTable1(o2);
        DisplayObject_SetDepthOffset(o2, -9);
        record->grid[0][1] = (s32)o2;
    }
    o = DISPLAY_OBJECT_VIEW(record->grid[0][0]);
    o2 = DISPLAY_OBJECT_VIEW(record->grid[0][1]);
    if (record->field_40 != 0) {
        *(u16 *)&o->field_60 += (u16)gGraphics_nBaseFrameStep;
    } else {
        *(u16 *)&o->field_60 -= (u16)gGraphics_nBaseFrameStep;
    }
    /* The faded-out arm sits first and joins the release arm at the step
       clear; the in-range fade continues after the switch. */
    switch (o->field_60 > 0) {
    case 0:
        o->attribute = (o->attribute & ~(GsALON | GsATWO | GsAONE)) | GsROTOFF;
        o->rgb = COLOR_RGB24_NEUTRAL_GREY;
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
    DISPLAY_OBJECT_COLOR_BYTES(o2)[2] = shade;
    DISPLAY_OBJECT_COLOR_BYTES(o2)[1] = shade;
    DISPLAY_OBJECT_COLOR_BYTES(o2)[0] = shade;
    DISPLAY_OBJECT_COLOR_BYTES(o)[2] = shade;
    DISPLAY_OBJECT_COLOR_BYTES(o)[1] = shade;
    DISPLAY_OBJECT_COLOR_BYTES(o)[0] = shade;
    idx = o->field_60 * 204 + 0x1000;
    *(u16 *)&o2->field_44.scale.y = idx;
    *(u16 *)&o->field_44.scale.y = idx;
    scale_x = DISPLAY_OBJECT_COLOR_BYTES(o)[0] << 5;
    *(u16 *)&o2->field_44.scale.x = scale_x;
    *(u16 *)&o->field_44.scale.x = scale_x;
}
