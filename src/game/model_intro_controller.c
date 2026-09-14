/*
 * Scene mode 19's intro controller. The control-module entry points and
 * eight-byte view offset are captured before the frame-step setter; the
 * signed phase, module data arguments and model fields remain live.
 *
 * The full reconstruction matches all 1,880 text bytes and the 17-entry
 * signed-phase table with the uniform GCC 2.8.1 G8 split-address profile.
 * The byte block-move view preserves the original unaligned eight-byte copy.
 * The timing callback is sequenced before the final live scale-byte read.
 */
#include "../types.h"
#include "../psyq/libgte.h"
#include "model.h"
#include "model_load_step.h"
#define MODEL_GRAPHICS_STATE_SCENE_BYTES
#include "model_graphics_state.h"
#include "model_slot_properties.h"
#include "model_slot_state_updates.h"
#include "model_state_setters.h"
#include "model_transfer_state.h"
#include "model_effect_state.h"
#include "func_80058E1C.h"
#include "func_80057AF4.h"
#include "func_8005D994.h"
#include "file_transfer.h"
#include "high_memory_addresses.h"
#include "fade.h"
#include "sound.h"
#include "sound_output.h"
#include "../unmatched.h"

#define MODEL_HANDLER_OFFSET_ABSOLUTE
#include "model_handler_state.h"
#include "model_scene_states.h"

