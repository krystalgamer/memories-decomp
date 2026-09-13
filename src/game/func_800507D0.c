#include "../types.h"
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
  s32 *active_color;
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
  s32 *outgoing_color;
  s32 *incoming_color;
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
          active_color = (s32 *)((ModelSlot *)((u8 *)D_800F2C40 + (active_offset) * 4))->field_DC0;
          red_sample = rand() >> 8;
          red_sample %= 24;
          red_target = red_sample + 8;
          green_sample = rand() >> 8;
          green_sample %= 24;
          green_target = green_sample + 8;
          blue_target = rand() >> 8;
          blue_target %= 24;
          blue_target += 8;
          red = *(u8 *)active_color;
          if ( red != red_target )
          {
            if ( red >= red_target )
              next_red = red - 1;
            else
              next_red = red + 1;
            *(u8 *)active_color = next_red;
          }
          green = *((u8 *)active_color + 1);
          if ( green != green_target )
          {
            if ( green >= green_target )
              next_green = green - 1;
            else
              next_green = green + 1;
            *((u8 *)active_color + 1) = next_green;
          }
          blue = *((u8 *)active_color + 2);
          if ( blue != blue_target )
          {
            if ( blue >= blue_target )
              next_blue = blue - 1;
            else
              next_blue = blue + 1;
            *((u8 *)active_color + 2) = next_blue;
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
        outgoing_color = (s32 *)((ModelSlot *)((u8 *)D_800F2C40 + (crossfade_offset) * 4))->field_DC0;
        incoming_color = (s32 *)((ModelSlot *)((u8 *)D_800F2C40 + (904 * ((u8)ACTIVE_SLOT ^ 1)) * 4))->field_DC0;
        if ( *(u8 *)outgoing_color )
          --*(u8 *)outgoing_color;
        if (*((u8 *)outgoing_color + 1))
          --*((u8 *)outgoing_color + 1);
        if (*((u8 *)outgoing_color + 2))
          --*((u8 *)outgoing_color + 2);
        peer_red = *(u8 *)incoming_color;
        if ( peer_red < 8 )
          *(u8 *)incoming_color = peer_red + 1;
        peer_green = *((u8 *)incoming_color + 1);
        if ( peer_green < 8 )
          *((u8 *)incoming_color + 1) = peer_green + 1;
        peer_blue = *((u8 *)incoming_color + 2);
        if ( peer_blue < 8 )
          *((u8 *)incoming_color + 2) = peer_blue + 1;
        if ( !*(u8 *)outgoing_color && !*((u8 *)outgoing_color + 1) && !*((u8 *)outgoing_color + 2) )
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
