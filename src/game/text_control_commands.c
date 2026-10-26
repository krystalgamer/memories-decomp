/* The text stream's primary control-byte handlers and the two text-box
   layout helpers after them, contiguous from Text_DispatchSecondaryCommand
   (0x80038B4C) through TextBox_RefreshDisplayObjects.

   The nine handlers are the ones TextBox_BuildStep reaches through
   gText_apfnPrimaryCommands for bytes F6 and F8 through FF: the F8 escape into the
   secondary table, the cursor set, the choice command, the page wait and
   campaign-flag command, the stream push, the new line and end of stream,
   and the display-effect command. See notes/text-control-bytecode.md. The
   other two primary handlers are Text_ExtendGlyphCode, now
   src/game/text_extend_glyph_code.c, and Text_SetStateFromStream in
   text_stream_commands.c.

   The layout helpers are TextBox_UpdateOutline, which TextBox_SetPos calls to lay
   the box's second object out as an outline around the first, and
   TextBox_RefreshDisplayObjects, which TextBox_BuildStep calls to create or refresh the
   box's frame object.

   The former sources were recorded at gcc_2_8_1_g0_split, gcc_2_8_1_g8,
   gcc_2_8_1_g0 and gcc_2_8_1_g8_split, and every member compiles to an
   identical object at gcc_2_8_1_g8_split. Bounded below by the
   secondary-table object commands in duel_effect_object_commands.c and
   above by TextBox_BuildStep. */

#include "../types.h"
#include "duel_effect_command_table.h"
#include "duel_effect.h"
#include "text_stream_read_le.h"
#include "text_stream_commands.h"
#include "duel_effect_entry_occupancy.h"
#include "dialog_choice.h"
#include "campaign_flags.h"
#include "text_box_wrap_line.h"
#include "display_effect_lifecycle.h"
#include "menu_record.h"
#include "display_menu_records.h"
#include "campaign_scene_package.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "display_object_core.h"
#include "display_object_layout.h"
#include "display_object_helpers.h"
#include "color_constants.h"
#include "text_box_draw_entries.h"
#include "text_box_runtime.h"
#include "text_control_commands.h"
#include "../unmatched.h"
#include "dialog_choice_control.h"

#define TEXT_STREAM_OWNER_VIEW(object) ((TextStreamOwner *)(object))
#ifndef TEXT_STREAM_CURSOR_HIGH_MASK
#define TEXT_STREAM_CURSOR_HIGH_MASK 0xFFFF0000
#endif

void Text_DispatchSecondaryCommand(DuelEffectChannel *object)
{
    u8 *G32 *pp = &TEXT_STREAM_OWNER_VIEW(object)->streams[object->stream_58];
    u8 *p = *pp;
    s32 op = *p;

    *pp = p + 1;
    gText_SecondaryCommandHandlers[op](object);
}

  void Text_SetCursorOffset(DuelEffectChannel *o){int v; u8 *G32 *p;v=TextStream_ReadU16LE(o);p=&TEXT_STREAM_OWNER_VIEW(o)->streams[o->stream_58];*p=(u8 *)(((u32)*p&TEXT_STREAM_CURSOR_HIGH_MASK)|(v&0xFFFF));}

void Text_HandleChoiceCommand(DuelEffectChannel *object)
{
    s32 t;
    s32 u;
    s32 w;
    s32 v;
    s32 c;
    s32 d;

    gText_CommandDisposition = 1;
    t = *TEXT_STREAM_OWNER_VIEW(object)->streams[object->stream_58]++;
    c = t;
    d = TEXT_CHOICE_OPTIONS_ENABLED_MASK;
    if (c & TEXT_CHOICE_COMMAND_HAS_OPTIONS) {
        u = *TEXT_STREAM_OWNER_VIEW(object)->streams[object->stream_58]++;
        t = u;
        d = t;
    }
    if (c & TEXT_CHOICE_COMMAND_BRANCH) {
        TEXT_STREAM_OWNER_VIEW(object)->streams[object->stream_58] +=
            gDialog_bChoice * 2;
        Text_SetCursorOffset(object);
    } else {
        gDialog_bChoiceCount = TEXT_CHOICE_COMMAND_COUNT_MASK;
        gDialog_bChoiceCount = c & gDialog_bChoiceCount;
        gDialog_bChoiceControl = c & DIALOG_CHOICE_CONTROL_MASK;
        gDialog_bChoiceEnabled = d & TEXT_CHOICE_OPTIONS_ENABLED_MASK;
        w = d & TEXT_CHOICE_OPTIONS_INPUT_FLAG;
        gDialog_bChoice = 0;
        gDialog_bInputState = 0;
        if (w != 0) {
            gDialog_bInputState = 1;
        }
#ifndef VERSION_EUROPE
        /* The European build no longer clears the channel's occupancy value
           here, as in TextBox_Destroy. */
        DuelEffect_ClearOccupancyValue(object->channel_index);
#endif
        DuelEffect_ClearMatchingMarker(object->channel_index);
        v = object->flags_34;
        object->explicit_newline_count = 0;
        gText_ChoiceLayoutCallback = Text_TryCompleteChoiceLayout;
        object->flags_34 = v | 0x1000;
    }
}

