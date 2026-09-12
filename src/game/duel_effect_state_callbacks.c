#define GINPUT_PAD1_HELD_IS_AGGREGATE
#define GINPUT_PAD1_PRESSED_IS_AGGREGATE
#define D_8009B0CC_IN_DATA
#define SCRIPT_STATE_TEXT_CALLBACK_VIEWS
#include "../types.h"
#include "func_80036D3C.h"
#include "../psyq/rand.h"
#include "dialog_choice_state.h"
#include "display_object_api.h"
#include "display_object_helpers.h"
#include "duel_effect_entry_control.h"
#include "file_constants.h"
#include "file_transfer.h"
#include "graphics_frame.h"
#include "input.h"
#include "display_effect_lifecycle.h"
#include "menu_record.h"
#include "duel_effect.h"
#include "duel_effect_state_callbacks.h"
#include "script_state.h"
#include "sound.h"
#define FUNC_80049120_IGNORES_OBJECT
#include "sound_sequence_state.h"

extern signed char D_8009B32C;
extern s16 D_8009B322;
extern u8 D_8009B335;
extern u16 D_8009B348[2];
extern u16 gGraphics_uViewportX[] asm("gGraphics_sViewportX");
extern u16 gGraphics_uViewportY[] asm("gGraphics_sViewportY");

void func_800374A8(DuelEffectChannel *object)
{
    u8 flags = object->state_51;

    if ((flags & 0x80) == 0) {
        object->state_51 = flags | 0x80;
        func_800373C8(object, 3, 0);
        object->state_51 = 0x82;
    }
}

u8 *Dialog_OpenChoice(DuelEffectChannel *record)
{
    u8 *cursor = func_800400AC((s32)func_8004006C(), 2);

    func_800404CC(
        cursor,
        record->field_3C + record->field_3E - 0x10,
        record->field_40 + record->field_42 - 0x10,
        3,
        0,
        0,
        11,
        0x20C
    );
    *(u16 *)(cursor + 8) |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                            DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    func_80042918((DisplayObject *)cursor);
    func_800428EC(cursor, (s8)(record->field_59 + 1));
    return cursor;
}

void func_800375A4(DuelEffectChannel*o){unsigned char f=o->state_51;if((f&0x80)==0){o->state_51=f|0x80;D_8009B32C=10;o->field_30=Dialog_OpenChoice(o);}else{if(gInput_wPad1Held[0]&PAD_BUTTON_SQUARE){D_8009B32C--;if(D_8009B32C<0)D_8009B32C=0;}else D_8009B32C=10;if(D_8009B32C!=0&&!(gInput_wPad1Pressed[0]&PAD_BUTTON_CONFIRM_MASK))return;SD_SEPlayFull(11);o->state_51=2;func_8004036C(o->field_30);o->field_30=0;}}

void func_8003767C(DuelEffectChannel *state)
{
    s32 result;

    D_8009B2AA[0] = 0;
    D_8009B2A8[0] = 0;
    result = func_80036D3C(state);
    D_8009B270[0] = result;

    if (result & 0x8000) {
        u8 **slot = (u8 **)state + state->stream_58;
        u8 *script = *slot;
        s32 value = *script;

        *slot = script + 1;
        Base2_8009B2AA[0] = value;
        Base2_8009B2A8[0] = func_80036D3C(state);
    }

    D_8009B357 = 5;
    D_8009B27C[0] = 5;
    state->state_51 = 10;
}

void func_8003771C(DuelEffectChannel *object)
{
    s32 signed_value;
    s32 raw_value;

    object->state_51 = 0;
    D_8009B2A8_scalar = func_80036D3C(object);
    D_8009B2AA_scalar = func_80036D3C(object);
    D_8009B29C = func_80036D3C(object);

    signed_value = D_8009B2AA_scalar;
    raw_value = (u16)D_8009B2AA_scalar;
    if (signed_value >= 0x1000) {
        D_8009B2AA_scalar = raw_value - 0x1000;
        object->state_51 = 10;
    }

    D_8009B357 = 7;
    D_8009B27C_scalar = 7;
}

void func_800377AC(DuelEffectChannel *object)
{
    if (D_8009B357 == 0) {
        object->state_51 = 0;
    }
}

void func_800377C8(DuelEffectChannel *arg0) {
    u8 v = arg0->state_51;
    MenuRecord *p;

    if (!(v & 0x80)) {
        arg0->state_51 = v | 0x80;
    }

    p = D_8009B328;

    if (p->display_effect_step != 0) {
        return;
    }

    {
        u8 w = arg0->state_51;

        if ((w & 0x40) || p->field_30 >= 0x41) {
            arg0->state_51 = 0;

            return;
        }

        arg0->state_51 = w | 0x40;
    }

    {
        MenuRecord *q = D_8009B328;

        q->display_effect_step = *(u8 *)&q->field_40;
    }

    {
        MenuRecord *r = D_8009B328;

        *(u16 *)&r->field_40 = 0x68;

        if (r->field_3C != 0) {
            *(u16 *)&r->field_40 = 0xD8;
        }
    }

    {
        MenuRecord *s = D_8009B328;

        *(u16 *)&s->field_42 = 0xB2;
        s->field_44 = -0x10;
    }
}

