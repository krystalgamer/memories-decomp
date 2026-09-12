#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "display_object_api.h"
#include "display_object_config.h"
#include "display_object_helpers.h"
#include "display_object_layout.h"
#include "display_object_projection.h"
#include "func_80040588.h"
#include "graphics_frame.h"
#include "ordering_tables.h"
#include "sprite_primitive.h"
#include "display_object_packet_submit.h"

extern u8 tail_data_start[];

s32 func_8004002C(void)
{
    DisplayObject *entry = D_800F0548;
    s32 i;

    for (i = DISPLAY_OBJECT_RESERVED_CAPACITY;
         i < DISPLAY_OBJECT_POOL_CAPACITY;
         i++, entry++) {
        if ((entry->flags & DISPLAY_OBJECT_FLAG_ALLOCATED) == 0) {
            return i;
        }
    }
    return -1;
}

s32 func_8004006C(void)
{
    DisplayObject *entry = D_800EFE48;
    s32 i;

    for (i = 0; i < DISPLAY_OBJECT_POOL_CAPACITY; i++, entry++) {
        if ((entry->flags & DISPLAY_OBJECT_FLAG_ALLOCATED) == 0) {
            return i;
        }
    }
    return -1;
}

void *func_800400AC(s32 index, s32 key)
{
    DisplayObject *slot;
    s16 other;
    u16 initialized;

    if (index < 0) {
        return 0;
    }

    slot = &D_800EFE48[index];
    initialized = slot->flags & DISPLAY_OBJECT_FLAG_ALLOCATED;
    D_8009B412++;
    if (!initialized) {
        other = D_800EFE38[key];
        if (other < 0) {
            D_800F2878[key] = index;
            slot->next = -1;
            slot->previous = -1;
        } else {
            D_800EFE48[other].previous = index;
            slot->next = (u16)D_800EFE38[key];
        }

        slot->previous = -1;
        D_800EFE38[key] = index;
        /* A fresh object starts with rotation off. */
        slot->attribute = GsROTOFF;
        slot->flags = DISPLAY_OBJECT_RENDERABLE_MASK;
        slot->ot_index = 2;
        slot->field_54 = tail_data_start;
        slot->field_6C = 0;
        slot->update = 0;
        slot->field_1E = key;
        slot->field_0A = index;
        slot->field_0B = 0;
        slot->field_0C = 0x00808080;
        slot->field_20.word = 0;
        slot->field_10 = 0;
        slot->field_1C = 0;
        slot->field_1A = 0;
        slot->field_18 = 0;
        slot->field_66 = 0;
        slot->field_48.word = 0;
        slot->field_44.word = 0x10001000;
        slot->field_40.word = 0;
        slot->field_5E = 0;
        slot->field_65 = 0;
        slot->field_16 = 0;
        slot->field_14 = D_8009AF74[slot->ot_index];
    }
    return slot;
}

void func_8004020C(DisplayObject *slot)
{
    s32 first = slot->previous;
    s32 second = slot->next;

    D_8009B410++;

    if (first < 0) {
        s16 index = slot->field_1E;
        D_800EFE38[index] = second;
        if (second >= 0) {
            D_800EFE48[second].previous = -1;
        }
    } else {
        D_800EFE48[first].next = second;
        if (second >= 0) {
            D_800EFE48[second].previous = first;
        }
    }

    slot->flags = 0;
}

void func_800402A0(DisplayObject *slot, s32 key)
{
    u16 saved = slot->flags;
    s32 v;

    func_8004020C(slot);
    v = *(s16 *)((u8 *)D_800EFE38 + key * 2);
    if (v < 0) {
        *(u16 *)((u8 *)D_800F2878 + key * 2) = slot->field_0A;
        slot->next = -1;
        slot->previous = -1;
    } else {
        D_800EFE48[v].previous = slot->field_0A;
        slot->next = *(u16 *)((u8 *)D_800EFE38 + key * 2);
    }
    slot->previous = -1;
    *(u16 *)((u8 *)D_800EFE38 + key * 2) = slot->field_0A;
    slot->flags = saved;
}

void func_8004036C(void *object)
{
    if (object != 0) {
        func_8004020C((DisplayObject *)object);
    }
}

void DisplayObject_ResetPool(void){int i=0;int neg=-1;s16*a; s16*b;a=D_800F2878;b=D_800EFE38;D_8009B410=0;D_8009B412=0;for(;i<DISPLAY_OBJECT_LIST_COUNT;i++){*b=neg;*a=neg;a++;b++;}{u8*p=(u8*)D_800EFE48;for(i=DISPLAY_OBJECT_POOL_CAPACITY-1;i>=0;i--){*(s16*)(p+8)=0;p+=DISPLAY_OBJECT_RECORD_SIZE;}}}

void func_800403F0(void)
{
    DisplayObject_ResetPool();
}

void func_80040410(DisplayObjectConfig *object, s32 value)
{
    object->field_69 = value;
    object->flags &= 0xFFEF;
}

void func_80040424(DisplayObjectConfig *object, s32 value)
{
    if (object->field_69 != value) {
        object->field_69 = value;
        object->flags &= 0xFFEF;
    }
}

void func_8004044C(
    DisplayObjectConfig *object,
    u8 field_67,
    u8 field_68,
    u8 field_69
)
{
    object->field_67 = field_67;
    object->field_68 = field_68;
    object->field_69 = field_69;
    object->flags &= 0xFFEF;
}