void Text_StartPageWait(DuelEffectChannel *value)
{
    value->playback_state = 4;
    gText_CommandDisposition = 1;
}

void Text_HandleCampaignFlagCommand(DuelEffectChannel *object)
{
    s32 flag = TextStream_ReadU16LE(object);

    flag &= CAMPAIGN_FLAG_COMMAND_WORD_MASK;
    if (flag & CAMPAIGN_FLAG_COMMAND_WRITE) {
        Library_UpdateCardUsedFlag(flag & CAMPAIGN_FLAG_COMMAND_PAYLOAD_MASK);
        return;
    }

    {
        s32 target = TextStream_ReadU16LE(object);

        target &= 0xFFFF;
        if (Campaign_TestStoryFlag(flag) != 0) {
            s32 *cursor = (s32 *)(
                (u32)object + (u32)&((u8 **)0)[object->stream_58]);

            *cursor = (*cursor & TEXT_STREAM_CURSOR_HIGH_MASK) | target;
        }
    }
}

void Text_PushStreamOffset(DuelEffectChannel *arg0)
{
    TextStreamOwner *owner = TEXT_STREAM_OWNER_VIEW(arg0);
    s32 c;
    s32 v;

    v = TextStream_ReadU16LE(arg0);
    c = arg0->stream_58;
    owner->streams[c + 1] =
        (u8 *)(((u32)owner->streams[c] & TEXT_STREAM_CURSOR_HIGH_MASK) |
               (v & 0xFFFF));
    arg0->stream_58++;
}

