/*
 * All 1864 bytes and the six-entry jump table match under the existing
 * uniform gcc_2_8_1_g8_split profile. The module callbacks consume the two
 * stored contexts; the secondary callback's result drives the state switch.
 *
 * Keep persistent callback decisions separate from transient reaction,
 * fallback and sound choices. Signed record-address cursors preserve the
 * parent/subrecord calculations without claiming adjacent subrecords form
 * a compact array. The initial base capture, reaction record capture and
 * final card-id capture keep the measured allocation and load-delay order.
 *
 * The first comparison retains a separate stats base and promotes its own
 * halfword before the opposing halfword. Updating the parent base in place
 * instead lets the first scheduler move that adjustment too early.
 * No hard-register bindings or instruction assembly are used.
 */
#define MODEL_HANDLER_DIAGNOSTICS_AS_ARRAY
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/rand.h"
#include "model.h"
#include "model_graphics_state.h"
#include "model_handler_state.h"
#include "model_control.h"
#include "model_transfer_flags.h"
#include "high_memory_addresses.h"
#include "model_control_slot_animation.h"
#include "func_80058E1C.h"
#include "func_8004DC38.h"
#include "duel_card.h"
#include "sound.h"

#define MODEL_SLOT_CF8_TAIL_VIEW(address) ((ModelSlotCF8TailView *)(address))

