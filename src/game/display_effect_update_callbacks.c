/* Seven contiguous display-effect position and update callbacks on the same
 * MenuRecord / DisplayEffectState storage.
 *
 * The final four scale a motion delta by the per-frame step multiplier and do
 * not agree on its width. graphics_frame.h declares gGraphics_nBaseFrameStep once, as s32,
 * and the disagreement is spelled at the use sites: DisplayEffect_FadeInRecord,
 * DisplayEffect_FadeOutRecord and DisplayEffect_CrossfadeResources read it plain, and DisplayEffect_UpdatePortraitTransition reads it
 * `(u16)` twice, which is the halfword load its retail code does. A unit that
 * needs the reload rather than a width selects that for the whole unit --
 * display_object_fade_callbacks.c defines D_8009B0D8_IS_VOLATILE. */
#include "../types.h"
#include "display_object_config.h"
#include "display_effect_lifecycle.h"
#include "campaign_scene_package.h"
#include "color_constants.h"
#include "graphics_frame.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_effect_set_object_row_blend.h"
#include "display_effect_build_resource_objects.h"
#include "display_object_core.h"
#include "display_object_helpers.h"
#include "display_object_position.h"
#include "menu_record.h"
#include "trig_constants.h"
#include "display_effect_update_position_tween.h"
#include "display_effect_fade_in_record.h"
#include "display_effect_fade_out_record.h"
#include "display_effect_update_callbacks.h"
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
   byte. The record is a MenuRecord, the element type of D_800EB010 and the
   callback argument type stored by gDisplayEffect_apfnSteps.

   Every use goes through that local, including the two calls that want a
   u8 * again. That is not tidiness: leaving `p` live alongside `r` makes
   GCC 2.8.1 hold both in callee-saved registers, which grows the frame by
   eight bytes and the function with it. One name, one register. */
void DisplayEffect_UpdatePositionTween(MenuRecord *p)
{
    MenuRecord *r = p;
    s32 d;
    s32 t;
    s32 c;
    s32 dx;
    s32 dy;

    if (DisplayEffect_MarkInitialized(DISPLAY_EFFECT_STATE_VIEW(r)) == 0) {
        r->field_48 = TRIG_ANGLE_QUARTER_TURN;
        d = TRIG_ANGLE_QUARTER_TURN / r->field_44;
        r->field_4A = d;
        if (d >= 0) {
            r->field_48 = 0;
        }
        r->field_44 = *(u16 *)&r->field_40 - r->field_34;
        r->field_46 = *(u16 *)&r->field_42 - r->field_36;
    }

    t = *(u16 *)&r->field_48 + *(u16 *)&r->field_4A;
    r->field_48 = t;

    if ((u16)(t - 1) >= TRIG_ANGLE_QUARTER_TURN - 1) {
        DisplayPositionGroup_SetPosition(
            DISPLAY_POSITION_GROUP_VIEW(r),
            r->field_40,
            r->field_42
        );
        r->display_effect_step = 0;
    } else {
        c = rcos((s16)t);
        dx = c * r->field_44 / ONE;
        dy = c * r->field_46 / ONE;
        if (r->field_4A < 0) {
            dx = r->field_44 - dx;
            dy = r->field_46 - dy;
        }
        DisplayPositionGroup_SetPosition(
            DISPLAY_POSITION_GROUP_VIEW(r),
            (s16)(*(u16 *)&r->field_40 - dx),
            (s16)(*(u16 *)&r->field_42 - dy)
        );
    }
}

void DisplayEffect_FadeInRecord(MenuRecord *p) {
    DisplayObject *q;
    s32 *e;
    s32 *c;
    s32 a;
    s32 b;
    s32 v;
    s32 m;
    s32 i;

    if (DisplayEffect_MarkInitialized(DISPLAY_EFFECT_STATE_VIEW(p)) == 0) {
        *(s16 *)&p->field_34 = 0x68;
        p->field_32 |= 0x10;
        if (p->slot_index != 0) {
            *(s16 *)&p->field_34 = 0xD8;
        }
        DisplayPositionGroup_SetChildPositions(
            DISPLAY_POSITION_GROUP_VIEW(p),
            *(s16 *)&p->field_34,
            *(s16 *)&p->field_36
        );
        q = DISPLAY_OBJECT_VIEW(p->grid[0][0]);
        a = q->depth_offset;
        b = q->field_67;
        DisplayEffect_SetObjectRowBlend((u8 **)p->grid[0], (GsALON | GsAONE), a);
        e = p->grid[1];
        DisplayEffect_BuildResourceObjects(p, (DisplayObject **)e, b);
        DisplayEffect_SetObjectRowBlend((u8 **)e, (GsALON | GsATWO), a - 1);
        p->field_40 = 0;
    }

    v = (u16)p->field_40 + gGraphics_nBaseFrameStep * 8;
    p->field_40 = v;

    if (p->field_40 >= 0x80) {
        p->display_effect_step = 0;
        DisplayEffect_SetObjectRowBlend((u8 **)p->grid[0], 0,
                      DISPLAY_OBJECT_VIEW(p->grid[0][0])->depth_offset);
        DisplayEffect_ReleaseObjectRow((void **)p->grid[1]);
        p->field_32 &= 0xEF;
    } else {
        m = p->field_40;
        m |= (m << 8) | (m << 16);
        for (i = 2, c = &p->grid[0][2]; i >= 0; i--, c--) {
            if (c[0] != 0) {
                DISPLAY_OBJECT_VIEW(c[0])->rgb = m;
            }
            if (c[3] != 0) {
                DISPLAY_OBJECT_VIEW(c[3])->rgb = m;
            }
        }
    }
}