void func_8004FE2C(void)
{
    SVECTOR offset;
    ModelSlot *slot;
    s32 (*first)(s32, s32);
    s32 (*second)(s32, s32);

    *(ModelBytes8 *)&offset = *(ModelBytes8 *)D_8009AFFC;
    slot = D_800F2C40;
    first = (s32 (*)(s32, s32))(D_80010014 + 4);
    second = (s32 (*)(s32, s32))(D_80010018 + 4);
    func_80059AEC(1);
    if ((u32)((u8)D_8009AF9A - 8) < 2 && slot->field_E15 == 0) {
        D_8009AFA0 = 0;
        if (first(D_80010024, -1) == 2) {
            slot->field_E15++;
        }
    }
    if (D_8009AF9A >= 10 && D_800F2C40[1].field_E15 == 0) {
        D_8009AFA0 = 0;
        if (second(D_80010028, -1) == 2) {
            func_80059700(0, 0);
            func_800156DC();
            D_8009AF9A = 15;
        }
    }
    switch ((s8)(D_8009AF9A + 1)) {
    case 0: {
        ModelSlot *current;
        if (D_800F2C40[0].field_E14 == 0xFF) {
            if ((D_8009B0F4_abs & 0x2000030) | D_8009B134_abs) {
                goto check_ready;
            }
            Model_LoadMonsterMerge(
                0, MODEL_SPECIAL_BATTLE_ID, 0, 0, 0, 0, 4);
        } else {
            func_80056828(0);
        }
check_ready:
        current = D_800F2C40;
        if (!current->field_E1F) {
            break;
        }
        func_8005F3B8(0, 10000, 0xE00, 0, 0);
        func_800597C8(0, 1, 0);
        if (slot->field_E0F) {
            func_80057AF4(0, 0, 0);
        }
        func_80059700(0, 1);
        func_80059590(0, 5, 0, 0, 0);
        slot->field_BF6 = 1;
        slot->field_BF4 = 2;
        func_80047314(0x7310);
        *(s32 *)&current->field_CF8.field_0C[2] = -1;
        *(s32 *)&current->field_CF8.field_0C[4] = -1;
        *(s32 *)&current->field_CF8.field_0C[6] = -1;
        D_8009AFA0 = 0;
        first(D_80010024, 0);
        second(D_80010028, 0);
        current[0].field_E15 = 0;
        current[1].field_E15 = 0;
        D_8009AF9A++;
        break;
    }
    case 1:
        if (func_8004703C() & 0x80) {
            offset.vy = -2000;
            func_800597C8(0, 0, 10);
            func_8005D994(0, 2000, 0x100, 0x100, (u8 *)&offset, 110);
            D_8009AF9A++;
        }
        break;
    case 2: {
        ModelSlot *current = D_800F2C40;
        if ((s8)current->field_DC0[0] >= 0) {
            current->field_DC0[0] += 2;
            current->field_DC0[1] += 2;
            current->field_DC0[2] += 2;
        } else {
            func_80059590(0, 0, 0x80, 0x80, 0x80);
            D_8009AF9A++;
        }
        break;
    }
    case 3:
        if (func_8005FB08() && slot->field_E06 >= 0x780) {
            func_800597C8(0, 0, 0);
            D_8009AF9A++;
        }
        break;
    case 4:
        if (slot->field_E06 >= 0x780) {
            func_800597C8(0, 0, 0);
            D_8009AF9A++;
        }
        break;
    case 5:
        if (slot->field_E06 >= 0x1E0) {
            func_800597C8(0, 0, 0);
            offset.vx = -2500;
            offset.vy = -1500;
            offset.vz = 1000;
            func_8005F3B8(0, 3000, -0x100, -0x100, &offset);
            D_8009AF9A++;
        }
        break;
    case 6:
        if (slot->field_E06 >= 0x1E0) {
            func_800597C8(0, 0, 0);
            offset.vx = 2500;
            offset.vy = -1500;
            offset.vz = 1000;
            func_8005F3B8(0, 3000, 0x100, -0x100, &offset);
            D_8009AF9A++;
        }
        break;
    case 7:
        if (slot->field_E06 >= 0x1E0) {
            func_800597C8(0, 0, 0);
            offset.vx = -500;
            offset.vy = 1500;
            offset.vz = 1000;
            func_8005F3B8(0, 3000, -0x200, -0x100, &offset);
            D_8009AF9A++;
        }
        break;
    case 8:
        if (slot->field_E06 >= 0x1E0) {
            func_800597C8(0, 0, 0);
            offset.vx = 500;
            offset.vy = 1500;
            offset.vz = 1000;
            func_8005F3B8(0, 3000, 0x200, -0x100, &offset);
            D_8009AF9A++;
        }
        break;
    case 9:
        if (slot->field_E06 >= 0x1E0) {
            offset.vy = -2000;
            func_8005F3B8(0, 3500, 0x40, 0x200, &offset);
            D_8009AF9A++;
        }
        break;
    case 10:
        if (slot->field_BF5 == 2) {
            D_8009AF9A++;
        }
        break;
    case 11:
        if (slot->field_E06 >= 0x8C0) {
            offset.vy = -1500;
            func_8005D994(0, 3500, 0, -0x80, (u8 *)&offset, 40);
            D_8009AF9A++;
        }
        break;
    case 12:
        if (slot->field_E06 >= 0xE60) {
            offset.vz = -1000;
            func_8005F3B8(0, 5000, 0x200, -0x100, &offset);
            func_800597C8(0, 0, 190);
            D_8009AF9A++;
        }
        break;
    case 13:
        if (slot->field_E06 >= 0xE60) {
            offset.vz = -1000;
            func_8005F3B8(0, 5000, -0x200, -0x100, &offset);
            func_800597C8(0, 0, 190);
            D_8009AF9A++;
        }
        break;
    case 14:
        if (slot->field_E06 >= 0xE60) {
            offset.vy = -1000;
            func_8005F3B8(0, 3000, 0, -0x80, &offset);
            func_800597C8(0, 0, 190);
            D_8009AF9A++;
        }
        break;
    case 15: {
        s32 remaining = slot->field_750[slot->field_BF5].max << 4;
        s32 timing = func_80058E1C();
        remaining -= slot->field_E0D * timing;
        if (slot->field_E06 >= remaining) {
            func_80059700(0, 0);
        }
        break;
    }
    case 16:
        D_8009AF9A = -2;
        break;
    }
}

