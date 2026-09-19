#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "gpu_packets.h"
#include "display_object_config.h"
#include "display_object_core.h"
#include "display_object_helpers.h"
#include "display_object_layout.h"
#include "display_object_lifecycle.h"
#include "display_object_packet_submit.h"

/* Scratchpad work areas: the four quad vertices at 0x1F800300, the
 * RotAverageNclip4 depth/flag results at 0x1F8002E0, and the DivideFT4
 * inputs - its DIVPOLYGON4 at 0x1F800000, the colour at 0x1F800280 and the
 * four texture coordinates from 0x1F800290. */
#define SCRATCH_VERTEX(i) ((SVECTOR *)0x1F800300 + (i))
#define GS_SPRITE_VIEW(sprite) ((GsSPRITE *)(sprite))
#define GS_OT_VIEW(ordering_table) ((GsOT *)(ordering_table))
#define POLY_G4_VIEW(packet) ((POLY_G4 *)(packet))
#define POLY_GT4_VIEW(packet) ((POLY_GT4 *)(packet))
#define POLY_FT4_VIEW(packet) ((POLY_FT4 *)(packet))

/* The high half of `mode` selects the handler and the low half is the
 * ordering-table depth. Cases 1-3 hand a GsSPRITE to libgs. Cases 4 and 5 get
 * the display object's attribute word in place of the sprite pointer and
 * submit a prepared POLY_G4 or POLY_GT4, projecting it first when GsPERS
 * (0x04000000) is set. Any other case builds a POLY_FT4 from the sprite and
 * projects it, subdividing through DivideFT4 when `extra` asks for it. */