void DisplayEffect_FadeOutRecord(MenuRecord *p)
{
    DisplayObject *h;
    u8 **d;
    DisplayObject *e;
    s32 x;
    s32 y;
    s32 t;
    s32 u;
    s32 m;
    s32 i;

    if (DisplayEffect_MarkInitialized(DISPLAY_EFFECT_STATE_VIEW(p)) == 0) {
        p->field_32 |= 0x10;
        h = DISPLAY_OBJECT_VIEW(p->grid[0][0]);
        x = h->depth_offset;
        y = h->field_67;
        DisplayEffect_SetObjectRowBlend((u8 **)p->grid[0], (GsALON | GsAONE), x);
        d = (u8 **)p->grid[1];
        DisplayEffect_BuildResourceObjects(p, (DisplayObject **)d, y);
        DisplayEffect_SetObjectRowBlend(d, (GsALON | GsATWO), x - 1);
        p->field_40 = 0x80;
    }

    t = *(u16 *)&p->field_40 - (gGraphics_nBaseFrameStep << 3);
    p->field_40 = t;
    u = (s16)t;
    if (u <= 0) {
        p->display_effect_step = 0;
        DisplayEffect_ReleaseObjectRow((void **)p->grid[1]);
        DisplayEffect_ReleaseRecord(p);
    } else {
        m = u;
        m |= (m << 8) | (m << 16);
        for (i = 2; i >= 0; i--) {
            e = DISPLAY_OBJECT_VIEW(p->grid[0][i]);
            if (e != 0) {
                e->rgb = m;
            }
            e = DISPLAY_OBJECT_VIEW(p->grid[1][i]);
            if (e != 0) {
                e->rgb = m;
            }
        }
    }
}

