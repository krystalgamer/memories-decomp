#include "../types.h"
#include "duel_effect_command_table.h"
#include "duel_effect.h"
#include "func_80036D3C.h"
#include "text_stream_commands.h"
#include "duel_effect_entry_occupancy.h"
#include "dialog_choice.h"
#include "campaign_flags.h"
#include "func_80037C74.h"
#include "display_effect_lifecycle.h"
#include "menu_record.h"
#include "campaign_scene_package.h"
#include "text_control_commands.h"
#include "../unmatched.h"

/* The text stream's primary control-byte handlers, the ones
   TextBox_BuildStep reaches through D_80090F18 for bytes F6 and F8 through
   FF: the F8 escape into the secondary table, the cursor set, the choice
   command, the page wait and campaign-flag command, the stream push, the
   new line and end of stream, and the display-effect command. See
   notes/text-control-bytecode.md. The other two primary handlers,
   Text_ExtendGlyphCode and Text_SetStateFromStream, live in
   text_stream_commands.c.

   The seven former sources were recorded at gcc_2_8_1_g0_split,
   gcc_2_8_1_g8, gcc_2_8_1_g0 and gcc_2_8_1_g8_split, and every member
   compiles to an identical object at gcc_2_8_1_g8_split. Bounded below by
   the secondary-table object commands and above by the text-box layout
   helpers func_80039140 and func_800391E4. */

void Text_DispatchSecondaryCommand(u8 *arg0)
{
    u8 **pp = (u8 **)(arg0 + *(s8 *)(arg0 + 0x58) * 4);
    u8 *p = *pp;
    s32 op = *p;

    *pp = p + 1;
    D_80090EAC[op](arg0);
}

  void Text_SetCursorOffset(DuelEffectChannel *o){int v; unsigned int *p;v=func_80036D3C(o);p=&((unsigned int*)o)[o->stream_58];*p=(*p&0xFFFF0000)|(v&0xFFFF);}

void Text_HandleChoiceCommand(u8 *p)
{
    s32 t;
    s32 u;
    s32 w;
    s32 v;
    s32 c;
    s32 d;

    D_8009B350 = 1;
    t = *(*(u8 **)(p - -(*(s8 *)(p + 0x58) * 4)))++;
    c = t;
    d = 0xF;
    if (c & 8) {
        u = *(*(u8 **)(p - -(*(s8 *)(p + 0x58) * 4)))++;
        t = u;
        d = t;
    }
    if (c & 0x80) {
        *(s32 *)(p - -(*(s8 *)(p + 0x58) * 4)) += gDialog_bChoice * 2;
        Text_SetCursorOffset((DuelEffectChannel *)p);
    } else {
        gDialog_bChoiceCount = 7;
        gDialog_bChoiceCount = c & gDialog_bChoiceCount;
        D_8009B34C = c & 0xF0;
        gDialog_bChoiceEnabled = d & 0xF;
        w = d & 0x80;
        gDialog_bChoice = 0;
        gDialog_bInputState = 0;
        if (w != 0) {
            gDialog_bInputState = 1;
        }
        func_80035CA8(p[0x57]);
        DuelEffect_ClearMatchingMarker(p[0x57]);
        v = *(u16 *)(p + 0x34);
        p[0x56] = 0;
        D_8009B340 = func_80037CE0;
        *(u16 *)(p + 0x34) = v | 0x1000;
    }
}

void Text_StartPageWait(DuelEffectChannel *value)
{
    value->state_51 = 4;
    D_8009B350 = 1;
}

void Text_HandleCampaignFlagCommand(DuelEffectChannel *object)
{
    s32 flag = func_80036D3C(object);

    flag &= CAMPAIGN_FLAG_COMMAND_WORD_MASK;
    if (flag & CAMPAIGN_FLAG_COMMAND_WRITE) {
        Library_UpdateCardUsedFlag(flag & CAMPAIGN_FLAG_COMMAND_PAYLOAD_MASK);
        return;
    }

    {
        s32 target = func_80036D3C(object);

        target &= 0xFFFF;
        if (Campaign_TestStoryFlag(flag) != 0) {
            s32 *cursor =
                (s32 *)((u8 *)object + object->stream_58 * 4);

            *cursor = (*cursor & 0xFFFF0000) | target;
        }
    }
}