void DisplayObject_SubmitPacket(SpritePrim *sprite, u8 *packet, s32 ot, s32 mode, u8 *extra)
{
    SVECTOR *v;
    long *otz;
    DisplayObjectPacketOrigin *origin = (DisplayObjectPacketOrigin *)extra;
    s32 pri = (s16)mode;

    switch ((u32)mode >> 16) {
    case 1:
        GsSortFastSprite(GS_SPRITE_VIEW(sprite), GS_OT_VIEW(ot), pri);
        return;
    case 2:
        GsSortFlipSprite(GS_SPRITE_VIEW(sprite), GS_OT_VIEW(ot), pri);
        return;
    case 3:
        GsSortSprite(GS_SPRITE_VIEW(sprite), GS_OT_VIEW(ot), pri);
        return;
    case 4: {
        if ((u32)sprite & 0x04000000) {
            otz = (long *)0x1F8002E0;
            v = SCRATCH_VERTEX(0);
            v[0].vx = POLY_G4_VIEW(packet)->x0 - origin->x;
            v[0].vy = POLY_G4_VIEW(packet)->y0 - origin->y;
            v[1].vx = POLY_G4_VIEW(packet)->x1 - origin->x;
            v[1].vy = POLY_G4_VIEW(packet)->y1 - origin->y;
            v[2].vx = POLY_G4_VIEW(packet)->x2 - origin->x;
            v[2].vy = POLY_G4_VIEW(packet)->y2 - origin->y;
            v[3].vx = POLY_G4_VIEW(packet)->x3 - origin->x;
            v[3].vy = POLY_G4_VIEW(packet)->y3 - origin->y;
            SCRATCH_VERTEX(3)->vz = 0;
            SCRATCH_VERTEX(2)->vz = 0;
            SCRATCH_VERTEX(1)->vz = 0;
            v[0].vz = 0;
            if (RotAverageNclip4(SCRATCH_VERTEX(0), SCRATCH_VERTEX(1), SCRATCH_VERTEX(2), SCRATCH_VERTEX(3),
                                 (long *)&POLY_G4_VIEW(packet)->x0, (long *)&POLY_G4_VIEW(packet)->x1,
                                 (long *)&POLY_G4_VIEW(packet)->x2, (long *)&POLY_G4_VIEW(packet)->x3,
                                 otz, otz + 1, otz + 2) <= 0) {
                return;
            }
        }
        if ((u32)sprite & 0x40000000) {
            func_8005B260((u32 *)packet, GS_OT_VIEW(ot), (u16)pri, ((u32)sprite >> 28) & 3);
        } else {
            GsSortPoly(packet, GS_OT_VIEW(ot), pri);
        }
        return;
    }
    case 5: {
        if ((u32)sprite & 0x04000000) {
            otz = (long *)0x1F8002E0;
            v = SCRATCH_VERTEX(0);
            v[0].vx = POLY_GT4_VIEW(packet)->x0 - origin->x;
            v[0].vy = POLY_GT4_VIEW(packet)->y0 - origin->y;
            v[1].vx = POLY_GT4_VIEW(packet)->x1 - origin->x;
            v[1].vy = POLY_GT4_VIEW(packet)->y1 - origin->y;
            v[2].vx = POLY_GT4_VIEW(packet)->x2 - origin->x;
            v[2].vy = POLY_GT4_VIEW(packet)->y2 - origin->y;
            v[3].vx = POLY_GT4_VIEW(packet)->x3 - origin->x;
            v[3].vy = POLY_GT4_VIEW(packet)->y3 - origin->y;
            SCRATCH_VERTEX(3)->vz = 0;
            SCRATCH_VERTEX(2)->vz = 0;
            SCRATCH_VERTEX(1)->vz = 0;
            v[0].vz = 0;
            if (RotAverageNclip4(SCRATCH_VERTEX(0), SCRATCH_VERTEX(1), SCRATCH_VERTEX(2), SCRATCH_VERTEX(3),
                                 (long *)&POLY_GT4_VIEW(packet)->x0, (long *)&POLY_GT4_VIEW(packet)->x1,
                                 (long *)&POLY_GT4_VIEW(packet)->x2, (long *)&POLY_GT4_VIEW(packet)->x3,
                                 otz, otz + 1, otz + 2) <= 0) {
                return;
            }
        }
        if ((u32)sprite & 0x40000000) {
            func_8005B260((u32 *)packet, GS_OT_VIEW(ot), (u16)pri, ((u32)sprite >> 28) & 3);
        } else {
            GsSortPoly(packet, GS_OT_VIEW(ot), pri);
        }
        return;
    }
    case 0:
        return;
    default: {
        SVECTOR *v2;
        SVECTOR *v3;
        SVECTOR *v1;
        u32 attribute;

        otz = (long *)0x1F8002E0;
        v = SCRATCH_VERTEX(0);
        attribute = sprite->attribute;

        /* GsSPRITE's colour-mode bits (24-25) and semi-transparency rate
         * (28-29) go into the tpage's mode and rate fields. */
        POLY_FT4_VIEW(packet)->tpage =
            sprite->tpage | (((attribute >> 17) & 0x180) | ((attribute >> 23) & 0x60));
        if (sprite->attribute & 0x40000000) {
            SetSemiTrans(packet, 1);
        }
        POLY_FT4_VIEW(packet)->clut = (sprite->cxcy.h.cy << 6) | ((sprite->cxcy.h.cx >> 4) & 0x3F);
        if (sprite->attribute & 0x800000) {
            /* Horizontally flipped: the left and right u columns swap. */
            POLY_FT4_VIEW(packet)->u1 = POLY_FT4_VIEW(packet)->u3 = sprite->uv.b.lo;
            POLY_FT4_VIEW(packet)->u0 = POLY_FT4_VIEW(packet)->u2 =
                sprite->uv.b.lo + sprite->extent.wh.w.word - 1;
            POLY_FT4_VIEW(packet)->v0 = POLY_FT4_VIEW(packet)->v1 = sprite->uv.b.hi;
            POLY_FT4_VIEW(packet)->v2 = POLY_FT4_VIEW(packet)->v3 =
                sprite->uv.b.hi + sprite->extent.wh.h - 1;
        } else {
            POLY_FT4_VIEW(packet)->u0 = POLY_FT4_VIEW(packet)->u2 = sprite->uv.b.lo;
            POLY_FT4_VIEW(packet)->v0 = POLY_FT4_VIEW(packet)->v1 = sprite->uv.b.hi;
            if (sprite->attribute & 0x80) {
                POLY_FT4_VIEW(packet)->u1 = POLY_FT4_VIEW(packet)->u3 =
                    sprite->uv.b.lo + sprite->extent.wh.w.word;
                POLY_FT4_VIEW(packet)->v2 = POLY_FT4_VIEW(packet)->v3 =
                    sprite->uv.b.hi + sprite->extent.wh.h;
            } else {
                POLY_FT4_VIEW(packet)->u1 = POLY_FT4_VIEW(packet)->u3 =
                    sprite->uv.b.lo + sprite->extent.wh.w.word - 1;
                POLY_FT4_VIEW(packet)->v2 = POLY_FT4_VIEW(packet)->v3 =
                    sprite->uv.b.hi + sprite->extent.wh.h - 1;
            }
        }
        v1 = v + 1;
        v2 = v + 2;
        v3 = v + 3;
        v->vx = v[2].vx = sprite->xy.h.x - origin->x;
        v[1].vx = v[3].vx = v->vx + sprite->extent.wh.w.word;
        v->vy = v1->vy = sprite->xy.h.y - origin->y;
        v[2].vy = v3->vy = v->vy + sprite->extent.wh.h;
        v3->vz = 0;
        v2->vz = 0;
        v1->vz = 0;
        v->vz = 0;
        if (RotAverageNclip4(v, v1, v2, v3,
                             (long *)&POLY_FT4_VIEW(packet)->x0, (long *)&POLY_FT4_VIEW(packet)->x1,
                             (long *)&POLY_FT4_VIEW(packet)->x2, (long *)&POLY_FT4_VIEW(packet)->x3,
                             otz, otz + 1, otz + 2) > 0) {
            if (origin->divisions == 0) {
                GsSortPoly(packet, GS_OT_VIEW(ot), pri);
                return;
            }
            {
                u32 *rgbc = (u32 *)0x1F800280;
                u32 *uv = (u32 *)0x1F800290;
                DIVPOLYGON4 *divp = (DIVPOLYGON4 *)0x1F800000;

                divp->ndiv = origin->divisions;
                divp->pih = 320;
                divp->piv = 272;
                *rgbc = *(u32 *)&POLY_FT4_VIEW(packet)->r0;
                uv[0] = *(u32 *)&POLY_FT4_VIEW(packet)->u0;
                uv[1] = *(u32 *)&POLY_FT4_VIEW(packet)->u1;
                uv[2] = *(u32 *)&POLY_FT4_VIEW(packet)->u2;
                uv[3] = *(u32 *)&POLY_FT4_VIEW(packet)->u3;
                D_800FE240 = (u32 *)DivideFT4(v, v1, v2, v3,
                                             uv, (u32 *)0x1F800294, (u32 *)0x1F800298, (u32 *)0x1F80029C,
                                             (CVECTOR *)rgbc, (POLY_FT4 *)D_800FE240,
                                             (u32 *)GS_OT_VIEW(ot)->org + pri, divp);
            }
        }
        return;
    }
    }
}