/* Same state_51/bit80 gating as func_800378D8, but additionally calls
   func_80039FD4(D_8009B328) before clearing state_51 when the display-effect
   step is zero. */
void func_8003787C(DuelEffectChannel *object)
{
    u8 flags;
    MenuRecord *record;

    flags = object->state_51;
    if (!(flags & 0x80)) {
        object->state_51 = flags | 0x80;
    }
    record = D_8009B328;
    if (record->display_effect_step == 0) {
        func_80039FD4((u8 *)record);
        object->state_51 = 0;
    }
}

void func_800378D8(DuelEffectChannel *object)
{
    u8 flags = object->state_51;

    if ((flags & 0x80) == 0) {
        object->state_51 = flags | 0x80;
    }
    if (D_8009B328->display_effect_step == 0) {
        object->state_51 = 0;
    }
}

void func_80037914(DuelEffectChannel *object)
{
    u8 flags = D_8009B328->field_32;

    if ((flags & 3) == 0) {
        D_8009B328->field_32 = flags | 0x10;
        D_8009B328->display_effect_step = 6;
        object->state_51 = 8;
    }
}

void func_80037950(DuelEffectChannel *object)
{
    u8 flags = D_8009B328->field_32;

    if ((flags & 3) == 0) {
        D_8009B328->field_32 = flags | 0x10;
        D_8009B328->display_effect_step = 4;
        object->state_51 = 8;
    }
}

void func_8003798C(DuelEffectChannel *object)
{
    if (((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
         D_8009B134_abs) == 0) {
        object->state_51 = 0;
    }
}

void func_800379C4(DuelEffectChannel *object)
{
    if (func_80049120(object) != 1) {
        object->state_51 = 0;
    }
}

void func_800379F8(DuelEffectChannel *object)
{
    u8 flags = object->state_51;

    if ((flags & 0x80) == 0) {
        object->state_51 = flags | 0x80;
        D_8009B322 = func_80036D3C(object);
    }
    D_8009B322--;
    if (D_8009B322 == 0) {
        object->state_51 = 0;
    }
}

void func_80037A58(DuelEffectChannel *object)
{
    u8 flags = object->state_51;

    if ((flags & 0x80) == 0) {
        object->state_51 = flags | 0x80;
        D_8009B322 = func_80036D3C(object);
        D_8009B348[0] = gGraphics_uViewportX[0];
        D_8009B348[1] = gGraphics_uViewportY[0];
    }
    if (D_8009B0CC & 1) {
        gGraphics_uViewportX[0] = D_8009B348[0] + ((rand() & 7) - 4);
        gGraphics_uViewportY[0] = D_8009B348[1] + ((rand() & 3) - 2);
    }
    D_8009B322--;
    if (D_8009B322 == 0) {
        gGraphics_uViewportX[0] = D_8009B348[0];
        gGraphics_uViewportY[0] = D_8009B348[1];
        object->state_51 = 0;
    }
}

/* The last of the eight, and the only one that waits on the file transfer
   itself: it drives the sector-range request through three D_8009B335 stages
   and, when byte 0x51 bit 0x40 was armed, repeats the whole run D_8009B33C
   times before clearing the state. The switch falls through deliberately -
   each stage re-arms the 0xFF countdown and drops into the next test in the
   same call. */
void func_80037B40(DuelEffectChannel *object)
{
    DuelEffectChannel *p = object;

    if ((p->state_51 & 0x80) == 0) {
        p->state_51 |= 0x80;
        p->delay_52 = 0xFF;
        D_8009B335 = 0;
        if (D_8009B33C != 0) {
            p->state_51 |= 0x40;
        }
    }

    p->delay_52--;

    if (p->delay_52 != 0) {
        switch (D_8009B335) {
        case 0:
            if ((D_8009B0F4_abs & FILE_TRANSFER_FLAG_SECTOR_RANGE) == 0) {
                return;
            }
            p->delay_52 = 0xFF;
            D_8009B335 = 1;
        case 1:
            if ((D_8009B112_abs & 0x4000) == 0) {
                return;
            }
            p->delay_52 = 0xFF;
            D_8009B335 = 2;
        case 2:
            if ((D_8009B112_abs & 0x4000) == 0) {
                break;
            }
            if ((p->state_51 & 0x40) == 0) {
                return;
            }
            D_8009B33C--;
            if (D_8009B33C > 0) {
                return;
            }
            break;
        }
    }

    p->state_51 = 0;
    p->delay_52 = 1;
}