void Text_PushStreamOffset(DuelEffectChannel *arg0)
{
    s32 c;
    s32 co;
    s32 no;
    s32 v;

    v = func_80036D3C(arg0);
    c = arg0->stream_58;
    no = (c + 1) * 4;
    co = c * 4;
    *(s32 *)((u8 *)arg0 + no) =
        (*(s32 *)((u8 *)arg0 + co) & 0xFFFF0000) | (v & 0xFFFF);
    arg0->stream_58++;
}

void Text_NewLine(u8 *object)
{
    object[0x56]++;
    *(u16 *)(object + 0x38) = 0x1000;
    if (func_80037C74((DuelEffectChannel *)object)) {
        object[0x51] = 4;
    }
    D_8009B350 = 1;
    if (D_8009B340) {
        D_8009B340(object);
    }
}

void Text_EndStream(u8 *object)
{
    u16 flags;

    object[0x58]--;
    if (*(s8 *)(object + 0x58) < 0) {
        flags = *(u16 *)(object + 0x34);
        D_8009B350 = 1;
        flags |= 0x2000;
        *(u16 *)(object + 0x34) = flags;
    }
}

/* Effect-script command handler: reads a command id and a flag byte from the
   object's current script stream, finds the display effect record for the id
   in D_800EB010 (ids below 0x41 live in the first two records, others in
   the third), and depending on the flags either stops it (bit 7), adjusts a
   running one (bits 5 and 6) or starts it in the slot given by bit 0, with
   the object's state byte set to the matching wait state. */
void Text_HandleDisplayEffectCommand(EffectObject *o) {
    MenuRecord *e;
    s32 id;
    s32 flags;
    s32 slot;
    s32 a;
    s32 b;

    D_8009B350 = 1;
    a = *o->streams[o->depth]++;
    b = *o->streams[o->depth]++;
    id = a;
    flags = b;

    e = D_800EB010;
    if (id >= CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID) {
        e += 2;
    } else if (e->field_30 != id) {
        e++;
        if (e->field_30 != id) {
            e = 0;
        }
    }

    if (flags & 0x80) {
        if (e == 0) {
            return;
        }
        D_8009B328 = e;
        if (flags & 2) {
            func_80039FD4((u8 *)e);
            return;
        }
        if (flags & 1) {
            o->state = 0xE;
            return;
        }
        if (e->field_3C != 0) {
            e->field_40 = 0x178;
        } else {
            e->field_40 = -0x38;
        }
        e->field_42 = 0xB2;
        e->field_44 = 0x10;
        o->state = 7;
        e->display_effect_step = 3;
        if (id >= CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID) {
            e->display_effect_step = 5;
            e->field_40 = 1;
        }
        return;
    }
    if (flags & 0x60) {
        if (e == 0) {
            return;
        }
        if (id >= CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID) {
            return;
        }
        D_8009B328 = e;
        if (flags & 0x40) {
            e->field_31 = flags & 3;
            o->state = 9;
            return;
        } else if (flags & 0x20) {
            e->field_32 &= 0xEF;
            if (flags & 1) {
                e->field_32 |= 0x10;
            }
            return;
        }
    }

    slot = flags & 1;
    if (id >= CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID) {
        slot = 2;
    }
    e = &D_800EB010[slot];
    func_80039F44((DisplayEffectState *)e);
    e->field_30 = id;
    e->field_3C = slot;
    if (slot != 0) {
        *(s16 *)&e->field_34 = 0x178;
    } else {
        *(s16 *)&e->field_34 = -0x38;
    }
    o->state = 6;
    D_8009B328 = e;
    if (id >= CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID) {
        e->display_effect_step = 5;
        e->field_3C = 2;
        *(s16 *)&e->field_34 = 0xF0;
        e->field_40 = 0;
        *(s16 *)&e->field_36 = 0x60;
        return;
    }
    e->display_effect_step = 2;
    e->field_40 = 3;
    if (flags & 8) {
        *(s16 *)&e->field_34 = 0x400;
        e->field_40 = 7;
    }
    if (flags & 0x10) {
        e->field_31 = (flags >> 1) & 3;
    }
}