void DisplayObject_InitializeGouraudQuad(
    DisplayObject *object,
    s32 has_secondary_quad
)
{
    u16 flags = object->flags;

    object->field_54 = 0;
    object->field_4C = 0;
    object->field_44.word = 0;
    object->field_3C.word = 0;
    object->field_34.word = 0;
    object->field_2C.word = 0;
    object->field_10 = 0;
    object->field_20.b.field_21 = 0;
    object->field_20.b.field_20 = 0;
    object->field_20.b.field_22 = 0;
    object->field_1C = 0;
    object->field_1A = 0;
    object->field_18 = 0;
    /* List 4 reads only this low byte to gate a second POLY_G4 submission. */
    *(u8 *)&object->field_5A = has_secondary_quad;
    object->flags = flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
}

void DisplayObject_InitializeTexturedGouraudQuad(
    DisplayObject *object,
    s32 has_secondary_quad
)
{
    u32 initial = 0x00808080;
    u16 flags = object->flags;

    *(u32 *)&object->field_68 = initial;
    *(u32 *)&object->field_5C = initial;
    object->field_50.word = initial;
    object->field_44.word = initial;
    object->field_38.word = initial;
    object->field_2C.word = initial;
    object->field_10 = 0;
    object->field_20.b.field_21 = 0;
    object->field_20.b.field_20 = 0;
    object->field_20.b.field_22 = 0;
    object->field_1C = 0;
    object->field_1A = 0;
    object->field_18 = 0;
    ((u8 *)object)[0x72] = has_secondary_quad;
    object->flags = flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
}

void DisplayObject_ConfigureSpriteWithResource(
    DisplayObject *object,
    s32 arg1,
    s32 arg2,
    s32 arg3,
    s32 arg4,
    s32 arg5,
    void *resource
)
{
    object->field_54 = resource;
    DisplayObject_ConfigureSpriteResource(object, arg1, arg2, arg3, arg4, arg5);
}