void func_800559D4(s32 index)
{
    ModelSlot *slot = &D_800F2C40[index];
    ModelSlot *other;
    ModelControlHandler handler;
    ModelControlHandler primary_handler;
    u8 *primary = (u8 *)slot->field_DE8;
    u8 *secondary = (u8 *)slot->field_DEC;
    u16 *own_stats;
    u16 *other_stats;
    u8 *stats_base;
    s32 command;
    s32 action;
    s32 state;
    s32 changed;
    s32 amount;
    s32 frame;
    s32 part;
    s32 primary_state;

    if (slot->field_E1F == 0) {
        return;
    }
    primary_state = ((ModelControlCommandView *)slot)->commands[2];
    D_8009AFA0 = index;
    if (primary_state >= 0) {
        if (index != 0) {
            primary_handler = (ModelControlHandler)(D_80010010 + 4);
        } else {
            primary_handler = (ModelControlHandler)(D_8001000C + 4);
        }
        func_8005F198(1);
        primary_handler(primary, -1);
        func_8005F198(0);
    }
    if (slot->field_E0E == 6) {
        if (slot->field_BF5 == slot->field_DFE + 3
            || slot->field_DF8 == MODEL_SPECIAL_BATTLE_ID) {
            slot->field_E0E = 7;
        } else {
            slot->field_E0E = 2;
        }
    }
    state = slot->field_E0E;
    if (state == 2) {
        return;
    }
    if (((ModelControlCommandView *)slot)->commands[slot->field_DFE] < 0) {
        goto no_handler;
    }
    if (index != 0) {
        handler = (ModelControlHandler)((u8 *)D_80010018 + 4);
    } else {
        handler = (ModelControlHandler)((u8 *)D_80010014 + 4);
    }
    if (slot->field_E0E == 7) {
        func_8005F198(1);
        {
            s32 request = ((ModelControlCommandView *)slot)->commands[slot->field_DFE];
            handler(secondary, request % 1000);
        }
        func_8005F198(0);
        slot->field_E0E = 8;
        slot->field_E08 = 0;
    }
    if (slot->field_BF5 != slot->field_DFE + 3
        && slot->field_DF8 != MODEL_SPECIAL_BATTLE_ID) {
        s32 eligible = slot->field_E0E;
        if (eligible != 0 && eligible != 3 && eligible != 1 && eligible != 4 && eligible != 5) {
            return;
        }
    }
    {
        s32 selected;
        s32 base;
        s32 stat_base;
        s32 own_offset;
        s32 other_offset;
        s32 own_record;
        s32 other_record;
        other_offset = (index ^ 1) * MODEL_SLOT_SIZE;
        do { base = (s32)D_800F2C40; } while (0);
        other = (ModelSlot *)(other_offset + base);
        own_offset = index * MODEL_SLOT_SIZE;
        stat_base = base + (u32)&((ModelSlot *)0)->field_CF8;
        own_record = own_offset + stat_base;
        other_record = other_offset + stat_base;
        if (other->field_DFF != 0) {
            {
                u16 own_value =
                    MODEL_SLOT_CF8_TAIL_VIEW(own_record)->prefix.values.field_00;
                u16 other_value =
                    MODEL_SLOT_CF8_TAIL_VIEW(other_record)->prefix.values.field_02;
                selected = own_value > other_value ? 6 : 8;
            }
        } else {
            u16 own_value =
                MODEL_SLOT_CF8_TAIL_VIEW(own_record)->prefix.values.field_00;
            u16 other_value =
                MODEL_SLOT_CF8_TAIL_VIEW(other_record)->prefix.values.field_00;
            selected = own_value > other_value ? 6 : 5;
            if (own_value == other_value) {
                selected |= 0x80;
                if (index != 0) {
                    selected = 6;
                }
            }
        }
        action = selected;
    }
    func_8005F198(1);
    slot->field_E0E = handler(secondary, -1);
    func_8005F198(0);
    switch (slot->field_E0E) {
    case 0:
        FntPrint(D_80011574);
        return;
    case 5:
        FntPrint(D_8009B030);
        if (other->field_E0F == 5 && other->field_E06 >= 0x60) {
            Model_ControlSlotAnimation(index ^ 1, 1, 1);
        }
        return;
    case 3:
        FntPrint(D_8009B038);
        if (action == 6) {
            other->field_E13 = 0x80;
            Model_ControlSlotAnimation(index ^ 1, 6, 1);
            return;
        }
        if (action & 0x80) {
            other->field_E13 = 0xFF;
            Model_ControlSlotAnimation(index ^ 1, 5, 1);
            return;
        }
    case 1:
    case 4: {
        s32 reaction;
        s32 offset;
        s32 own_record;
        s32 other_record;
        amount = Model_GetFrameStep();
        offset = index * MODEL_SLOT_SIZE;
        do { own_record = offset + (s32)&D_800F3938; } while (0);
        offset = (index ^ 1) * MODEL_SLOT_SIZE;
        other_record = offset + (s32)&D_800F3938;
        amount = slot->field_E0D * (amount + 1);
        if ((((u8 *)&D_800F3938) + offset)[MODEL_SLOT_CF8_DFF_OFFSET] != 0) {
            {
                u32 own_value =
                    MODEL_SLOT_CF8_TAIL_VIEW(own_record)->prefix.values.field_00;
                u32 other_value =
                    MODEL_SLOT_CF8_TAIL_VIEW(other_record)->prefix.values.field_02;
                reaction = other_value < own_value ? 6 : 8;
            }
        } else {
            u32 own_value =
                MODEL_SLOT_CF8_TAIL_VIEW(own_record)->prefix.values.field_00;
            u32 other_value =
                MODEL_SLOT_CF8_TAIL_VIEW(other_record)->prefix.values.field_00;
            reaction = other_value < own_value ? 6 : 5;
            if (own_value == other_value) {
                reaction |= 0x80;
                if (index != 0) {
                    reaction = 6;
                }
            }
        }
        command = reaction & 0x7F;
        changed = 0;
        if (slot->field_E0E == 1) {
            FntPrint(D_8009B040);
        }
        if (slot->field_E0E == 4) {
            FntPrint(D_8009B048);
            if (slot->field_E0E == 4 && command == 6 && other->field_E0F != 6) {
                command = 5;
            }
        }
        if (other->field_E0F == command) {
            if (other->field_E16 != 0x23 && other->field_E06 >= 0xA0 &&
                ((rand() >> 8) & 3) != 0) {
                other->field_E06 -= amount;
                frame = other->field_E06;
                for (part = 0; part < other->field_E1B; part++) {
                    func_8004DC38(other, part, other->field_BF5, frame);
                }
                if ((rand() >> 8) & 1) {
                    changed++;
                }
            }
        } else {
            Model_ControlSlotAnimation(index ^ 1, command, 1);
            changed++;
        }
        if (D_8009AF94 == 0xF && changed != 0 && command == 8) {
            u32 card_id;
            s32 sound_id;
            do { card_id = other->field_DF8; } while (0);
            sound_id = ((gDuel_adwCardStats[card_id] >> 26) & 0x1F) == 1 ? 3 : 4;
            SD_SEPlay(sound_id, 0xFF, 0);
        }
        return;
    }
    case 2:
        FntPrint(D_8009B050);
        return;
    default:
        FntPrint(D_8009AFF4);
        slot->field_E0E = 2;
        return;
    }

no_handler:
    {
        s32 fallback_action;
        s32 offset;
        s32 own_record;
        s32 other_record;
        if (slot->field_BF5 == slot->field_DFE + 3) {
            s32 current_frame = slot->field_E06 >> 4;
            s32 midpoint = slot->field_750[slot->field_BF5].max >> 1;
            if (current_frame >= midpoint) {
                offset = index * MODEL_SLOT_SIZE;
                own_record = offset + (s32)&D_800F3938;
                offset = (index ^ 1) * MODEL_SLOT_SIZE;
                other_record = offset + (s32)&D_800F3938;
                if ((((u8 *)&D_800F3938) +
                     offset)[MODEL_SLOT_CF8_DFF_OFFSET] != 0) {
                    {
                        u32 own_value =
                            MODEL_SLOT_CF8_TAIL_VIEW(own_record)->
                                prefix.values.field_00;
                        u32 other_value =
                            MODEL_SLOT_CF8_TAIL_VIEW(other_record)->
                                prefix.values.field_02;
                        fallback_action = other_value < own_value ? 6 : 8;
                    }
                } else {
                    u32 own_value =
                        MODEL_SLOT_CF8_TAIL_VIEW(own_record)->
                            prefix.values.field_00;
                    u32 other_value =
                        MODEL_SLOT_CF8_TAIL_VIEW(other_record)->
                            prefix.values.field_00;
                    fallback_action = other_value < own_value ? 6 : 5;
                    if (own_value == other_value) {
                        fallback_action |= 0x80;
                        if (index != 0) {
                            fallback_action = 6;
                        }
                    }
                }
                Model_ControlSlotAnimation(index ^ 1, fallback_action & 0x7F, 1);
                slot->field_E0E = 2;
            }
        } else if (state == 8) {
            slot->field_E0E = 2;
        }
        if (slot->field_E0E == 7) {
            slot->field_E0E = 8;
        }
    }
}