void Text_NewLine(DuelEffectChannel *record)
{
    record->explicit_newline_count++;
    record->pen_x = 0x1000;
#ifdef VERSION_EUROPE
    /* The European build moves to state 4 only from the fourth line on. */
    if (TextBox_WrapLineIfNeeded(record) && record->explicit_newline_count >= 4) {
#else
    if (TextBox_WrapLineIfNeeded(record)) {
#endif
        record->playback_state = 4;
    }
    gText_CommandDisposition = 1;
    if (gText_ChoiceLayoutCallback) {
        CALL32(void (*)(volatile DuelEffectChannel *), gText_ChoiceLayoutCallback)(record);
    }
}

void Text_EndStream(DuelEffectChannel *record)
{
    u16 flags;

    record->stream_58--;
    if (record->stream_58 < 0) {
        flags = record->flags_34;
        gText_CommandDisposition = 1;
        flags |= 0x2000;
        record->flags_34 = flags;
    }
}

/* Effect-script command handler: reads a command id and a flag byte from the
   object's current script stream, finds the display effect record for the id
   in gDisplayEffect_aMenuRecords (ids below 0x41 live in the first two records, others in
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

    gText_CommandDisposition = 1;
    a = *o->streams[o->depth]++;
    b = *o->streams[o->depth]++;
    id = a;
    flags = b;

    e = gDisplayEffect_aMenuRecords;
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
        gText_DisplayEffectRecord = e;
        if (flags & 2) {
            DisplayEffect_ReleaseRecord(e);
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
        gText_DisplayEffectRecord = e;
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
    e = &gDisplayEffect_aMenuRecords[slot];
    DisplayEffect_ResetRecordState((DisplayEffectState *)e);
    e->field_30 = id;
    e->field_3C = slot;
    if (slot != 0) {
        *(s16 *)&e->field_34 = 0x178;
    } else {
        *(s16 *)&e->field_34 = -0x38;
    }
    o->state = 6;
    gText_DisplayEffectRecord = e;
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

/* Lays the text box's second object out as a six-point outline around the
 * first, eight pixels outside its top, sides, and bottom. */
void TextBox_UpdateOutline(struct DuelEffectChannel *record) {
    DisplayObject *a = record->render_object, *b = record->aux_object;
    u16 v;
    b->field_18 = a->field_18;
    b->field_1A = a->field_1A;
    v = a->field_30.h.field_30;
    b->field_48.h.field_48 = v;
    b->position.h.field_28 = v;
    b->field_38.h.field_38 = v - 8;
    v = a->field_30.h.field_30 + a->field_3C.h.field_3C;
    b->field_50.h.field_50 = v;
    b->field_30.h.field_30 = v;
    b->field_40.h.field_40 = v + 8;
    v = a->field_30.h.field_32 - 8;
    b->field_30.h.field_32 = v;
    b->position.h.field_2A = v;
    v = a->field_30.h.field_32 + a->field_48.h.field_4A;
    b->field_40.h.field_42 = v;
    b->field_38.h.field_3A = v;
    v = a->field_30.h.field_32 + a->field_3C.h.field_3E + 8;
    b->field_50.h.field_52 = v;
    b->field_48.h.field_4A = v;
}

/* Regional value: the flags_34 bit under which the box rebuilds its gouraud
 * quad (0x20 -> 0x40). The European build (src/game/european/) defines its
 * own. */
#ifndef TEXT_BOX_LAYOUT_QUAD_FLAG
#define TEXT_BOX_LAYOUT_QUAD_FLAG 0x20
#endif

#ifndef TEXT_BOX_LAYOUT_WIDTH
#define TEXT_BOX_LAYOUT_WIDTH 0x280
#endif

#ifndef TEXT_BOX_LAYOUT_HEIGHT
#define TEXT_BOX_LAYOUT_HEIGHT 0xE8
#endif

void TextBox_RefreshDisplayObjects(DuelEffectChannel *p) {
    DisplayObject *e;
    s32 v;
    s32 b;
    s32 f;

    e = p->render_object;
    if (e == (DisplayObject *)0) {
        e = DisplayObject_AcquireSlot(DisplayObject_FindFreeSlot(), 6);
        b = p->channel_index;
        e->field_40.h.field_40 = TEXT_BOX_LAYOUT_WIDTH;
        e->field_40.h.field_42 = TEXT_BOX_LAYOUT_HEIGHT;
        e->field_66 = 0xA;
        f = e->flags;
        e->field_4C = (s32)TextBox_DrawEntries;
        e->field_67 = b;
        e->flags = f | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        p->render_object = e;
    }

    DisplayObject_SelectOrderingTable1(e);
    DisplayObject_SetDepthOffset(e, (s8)p->depth_offset);

    e->field_30.h.field_30 = (u16)p->rect_x;
    e->field_30.h.field_32 = (u16)p->rect_y;
    e->field_3C.h.field_3C = (u16)p->rect_width;
    e->field_3C.h.field_3E = (u16)p->rect_height;
    v = p->rect_width / 2;
    e->field_18 = v;
    e->field_48.h.field_48 = v;
    v = p->rect_height / 2;
    e->field_1A = v;
    e->field_48.h.field_4A = v;

    if ((p->flags_34 & TEXT_BOX_LAYOUT_QUAD_FLAG) != 0) {
        if (p->aux_object != 0) {
            DisplayObject_ReleaseIfPresent(p->aux_object);
        }
        e = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 4);
        DisplayObject_InitializeGouraudQuad(e, 1);
        p->aux_object = e;
        e->attribute = e->attribute | (GsALON | GsATWO);
        DisplayObject_SelectOrderingTable1(e);
        DisplayObject_SetDepthOffset(e, (s8)(p->depth_offset - 1));
        *(s32 *)&e->field_54 = 0xA0A0A0;
        e->field_4C = 0xA0A0A0;
        e->field_34.word = 0xA0A0A0;
        e->field_2C.word = 0xA0A0A0;
        e->field_44.word = COLOR_RGB24_NEUTRAL_GREY;
        e->field_3C.word = COLOR_RGB24_NEUTRAL_GREY;
        TextBox_UpdateOutline(p);
    }
}
