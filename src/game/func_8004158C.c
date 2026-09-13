#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "display_object_layout.h"
#include "display_object_projection.h"
#define GRAPHICS_VIEWPORT_IN_DATA
#include "graphics_frame.h"
#include "sprite_primitive.h"
#include "display_object_packet_submit.h"
#include "func_8004158C.h"

/* Emits one sprite per sheet part. The object's position is offset by the
 * viewport unless it is screen-space (flag 8). With the clip-test flag (4)
 * the object is projected once through func_80041F90, which may ask
 * (D_8009B424) for the setup to run again, and every part is submitted as a
 * 9-word semi-transparent quad at 0x1F800344. Otherwise each part either
 * goes out as a plain sprite, culled against the 320x240 screen when the
 * attribute's 0x08000000 bit asked for that, or as a rotated/scaled sprite
 * about the object's +0x48 pivot. The scale and colour are written through
 * GsSPRITE's own members, which is the view libgs gives the record. */
void func_8004158C(DisplayObject *object, s32 ot, s32 depth)
{
    SpritePrim *sprite;
    SpriteSheetWork *work;
    SpriteSheetState *state;
    u8 *quad;
    u8 *sheet;
    SpriteSheetPart *part;
    s32 count;
    s32 x;
    s32 y;
    s32 dx;
    s32 dy;
    s32 value;
    s32 tpage;
    u16 cx;

    state = (SpriteSheetState *)0x1F8003A4;
    x = (s16)object->field_30.h.field_30;
    y = (s16)object->field_30.h.field_32;
    if ((object->flags & DISPLAY_OBJECT_FLAG_SCREEN_SPACE) == 0) {
        x -= gGraphics_sViewportX;
        y -= gGraphics_sViewportY;
    }
    work = (SpriteSheetWork *)0x1F800378;
    work->mx = object->field_48.h.field_48;
    work->my = object->field_48.h.field_4A;
    sprite = (SpritePrim *)0x1F800320;
    work->clip = object->flags & DISPLAY_OBJECT_FLAG_CLIP_TEST;
retry:
    work->attribute = object->attribute;
    work->u = ((u8 *)&object->field_5E)[0];
    work->v = ((u8 *)&object->field_5E)[1];
    count = ((SpriteSheetHeader *)object->field_4C)->count;
    if (count == 0) {
        return;
    }
    state->flags = ((SpriteSheetHeader *)object->field_4C)->flags;
    if (work->attribute & 0x01000000) {
        work->wide = 1;
        tpage = object->field_66 + ((SpriteSheetHeader *)object->field_4C)->tpage * 2;
    } else {
        work->wide = 0;
        tpage = object->field_66 + ((SpriteSheetHeader *)object->field_4C)->tpage;
    }
    sprite->tpage = work->tpage = tpage;
    work->cx = object->field_40.h.field_40;
    work->cy = object->field_40.h.field_42;
    if (object->flags & DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET) {
        work->cx += (((SpriteSheetHeader *)object->field_4C)->clut & 0xF) << 4;
        work->cy += ((SpriteSheetHeader *)object->field_4C)->clut >> 4;
    }
    sheet = (u8 *)object->field_4C;
    sprite->rotate = object->field_20.h.field_22 * 5760;
    ((GsSPRITE *)sprite)->scalex = object->field_44.h.field_44;
    ((GsSPRITE *)sprite)->scaley = object->field_44.h.field_46;
    sprite->mxmy.h.y = 0;
    sprite->mxmy.h.x = 0;
    ((GsSPRITE *)sprite)->r = ((u8 *)&object->field_0C)[0];
    ((GsSPRITE *)sprite)->g = ((u8 *)&object->field_0C)[1];
    ((GsSPRITE *)sprite)->b = ((u8 *)&object->field_0C)[2];
    part = (SpriteSheetPart *)(sheet + sizeof(SpriteSheetHeader));
    sprite->cxcy.h.cx = work->cx;
    sprite->cxcy.h.cy = work->cy;
    state->flip = 0;
    state->mirror = 0;
    state->screen = work->attribute & 0x08000000;
    if (state->screen != 0) {
        work->attribute &= 0xF7FFFFFF;
    }
    if (work->attribute & 0x800000) {
        work->attribute &= 0xF7FFFFFF;
        state->flip = 0x2000;
        state->mirror = 0x2000;
    }
    quad = 0;
    work->mode = 0;
    if (work->clip != 0) {
        quad = (u8 *)0x1F800344;
        *(u32 *)&((POLY_FT4 *)quad)->r0 = sprite->rgb;
        setlen(quad, 9);
        setcode(quad, 0x2C);
        D_8009B424 = 0;
        if (func_80041F90(object, x + (s16)object->field_18, y + (s16)object->field_1A,
                          &work->proj) <= 0) {
            return;
        }
        if (D_8009B424 != 0) {
            goto retry;
        }
        work->mode = 0xF0000;
    }
    do {
        sprite->attribute = work->attribute;
        work->cell = part->cell;
        work->size = part->size;
        sprite->extent.wh.w.word = ((work->size >> 2) & 0x78) + 8;
        sprite->extent.wh.h = ((work->size >> 6) & 0x78) + 8;
        if (state->flags & 0x10) {
            dx = (u8)part->dx | ((part->cell & 0xC000) >> 6);
            if (dx & 0x200) {
                dx |= ~0x1FF;
            }
            dy = (u8)part->dy | ((part->size & 0xC000) >> 6);
            if (dy & 0x200) {
                dy |= ~0x1FF;
            }
        } else {
            dx = part->dx;
            dy = part->dy;
        }
        if (state->mirror != 0) {
            dx = -(dx + sprite->extent.wh.w.word);
        }
        if (state->flags & 0xE0) {
            state->flip = (work->cell ^ state->mirror) & 0x2000;
            if (work->wide != 0) {
                sprite->tpage = work->tpage + ((part->cell >> 9) & 0xE);
                sprite->cxcy.h.cx = work->cx;
                sprite->cxcy.h.cy = work->cy + (work->size & 0x1F);
            } else {
                cx = work->cx;
                value = cx & 0xFF;
                value += (work->size & 0xF) << 4;
                sprite->tpage = work->tpage + ((part->cell >> 10) & 7);
                sprite->cxcy.h.cx = (cx & 0x300) | (value & 0xFF);
                sprite->cxcy.h.cy = work->cy + (value >> 8);
            }
        }
        sprite->xy.h.x = x + dx;
        sprite->xy.h.y = y + dy;
        value = ((work->cell & 0x1F) << 3) + work->u;
        sprite->uv.b.lo = value;
        sprite->uv.b.hi = ((work->cell & 0x3E0) >> 2) + (u8)work->v;
        sprite->tpage += value >> 8;
        /* From here `value` is the func_80042188 mode: case 1 sorts a fast
         * sprite, 2 a flipped one, 3 a rotated/scaled one. */
        value = (u16)depth | 0x10000;
        if (state->flip != 0) {
            value = (u16)depth | 0x20000;
            sprite->attribute |= 0x800000;
            if (sprite->uv.b.hi + sprite->extent.wh.h >= 0x100) {
                sprite->extent.wh.h--;
            }
            if (work->wide != 0) {
                if (sprite->uv.b.lo + sprite->extent.wh.w.word >= 0x100 &&
                    (s8)sprite->uv.b.lo < 0 && sprite->tpage != 0x1F) {
                    sprite->tpage++;
                    sprite->uv.b.lo += 0x80;
                    state->flags |= 0x80;
                } else if (sprite->uv.b.lo == 0) {
                    if (sprite->tpage != 0x10) {
                        sprite->tpage--;
                        sprite->uv.b.lo = 0x80;
                        state->flags |= 0x80;
                    } else {
                        goto narrow;
                    }
                }
            } else {
narrow:
                if (sprite->uv.b.lo == 0) {
                    sprite->uv.b.lo++;
                    sprite->extent.wh.w.word--;
                }
            }
        }
        if (work->clip == 0) {
            if (state->screen != 0) {
                if ((s16)sprite->xy.h.x >= 320 ||
                    (s16)sprite->xy.h.x + sprite->extent.wh.w.word <= 0 ||
                    (s16)sprite->xy.h.y >= 240 ||
                    (s16)sprite->xy.h.y + sprite->extent.wh.h <= 0) {
                    goto next;
                }
            } else {
                value = (u16)depth | 0x30000;
                sprite->mxmy.h.x = work->mx - dx;
                sprite->mxmy.h.y = work->my - dy;
                sprite->xy.h.x += sprite->mxmy.h.x;
                sprite->xy.h.y += sprite->mxmy.h.y;
            }
        }
        func_80042188(sprite, quad, ot, value | work->mode, (u8 *)&work->proj);
next:
        part++;
    } while (--count != 0);
}