#include "../unmatched.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/rand.h"
#include "card_constants.h"
#include "file_transfer.h"
#include "model.h"
#include "model_load_step.h"
#include "func_8005922C.h"
#include "model_cleanup.h"
#include "model_scene_states.h"

void func_80050584(s32 arg0) {
    ModelSlot *p;
    ModelSlot *b;
    ModelSlot *b0;
    ModelSlot *q;
    GsCOORDUNIT *r;
    ModelSlot *s;
    s32 v;
    s32 t;
    s32 a;
    s32 m1;

    b0 = D_800F2C40;
    p = b0 + arg0;
    if (p->field_E1F == 0) {
        if (p->field_E14 == 0xFF) {
            if (((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                 D_8009B134_abs) == 0) {
                do {
                    t = rand() >> 8;
                    v = t % CARD_COUNT;
                } while (v < 0 || v >= CARD_COUNT ||
                         (v >= MODEL_MRG_FIRST_GAP_START
                          && v < MODEL_MRG_FIRST_GAP_END) ||
                         (v >= MODEL_MRG_SECOND_GAP_START
                          && v < MODEL_MRG_SECOND_GAP_END) ||
                         v == MODEL_MRG_SINGLE_GAP_ID);
                m1 = -1;
                Model_LoadMonsterMerge(arg0 | 0x80, v, m1, m1, m1, m1, 0);
            }
        } else {
            func_80056828(arg0);
        }

        b = D_800F2C40;
        if ((b + arg0)->field_E1F != 0) {
            q = b + (arg0 ^ 1);
            a = MODEL_ANGLE_QUARTER_TURN;
            if (q->field_E1F != 0) {
                r = q->field_D18;
                if (r != (GsCOORDUNIT *)0) {
                    t = r->rot.vy + MODEL_ANGLE_QUARTER_TURN;
                    a = t / MODEL_ANGLE_FULL_TURN;
                    a = t - a * MODEL_ANGLE_FULL_TURN;
                }
            }
            s = D_800F2C40 + arg0;
            if (s->field_D18 != (GsCOORDUNIT *)0) {
                s->field_D18->rot.vx = 0;
                s->field_D18->rot.vy = a;
                s->field_D18->rot.vz = 0;
                s->field_D18->matrix.t[0] = 0;
                s->field_D18->matrix.t[1] = 0;
                s->field_D18->matrix.t[2] = 0;
            }
            func_8005922C(s->field_D18, 0);
            func_80059DD8(arg0);
            s->field_E15 = 0;
        }
    }
}

#include "../psyq/rand.h"
#include "model.h"
#define MODEL_GRAPHICS_STATE_SCENE_BYTES
#include "model_graphics_state.h"
#include "model_scene_states.h"
#include "model_scene_imports.h"
#include "high_memory_addresses.h"
#include "model_handler_state.h"
#include "model_state_setters.h"
#include "model_slot_properties.h"
#include "model_effect_requests.h"
#include "func_80057AF4.h"
#include "file_transfer.h"
#include "file_names.h"
#include "sound_init.h"
#include "sound_output.h"
#include "sound_pending_entries.h"

#define ACTIVE_SLOT D_8009AFA4[3]

void func_800507D0(void)
{
  s8 phase_value;
  ModelSlot *first_base;
  ModelSlot *cross_base;
  ModelSlot *lookup_base;
  ModelSlot *fade_models;
  u8 *fade_a;
  u8 *fade_b;
  s32 active_offset;
  u8 *active_color;
  s32 red_sample;
  s32 red_target;
  s32 green_sample;
  s32 green_target;
  s32 red;
  s32 blue_target;
  s8 next_red;
  s32 green;
  s8 next_green;
  s32 blue;
  s8 next_blue;
  s32 *active_model;
  u32 frame;
  s32 animation;
  s8 next_phase;
  s32 peer;
  s32 crossfade_offset;
  u8 *outgoing_color;
  u8 *incoming_color;
  u32 peer_red;
  u32 peer_green;
  u32 peer_blue;
  s32 result;

  func_80059AEC(1);
  switch ( D_8009AF9A )
  {
    case -1:
      File_RequestAsyncTransfer(1, D_800114F8, 1223, 16, 0, 0, D_80010030);
      File_WaitForTransfers();
      ((u8 *)&D_8009B004)[1] = 0;
      ((u8 *)&D_8009B004)[0] = 0;
      ACTIVE_SLOT = 0;
      D_800F2C40[0].field_E1F = 0;
      D_800F2C40[1].field_E1F = 0;
      func_80059590(0, 2, 0, 0, 0);
      func_80059590(1, 2, 0, 0, 0);
      func_801807B0();
      func_80181C4C(0);
      func_80049394(D_80010034);
      func_80047314(29488);
      phase_value = D_8009AF9A + 1;
      goto store_phase;
    case 0:
      first_base = D_800F2C40;
      active_offset = 904 * (u8)ACTIVE_SLOT;
      if ( ((ModelSlot *)((u8 *)first_base + (active_offset) * 4))->field_E1F )
      {
        if ( (((ModelSlot *)((u8 *)D_800F2C40 + (active_offset) * 4))->field_E15 & 3) == 0 )
        {
          active_color = ((ModelSlot *)((u8 *)D_800F2C40 + (active_offset) * 4))->field_DC0;
          red_sample = rand() >> 8;
          red_sample %= 24;
          red_target = red_sample + 8;
          green_sample = rand() >> 8;
          green_sample %= 24;
          green_target = green_sample + 8;
          blue_target = rand() >> 8;
          blue_target %= 24;
          blue_target += 8;
          red = active_color[0];
          if ( red != red_target )
          {
            if ( red >= red_target )
              next_red = red - 1;
            else
              next_red = red + 1;
            active_color[0] = next_red;
          }
          green = active_color[1];
          if ( green != green_target )
          {
            if ( green >= green_target )
              next_green = green - 1;
            else
              next_green = green + 1;
            active_color[1] = next_green;
          }
          blue = active_color[2];
          if ( blue != blue_target )
          {
            if ( blue >= blue_target )
              next_blue = blue - 1;
            else
              next_blue = blue + 1;
            active_color[2] = next_blue;
          }
        }
        if ( !((u8 *)&D_8009B004)[1] )
          func_80050584((u8)ACTIVE_SLOT ^ 1);
        lookup_base = D_800F2C40;
        active_model = (s32 *)&lookup_base[(u8)ACTIVE_SLOT];
        if ( !((ModelSlot *)active_model)->field_E0F )
        {
          frame = ((ModelSlot *)active_model)->field_E15;
          animation = -1;
          if ( !(u8)(frame % 0x1E) )
          {
            switch ( (u8)(frame / 0x1E) )
            {
              case 1u:
                animation = 2;
                break;
              case 3u:
                animation = 7;
                break;
              case 0u:
              case 2u:
              case 4u:
                animation = 1;
                break;
              default:
                break;
            }
            if ( animation < 0 )
            {
              if ( ((u8 *)&D_8009B004)[1] )
              {
                next_phase = 2;
                goto set_phase_and_tick;
              }
              peer = (u8)ACTIVE_SLOT ^ 1;
              if ( ((ModelSlot *)((u8 *)D_800F2C40 + (904 * peer) * 4))->field_E1F )
              {
                func_80059F18(1, -1, peer, 90);
                next_phase = D_8009AF9A + 1;
                goto set_phase_and_tick;
              }
            }
            else
            {
              func_80057AF4((u8)ACTIVE_SLOT, animation, 1);
            }
          }
          goto tick_active_slot;
        }
      }
      else
      {
        func_80050584((u8)ACTIVE_SLOT);
        if ( ((ModelSlot *)((u8 *)D_800F2C40 + (904 * (u8)ACTIVE_SLOT) * 4))->field_E1F )
          func_80059F18(1, -1, (u8)ACTIVE_SLOT, 30);
      }
      goto poll_module;
    case 1:
      cross_base = D_800F2C40;
      crossfade_offset = 904 * (u8)ACTIVE_SLOT;
      if ( (((ModelSlot *)((u8 *)cross_base + (crossfade_offset) * 4))->field_E15 & 3) == 0 )
      {
        outgoing_color = ((ModelSlot *)((u8 *)D_800F2C40 + (crossfade_offset) * 4))->field_DC0;
        incoming_color = ((ModelSlot *)((u8 *)D_800F2C40 + (904 * ((u8)ACTIVE_SLOT ^ 1)) * 4))->field_DC0;
        if ( outgoing_color[0] )
          --outgoing_color[0];
        if (outgoing_color[1])
          --outgoing_color[1];
        if (outgoing_color[2])
          --outgoing_color[2];
        peer_red = incoming_color[0];
        if ( peer_red < 8 )
          incoming_color[0] = peer_red + 1;
        peer_green = incoming_color[1];
        if ( peer_green < 8 )
          incoming_color[1] = peer_green + 1;
        peer_blue = incoming_color[2];
        if ( peer_blue < 8 )
          incoming_color[2] = peer_blue + 1;
        if ( !outgoing_color[0] && !outgoing_color[1] && !outgoing_color[2] )
        {
          ((ModelSlot *)((u8 *)cross_base + (904 * (u8)ACTIVE_SLOT) * 4))->field_E1F = 0;
          D_8009AF9A = 0;
          ACTIVE_SLOT ^= 1u;
        }
      }
      goto tick_active_slot;
    case 2:
      fade_models = D_800F2C40;
      if ( (((ModelSlot *)((u8 *)fade_models + (904 * (u8)ACTIVE_SLOT) * 4))->field_E15 & 3) != 0 )
        goto tick_active_slot;
      fade_a = fade_models[0].field_DC0;
      fade_b = fade_models[1].field_DC0;
      if ( fade_models[0].field_DC0[0] )
        --fade_models[0].field_DC0[0];
      if ( fade_a[1] )
        --fade_a[1];
      if ( fade_a[2] )
        --fade_a[2];
      if ( fade_models[1].field_DC0[0] )
        fade_models[1].field_DC0[0] = fade_models[1].field_DC0[0] - 1;
      if ( fade_b[1] )
        --fade_b[1];
      if ( fade_b[2] )
        --fade_b[2];
      if ( fade_models[0].field_DC0[0]
        || fade_a[1] || fade_a[2]
        || fade_models[1].field_DC0[0]
        || fade_b[1] || fade_b[2] )
      {
        goto tick_active_slot;
      }
      fade_models[1].field_E1F = 0;
      fade_models[0].field_E1F = 0;
      next_phase = D_8009AF9A + 1;
      goto set_phase_and_tick;
set_phase_and_tick:
      D_8009AF9A = next_phase;
tick_active_slot:
      ++((ModelSlot *)((u8 *)D_800F2C40 + (904 * (u8)ACTIVE_SLOT) * 4))->field_E15;
      goto poll_module;
    case 3:
      func_800493F8();
      func_8004763C();
      phase_value = -2;
store_phase:
      D_8009AF9A = phase_value;
      goto poll_module;
    default:
      goto poll_module;
  }
poll_module:
      if ( ((u8 *)&D_8009B004)[0] )
      {
        result = func_80180A24();
        if ( result )
        {
          result = ((u8 *)&D_8009B004)[1];
          if ( !((u8 *)&D_8009B004)[1] )
          {
            result = (s16)func_8004703C();
            if ( (s16)result != 128 )
            {
              ++((u8 *)&D_8009B004)[1];
              D_8009AF9A = 2;
            }
          }
        }
      }
      else
      {
        if (func_8004703C() & 0x80)
        {
          result = (u8)D_800F2C40[0].field_E1F;
          if ( D_800F2C40[0].field_E1F )
          {
            result = ((u8 *)&D_8009B004)[0] + 1;
            ++((u8 *)&D_8009B004)[0];
          }
        }
      }
      return;
}