void DisplayObject_ConfigureSpriteAtPositionWithResource(
    DisplayObject *object,
    s32 arg1,
    s32 arg2,
    s32 arg3,
    s32 arg4,
    s32 arg5,
    s32 arg6,
    s32 arg7,
    void *resource
)
{
    object->field_54 = resource;
    DisplayObject_ConfigureSpriteAtPosition(object, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}

s32 DisplayObject_SetDepthOffset(DisplayObject *obj, s8 value)
{
    u32 index = obj->ot_index;
    volatile u16 *table = D_8009AF74;
    s32 result;

    obj->field_16 = value;
    result = table[index] - value;
    obj->field_14 = result;
    return result;
}

void DisplayObject_SelectOrderingTable1(DisplayObject *object)
{
    object->ot_index = 1;
    object->field_14 = D_8009AF74[1] - object->field_16;
}

void DisplayObject_SelectOrderingTable3(DisplayObject *object)
{
    object->ot_index = 3;
    object->field_14 = D_8009AF74[3] - object->field_16;
}

int DisplayObject_RunUpdateAndCheckRenderable(DisplayObject *object)
{
    /* Called as void (*)(void), with no argument, on purpose: the slot's
       declared type takes a u8 *, but the ambient argument register is what
       retail passes. See #2887. */
    void (*callback)(void) = (void (*)(void))object->update;

    if (callback != 0)
        callback();
    return ((object->flags & DISPLAY_OBJECT_RENDERABLE_MASK) ==
            DISPLAY_OBJECT_RENDERABLE_MASK);
}

u32 DisplayObjectStream_ReadU16LE(const u8 *data)
{
    return (data[1] << 8) | data[0];
}

u8 *DisplayObjectStream_ResolveOffset(
    DisplayObjectStream *object,
    const u8 *data
)
{
    return object->base + ((data[1] << 8) | data[0]);
}

void DisplayObject_ResetVelocity(DisplayObjectVelocity *object)
{
    if (object != 0) {
        object->velocity_x = 0;
        object->velocity_y = 0;
        object->velocity_z = 0;
        object->fraction_x = 0x80;
        object->fraction_y = 0x80;
        object->fraction_z = 0x80;
    }
}

void DisplayObject_StepPositionX(DisplayObjectVelocity *object)
{
    int value = (object->x << 8) | object->fraction_x;

    value += object->velocity_x;
    object->fraction_x = value;
    object->x = value >> 8;
}

void DisplayObject_StepPositionY(DisplayObjectVelocity *object)
{
    int value = (object->y << 8) | object->fraction_y;

    value += object->velocity_y;
    object->fraction_y = value;
    object->y = value >> 8;
}

void DisplayObject_StepPositionZ(DisplayObjectVelocity *object)
{
    int value = (object->z << 8) | object->fraction_z;

    value += object->velocity_z;
    object->fraction_z = value;
    object->z = value >> 8;
}

void DisplayObject_StepPositionXY(DisplayObjectVelocity *object)
{
    DisplayObject_StepPositionX(object);
    DisplayObject_StepPositionY(object);
}

void DisplayObject_StepPositionXYZ(DisplayObjectVelocity *object)
{
    DisplayObject_StepPositionX(object);
    DisplayObject_StepPositionY(object);
    DisplayObject_StepPositionZ(object);
}

s32 DisplayObject_StepToward(s32 value, s32 target, s32 step)
{
    if (target < 0) {
        value -= step;
        if (value < target) {
            value = target;
        }
    } else {
        value += step;
        if (value > target) {
            value = target;
        }
    }
    return value;
}

s32 DisplayObject_StepTowardZero(s32 value, s32 step)
{
    if (value < 0) {
        value += step;
        if (value > 0) {
            value = 0;
        }
    } else {
        value -= step;
        if (value < 0) {
            value = 0;
        }
    }
    return value;
}

#define DISPLAY_OBJECT_FROM_FIELD_6C(field) \
    ((DisplayObject *)((field) - 0x6C))

void *DisplayObject_FindAllocatedByTag(s32 value)
{
    u8 *object = (u8 *)D_800EFE48;
    s32 count = DISPLAY_OBJECT_POOL_CAPACITY;
    u8 *field = (u8 *)&((DisplayObject *)object)->field_6C;

    do {
        if ((DISPLAY_OBJECT_FROM_FIELD_6C(field)->flags &
             DISPLAY_OBJECT_FLAG_ALLOCATED) &&
            ((DISPLAY_OBJECT_FROM_FIELD_6C(field)->field_6C & 0xF) == value))
            return object;
        field += DISPLAY_OBJECT_RECORD_SIZE;
        count--;
        object += DISPLAY_OBJECT_RECORD_SIZE;
    } while (count != 0);
    return 0;
}

s32 DisplayObject_MarkInitialized(DisplayObjectLifecycle *object)
{
    if ((object->flags & DISPLAY_OBJECT_FLAG_ALLOCATED) == 0) {
        object->flags |= DISPLAY_OBJECT_FLAG_ALLOCATED;
        return 0;
    }
    return 1;
}

void DisplayObject_FadeBrightnessAndRelease(DisplayObjectLifecycle *object)
{
    s32 value = object->red - object->fade_step;

    if (value > 0) {
        object->blue = value;
        object->green = value;
        object->red = value;
    } else {
        DisplayObject_ReleaseIfPresent(object);
    }
}