void *func_80040468(u8 *object, int field_67, int field_68, int field_69,
                    int color, int texture)
{
    u16 flags;

    object[0x67] = field_67;
    object[0x68] = field_68;
    object[0x69] = field_69;
    object[0x66] = color;
    object[0x5E] = color >> 16;
    object[0x5F] = color >> 8;
    *(u16 *)(object + 0x40) = texture & 0x3F0;
    *(u16 *)(object + 0x42) = (texture & 0xF) + 0xF0;
    flags = *(u16 *)(object + 8) & 0xFFDF;
    *(u16 *)(object + 8) = flags;
    if (texture & 0x8000) {
        *(u16 *)(object + 8) = flags | 0x20;
    }
    return object;
}

void func_800404CC(void *object, s32 x, s32 y, s32 field_67, s32 field_68,
                   s32 field_69, s32 color, s32 texture)
{
    *(s16 *)((u8 *)object + 0x30) = x;
    *(s16 *)((u8 *)object + 0x32) = y;
    func_80040468(object, field_67, field_68, field_69, color, texture);
}

DisplayObjectConfigView *func_80040510(
    DisplayObjectConfigView *object,
    s32 x,
    s32 y,
    s32 height,
    s32 width,
    s32 field_5C,
    s32 field_5D,
    s32 field_66,
    s32 field_40,
    s32 field_42)
{
    s32 half_height;
    s32 half_width;

    object->height = height;
    half_height = height / 2;
    object->x = x;
    object->y = y;
    object->half_height = half_height;
    object->half_height_2 = half_height;

    object->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;

    object->width = width;
    half_width = width / 2;
    object->field_5C = field_5C;
    object->field_5D = field_5D;
    object->field_66 = field_66;
    object->field_40 = field_40;
    object->field_42 = field_42;
    object->half_width = half_width;
    object->half_width_2 = half_width;
    return object;
}

extern s16 gGraphics_sViewportX_data asm("gGraphics_sViewportX")
    __attribute__((section(".data")));
extern s16 gGraphics_sViewportY_data asm("gGraphics_sViewportY")
    __attribute__((section(".data")));
#define gGraphics_sViewportX gGraphics_sViewportX_data
#define gGraphics_sViewportY gGraphics_sViewportY_data
extern s16 D_800EFE3A[];

/* Walks the display-object list rooted at D_800EFE3A: calls each object's
   callback, and for every renderable object fills the sprite primitive in
   the scratchpad at 0x1F800320 from the object, offsets it by the viewport
   origin unless bit 3 is set, and submits it through func_80042188. With
   bit 2 the position first goes through the clip test in func_80041F90,
   which may ask (D_8009B424) for the object to be run again, and the quad
   at 0x1F800344 is set up as a 9-word semi-transparent packet instead. */
void func_80040588(void) {
    SpritePrim *p;
    u8 *g;
    u8 *h;
    DisplayObject *e;
    GsOT **tb;
    DisplayObjectCallback fn;
    s32 i;
    GsOT *ot;
    s32 mode;
    s32 idx;

    i = D_800EFE3A[0];
    if (i >= 0) {
        g = (u8 *)0x1F800344;
        p = (SpritePrim *)0x1F800320;
        h = (u8 *)0x1F800378;
        tb = D_800E9D90;

        do {
            e = (DisplayObject *)((u8 *)D_800EFE48 + i * DISPLAY_OBJECT_RECORD_SIZE);
            i = e->next;
            while (1) {
                fn = e->update;
                if (fn != (DisplayObjectCallback)0) {
                    fn((u8 *)e);
                }
                if (((e->flags & DISPLAY_OBJECT_RENDERABLE_MASK) ^
                     DISPLAY_OBJECT_RENDERABLE_MASK) != 0) {
                    break;
                }
                p->attribute = e->attribute;
                idx = e->ot_index;
                p->rgb = e->field_0C;
                p->xy.word = e->field_30.word;
                p->extent.word = e->field_3C.word;
                p->cxcy.word = e->field_40.word;
                p->uv.word = e->field_5C;
                mode = e->field_14 | 0x10000;
                ot = tb[idx];
                p->tpage = e->field_66;

                if ((e->flags & DISPLAY_OBJECT_FLAG_SCREEN_SPACE) == 0) {
                    p->xy.h.x = p->xy.h.x - gGraphics_sViewportX;
                    p->xy.h.y = p->xy.h.y - gGraphics_sViewportY;
                }

                if ((e->flags & DISPLAY_OBJECT_FLAG_CLIP_TEST) != 0) {
                    D_8009B424 = 0;
                    if (func_80041F90(e, (s16)p->xy.h.x + (s16)e->field_18,
                                      (s16)p->xy.h.y + (s16)e->field_1A,
                                      (struct ProjectionOut *)(h + 0x20)) <= 0) {
                        break;
                    }
                    if (D_8009B424 != 0) {
                        continue;
                    }
                    g[3] = 9;
                    *(s32 *)(g + 4) = p->rgb;
                    g[7] = 0x2C;
                    if ((p->attribute & GsALON) != 0) {
                        SetSemiTrans(g, 1);
                    }
                    mode = e->field_14 | 0xF0000;
                } else if ((p->attribute & GsROTOFF) == 0) {
                    p->rotate = e->field_20.h.field_22 * 5760;
                    p->scale = e->field_44.word;
                    p->mxmy.word = e->field_48.word;
                    p->xy.h.x = p->xy.h.x + p->mxmy.h.x;
                    p->xy.h.y = p->xy.h.y + p->mxmy.h.y;
                    mode = e->field_14 | 0x30000;
                }
                func_80042188(p, g, (s32)ot, mode, h + 0x20);
                break;
            }
        } while (i >= 0);
    }
}