void DisplayEffect_CrossfadeResources(MenuRecord *p)
{
    s32 dd[2];
    DisplayObject *r;
    s32 a;
    s32 b;
    s32 d;
    s32 m;
    s16 n;
    s32 c;
    s32 w;
    s16 y;
    s32 x;
    DisplayObject *o;

    if (DisplayEffect_MarkInitialized(DISPLAY_EFFECT_STATE_VIEW(p)) == 0) {
        p->field_32 |= 0x10;
        r = DISPLAY_OBJECT_VIEW(p->grid[0][0]);
        a = r->field_67;
        b = r->depth_offset;
        DisplayEffect_ReleaseObjectRow((void **)p->grid[0]);
        DisplayEffect_BuildResourceObjects(
            p, (DisplayObject **)p->grid[0], p->resource_entry_index);
        DisplayEffect_SetObjectRowBlend((u8 **)p->grid[0], (GsALON | GsAONE), b);
        DisplayEffect_BuildResourceObjects(
            p, (DisplayObject **)p->grid[1], p->resource_entry_index);
        d = b - 1;
        DisplayEffect_SetObjectRowBlend((u8 **)p->grid[1], (GsALON | GsATWO), d);
        DisplayEffect_BuildResourceObjects(
            p, (DisplayObject **)p->grid[2], a);
        DisplayEffect_SetObjectRowBlend((u8 **)p->grid[2], (GsALON | GsAONE), b);
        DisplayEffect_BuildResourceObjects(
            p, (DisplayObject **)p->grid[3], a);
        DisplayEffect_SetObjectRowBlend((u8 **)p->grid[3], (GsALON | GsATWO), d);
        p->field_40 = 0x80;
    }
    n = (u16)p->field_40 - gGraphics_nBaseFrameStep * 8;
    p->field_40 = n;
    if (n <= 0) {
        p->display_effect_step = 0;
        DisplayEffect_SetObjectRowBlend((u8 **)p->grid[0], 0,
                      DISPLAY_OBJECT_VIEW(p->grid[0][0])->depth_offset);
        DisplayPositionGroup_SetChildPositions(
            DISPLAY_POSITION_GROUP_VIEW(p),
            *(s16 *)&p->field_34,
            *(s16 *)&p->field_36
        );
        DisplayEffect_ReleaseObjectRow((void **)p->grid[1]);
        DisplayEffect_ReleaseObjectRow((void **)p->grid[2]);
        DisplayEffect_ReleaseObjectRow((void **)p->grid[3]);
        p->field_32 &= 0xEF;
        return;
    }
    c = n;
    c |= (c << 8) | (c << 0x10);
    for (m = 2; m >= 0; m--) {
        o = *(DisplayObject **)&p->grid[2][m];
        if (o != 0) {
            o->rgb = c;
        }
        o = *(DisplayObject **)&p->grid[3][m];
        if (o != 0) {
            o->rgb = c;
        }
    }
    c = 0x80 - p->field_40;
    c |= (c << 8) | (c << 0x10);
    for (m = 2; m >= 0; m--) {
        o = *(DisplayObject **)&p->grid[0][m];
        if (o != 0) {
            o->rgb = c;
        }
        o = *(DisplayObject **)&p->grid[1][m];
        if (o != 0) {
            o->rgb = c;
        }
    }
    dd[0] = p->field_40 / 8;
    w = (0x80 - p->field_40) / 8;
    dd[1] = -w;
    if (p->slot_index == 0) {
        dd[0] = -dd[0];
        dd[1] = w;
    }
    y = *(s16 *)&p->field_36;
    x = *(s16 *)&p->field_34 + dd[0];
    x = (s16)x;
    DisplayPositionGroup_SetChildPositions(DISPLAY_POSITION_GROUP_VIEW(p), x, y);
    DisplayPositionGroup_SetChildPositions(
        DISPLAY_POSITION_GROUP_VIEW(p->grid[1]), x, y
    );
    x = *(s16 *)&p->field_34 - dd[1];
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
    s32 c;
    s32 q;

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
            DisplayObject_ConfigureScreenSprite(o, (s16)record->field_34, (s16)record->field_36, 0x30, 0x30, 0, 0, 0xE, 0x380, 0xF0);
            *(u16 *)&o->field_40.h.field_40 += (idx >> 4) << 6;
            *(u8 *)&o->field_5C =
                (idx % CAMPAIGN_DIALOG_PORTRAIT_GRID_COLUMN_COUNT) *
                CAMPAIGN_DIALOG_PORTRAIT_IMAGE_WIDTH;
            ((u8 *)&o->field_5C)[1] =
                (idx / CAMPAIGN_DIALOG_PORTRAIT_GRID_COLUMN_COUNT) *
                CAMPAIGN_DIALOG_PORTRAIT_IMAGE_HEIGHT;
            *(u16 *)&o->field_40.h.field_42 +=
                idx % CAMPAIGN_DIALOG_PORTRAIT_CLUT_ROWS_PER_COLUMN;
            *(u16 *)&o->field_44.h.field_46 = 0;
            o->rgb = 0;
            o->attribute |=
                GsALON | GsAONE | DISPLAY_OBJECT_ATTRIBUTE_8BPP;
            DisplayObject_SelectOrderingTable1(o);
            DisplayObject_SetDepthOffset(o, -8);
            record->grid[0][0] = (s32)o;
            *(u16 *)&o->field_60 = 0x14;
        }
        o->attribute = (o->attribute | (GsALON | GsAONE)) & ~GsROTOFF;
        *(u16 *)&o->field_48.h.field_4A = 0;
        o2 = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
        DisplayObject_ConfigureScreenSprite(o2, *(s16 *)&o->field_30.h.field_30, *(s16 *)&o->field_30.h.field_32, 0x30, 0x30, *(u8 *)&o->field_5C, ((u8 *)&o->field_5C)[1], 0xE, DISPLAY_EFFECT_SPRITE_CX, 0xFD);
        o2->attribute = (o2->attribute |
            GsALON | GsATWO | DISPLAY_OBJECT_ATTRIBUTE_8BPP) & ~GsROTOFF;
        *(u16 *)&o2->field_44.h.field_46 = *(u16 *)&o->field_44.h.field_46;
        *(u16 *)&o2->field_48.h.field_4A = 0;
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
    if (o->field_60 <= 0) {
        o->attribute = (o->attribute & ~(GsALON | GsATWO | GsAONE)) | GsROTOFF;
        o->rgb = COLOR_RGB24_NEUTRAL_GREY;
        *(u16 *)&o->field_44.h.field_46 = 0x1000;
        DisplayObject_ReleaseIfPresent(o2);
        record->grid[0][1] = 0;
        goto clear;
    }
    if (o->field_60 >= 0x14) {
        DisplayEffect_ReleaseRecord(record);
    clear:
        record->display_effect_step = 0;
        return;
    }
    c = -0x80 - o->field_60 * 6;
    DISPLAY_OBJECT_COLOR_BYTES(o2)[2] = c;
    DISPLAY_OBJECT_COLOR_BYTES(o2)[1] = c;
    DISPLAY_OBJECT_COLOR_BYTES(o2)[0] = c;
    DISPLAY_OBJECT_COLOR_BYTES(o)[2] = c;
    DISPLAY_OBJECT_COLOR_BYTES(o)[1] = c;
    DISPLAY_OBJECT_COLOR_BYTES(o)[0] = c;
    idx = o->field_60 * 204 + 0x1000;
    *(u16 *)&o2->field_44.h.field_46 = idx;
    *(u16 *)&o->field_44.h.field_46 = idx;
    q = DISPLAY_OBJECT_COLOR_BYTES(o)[0] << 5;
    *(u16 *)&o2->field_44.h.field_44 = q;
    *(u16 *)&o->field_44.h.field_44 = q;
}
