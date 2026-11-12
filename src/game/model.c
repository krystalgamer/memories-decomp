/* The model runtime: the slot-part animation seek and read-back, the
 * background renderer, the scene controllers, the debug controller, the slot
 * renderer and slot control, the positional sound helper and the tint-request
 * pair, with the initialized and zero-initialized data they own. In every
 * executable these are one contiguous run of text (0x8004DB14-0x80058A7C in
 * the US build) whose .rodata, .sdata, .sbss and .bss are each contiguous
 * too, so they were one translation unit: each part reads the others' small
 * data through $gp. */
#include "../types.h"

#include "model_record_tables.h"
#include "model.h"
#include "model_slot_getters.h"
#include "graphics_frame_controls.h"

ModelBackgroundRecord *gModel_pBackgroundRecord =
    (ModelBackgroundRecord *)gModel_abBackgroundRecords;
u16 gModel_wProjectionDistance = 0x012C;
s16 gModel_wBackgroundScrollX = 0;
s16 gModel_wBackgroundOffsetY = 0;
u16 gModel_wScreenYOverride = 0;
u8 gModel_bSceneMode = 0;
static u8 sModelGraphicsState_Pad95 = 0;
u16 gModel_wPresentationBgmCommand = 0;
u8 gModel_bOrbitDirectionTimer = 0;
s8 gModel_bOrbitDirection = 1;
s8 gModel_bPresentationPhase = -1;
u8 gModel_bDrawMaskedPartsOnly = 0;
s32 gModel_PartMaskAddress = 0;
u8 gModel_bActiveSlotIndex = 0;
#ifndef VERSION_JAPAN
u8 gModelDebug_bSelectedDigit = 0;
#endif
u8 gGraphics_bPacketBufferIndex = 0;
volatile u8 gGraphics_bFrameStep = 0;
u8 gGraphics_FrameStepControl = 1;
u8 D_8009AFA5 = 0;
u8 D_8009AFA6 = 0;
u8 gModel_bCreditsActiveSlot = 0;
#ifdef VERSION_JAPAN
/* The Japanese build has no selected-digit byte, so the four bytes above sit
 * unaligned at 0x8009AF0B; the rest of the block is single values. */
u8 D_8009AF0F_slpm_86398 = 1;
u8 D_8009AF10_slpm_86398 = 0;
u8 D_8009AF11_slpm_86398 = 0;
u16 D_8009AF12_slpm_86398 = 0;
#else
static u16 sModelGraphicsState_ValueA8
    = 1;
static u16 sModelGraphicsState_PadAA
    = 0;
#endif

/* Small zero-initialized storage this unit owns. Each build's generated
 * model_commons subsegment places it at the retail addresses. */
s16 gModel_ImageCopyRectX;
s16 gModel_ImageCopyRectY;
s16 gModel_ImageCopyRectW;
s16 gModel_ImageCopyRectH;
s16 gModel_sTextureTintRectX;
s16 gModel_sTextureTintRectY;
s16 gModel_sTextureTintRectW;
s16 gModel_sTextureTintRectH;
u16 gModel_wViewDistance;
s16 gModel_wViewYaw;
s16 gModel_wViewPitch;
ModelBytes8 gModel_ViewSnapshot;
#ifndef VERSION_JAPAN
/* The Japanese block has no slot model ids or debug slot flags. */
s16 gModel_awSlotModelIds[3];
u8 gModelDebug_abSlotAfFlags[2];
u8 gModelDebug_abSlotGfFlags[2];
#endif

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model_slot_support.h"
#include "camera_view.h"
#include "ordering_tables.h"
#include "model_scene_controllers_data.h"
#include "model_primitive_templates.h"
#include "primitive_handler_modes.h"

#define CVECTOR_VIEW(color) ((CVECTOR *)(color))

#ifndef BACKGROUND_RENDER_CLIP_LIMIT
#define BACKGROUND_RENDER_CLIP_LIMIT 241
#endif

/* The model renderer's prototype GPU primitive words. See the header for what
 * the opcode and colour fields are, and for what is deliberately not claimed
 * about which handler uses which word.
 *
 * Each object carries an explicit .sdata section attribute. The last three
 * words are zero, and a zero-initialised object would otherwise be placed in
 * .sbss; the attribute is what keeps them in .sdata where retail has them.
 * save_data_payload.c spells its two words the same way. */
u32 gModel_dwPolyFt3Template = 0x24808080;
u32 gModel_dwPolyFt4Template = 0x2C808080;
u32 gModel_dwPolyGt3Template = 0x34808080;
u32 gModel_dwPolyGt4Template = 0x3C808080;
u32 gModel_dwPolyFt3SemiTemplate = 0x24404040;
u32 gModel_dwPolyFt4SemiTemplate = 0x2C404040;
u32 gModel_dwPolyGt3SemiTemplate = 0x34404040;
u32 gModel_dwPolyGt4SemiTemplate = 0x3C404040;
u32 gModel_dwLineF3Template = 0x4A808080;
u32 gModel_dwLineF4Template = 0x4E808080;
u32 gModel_dwDrawModeTemplate = 0xE1000220;
u32 gModel_dwTpageBlendRateBits = 0x00000000;
u32 gModel_dwSecondPassClutBits = 0x00000000;
u32 gModel_dwLitColorCacheBase = 0x00000000;

/* The GsSEQ fields read here are the ones Model_SeekSlotPartAnimation writes: ti is the
 * command the part stopped at, tframe the length of that command and rframe
 * the time left in it. 0x6000 is the clamp Model_SeekSlotPartAnimation applies in mode
 * 0x3C. */
s32 tent_Model_GetSlotPartAnimationPosition(ModelSlot *slot, s32 part_index)
{
    s32 duration_units;
    s32 animation_row;
    ModelSlotPart *part_sequence;
    s32 *command_base;
    s32 *current_command;
    s32 *row_command;
    s32 first_command_index;
    s32 remaining_frame;

    duration_units = 0;
    animation_row = 1;
    part_sequence = slot->animation_sequences[part_index];
    if (part_sequence == 0) {
        return 0;
    }
    current_command = slot->animation_commands + part_sequence->ti;
    if (*current_command < 0) {
        return 0;
    }
    command_base = slot->animation_commands;
    for (animation_row = 1; animation_row < 10; animation_row++) {
        first_command_index = slot->animation_command_indices[animation_row][part_index];
        row_command = command_base + first_command_index;
        if (first_command_index != 0xFFFF && current_command >= row_command - 1 &&
            command_base + *(u16 *)(row_command - 1) >= current_command) {
            break;
        }
    }
    if (animation_row < 10) {
        while (row_command < current_command) {
            duration_units += ((u8 *)row_command)[2];
            row_command++;
        }
        remaining_frame = part_sequence->rframe;
        if (remaining_frame != 0x6000 && remaining_frame != 0x7000) {
            duration_units += (part_sequence->tframe - remaining_frame) / 16;
        }
    } else {
        animation_row = 0;
    }
    return (animation_row << 24) | duration_units;
}

/* Seeks channel part_index of model slot slot to position scaled_position within sequence animation_row. The
 * sequence's entry index comes from animation_command_indices[animation_row][part_index] and its step count from
 * animation_duration_rows[animation_row].values[part_index]. The command list at animation_commands holds 4-byte entries
 * whose byte 2 is multiplied by 16 for this walk, and the halfword of the entry
 * before a sequence's first is its length. The walk consumes scaled_position modulo one
 * step's length, entry by entry. It stops at the first entry that outlasts
 * the remainder, at a negative entry, or at the end, then skips forward over
 * non-negative entries to find the next stop. The part's GsSEQ receives the
 * stop index (ti), the entry before it (ci), the entry length (tframe) and
 * the time left in it (rframe); mode 0x3C clamps that last value to 0x6000. */
void Model_SeekSlotPartAnimation(ModelSlot *slot, s32 part_index, s32 animation_row, u32 scaled_position) {
    s32 *command_base;
    ModelSlotPart *part_sequence;
    s32 *command_cursor;
    s32 *row_header;
    s32 *stop_cursor;
    s32 first_command_index;
    s32 row_frame_duration;
    s32 remaining_position;
    s32 entry_count;
    s32 entry_number;
    s32 command_index;
    s32 entry_frame_duration;
    s32 remaining_frame;
    s32 position_after_entry;
    s32 part_pointer_offset;
    s32 part_index_offset;

    /* Both channel offsets are materialised up front and never read by
       name: cse reuses them for the animation_sequences and animation_command_indices indexing just
       below, and that is what moves slot out of $a0 into $t3 at entry.
       Dropping either one changes the entry sequence. */
    part_pointer_offset = part_index * 4;
    part_index_offset = part_index * 2;
    first_command_index = slot->animation_command_indices[animation_row][part_index];
    command_base = slot->animation_commands;
    part_sequence = slot->animation_sequences[part_index];
    command_cursor = command_base + first_command_index;
    if (part_sequence == 0) {
        return;
    }
    if (first_command_index == 0xFFFF) {
        return;
    }
    /* Read twice rather than through a local: cse folds the second read,
       and the extra reference is what puts the address in $v1. */
    if (slot->animation_duration_rows[animation_row].values[part_index] == 0) {
        return;
    }
    row_frame_duration = slot->animation_duration_rows[animation_row].values[part_index] * 16;
    remaining_position = scaled_position % row_frame_duration;
    row_header = command_cursor - 1;
    if (remaining_position == 0) {
        if (scaled_position != 0 && slot->playback_state != 0x3C) {
            remaining_position = row_frame_duration;
        }
    }
    /* The guard and the loop both read the length from memory. jump.c
       duplicates the loop's exit test at its entry, cse folds it against
       the guard, and the loop optimiser hoists the mode byte, the 0x3C
       and the in-loop length read (which cse2 then turns into the copy
       retail keeps in $a3). A named length local defeats all of that. */
    entry_number = 0;
    if (entry_number >= *(u16 *)row_header) {
        return;
    }
    for (; entry_number < *(u16 *)row_header; entry_number++) {
        if (slot->playback_state != 0x3C) {
            position_after_entry = remaining_position - (((u8 *)command_cursor)[2] << 4);
            if (position_after_entry <= 0) {
                break;
            }
        } else {
            position_after_entry = remaining_position - (((u8 *)command_cursor)[2] << 4);
            if (position_after_entry < 0) {
                break;
            }
        }
        remaining_position = remaining_position - (((u8 *)command_cursor)[2] << 4);
        if (command_cursor[1] < 0) {
            break;
        }
        command_cursor++;
    }

    entry_count = *(u16 *)row_header;
    if (entry_number >= entry_count) {
        return;
    }
    /* A goto loop: a while would be rotated and undo the advance on the
       exit path. The advance sits before the count test on purpose, since
       retail's back-branch delay slot runs it on the exit path too. */
    stop_cursor = command_cursor;
scan:
    if (stop_cursor[1] >= 0) {
        entry_number++;
        stop_cursor++;
        if (entry_number < entry_count) {
            goto scan;
        }
    }
    command_index = command_cursor - slot->animation_commands;
    part_sequence->ti = command_index;
    if (command_index != 0) {
        command_index = command_index - 1;
    } else {
        command_index = stop_cursor - slot->animation_commands;
    }
    part_sequence->ci = command_index;
    entry_frame_duration = ((u8 *)command_cursor)[2] << 4;
    remaining_frame = entry_frame_duration - remaining_position;
    part_sequence->tframe = entry_frame_duration;
    part_sequence->rframe = remaining_frame;
    if (slot->playback_state != 0x3C) {
        return;
    }
    if ((s16)remaining_frame < part_sequence->tframe) {
        return;
    }
    part_sequence->rframe = 0x6000;
}

void Model_DrawBackground(void)
{
    CVECTOR colors[5] = {
        {128, 128, 128, 0},
        {96, 64, 64, 0},
        {64, 42, 42, 0},
        {32, 21, 21, 0},
        {0, 0, 0, 0}
    };
    static const BackgroundNormals initial_normals = {
        {{4096, 0, 0, 0}, {0, -4096, 0, 0}}
    };
    BackgroundNormals normals = initial_normals;
    SVECTOR vertices[8];
    GsSPRITE sprite;
    POLY_G4 polygons[3];
    long projection[4];
    ModelSlot *background_slot = &gModel_aSlots[2];
    u8 texture_v_base;
    s32 ring_height;
    s32 texture_u_base;
    s16 initial_sprite_y;
    u16 background_offset_bits;
    s32 minimum_y;
    s32 wrapped_u;
    s32 ring_angle;
    s32 cosine;
    s32 sine;
    s32 inner_band_depth;
    s32 outer_band_depth;

    texture_v_base = gModel_pBackgroundRecord->texture_v_page.bytes.v;
    texture_u_base = (gModel_pBackgroundRecord->texture_u_page & 0x3F) <<
        (2 - ((gModel_pBackgroundRecord->texture_height_flags >> 13) & 3));
    ring_height = background_slot->root_coordinate->matrix.t[1] + 300;
    if (!background_slot->is_active)
        return;
    if (background_slot->ambient_rgb_scale[0] == -4096 &&
        background_slot->ambient_rgb_scale[1] == -4096 && background_slot->ambient_rgb_scale[2] == -4096)
        return;
    Model_UpdateBackgroundScroll(0);
    sprite.attribute = 0;
    if (gModel_pBackgroundRecord->texture_height_flags & 0xE000)
        sprite.attribute = 0x01000000;
    GsSetFlatLight(0, (GsF_LIGHT *)&background_slot->flat_lights[0]);
    GsSetFlatLight(1, (GsF_LIGHT *)&background_slot->flat_lights[1]);
    GsSetFlatLight(2, (GsF_LIGHT *)&background_slot->flat_lights[2]);
    GsSetAmbient(background_slot->ambient_rgb_scale[0], background_slot->ambient_rgb_scale[1], background_slot->ambient_rgb_scale[2]);
    GsSetLightMatrix(&gModel_CameraCoordinateUnit.matrix);
    NormalColorCol(&normals.values[0], &colors[0], CVECTOR_VIEW(&sprite.r));
    sprite.tpage = ((gModel_pBackgroundRecord->texture_height_flags >> 6) & 0x180) |
        ((gModel_pBackgroundRecord->texture_v_page.packed & 0x100) >> 4) |
        ((gModel_pBackgroundRecord->texture_u_page & 0x3FF) >> 6) |
        ((gModel_pBackgroundRecord->texture_v_page.packed & 0x200) << 2);
    sprite.cx = gModel_pBackgroundRecord->clut_x;
    sprite.cy = gModel_pBackgroundRecord->clut_y;
    sprite.w = 64;
    background_offset_bits = *(u16 *)&gModel_wBackgroundOffsetY;
    initial_sprite_y = background_offset_bits - 48;
    sprite.u = texture_u_base;
    sprite.v = texture_v_base;
    sprite.h = 256;
    sprite.y = initial_sprite_y;
    minimum_y = -(gModel_pBackgroundRecord->texture_height_flags & 0x1FFF);
    if ((s16)background_offset_bits >= minimum_y && initial_sprite_y >= minimum_y) {
        if (initial_sprite_y > 0)
            sprite.y = 0;
        if (sprite.y < 0) {
            sprite.v = __builtin_abs(sprite.y);
            sprite.y = 0;
        }
        {
            s32 screen_end_y = sprite.y + 256;
            s32 screen_limit_y = (gModel_pBackgroundRecord->texture_height_flags & 0x1FFF) + (s16)background_offset_bits;
            s32 screen_input_height;
            s32 screen_trim_amount;
            if (screen_limit_y < BACKGROUND_RENDER_CLIP_LIMIT
                    ? screen_limit_y < screen_end_y
                    : screen_end_y >= BACKGROUND_RENDER_CLIP_LIMIT) {
                screen_input_height = sprite.h;
                screen_trim_amount = sprite.y + screen_input_height -
                    ((gModel_pBackgroundRecord->texture_height_flags & 0x1FFF) + gModel_wBackgroundOffsetY);
                sprite.h = screen_input_height - (screen_trim_amount < screen_input_height ? screen_trim_amount : screen_input_height);
            }
        }
        {
            s32 texture_end_v = sprite.v + sprite.h;
            s32 texture_limit_v = gModel_pBackgroundRecord->texture_height_flags & 0x1FFF;
            s32 texture_input_height;
            s32 texture_trim_amount;
            if ((u32)texture_limit_v < BACKGROUND_RENDER_CLIP_LIMIT
                    ? texture_limit_v < texture_end_v
                    : texture_end_v >= BACKGROUND_RENDER_CLIP_LIMIT) {
                texture_input_height = sprite.h;
                texture_trim_amount =
                    sprite.v + texture_input_height - (gModel_pBackgroundRecord->texture_height_flags & 0x1FFF);
                sprite.h = texture_input_height - (texture_trim_amount < texture_input_height ? texture_trim_amount : texture_input_height);
            }
        }
        sprite.x = (s16)*(u16 *)&gModel_wBackgroundScrollX / 10 -
            160 * (u16)(gModel_pBackgroundRecord->texture_width / 0x500u);
        while (sprite.x < 320) {
            if (sprite.x + sprite.w > 0)
                GsSortFastSprite(&sprite, gGraphics_apOrderingTables[3],
                    (u16)((1u << gGraphics_apOrderingTables[3]->length) - 1));
            wrapped_u = (sprite.u + sprite.w) %
                (u16)(gModel_pBackgroundRecord->texture_width / 10u);
            sprite.x += sprite.w;
            sprite.u = wrapped_u + texture_u_base;
        }
    }
    if (!(gModel_pBackgroundRecord->flags & 1))
        return;
    NormalColorCol(&normals.values[1], &colors[1],
                   CVECTOR_VIEW(&polygons[0].r0));
    NormalColorCol(&normals.values[1], &colors[1],
                   CVECTOR_VIEW(&polygons[0].r1));
    NormalColorCol(&normals.values[1], &colors[2],
                   CVECTOR_VIEW(&polygons[0].r2));
    NormalColorCol(&normals.values[1], &colors[2],
                   CVECTOR_VIEW(&polygons[0].r3));
    NormalColorCol(&normals.values[1], &colors[2],
                   CVECTOR_VIEW(&polygons[1].r0));
    NormalColorCol(&normals.values[1], &colors[2],
                   CVECTOR_VIEW(&polygons[1].r1));
    NormalColorCol(&normals.values[1], &colors[3],
                   CVECTOR_VIEW(&polygons[1].r2));
    NormalColorCol(&normals.values[1], &colors[3],
                   CVECTOR_VIEW(&polygons[1].r3));
    NormalColorCol(&normals.values[1], &colors[3],
                   CVECTOR_VIEW(&polygons[2].r0));
    NormalColorCol(&normals.values[1], &colors[3],
                   CVECTOR_VIEW(&polygons[2].r1));
    NormalColorCol(&normals.values[1], &colors[4],
                   CVECTOR_VIEW(&polygons[2].r2));
    NormalColorCol(&normals.values[1], &colors[4],
                   CVECTOR_VIEW(&polygons[2].r3));
    setPolyG4(&polygons[0]);
    setPolyG4(&polygons[1]);
    setPolyG4(&polygons[2]);
    vertices[0].vx = 1000;
    vertices[2].vx = 1600;
    vertices[4].vx = 2200;
    vertices[0].vy = ring_height;
    vertices[0].vz = 0;
    vertices[2].vy = ring_height;
    vertices[2].vz = 0;
    vertices[4].vy = ring_height;
    vertices[4].vz = 0;
    vertices[6].vx = 2800;
    vertices[6].vy = ring_height;
    vertices[6].vz = 0;
    vertices[1].vy = ring_height;
    vertices[3].vy = ring_height;
    vertices[5].vy = ring_height;
    vertices[7].vy = ring_height;
    GsSetLsMatrix(Model_GetLightSourceMatrix());
    ring_angle = 256;
    do {
        cosine = rcos(ring_angle);
        sine = rsin(ring_angle);
        vertices[1].vx = 1000 * cosine / 4096;
        vertices[1].vz = 1000 * sine / 4096;
        vertices[3].vx = 1600 * cosine / 4096;
        vertices[3].vz = 1600 * sine / 4096;
        vertices[5].vx = 2200 * cosine / 4096;
        vertices[5].vz = 2200 * sine / 4096;
        vertices[7].vx = 2800 * cosine / 4096;
        vertices[7].vz = 2800 * sine / 4096;
        inner_band_depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (long *)&polygons[0].x0, (long *)&polygons[0].x1,
            (long *)&polygons[0].x2, (long *)&polygons[0].x3,
            &projection[0], &projection[1]);
        outer_band_depth = RotAverage4(&vertices[4], &vertices[5], &vertices[6], &vertices[7],
            (long *)&polygons[2].x0, (long *)&polygons[2].x1,
            (long *)&polygons[2].x2, (long *)&polygons[2].x3,
            &projection[2], &projection[3]);
        polygons[1].x0 = (u16)polygons[0].x2;
        polygons[1].y0 = (u16)polygons[0].y2;
        polygons[1].x1 = (u16)polygons[0].x3;
        polygons[1].y1 = (u16)polygons[0].y3;
        polygons[1].x2 = (u16)polygons[2].x0;
        polygons[1].y2 = (u16)polygons[2].y0;
        polygons[1].x3 = (u16)polygons[2].x1;
        polygons[1].y3 = (u16)polygons[2].y1;
        if (inner_band_depth >= 0) {
            GsSortPoly(&polygons[0], gGraphics_apOrderingTables[3],
                (u16)((1u << gGraphics_apOrderingTables[3]->length) - 1));
        } else if (outer_band_depth < 0) {
            goto advance;
        }
        GsSortPoly(&polygons[1], gGraphics_apOrderingTables[3],
            (u16)((1u << gGraphics_apOrderingTables[3]->length) - 1));
        if (outer_band_depth >= 0)
            GsSortPoly(&polygons[2], gGraphics_apOrderingTables[3],
                (u16)((1u << gGraphics_apOrderingTables[3]->length) - 1));
advance:
        ring_angle += 256;
        vertices[0] = vertices[1];
        vertices[2] = vertices[3];
        vertices[4] = vertices[5];
        vertices[6] = vertices[7];
    } while (ring_angle <= 4096);
}

/*
 * Direct signed angle differences and the semantic absolute-value predicate
 * recover the initial loads and branch-delay copy without register bindings.
 * Each difference is between signed halfwords, so abs cannot receive INT_MIN.
 * All 496 bytes match with the existing uniform G8 split COMMON profile.
 */
void Model_UpdateBackgroundScroll(s32 force)
{
    s32 yaw_delta;
    s32 pitch_delta;
    s32 initial_turn_difference;
    s32 initial_pitch;
    s32 reloaded_pitch;
    s32 reloaded_turn_difference;
    s32 background_offset_y;
    s32 texture_width;

    yaw_delta = gModel_wViewYaw - *(s16 *)&gModel_ViewSnapshot.bytes[2];
    pitch_delta = gModel_wViewPitch - *(s16 *)&gModel_ViewSnapshot.bytes[4];
    if (__builtin_abs(yaw_delta) >= 0x801) {
        if (yaw_delta > 0) {
            yaw_delta -= MODEL_ANGLE_FULL_TURN;
        } else {
            yaw_delta += MODEL_ANGLE_FULL_TURN;
        }
    }
    if (force != 0 || yaw_delta != 0) {
        texture_width = gModel_pBackgroundRecord->texture_width;
        gModel_wBackgroundScrollX = (yaw_delta * 1280 / 384 + gModel_wBackgroundScrollX + texture_width) % texture_width;
    }
    if (force != 0 || pitch_delta != 0) {
        initial_pitch = gModel_wViewPitch;
        if (initial_pitch >= 0x801) {
            initial_turn_difference = initial_pitch - MODEL_ANGLE_FULL_TURN;
            if (initial_turn_difference >= 0) {
                background_offset_y = -0x139;
                if (initial_turn_difference < MODEL_ANGLE_QUARTER_TURN) {
                    goto compute;
                }
            } else {
                background_offset_y = -0x139;
                if (MODEL_ANGLE_FULL_TURN - initial_pitch < MODEL_ANGLE_QUARTER_TURN) {
                compute:
                    reloaded_pitch = gModel_wViewPitch;
                    reloaded_turn_difference = reloaded_pitch - MODEL_ANGLE_FULL_TURN;
                    if (reloaded_turn_difference >= 0) {
                        background_offset_y = -(reloaded_turn_difference * 26) / 85;
                    } else {
                        background_offset_y = -((MODEL_ANGLE_FULL_TURN - reloaded_pitch) * 26) / 85;
                    }
                }
            }
        } else if (initial_pitch < MODEL_ANGLE_QUARTER_TURN) {
            background_offset_y = (initial_pitch * 26) / 85;
        } else {
            background_offset_y = 0x139;
        }
        gModel_wBackgroundOffsetY = background_offset_y;
    }
    gModel_ViewSnapshot = *(ModelBytes8 *)&gModel_wViewDistance;
}

#include "model_effect_handlers.h"
#include "model_subdivided_effect.h"
#include "model_scene_effect_handler_table.h"
#include "file_names.h"
#include "model_handler_state.h"

/* The effect handlers the scene controllers dispatch to, and the paths and
 * trace formats the scene controllers, debug controller and slot control
 * print, in the order retail keeps them in .rodata. The Japanese build drops
 * the debug traces and keeps only the table and the two paths. */
s32 (*const gModel_apfnEffectHandlers[4])(s32, s32) = {
    (s32 (*)(s32, s32))Model_UpdateBurstEffect,
    (s32 (*)(s32, s32))Model_UpdateSubdividedEffect,
    (s32 (*)(s32, s32))Model_UpdateDiscEffect,
    (s32 (*)(s32, s32))Model_UpdateSparkEffect,
};
const u8 gModel_szSuMrgPath[] = "M:/mrgSU/SU.mrg";
#ifndef VERSION_JAPAN
const u8 gModel_szAnimationSoundTraceFormat[] = "e%03ld(%ld)=%d\n";
const char gModelDebug_szLoadStageFormat[] = "LOAD STAGE(%d,%d)\n";
const char gModelDebug_szLoadModel0Format[] = "LOAD MODEL0(%d,%d)\n";
const char gModelDebug_szLoadModel1Format[] = "LOAD MODEL1(%d,%d)\n";
const char gModelDebug_szSlotFlagsFormat[] = "(AF=%d,GF=%d)";
const char gModelDebug_szStageFormat[] = "STAGE=%d,Y=%d\n";
const char gModel_szContinueTrace[] = "CONTINUE\n";
#endif
const u8 gFile_szModelMrgPath[] = "M:/mrgSU/model.mrg";
#ifndef VERSION_JAPAN
const char gModel_szLoadModelTraceFormat[] = "LoadModelDO(%d)=%03X\n";
#endif

/*
 * The model scene runtime between the background renderer and the debug
 * controller: the background VRAM animation step, the four scene-mode
 * controllers (animated battle, Exodia, Model_UpdateCreditsMonsterSlot, credits), the
 * Model_StartBattleSlotAction/Model_PlaySlotAnimationSounds pair, and the camera/view correction, tint,
 * camera-move setup and scene reset/configuration functions.
 */

#include "../psyq/libetc.h"
#include "model_image_copy_frames.h"
#include "model_scene_runtime.h"
#include "model_camera_effects.h"
#include "sound_output_state.h"
#include "../psyq/stdio.h"

/* Zero-initialized storage this unit owns. Each build's generated
 * model_bss_commons subsegment places it at the retail addresses. */
ModelCameraMove gModel_CameraMove;
ModelTintRequest gModel_aTintRequests[10];
ModelSlot gModel_aSlots[MODEL_SLOT_COUNT];

#include "model_slot_control.h"
#include "sound.h"
#include "model_update_view_metrics.h"
#include "duel_card.h"
#include "sound_driver.h"
#include "model_scene_states.h"
#include "fade.h"
#include "sound_pending_entries.h"
#include "model_queue_tint_request.h"
#include "file_transfer.h"
#include "high_memory_addresses.h"
#include "model_load_words.h"
/* libhmd.h names MATRIX, GsOT and GsCOORDUNIT without including anything, so
   it only parses after libgte.h, libgpu.h and libgs.h, all included above. */
#include "../psyq/rand.h"
#include "card_constants.h"
#include "model_scene_imports.h"
#include "sound_init.h"
#include "../psyq/memory.h"
#include "../ygo_types.h"
#include "../psyq/stdarg.h"
#include "model_camera_state.h"

void Model_StepBackgroundVramAnimation(void) {
    s32 k;
    s32 u_0;
    s32 u_1;
    s32 r;
    s32 r4;
    s32 n;
    s32 q;

    if (gModel_aSlots[2].is_active != 0) {
        k = gModel_pBackgroundRecord->selector;
        switch (k) {
        case 1:
            u_0 = VSync(-1) / 8;
            gModel_ImageCopyRectY = 0x1C0;
            gModel_ImageCopyRectW = 0x10;
            gModel_ImageCopyRectH = 0x40;
            r = u_0 % 3;
            gModel_ImageCopyRectX = r * 16 + 0x2D0;
            MoveImage(MODEL_IMAGE_COPY_RECT, 0x2C0, 0x1C0);
            break;
        case 4:
            n = MODEL_IMAGE_COPY_FRAME_COUNT;
            u_1 = VSync(-1) / 10;
            r4 = u_1 % n;
            gModel_ImageCopyRectX = 0x260;
            q = gModel_abImageCopyFrameRows[r4];
            gModel_ImageCopyRectW = 0x20;
            gModel_ImageCopyRectH = 0x18;
            gModel_ImageCopyRectY = q * 3 * 8 + 0x180;
            MoveImage(MODEL_IMAGE_COPY_RECT, 0x220, 0x168);
            break;
        }
    }
}

/*
 * Model scene controllers: the mode 15 controller, then the mode 19 battle
 * intro and its two neighbours.
 *
 * Model_UpdateAnimatedBattle, the mode 15 controller, copies the scene's four
 * effect handlers and its eight-byte view offset, then steps the signed
 * gModel_bPresentationPhase phase through lighting, fades, effect requests and the per-slot
 * handler calls. It matches all 4,908 text bytes and owns the 43-entry phase
 * table and the 7-entry model-kind table that precede the intro controller's
 * tables. Each phase keeps its temporaries in its own block, which is what lets
 * retail reuse $s0 across phases; the angle-window tests keep a bound compare
 * in each signed arm, and phases 13 and 17 classify the animation state with
 * nested inequality tests rather than a switch.
 *
 * The view-offset words and the diagnostics string use their unknown-bound
 * declarations, which select retail's split absolute addresses; the
 * switches for them, and for the two intro-controller views, are at the top
 * of the unit.
 */

u8 gModel_bCurrentDrawMode = 0;
u8 gModel_bBlendRateActive = 0;
u16 gModel_wDrawTintRedGreen = 0;
u8 gModel_bDrawTintBlue = 0;
u8 gModel_bPresentationTimer = 0;
static u16 sModelHandlerState_PadAFEA
    = 0;
u32 gModel_AnimatedBattleCameraEndpoint[2] = {
    0x10000000,
    0x00040000,
};

void Model_UpdateAnimatedBattle(void)
{
    s32 (*effect_handlers[4])(s32, s32);
    SVECTOR camera_endpoint;
    s32 defender_handler_index;

    *(ModelSlotS32Quad *)effect_handlers = *(ModelSlotS32Quad *)gModel_apfnEffectHandlers;
    defender_handler_index = 1;
    if (((gDuel_adwCardStats[gModel_aSlots[1].model_id] >> 0x1A) & 0x1F) == 1) {
        defender_handler_index = 2;
    }
    switch ((s8)(gModel_bPresentationPhase + 1)) {
    case 0:
        break;
    case 1:
        Model_SetSlotAmbient(2, -0x1000, -0x1000, -0x1000);
        Model_SetSlotTintTarget(0, 5, 0, 0, 0);
        Model_SetSlotTintTarget(1, 5, 0, 0, 0);
        gModel_bPresentationPhase++;
        break;
    case 2:
        SD_BGMPlayDefaultStep(gModel_wPresentationBgmCommand);
        gModel_bPresentationPhase++;
        break;
    case 3: {
        ModelCameraOrbitPreset *preset;
        s32 key_magnitude;

        *(ModelBytes8 *)&camera_endpoint = *(ModelBytes8 *)gModel_AnimatedBattleCameraEndpoint;
        key_magnitude = 0x78;
        switch (gModel_pBackgroundRecord->selector) {
        case 0:
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
            Model_SetCameraOrbitFromPreset(2, 0xA, 0);
            break;
        case 2:
            preset = ModelEffect_GetCoefficient(0xA);
            Model_SetCameraOrbit(2, 0x64, preset->yaw + 0x800, -0x400, 0);
            camera_endpoint.vx = preset->distance;
            camera_endpoint.vy = -0x1000;
            camera_endpoint.vz = preset->pitch + 0x400;
            key_magnitude = 0x96;
            break;
        }
        Model_ApplyEffectKeyQueueCommand(0, (void *)&camera_endpoint, (void *)0, key_magnitude);
        gModel_bPresentationPhase++;
        break;
    }
    case 4:
        if (gModel_aSlots[2].ambient_rgb_scale[0] < 0x800) {
            Model_SetSlotDrawMode(2, 3);
            gModel_aSlots[2].ambient_rgb_scale[0] += 0x40;
            gModel_aSlots[2].ambient_rgb_scale[1] += 0x40;
            gModel_aSlots[2].ambient_rgb_scale[2] += 0x40;
        } else if (Model_IsEffectKeyQueueEmpty() != 0) {
            Model_RequestCameraMove(0);
            Model_SetEffectKeyBatchingEnabled(1);
            Model_QueueCameraOrbitFromPreset(0, 0, 0, -0xC);
            Model_QueueEffectEndpointPair(0, 0, 0x4000);
            gModel_bPresentationPhase++;
        }
        break;
    case 5:
        if (gModel_aSlots[0].is_active != 0) {
            s16 *current_key_halfwords = (s16 *)Model_GetCurrentEffectKeyAddress();
            if (current_key_halfwords != 0 && current_key_halfwords[3] == 2) {
                Model_SetEffectKeyBatchingEnabled(1);
                Model_QueueCameraOrbitFromPreset(0, 0, 0, -0xC);
                Model_QueueEffectEndpointPair(0, 0, 0x4000);
                gModel_bActiveSlotIndex = 0;
                effect_handlers[0](gModel_aSlots[0].variant_module_data_address, 0);
                gModel_aSlots[0].field_E15 = 0;
                gModel_bPresentationPhase++;
            }
        }
        break;
    case 6:
        if (gModel_aSlots[0].field_E15 == 0) {
            gModel_bActiveSlotIndex = 0;
            if (effect_handlers[0](gModel_aSlots[0].variant_module_data_address, -1) != 2) {
                break;
            }
            gModel_aSlots[0].field_E15++;
        }
        if ((s8)gModel_aSlots[0].tint[0] >= 0) {
            gModel_aSlots[0].tint[0] += 2;
            gModel_aSlots[0].tint[1] += 2;
            gModel_aSlots[0].tint[2] += 2;
        } else {
            if (gModel_aSlots[0].draw_mode_request != 4) {
                Model_SetSlotDrawMode(0, 3);
            }
            Model_SetSlotTintTarget(0, 0, 0x80, 0x80, 0x80);
            Model_SetSlotTintTarget(1, 0, 0x80, 0x80, 0x80);
            Model_ControlSlotAnimation(0, 2, 1);
            gModel_bPresentationTimer = 0;
            gModel_bPresentationPhase++;
        }
        break;
    case 7:
        if (gModel_aSlots[0].draw_mode_request != 4) {
            Model_SetSlotDrawMode(0, 3);
        }
        if (gModel_aSlots[0].active_animation_command_row == 0) {
            gModel_bPresentationTimer += Model_GetFrameStep();
            if (gModel_bPresentationTimer >= 0x3C && gModel_aSlots[1].is_active != 0) {
                Model_SetEffectKeyBatchingEnabled(1);
                Model_QueueCameraOrbitFromPreset(1, 0, 0, -0xA);
                Model_QueueEffectEndpointPair(1, 1, 0x4000);
                gModel_bPresentationPhase++;
            }
        }
        break;
    case 8: {
        s16 *current_key_halfwords;

        if (gModel_aSlots[0].draw_mode_request != 4) {
            Model_SetSlotDrawMode(0, 3);
        }
        current_key_halfwords = (s16 *)Model_GetCurrentEffectKeyAddress();
        if (current_key_halfwords != 0 && current_key_halfwords[3] == 2) {
            Model_SetCameraOrbitFromPreset(1, 0, 0);
            gModel_bPresentationPhase++;
        }
        break;
    }
    case 9:
        if ((s8)gModel_aSlots[1].tint[0] >= 0) {
            gModel_aSlots[1].tint[0] += 2;
            gModel_aSlots[1].tint[1] += 2;
            gModel_aSlots[1].tint[2] += 2;
        } else {
            Model_ControlSlotAnimation(1, 2, 1);
            gModel_bPresentationTimer = 0;
            gModel_bPresentationPhase++;
        }
        break;
    case 10:
        if (gModel_aSlots[1].active_animation_command_row == 0) {
            gModel_bPresentationTimer += Model_GetFrameStep();
            if (gModel_bPresentationTimer >= 0x3C) {
                s32 preset_index;
                s32 key_magnitude;

                preset_index = gModel_aSlots[0].metadata.prefix.bytes.guardian_star_options[gModel_aSlots[0].guardian_star_selection] & 0x1F;
                key_magnitude = -0x14;
                switch (preset_index) {
                case 1:
                    preset_index = 0;
                    break;
                case 2:
                    preset_index = 1;
                    break;
                case 3:
                    preset_index = 2;
                    break;
                case 0:
                default:
                    preset_index = 3;
                    break;
                }
                if ((u32)preset_index < 2) {
                    key_magnitude = key_magnitude / 2;
                }
                Model_QueueCameraOrbitFromPreset(0, preset_index, 0, key_magnitude);
                gModel_bPresentationPhase++;
            }
        }
        break;
    case 11:
        if (Model_IsEffectKeyQueueEmpty() != 0) {
            Model_RequestCameraMoveToSlots(1, -1, 0, -0x1E);
            gModel_bPresentationPhase++;
        }
        break;
    case 12:
        if (Model_IsCameraMoveComplete(1) != 0) {
            s32 target_yaw;
            s32 yaw_step_limit;
            s32 yaw_delta;

            target_yaw = Model_GetSlotCameraPresetYaw(0);
            yaw_step_limit = (Model_GetFrameStep() + 2) * 8;
            yaw_delta = (s16)gModel_wViewYaw - target_yaw;
            if (yaw_delta >= 0 ? yaw_step_limit >= yaw_delta : yaw_step_limit >= target_yaw - (s16)gModel_wViewYaw) {
                Model_StartBattleSlotAction(0);
                gModel_bActiveSlotIndex = 1;
                effect_handlers[defender_handler_index](gModel_aSlots[1].variant_module_data_address, 0x18E);
                gModel_aSlots[1].field_E15 = 0xFF;
                gModel_bPresentationPhase++;
            }
        }
        break;
    case 13: {
        s32 a;
        s32 move_magnitude;

        if (gModel_aSlots[1].active_animation_command_row != 5) {
            if (gModel_aSlots[1].active_animation_command_row != 6) {
                if (gModel_aSlots[1].active_animation_command_row == 8) {
                    a = 3;
                } else {
                    a = 0;
                }
            } else {
                a = 2;
            }
        } else {
            a = 1;
        }
        if (gModel_aSlots[1].field_E15 == 0xFF && gModel_aSlots[1].active_animation_command_row == 8) {
            gModel_aSlots[1].field_E15 = 0;
        }
        if (gModel_aSlots[1].field_E15 == 0) {
            gModel_bActiveSlotIndex = 1;
            effect_handlers[defender_handler_index](gModel_aSlots[1].variant_module_data_address, -1);
        }
        if (gModel_aSlots[0].animation_control_state == 2) {
            if (gModel_aSlots[0].active_animation_command_row == 0) {
                a = 4;
            }
        } else if (Model_GetCameraHoldState() == 0) {
            a = 0;
        }
        if (a != 0) {
            a = Model_GetSlotDistanceFromCameraTarget(1) * 0x1E / 1000;
            if (Model_GetCameraHoldState() == 2) {
                Model_SetCameraHoldState(1);
            }
            Model_ApplyEffectKeyQueueCommand(0, (void *)0, (void *)0, 0);
            move_magnitude = a;
            if (move_magnitude < 0xA) {
                move_magnitude = 0xA;
            }
            Model_RequestCameraMoveToSlots(1, -1, 1, move_magnitude);
            gModel_bPresentationPhase++;
        }
        break;
    }
    case 14:
        if (gModel_aSlots[1].active_animation_command_row == 6) {
            gModel_bPresentationPhase = 0x14;
            break;
        }
        if (gModel_aSlots[1].field_E15 == 0) {
            gModel_bActiveSlotIndex = 1;
            effect_handlers[defender_handler_index](gModel_aSlots[1].variant_module_data_address, -1);
        }
        if (Model_IsCameraMoveComplete(1) != 0 && Model_IsEffectKeyQueueEmpty() != 0
            && gModel_aSlots[0].animation_control_state == 2 && gModel_aSlots[0].active_animation_command_row == 0
            && gModel_aSlots[1].animation_control_state == 2 && gModel_aSlots[1].active_animation_command_row == 0) {
            if (gModel_aSlots[1].defense_position != 0) {
                Model_SetCameraFollowCountdown(0x78);
                gModel_bPresentationPhase = 0x1E;
            } else {
                gModel_bPresentationPhase++;
            }
        }
        break;
    case 15:
        if (Model_IsEffectKeyQueueEmpty() != 0) {
            Model_RequestCameraMoveToSlots(1, -1, 1, -0x1E);
            gModel_bPresentationPhase++;
        }
        break;
    case 16:
        if (Model_IsCameraMoveComplete(1) != 0) {
            s32 target_yaw;
            s32 yaw_step_limit;
            s32 yaw_delta;

            target_yaw = Model_GetSlotCameraPresetYaw(1);
            yaw_step_limit = (Model_GetFrameStep() + 2) * 8;
            yaw_delta = (s16)gModel_wViewYaw - target_yaw;
            if (yaw_delta >= 0 ? yaw_step_limit >= yaw_delta : yaw_step_limit >= target_yaw - (s16)gModel_wViewYaw) {
                Model_StartBattleSlotAction(1);
                gModel_bPresentationPhase++;
            }
        }
        break;
    case 17: {
        s32 a;
        s32 move_magnitude;

        if (gModel_aSlots[0].active_animation_command_row != 5) {
            if (gModel_aSlots[0].active_animation_command_row != 6) {
                if (gModel_aSlots[0].active_animation_command_row == 8) {
                    a = 3;
                } else {
                    a = 0;
                }
            } else {
                a = 2;
            }
        } else {
            a = 1;
        }
        if (gModel_aSlots[1].animation_control_state == 2) {
            if (gModel_aSlots[1].active_animation_command_row == 0) {
                a = 4;
            }
        } else if (Model_GetCameraHoldState() == 0) {
            a = 0;
        }
        if (a != 0) {
            a = Model_GetSlotDistanceFromCameraTarget(0) * 0x1E / 1000;
            if (Model_GetCameraHoldState() == 2) {
                Model_SetCameraHoldState(1);
            }
            Model_ApplyEffectKeyQueueCommand(0, (void *)0, (void *)0, 0);
            move_magnitude = a;
            if (move_magnitude < 0xA) {
                move_magnitude = 0xA;
            }
            Model_RequestCameraMoveToSlots(1, -1, 0, move_magnitude);
            gModel_bPresentationPhase++;
        }
        break;
    }
    case 18:
        if (gModel_aSlots[0].active_animation_command_row == 6) {
            gModel_bPresentationPhase = 0x14;
            break;
        }
#ifdef VERSION_JAPAN
        /* The Japanese build polls the same two calls but prints nothing. */
        if (Model_IsCameraMoveComplete(1) != 0) {
            Model_IsEffectKeyQueueEmpty();
        }
#else
        if (Model_IsCameraMoveComplete(1) != 0 && Model_IsEffectKeyQueueEmpty() != 0
            && gModel_aSlots[0].animation_control_state == 2 && gModel_aSlots[0].active_animation_command_row == 0
            && gModel_aSlots[1].animation_control_state == 2 && gModel_aSlots[1].active_animation_command_row == 0) {
            FntPrint("WHY?\n");
        }
#endif
        break;
    case 21: {
        s32 selected_slot_index = gModel_aSlots[0].active_animation_command_row != 6;
        ModelSlot *opposite_slot = &gModel_aSlots[selected_slot_index ^ 1];

        if (opposite_slot->animation_control_state == 2) {
            if (opposite_slot->active_animation_command_row == 0 && gModel_aSlots[selected_slot_index].playback_state == 0x23) {
                if (gModel_aSlots[1].defense_position == 0
                    && gModel_aSlots[0].metadata.prefix.values.attack_compare_value == gModel_aSlots[1].metadata.prefix.values.attack_compare_value) {
                    if (gModel_aSlots[1].stone_texture_state == 0xFF) {
                        gModel_aSlots[1].stone_texture_state = 0x80;
                    }
                    Model_ControlSlotAnimation(1, 6, 1);
                }
                Model_ApplyEffectKeyQueueCommand(0, (void *)0, (void *)0, 0);
                if (Model_IsCameraMoveComplete(1) != 0) {
                    s32 move_magnitude = Model_GetSlotDistanceFromCameraTarget(selected_slot_index) * 0x1E / 1000;
                    if (move_magnitude < 0xA) {
                        move_magnitude = 0xA;
                    }
                    Model_RequestCameraMoveToSlots(1, -1, selected_slot_index, -move_magnitude);
                }
                gModel_bPresentationPhase++;
            }
        }
        break;
    }
    case 22: {
        s32 selected_slot_index = gModel_aSlots[0].active_animation_command_row != 6;
        ModelSlot *selected_slot = &gModel_aSlots[selected_slot_index];

        if (selected_slot->stone_texture_state == 0 && Model_IsCameraMoveComplete(1) != 0) {
            Model_SetSlotTintTarget(selected_slot_index, 5, 0x80, 0x80, 0x80);
            gModel_bActiveSlotIndex = selected_slot_index;
            effect_handlers[3](selected_slot->variant_module_data_address, 0);
            selected_slot->field_E15 = 0;
            gModel_bPresentationPhase++;
        }
        break;
    }
    case 23: {
        s32 selected_slot_index = gModel_aSlots[0].active_animation_command_row != 6;
        ModelSlot *selected_slot = &gModel_aSlots[selected_slot_index];
        s32 biased_yaw;
        s32 current_yaw;

        if (selected_slot->field_E15 == 0) {
            gModel_bActiveSlotIndex = selected_slot_index;
            if (effect_handlers[3](selected_slot->variant_module_data_address, -1) != 2) {
                break;
            }
            selected_slot->field_E15++;
        }
        if (selected_slot->tint[0] != 0) {
            selected_slot->tint[0] -= 2;
            selected_slot->tint[1] -= 2;
            selected_slot->tint[2] -= 2;
            break;
        }
        current_yaw = (s16)gModel_wViewYaw;
        if (selected_slot_index > 0) {
            biased_yaw = (current_yaw + 0xC00) % 0x1000;
        } else {
            biased_yaw = (current_yaw + 0x400) % 0x1000;
        }
        if ((u32)(biased_yaw - 0x201) < 0xBFF) {
            s32 opposite_slot_index = selected_slot_index ^ 1;
            s32 move_magnitude = Model_GetSlotDistanceFromCameraTarget(opposite_slot_index) * 0x1E / 1000;

            if (move_magnitude < 0x1E) {
                move_magnitude = 0x1E;
            }
            Model_RequestCameraMoveToSlots(1, -1, opposite_slot_index, -move_magnitude);
            gModel_bPresentationPhase++;
        }
        break;
    }
    case 24: {
        s32 selected_slot_index = gModel_aSlots[0].active_animation_command_row == 6;

        if (Model_IsEffectKeyQueueEmpty() != 0) {
            s32 current_distance = (s16)gModel_wViewDistance;
            if (Model_AdjustCameraDistanceForSlot(selected_slot_index, 0x2BC) + 0xA >= current_distance && Model_IsCameraMoveComplete(1) != 0) {
                if (gModel_aSlots[0].metadata.prefix.values.attack_compare_value == gModel_aSlots[1].metadata.prefix.values.attack_compare_value
                    && gModel_aSlots[1].defense_position == 0) {
                    gModel_bActiveSlotIndex = 1;
                    effect_handlers[3](gModel_aSlots[1].variant_module_data_address, 0);
                    gModel_aSlots[1].field_E15 = 0;
                    gModel_bPresentationPhase = 0x19;
                    gModel_bPresentationTimer = 0;
                } else {
                    Model_QueueEffectEndpointPair(-1, selected_slot_index, 0x4000);
                    Model_SetCameraTargetSlots(-1, selected_slot_index);
                    Model_ControlSlotAnimation(selected_slot_index, 7, 1);
                    gModel_bPresentationPhase++;
                }
            }
        }
        break;
    }
    case 25: {
        s32 selected_slot_index = gModel_aSlots[0].active_animation_command_row == 6;

        if (gModel_aSlots[selected_slot_index].active_animation_command_row != 7) {
            Model_ApplyEffectKeyQueueCommand(0, (void *)0, (void *)0, 0);
            Model_RequestCameraMoveToSlots(1, -1, selected_slot_index, -0x1E);
            gModel_bPresentationTimer = 0x3C;
            gModel_bPresentationPhase++;
        }
        break;
    }
    case 26: {
        s32 selected_slot_index = gModel_aSlots[0].active_animation_command_row == 6;

        if (gModel_aSlots[selected_slot_index].stone_texture_state == 0 && Model_IsCameraMoveComplete(1) != 0) {
            gModel_bPresentationTimer += Model_GetFrameStep();
            if (gModel_bPresentationTimer >= 0x3C) {
                Model_SetSlotTintTarget(selected_slot_index, 5, 0x80, 0x80, 0x80);
                gModel_bPresentationPhase++;
            }
        }
        break;
    }
    case 27: {
        s32 selected_slot_index = gModel_aSlots[0].active_animation_command_row == 6;
        ModelSlot *selected_slot;
        ModelSlot *slot_base;

        if (gModel_aSlots[0].metadata.prefix.values.attack_compare_value == gModel_aSlots[1].metadata.prefix.values.attack_compare_value
            && gModel_aSlots[1].defense_position == 0 && gModel_aSlots[1].field_E15 == 0) {
            gModel_bActiveSlotIndex = 1;
            if (effect_handlers[3](gModel_aSlots[1].variant_module_data_address, -1) != 2) {
                break;
            }
            gModel_aSlots[1].field_E15++;
        }
        slot_base = gModel_aSlots;
        selected_slot = &slot_base[selected_slot_index];
        if (selected_slot->tint[0] != 0) {
            selected_slot->tint[0] -= 2;
            selected_slot->tint[1] -= 2;
            selected_slot->tint[2] -= 2;
        } else {
            gModel_bPresentationPhase++;
        }
        break;
    }
    case 28:
        if (Model_IsCameraMoveComplete(1) != 0) {
            gModel_bPresentationPhase = 0x28;
        }
        break;
    case 31:
        if (gModel_aSlots[1].field_E15 == 0) {
            gModel_bActiveSlotIndex = 1;
            if (effect_handlers[defender_handler_index](gModel_aSlots[1].variant_module_data_address, -2) == 2) {
                gModel_aSlots[1].field_E15++;
            }
        } else if (Model_IsCameraMoveComplete(1) != 0) {
            Model_SetSlotTintTarget(1, 5, 0x80, 0x80, 0x80);
            gModel_bPresentationPhase++;
        }
        break;
    case 32:
        if (gModel_aSlots[1].tint[0] != 0) {
            gModel_aSlots[1].tint[0] -= 2;
            gModel_aSlots[1].tint[1] -= 2;
            gModel_aSlots[1].tint[2] -= 2;
        } else {
            s32 biased_yaw = (s16)gModel_wViewYaw + 0xC00;

            if ((u32)(biased_yaw % 0x1000 - 0x201) < 0xBFF) {
                s32 move_magnitude = Model_GetSlotDistanceFromCameraTarget(0) * 0x1E / 1000;
                if (move_magnitude < 0x1E) {
                    move_magnitude = 0x1E;
                }
                Model_RequestCameraMoveToSlots(1, -1, 0, -move_magnitude);
                gModel_bPresentationPhase++;
            }
        }
        break;
    case 33:
        if (Model_IsEffectKeyQueueEmpty() != 0 && Model_IsCameraMoveComplete(1) != 0) {
            Model_SetSlotTintTarget(0, 5, 0x80, 0x80, 0x80);
            gModel_bPresentationPhase++;
        }
        break;
    case 34:
        if (gModel_aSlots[0].tint[0] != 0) {
            gModel_aSlots[0].tint[0] -= 2;
            gModel_aSlots[0].tint[1] -= 2;
            gModel_aSlots[0].tint[2] -= 2;
        } else {
            gModel_bPresentationPhase = 0x28;
        }
        break;
    case 41:
        Model_SetSlotDrawMode(2, 3);
        Fade_StartOut();
        gModel_bPresentationPhase++;
        break;
    case 42:
        Model_SetSlotDrawMode(2, 3);
        if (!(gFade_State.flags & 0x80)) {
            SD_ResetPendingEntries();
            gModel_bPresentationPhase = -2;
        }
        break;
    }
    Model_LockOrbitDirection(1);
}

/* Defined after Model_UpdateAnimatedBattle, whose "WHY?\n" trace literal
   retail keeps before it in .sdata. */
u32 gModel_ExodiaOrbitOffset[2] = {
    0xFA240000,
    0,
};

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

void Model_UpdateExodiaPresentation(void)
{
    SVECTOR camera_offset;
    ModelSlot *primary_slot;
    s32 (*slot0_module_handler)(s32, s32);
    s32 (*slot1_module_handler)(s32, s32);

    *(ModelBytes8 *)&camera_offset = *(ModelBytes8 *)gModel_ExodiaOrbitOffset;
    primary_slot = gModel_aSlots;
    slot0_module_handler = (s32 (*)(s32, s32))(gModel_Slot0VariantModuleAddress + 4);
    slot1_module_handler = (s32 (*)(s32, s32))(gModel_Slot1VariantModuleAddress + 4);
    Model_SetFrameStepOverride(1);
    if ((u32)((u8)gModel_bPresentationPhase - 8) < 2 && primary_slot->field_E15 == 0) {
        gModel_bActiveSlotIndex = 0;
        if (slot0_module_handler(gModel_Slot0VariantDataAddress, -1) == 2) {
            primary_slot->field_E15++;
        }
    }
    if (gModel_bPresentationPhase >= 10 && gModel_aSlots[1].field_E15 == 0) {
        gModel_bActiveSlotIndex = 0;
        if (slot1_module_handler(gModel_Slot1VariantDataAddress, -1) == 2) {
            Model_SetSlotPlaybackDirection(0, 0);
            Fade_CutToWhite();
            gModel_bPresentationPhase = 15;
        }
    }
    switch ((s8)(gModel_bPresentationPhase + 1)) {
    case 0: {
        ModelSlot *load_slots;
        if (gModel_aSlots[0].load_step == 0xFF) {
            if (((gFile_dwTransferState & 0x2000030) | gFile_dwCancelRequestState) == 0) {
                Model_LoadMonsterMerge(
                    0, MODEL_SPECIAL_BATTLE_ID, 0, 0, 0, 0, 4);
            }
        } else {
            Model_AdvanceSlotLoad(0);
        }
        load_slots = gModel_aSlots;
        if (!load_slots->is_active) {
            break;
        }
        Model_SetCameraOrbit(0, 10000, 0xE00, 0, 0);
        Model_SeekSlotAnimation(0, 1, 0);
        if (primary_slot->active_animation_command_row) {
            Model_ControlSlotAnimation(0, 0, 0);
        }
        Model_SetSlotPlaybackDirection(0, 1);
        Model_SetSlotTintTarget(0, 5, 0, 0, 0);
        primary_slot->queued_animation_row_latch = 1;
        primary_slot->queued_animation_row = 2;
        SD_BGMPlayDefaultStep(0x7310);
        load_slots->metadata.first_star_module_command = -1;
        load_slots->metadata.second_star_module_command = -1;
        load_slots->metadata.primary_module_command = -1;
        gModel_bActiveSlotIndex = 0;
        slot0_module_handler(gModel_Slot0VariantDataAddress, 0);
        slot1_module_handler(gModel_Slot1VariantDataAddress, 0);
        load_slots[0].field_E15 = 0;
        load_slots[1].field_E15 = 0;
        gModel_bPresentationPhase++;
        break;
    }
    case 1:
        if (SD_GetStatusFlags() & 0x80) {
            camera_offset.vy = -2000;
            Model_SeekSlotAnimation(0, 0, 10);
            Model_QueueRelativeCameraOrbit(0, 2000, 0x100, 0x100, &camera_offset, 110);
            gModel_bPresentationPhase++;
        }
        break;
    case 2: {
        ModelSlot *tint_slot = gModel_aSlots;
        if ((s8)tint_slot->tint[0] >= 0) {
            tint_slot->tint[0] += 2;
            tint_slot->tint[1] += 2;
            tint_slot->tint[2] += 2;
        } else {
            Model_SetSlotTintTarget(0, 0, 0x80, 0x80, 0x80);
            gModel_bPresentationPhase++;
        }
        break;
    }
    case 3:
        if (Model_IsEffectKeyQueueEmpty() && primary_slot->animation_position >= 0x780) {
            Model_SeekSlotAnimation(0, 0, 0);
            gModel_bPresentationPhase++;
        }
        break;
    case 4:
        if (primary_slot->animation_position >= 0x780) {
            Model_SeekSlotAnimation(0, 0, 0);
            gModel_bPresentationPhase++;
        }
        break;
    case 5:
        if (primary_slot->animation_position >= 0x1E0) {
            Model_SeekSlotAnimation(0, 0, 0);
            camera_offset.vx = -2500;
            camera_offset.vy = -1500;
            camera_offset.vz = 1000;
            Model_SetCameraOrbit(0, 3000, -0x100, -0x100, &camera_offset);
            gModel_bPresentationPhase++;
        }
        break;
    case 6:
        if (primary_slot->animation_position >= 0x1E0) {
            Model_SeekSlotAnimation(0, 0, 0);
            camera_offset.vx = 2500;
            camera_offset.vy = -1500;
            camera_offset.vz = 1000;
            Model_SetCameraOrbit(0, 3000, 0x100, -0x100, &camera_offset);
            gModel_bPresentationPhase++;
        }
        break;
    case 7:
        if (primary_slot->animation_position >= 0x1E0) {
            Model_SeekSlotAnimation(0, 0, 0);
            camera_offset.vx = -500;
            camera_offset.vy = 1500;
            camera_offset.vz = 1000;
            Model_SetCameraOrbit(0, 3000, -0x200, -0x100, &camera_offset);
            gModel_bPresentationPhase++;
        }
        break;
    case 8:
        if (primary_slot->animation_position >= 0x1E0) {
            Model_SeekSlotAnimation(0, 0, 0);
            camera_offset.vx = 500;
            camera_offset.vy = 1500;
            camera_offset.vz = 1000;
            Model_SetCameraOrbit(0, 3000, 0x200, -0x100, &camera_offset);
            gModel_bPresentationPhase++;
        }
        break;
    case 9:
        if (primary_slot->animation_position >= 0x1E0) {
            camera_offset.vy = -2000;
            Model_SetCameraOrbit(0, 3500, 0x40, 0x200, &camera_offset);
            gModel_bPresentationPhase++;
        }
        break;
    case 10:
        if (primary_slot->current_animation_row == 2) {
            gModel_bPresentationPhase++;
        }
        break;
    case 11:
        if (primary_slot->animation_position >= 0x8C0) {
            camera_offset.vy = -1500;
            Model_QueueRelativeCameraOrbit(0, 3500, 0, -0x80, &camera_offset, 40);
            gModel_bPresentationPhase++;
        }
        break;
    case 12:
        if (primary_slot->animation_position >= 0xE60) {
            camera_offset.vz = -1000;
            Model_SetCameraOrbit(0, 5000, 0x200, -0x100, &camera_offset);
            Model_SeekSlotAnimation(0, 0, 190);
            gModel_bPresentationPhase++;
        }
        break;
    case 13:
        if (primary_slot->animation_position >= 0xE60) {
            camera_offset.vz = -1000;
            Model_SetCameraOrbit(0, 5000, -0x200, -0x100, &camera_offset);
            Model_SeekSlotAnimation(0, 0, 190);
            gModel_bPresentationPhase++;
        }
        break;
    case 14:
        if (primary_slot->animation_position >= 0xE60) {
            camera_offset.vy = -1000;
            Model_SetCameraOrbit(0, 3000, 0, -0x80, &camera_offset);
            Model_SeekSlotAnimation(0, 0, 190);
            gModel_bPresentationPhase++;
        }
        break;
    case 15: {
        s32 stop_position = primary_slot->animation_duration_rows[primary_slot->current_animation_row].max << 4;
        s32 frame_step = Model_GetFrameStep();
        stop_position -= primary_slot->playback_speed * frame_step;
        if (primary_slot->animation_position >= stop_position) {
            Model_SetSlotPlaybackDirection(0, 0);
        }
        break;
    }
    case 16:
        gModel_bPresentationPhase = -2;
        break;
    }
}

void Model_UpdateCreditsMonsterSlot(s32 slot_index) {
    ModelSlot *initial_slot;
    ModelSlot *ready_base;
    ModelSlot *initial_base;
    ModelSlot *peer_slot;
    GsCOORDUNIT *peer_coordinate;
    ModelSlot *ready_slot;
    s32 model_id;
    s32 t;
    s32 yaw_work;
    s32 m1;

    initial_base = gModel_aSlots;
    initial_slot = initial_base + slot_index;
    if (initial_slot->is_active == 0) {
        if (initial_slot->load_step == 0xFF) {
            if (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                 gFile_dwCancelRequestState) == 0) {
                do {
                    t = rand() >> 8;
                    model_id = t % CARD_COUNT;
                } while (model_id < 0 || model_id >= CARD_COUNT ||
                         (model_id >= MODEL_MRG_FIRST_GAP_START
                          && model_id < MODEL_MRG_FIRST_GAP_END) ||
                         (model_id >= MODEL_MRG_SECOND_GAP_START
                          && model_id < MODEL_MRG_SECOND_GAP_END) ||
                         model_id == MODEL_MRG_SINGLE_GAP_ID);
                m1 = -1;
                Model_LoadMonsterMerge(slot_index | 0x80, model_id, m1, m1, m1, m1, 0);
            }
        } else {
            Model_AdvanceSlotLoad(slot_index);
        }

        ready_base = gModel_aSlots;
        if ((ready_base + slot_index)->is_active != 0) {
            peer_slot = ready_base + (slot_index ^ 1);
            yaw_work = MODEL_ANGLE_QUARTER_TURN;
            if (peer_slot->is_active != 0) {
                peer_coordinate = peer_slot->root_coordinate;
                if (peer_coordinate != (GsCOORDUNIT *)0) {
                    t = peer_coordinate->rot.vy + MODEL_ANGLE_QUARTER_TURN;
                    yaw_work = t / MODEL_ANGLE_FULL_TURN;
                    yaw_work = t - yaw_work * MODEL_ANGLE_FULL_TURN;
                }
            }
            ready_slot = gModel_aSlots + slot_index;
            if (ready_slot->root_coordinate != (GsCOORDUNIT *)0) {
                ready_slot->root_coordinate->rot.vx = 0;
                ready_slot->root_coordinate->rot.vy = yaw_work;
                ready_slot->root_coordinate->rot.vz = 0;
                ready_slot->root_coordinate->matrix.t[0] = 0;
                ready_slot->root_coordinate->matrix.t[1] = 0;
                ready_slot->root_coordinate->matrix.t[2] = 0;
            }
            Model_UpdateCoordinateUnitMatrix(ready_slot->root_coordinate, 0);
            Model_RunSlotHandlers(slot_index);
            ready_slot->field_E15 = 0;
        }
    }
}

#define MODEL_SLOT_VIEW(slot) ((ModelSlot *)(slot))
#define MODEL_SLOT_RAW_BYTES(slot) ((u8 *)(slot))

#ifndef MODEL_INTRO_PACKAGE_START_SECTOR
#define MODEL_INTRO_PACKAGE_START_SECTOR 1223
#endif

void Model_UpdateCreditsPresentation(void)
{
  s8 phase_value;
  ModelSlot *load_check_base;
  ModelSlot *crossfade_base;
  ModelSlot *active_lookup_base;
  ModelSlot *fade_slot_base;
  u8 *slot0_tint;
  u8 *slot1_tint;
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
  s32 *active_slot_words;
  u32 slot_tick_count;
  s32 animation_row;
  s8 next_phase;
  s32 peer_slot_index;
  s32 crossfade_offset;
  u8 *outgoing_color;
  u8 *incoming_color;
  u32 peer_red;
  u32 peer_green;
  u32 peer_blue;
  s32 result;

  Model_SetFrameStepOverride(1);
  switch ( gModel_bPresentationPhase )
  {
    case -1:
      File_RequestAsyncTransfer(
          1, gModel_szSuMrgPath, MODEL_INTRO_PACKAGE_START_SECTOR,
          16, 0, 0, gModel_CreditsModuleAddress);
      File_WaitForTransfers();
      D_8009B004.fields.credits_exit_requested = 0;
      D_8009B004.fields.credits_update_enabled = 0;
      gModel_bCreditsActiveSlot = 0;
      gModel_aSlots[0].is_active = 0;
      gModel_aSlots[1].is_active = 0;
      Model_SetSlotTintTarget(0, 2, 0, 0, 0);
      Model_SetSlotTintTarget(1, 2, 0, 0, 0);
      func_801807B0();
      func_80181C4C(0);
      SD_SetMusicTrackBuffer((u16 *)gModel_pCreditsMusicTrackBuffer);
      SD_BGMPlayDefaultStep(29488);
      phase_value = gModel_bPresentationPhase + 1;
      gModel_bPresentationPhase = phase_value;
      break;
    case 0:
      load_check_base = gModel_aSlots;
      active_offset = (u32)&((ModelSlot *)0)[(u8)gModel_bCreditsActiveSlot];
      if ( MODEL_SLOT_VIEW(MODEL_SLOT_RAW_BYTES(load_check_base) + active_offset)->is_active )
      {
        if ( (MODEL_SLOT_VIEW(MODEL_SLOT_RAW_BYTES(gModel_aSlots) + active_offset)->field_E15 & 3) == 0 )
        {
          active_color = MODEL_SLOT_VIEW(
              MODEL_SLOT_RAW_BYTES(gModel_aSlots) + active_offset)->tint;
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
        if ( !D_8009B004.fields.credits_exit_requested )
          Model_UpdateCreditsMonsterSlot((u8)gModel_bCreditsActiveSlot ^ 1);
        active_lookup_base = gModel_aSlots;
        active_slot_words = (s32 *)&active_lookup_base[(u8)gModel_bCreditsActiveSlot];
        if ( !MODEL_SLOT_VIEW(active_slot_words)->active_animation_command_row )
        {
          slot_tick_count = MODEL_SLOT_VIEW(active_slot_words)->field_E15;
          animation_row = -1;
          if ( !(u8)(slot_tick_count % 0x1E) )
          {
            switch ( (u8)(slot_tick_count / 0x1E) )
            {
              case 1u:
                animation_row = 2;
                break;
              case 3u:
                animation_row = 7;
                break;
              case 0u:
              case 2u:
              case 4u:
                animation_row = 1;
                break;
              default:
                break;
            }
            if ( animation_row < 0 )
            {
              if ( D_8009B004.fields.credits_exit_requested )
              {
                next_phase = 2;
                goto set_phase_and_tick;
              }
              peer_slot_index = (u8)gModel_bCreditsActiveSlot ^ 1;
              if ( MODEL_SLOT_VIEW(
                       MODEL_SLOT_RAW_BYTES(gModel_aSlots) +
                       (904 * peer_slot_index) * 4)->is_active )
              {
                Model_RequestCameraMoveToSlots(1, -1, peer_slot_index, 90);
                next_phase = gModel_bPresentationPhase + 1;
                goto set_phase_and_tick;
              }
            }
            else
            {
              Model_ControlSlotAnimation((u8)gModel_bCreditsActiveSlot, animation_row, 1);
            }
          }
          ++MODEL_SLOT_VIEW(MODEL_SLOT_RAW_BYTES(gModel_aSlots) + (904 * (u8)gModel_bCreditsActiveSlot) * 4)->field_E15;
          break;
        }
      }
      else
      {
        Model_UpdateCreditsMonsterSlot((u8)gModel_bCreditsActiveSlot);
        if ( MODEL_SLOT_VIEW(
                 MODEL_SLOT_RAW_BYTES(gModel_aSlots) +
                 (904 * (u8)gModel_bCreditsActiveSlot) * 4)->is_active )
          Model_RequestCameraMoveToSlots(1, -1, (u8)gModel_bCreditsActiveSlot, 30);
      }
      break;
    case 1:
      crossfade_base = gModel_aSlots;
      crossfade_offset = 904 * (u8)gModel_bCreditsActiveSlot;
      if ( (MODEL_SLOT_VIEW(
                MODEL_SLOT_RAW_BYTES(crossfade_base) +
                (crossfade_offset) * 4)->field_E15 & 3) == 0 )
      {
        outgoing_color = MODEL_SLOT_VIEW(
            MODEL_SLOT_RAW_BYTES(gModel_aSlots) +
            (crossfade_offset) * 4)->tint;
        incoming_color = MODEL_SLOT_VIEW(
            MODEL_SLOT_RAW_BYTES(gModel_aSlots) +
            (904 * ((u8)gModel_bCreditsActiveSlot ^ 1)) * 4)->tint;
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
          MODEL_SLOT_VIEW(
              MODEL_SLOT_RAW_BYTES(crossfade_base) +
              (904 * (u8)gModel_bCreditsActiveSlot) * 4)->is_active = 0;
          gModel_bPresentationPhase = 0;
          gModel_bCreditsActiveSlot ^= 1u;
        }
      }
      ++MODEL_SLOT_VIEW(MODEL_SLOT_RAW_BYTES(gModel_aSlots) + (904 * (u8)gModel_bCreditsActiveSlot) * 4)->field_E15;
      break;
    case 2:
      fade_slot_base = gModel_aSlots;
      if ( (MODEL_SLOT_VIEW(
                MODEL_SLOT_RAW_BYTES(fade_slot_base) +
                (904 * (u8)gModel_bCreditsActiveSlot) * 4)->field_E15 & 3) == 0 )
      {
        slot0_tint = fade_slot_base[0].tint;
        slot1_tint = fade_slot_base[1].tint;
        if ( fade_slot_base[0].tint[0] )
          --fade_slot_base[0].tint[0];
        if ( slot0_tint[1] )
          --slot0_tint[1];
        if ( slot0_tint[2] )
          --slot0_tint[2];
        if ( fade_slot_base[1].tint[0] )
          fade_slot_base[1].tint[0] = fade_slot_base[1].tint[0] - 1;
        if ( slot1_tint[1] )
          --slot1_tint[1];
        if ( slot1_tint[2] )
          --slot1_tint[2];
        if ( fade_slot_base[0].tint[0]
          || slot0_tint[1] || slot0_tint[2]
          || fade_slot_base[1].tint[0]
          || slot1_tint[1] || slot1_tint[2] )
        {
          ++MODEL_SLOT_VIEW(MODEL_SLOT_RAW_BYTES(gModel_aSlots) + (904 * (u8)gModel_bCreditsActiveSlot) * 4)->field_E15;
          break;
        }
        fade_slot_base[1].is_active = 0;
        fade_slot_base[0].is_active = 0;
        next_phase = gModel_bPresentationPhase + 1;
set_phase_and_tick:
        gModel_bPresentationPhase = next_phase;
      }
      ++MODEL_SLOT_VIEW(MODEL_SLOT_RAW_BYTES(gModel_aSlots) + (904 * (u8)gModel_bCreditsActiveSlot) * 4)->field_E15;
      break;
    case 3:
      SD_ResetMusicTrackBuffer();
      SD_ResetPendingEntries();
      phase_value = -2;
      gModel_bPresentationPhase = phase_value;
      break;
    default:
      break;
  }
      if ( D_8009B004.fields.credits_update_enabled )
      {
        result = func_80180A24();
        if ( result )
        {
          result = D_8009B004.fields.credits_exit_requested;
          if ( !D_8009B004.fields.credits_exit_requested )
          {
            result = (s16)SD_GetStatusFlags();
            if ( (s16)result != 128 )
            {
              ++D_8009B004.fields.credits_exit_requested;
              gModel_bPresentationPhase = 2;
            }
          }
        }
      }
      else
      {
        if (SD_GetStatusFlags() & 0x80)
        {
          result = (u8)gModel_aSlots[0].is_active;
          if ( gModel_aSlots[0].is_active )
          {
            result = D_8009B004.fields.credits_update_enabled + 1;
            ++D_8009B004.fields.credits_update_enabled;
          }
        }
      }
      return;
}

void Model_StartBattleSlotAction(s32 index)
{
    ModelSlot *slot;
    ModelSlotMetadataBlock *metadata;
    s32 action_row;
    s32 camera_duration;

    slot = &gModel_aSlots[index];
    metadata = &gModel_aSlots[index].metadata;
    action_row = slot->guardian_star_selection + 3;
    camera_duration = slot->animation_duration_rows[action_row].max;
    if (camera_duration != 0) {
        Model_ControlSlotAnimation(index, action_row, 1);
    }
    camera_duration = camera_duration * 2 / 3;
    if (camera_duration < 60) {
        camera_duration = 60;
    }
    if (metadata->battle_camera_duration_overrides[slot->guardian_star_selection] != 0) {
        camera_duration = metadata->battle_camera_duration_overrides[slot->guardian_star_selection];
    }
    Model_RequestCameraMoveToSlots(1, -1, index ^ 1, camera_duration);
    Model_SetCameraHoldState(
        ((metadata->prefix.bytes.guardian_star_options[slot->guardian_star_selection] & 0x80) == 0) << 1
    );
    if ((metadata->prefix.bytes.guardian_star_options[slot->guardian_star_selection] & 0x40) != 0) {
        Model_SetCameraHoldState(1);
    }
}

/* "e%03ld(%ld)=%d\n" */
void Model_PlaySlotAnimationSounds(s32 index) {
#ifndef VERSION_JAPAN
    /* The Japanese build has no debug print, and no buffer for it. */
    u8 trace_text[0x50];
#endif
    u8 spatial_bytes[8];
    ModelSlot *slot;
    ModelSlotSoundEntry *cue;
    s32 animation_row;
    s32 row_end_position;
    s32 position_step;
    s32 sound_position_step;
    s32 sound_id;
    s32 cue_position;
    s32 slot_sound_tag;
    s32 use_command_cursor;
    s32 cue_index;
    s32 selected_command;
    s32 guardian_star;
    s32 sound_command;
    u16 cd_seek_flag;

    slot = &gModel_aSlots[index];
    if (slot->is_active == 0) {
        return;
    }
    if (slot->tint[3] >= 2) {
        if ((*(u32 *)slot->tint & 0xFFFFFF) == 0) {
            return;
        }
    }
    if (slot->animation_sound_enabled == 0) {
        return;
    }
    animation_row = slot->current_animation_row;
    row_end_position = slot->animation_duration_rows[animation_row].max << 4;
    position_step = slot->playback_speed * Model_GetFrameStep();
    cue = slot->sound_entries;
    slot_sound_tag = (index << 8) + 0x4000;
    if (row_end_position != 0) {
        cue_position = slot->animation_position % row_end_position;
    } else {
        cue_position = 0;
    }
    guardian_star = slot->guardian_star_selection;
    selected_command = ((ModelControlCommandView *)slot)->commands[guardian_star];
    use_command_cursor = 0;
    if (selected_command >= 0) {
        if (slot->animation_control_state != 8) {
            if (slot->animation_control_state != 2 || guardian_star + 3 == animation_row) {
                use_command_cursor = 1;
            }
        }
        if (use_command_cursor) {
#ifdef VERSION_JAPAN
            sound_position_step = slot->playback_speed * Model_GetFrameStep();
            cue_position = slot->animation_sound_position;
            slot->animation_sound_position = cue_position + sound_position_step;
            animation_row = slot->guardian_star_selection + 3;
#else
            s32 trace_star;
            s32 trace_command;

            sound_position_step = slot->playback_speed * Model_GetFrameStep();
            trace_star = slot->guardian_star_selection;
            cue_position = slot->animation_sound_position;
            slot->animation_sound_position = cue_position + sound_position_step;
            trace_command = ((ModelControlCommandView *)slot)->commands[trace_star];
            animation_row = slot->guardian_star_selection + 3;
            /* Retail loads animation_sound_position again after the store above; read
               plainly, GCC reuses the value it just stored instead. */
            sprintf(trace_text, gModel_szAnimationSoundTraceFormat, trace_command / 1000, trace_command % 1000, *(volatile u16 *)&slot->animation_sound_position >> 4);
            FntPrint(trace_text);
#endif
        }
    }
    if (use_command_cursor == 0) {
        if (slot->playback_state == 0x23) {
            return;
        }
    }
    SD_CalcSpatialVolumePan(
        slot->cached_position[0], slot->cached_position[2], &spatial_bytes[0], &spatial_bytes[1]
    );
    cue_index = 0;
loop:
    cd_seek_flag = cue->flags & 0x8000;
    sound_id = cue->sound_id;
    /* A one-pass block around the loop body; it decides which of cue_position
     * and the counter get $s1 and $s3. */
    do {
        if (cd_seek_flag) {
            sound_command = cd_seek_flag | sound_id;
        } else {
            sound_command = (u16)slot_sound_tag | sound_id;
        }
        if (cue->animation_row == 0) {
            return;
        }
        if (cue->animation_row == animation_row) {
            if (cd_seek_flag != 0 && cue_position == 0) {
                SD_EnqueueCdSeek(sound_command);
            }
            if ((cue->flags & 0x7FFF) >= cue_position && (cue->flags & 0x7FFF) < cue_position + position_step) {
                SD_SEPlay(sound_command, 0xFF, 0);
            }
        }
        cue_index++;
        cue++;
    } while (0);
    if (cue_index < 0x40) {
        goto loop;
    }
}

#define MODEL_TINT_COLOR_VIEW(color) ((ModelTintColor *)(color))
#define SVECTOR_VIEW(vector) ((SVECTOR *)(vector))
#define MODEL_SLOT_BYTES(slots, index) \
    ((u8 *)(slots) + (index) * MODEL_SLOT_SIZE)

/* The contiguous model camera and scene runtime. Model_SeparateViewFromSlots pushes the
   gModel_RView reference apart from the two model records; Model_UpdateCamera
   steps the camera move at gModel_CameraMove each frame and calls the second
   camera/view correction pass, Model_CorrectViewDistance, which in turn calls
   Model_SeparateViewFromSlots. The two correction passes work on gModel_RView, gModel_CameraMove
   and the gModel_wViewDistance/gModel_wViewPitch view values; the tint
   request pass Model_UpdateTintRequests follows them; camera-move setup
   (Model_StartCameraMove) and scene reset/configuration close the unit. */

/*
 * Recursively separates the two model records when their projected distance
 * is below the largest paired half-extent, pushing gModel_RView apart and
 * retrying up to depth three. Matching C under gcc_2_8_1_g8_split with no
 * register pins, inline assembly or symbol aliases.
 *
 * Two behavioural points read off the retail listing:
 *  - `separation_extent = separation_limit` is set after the main body as well as on the apply_request == 0
 *    path (retail's `j next` carries `move s1,s5` in its slot), so the loop
 *    is `if (separation_extent) { extra_overlap_count++; continue; } if (apply_request != 0) { body } separation_extent =
 *    separation_limit;`;
 *  - the push is scaled by the extents, `push_x = e0.values[slot_index] * penetration_scale / 4096`
 *    and `push_z = e2.values[slot_index] * penetration_scale / 4096`; dx[slot_index] and dz[slot_index] decide only the
 *    sign.
 *
 * Shape that the compiler needs, in the order it was found:
 *  - each clamp pair is written into the struct declared after its
 *    destination and copied down (`e3 = e0`, `e0 = e1`, ...), with a
 *    do { } while (0) around the store before each copy: the loop notes are
 *    sched1 barriers that keep retail's global loads below the pair copies;
 *  - the dead read `view_work = gModel_RView.vpx` above the last pin, and the three
 *    reference names probe_x/view_work/probe_z, leave the `lui` of gModel_RView as the join
 *    block's first insn so reorg can hoist it into the beqz slot;
 *  - the `extra_overlap_count++` arm sits out of line after the loop bottom;
 *  - `side_distance = ... / normal_length` sits in its own do { } while (0), `pushed_eye_x`/`pushed_eye_z` are
 *    named before the length call, and `line_constant` after them;
 *  - the loop-top reads of dy.values[slot_index] and e1.values[slot_index] go through two
 *    pointers that start at the frame base and are stepped twice, and the
 *    separation_limit comparison is stored back into height_gate_work; see the comments in the loop.
 *
 * See notes/research/func-80051350-decode.md for the structural map.
 */
s32 Model_SeparateViewFromSlots(s32 apply_request, s32 min_extent, s32 recursion_depth)
{
    ModelSeparationPair e3;
    ModelSeparationPair e0;
    ModelSeparationPair e1;
    ModelSeparationPair e2;
    ModelSeparationPair dx;
    ModelSeparationPair dy;
    ModelSeparationPair dz;
    ModelSeparationPair dist;
    ModelSeparationPair t;
    s32 separation_extent;
    s32 extra_overlap_count;
    s32 slot_index;
    s32 probe_offset_x;
    s32 probe_offset_z;
    s32 probe_x;
    s32 view_work;
    s32 probe_z;
    s32 slot1_delta_y;
    s32 extent_work;
    s32 slot1_extent;
    ModelSlot *slots;

    probe_offset_x = rcos(*(s16 *)&gModel_wViewYaw + 0x800) * min_extent / 4096;
    probe_offset_z = rsin(*(s16 *)&gModel_wViewYaw + 0x800) * min_extent / 4096;

    slots = gModel_aSlots;
    extent_work = (s16)slots[0].cached_dimensions[3] / 2;
    if (extent_work < min_extent) {
        extent_work = min_extent;
    }
    e0.values[0] = extent_work;
    extent_work = (s16)slots[1].cached_dimensions[3] / 2;
    if (extent_work < min_extent) {
        extent_work = min_extent;
    }
    do {
        e0.values[1] = extent_work;
    } while (0);
    e3 = e0;
    extent_work = (s16)slots[0].cached_dimensions[0] / 2;
    if (extent_work < min_extent) {
        extent_work = min_extent;
    }
    e1.values[0] = extent_work;
    extent_work = (s16)slots[1].cached_dimensions[0] / 2;
    if (extent_work < min_extent) {
        extent_work = min_extent;
    }
    do {
        e1.values[1] = extent_work;
    } while (0);
    e0 = e1;
    extent_work = (s16)slots[0].cached_dimensions[1] / 2;
    if (extent_work < min_extent) {
        extent_work = min_extent;
    }
    e2.values[0] = extent_work;
    extent_work = (s16)slots[1].cached_dimensions[1] / 2;
    if (extent_work < min_extent) {
        extent_work = min_extent;
    }
    do {
        e2.values[1] = extent_work;
    } while (0);
    e1 = e2;
    extent_work = (s16)slots[0].cached_dimensions[2] / 2;
    if (extent_work < min_extent) {
        extent_work = min_extent;
    }
    dx.values[0] = extent_work;
    slot1_extent = (s16)slots[1].cached_dimensions[2] / 2;
    if (slot1_extent < min_extent) {
        slot1_extent = min_extent;
    }
    view_work = gModel_RView.vpx;
    do {
        dx.values[1] = slot1_extent;
    } while (0);
    e2 = dx;

    probe_x = gModel_RView.vpx;
    probe_x = probe_x + probe_offset_x;
    dy.values[0] = probe_x - *(s16 *)&slots[0].cached_position[0];
    dy.values[1] = probe_x - *(s16 *)&slots[1].cached_position[0];
    dx = dy;
    view_work = gModel_RView.vpy;
    dz.values[0] = view_work - *(s16 *)&slots[0].cached_position[1];
    slot1_delta_y = view_work - *(s16 *)&slots[1].cached_position[1];
    do {
        dz.values[1] = slot1_delta_y;
    } while (0);
    dy = dz;
    probe_z = gModel_RView.vpz + probe_offset_z;
    dist.values[0] = probe_z - *(s16 *)&slots[0].cached_position[2];
    dist.values[1] = probe_z - *(s16 *)&slots[1].cached_position[2];
    dz = dist;

    t.values[0] = SquareRoot0(
        dx.values[0] * dx.values[0] + dz.values[0] * dz.values[0]);
    t.values[1] = SquareRoot0(
        dx.values[1] * dx.values[1] + dz.values[1] * dz.values[1]);
    dist = t;

    separation_extent = 0;
    extra_overlap_count = 0;
    if (gModel_aSlots[0].is_active != 0) {
        if (gModel_aSlots[0].tint[3] >= 2) {
            if ((*(u32 *)gModel_aSlots[0].tint & 0xFFFFFF) == 0) {
                dist.values[0] = -1;
            }
        }
    } else {
        dist.values[0] = -1;
    }
    if (gModel_aSlots[1].is_active != 0) {
        if (gModel_aSlots[1].tint[3] >= 2) {
            if ((*(u32 *)gModel_aSlots[1].tint & 0xFFFFFF) == 0) {
                dist.values[1] = -1;
            }
        }
    } else {
        dist.values[1] = -1;
    }

    recursion_depth = recursion_depth + 1;
    for (slot_index = 0; slot_index < 2; slot_index++) {
        s32 separation_limit;
        s32 height_gate_work;
        s32 radial_distance;
        s32 vertical_work;
        s32 *vertical_pointer;
        s32 *height_pointer;

        separation_limit = e2.values[slot_index];
        if (separation_limit < e0.values[slot_index]) {
            separation_limit = e0.values[slot_index];
        }
        if (separation_limit < e3.values[slot_index]) {
            separation_limit = e3.values[slot_index];
        }
        /* dy.values[slot_index] and e1.values[slot_index] through two pointers that start at
         * the frame base (e3 is the first pair, at sp+16) and step by slot_index and
         * then by one. The final step folds into the load offsets, but it
         * leaves each pointer with two sets, so neither address insn gets
         * sched1's launch priority and both are computed before the first
         * load; reload_cse then turns the second into retail's copy. */
        vertical_pointer = e3.values - 4;
        vertical_pointer += slot_index;
        vertical_pointer++;
        height_pointer = e3.values - 4;
        height_pointer += slot_index;
        height_pointer++;
        vertical_work = vertical_pointer[13];
        height_gate_work = height_pointer[7];
        if (vertical_work < 0) {
            vertical_work = -vertical_work;
        }
        /* The comparison lands in height_gate_work itself: a block-local flag would be
         * placed first by local-alloc and take v0, pushing vertical_work to v1. */
        height_gate_work = height_gate_work < vertical_work;
        if (height_gate_work) {
            continue;
        }
        radial_distance = dist.values[slot_index];
        if (radial_distance < 0) {
            continue;
        }
        if (radial_distance >= separation_limit) {
            continue;
        }
        if (separation_extent == 0) {
            if (apply_request != 0) {
                s32 penetration_scale;
                s32 push_x;
                s32 push_z;

                penetration_scale = ((separation_limit - radial_distance) << 12) / separation_limit;
                push_x = e0.values[slot_index] * penetration_scale / 4096;
                if (dx.values[slot_index] <= 0) {
                    push_x = -push_x;
                }
                push_z = e2.values[slot_index] * penetration_scale / 4096;
                if (dz.values[slot_index] <= 0) {
                    push_z = -push_z;
                }
                {
                    s32 target_x = gModel_RView.vrx;
                    s32 eye_z = gModel_RView.vpz;
                    s32 target_z = gModel_RView.vrz;
                    s32 eye_x = gModel_RView.vpx;
                    s32 normal_x = target_z - eye_z;
                    s32 normal_z = eye_x - target_x;
                    s32 pushed_eye_x = eye_x + push_x;
                    s32 pushed_eye_z = eye_z + push_z;
                    s32 line_constant = target_x * eye_z - eye_x * target_z;
                    s32 normal_length = SquareRoot0(normal_x * normal_x + normal_z * normal_z);
                    s32 side_distance = 0;

                    if (normal_length != 0) {
                        do {
                            side_distance = (pushed_eye_x * normal_x + pushed_eye_z * normal_z + line_constant) / normal_length;
                        } while (0);
                    }
                    if (gModel_bOrbitDirectionTimer == 0) {
                        if (side_distance < 0) {
                            gModel_bOrbitDirection = -1;
                        } else {
                            gModel_bOrbitDirection = 1;
                        }
                        gModel_bOrbitDirectionTimer = 0x1E;
                    } else if (gModel_bOrbitDirectionTimer < 0xFF) {
                        gModel_bOrbitDirectionTimer = gModel_bOrbitDirectionTimer - 1;
                    }
                }
                gModel_RView.vpx = gModel_RView.vpx + push_x;
                gModel_RView.vpz = gModel_RView.vpz + push_z;
            }
            separation_extent = separation_limit;
            continue;
        }
        extra_overlap_count = extra_overlap_count + 1;
    }

    if (apply_request != 0 && separation_extent != 0) {
        Model_UpdateViewMetrics(0);
    }
    if (extra_overlap_count != 0 && apply_request != 0 && recursion_depth < 3) {
        Model_SeparateViewFromSlots(apply_request, min_extent, recursion_depth);
    }
    return separation_extent;
}

void Model_UpdateCamera(void)
{
    s32 eye_slots[2] = {gModel_CameraMove.eye.pair_slot, gModel_CameraMove.eye.slot};
    s32 target_slots[2] = {gModel_CameraMove.target.pair_slot, gModel_CameraMove.target.slot};
    s32 rate;
    s32 progress;
    Key *key;

    if (Model_IsEffectKeyQueueEmpty() && gModel_CameraMove.flags) {
        if (gModel_CameraMove.duration <= 0xFFFE) {
            s32 next_elapsed;

            rate = Model_GetFrameStep();
            if (gModel_CameraMove.elapsed < (u16)(gModel_CameraMove.duration / 5u) ||
                gModel_CameraMove.elapsed > 4 * gModel_CameraMove.duration / 5) {
                rate /= 2;
                if (rate < 2)
                    rate = 2;
            }
            next_elapsed = gModel_CameraMove.elapsed + rate;
            gModel_CameraMove.elapsed = next_elapsed < gModel_CameraMove.duration ?
                next_elapsed : gModel_CameraMove.duration;
        }
        if (eye_slots[0] >= 0) {
            ModelSlot *base = gModel_aSlots;
            ModelSlot *slot;
            s32 dx;
            s32 dy;

            slot = base + eye_slots[0];
            dx = (s16)slot->cached_position[0] - gModel_CameraMove.eye.start_x;
            dy = (s16)slot->cached_position[1] - gModel_CameraMove.eye.start_y;
            gModel_CameraMove.eye.start_x = gModel_CameraMove.eye.start_x +
                (dx > 0 ? (dx < 31 ? dx : 30) :
                (dx >= -30 ? dx : -30));
            gModel_CameraMove.eye.start_y = gModel_CameraMove.eye.start_y +
                (dy > 0 ? (dy < 31 ? dy : 30) :
                (dy >= -30 ? dy : -30));
        }
        if (eye_slots[1] >= 0) {
            ModelSlot *base = gModel_aSlots;
            ModelSlot *slot;
            s32 dx;
            s32 dy;
            s32 dz;

            slot = base + eye_slots[1];
            dx = (s16)slot->cached_position[0] - gModel_CameraMove.eye.end_x;
            dy = (s16)slot->cached_position[1] - gModel_CameraMove.eye.end_y;
            dz = (s16)slot->cached_position[2] - gModel_CameraMove.eye.end_z;
            gModel_CameraMove.eye.end_x = gModel_CameraMove.eye.end_x +
                (dx > 0 ? (dx < 31 ? dx : 30) :
                (dx >= -30 ? dx : -30));
            gModel_CameraMove.eye.end_y = gModel_CameraMove.eye.end_y +
                (dy > 0 ? (dy < 31 ? dy : 30) :
                (dy >= -30 ? dy : -30));
            gModel_CameraMove.eye.end_z = gModel_CameraMove.eye.end_z +
                (dz > 0 ? (dz < 31 ? dz : 30) :
                (dz >= -30 ? dz : -30));
        }
        if (target_slots[0] >= 0) {
            ModelSlot *base = gModel_aSlots;
            ModelSlot *slot;
            s32 dx;
            s32 dy;

            slot = base + target_slots[0];
            dx = (s16)slot->cached_position[0] - gModel_CameraMove.target.start_x;
            dy = (s16)slot->cached_position[1] - gModel_CameraMove.target.start_y;
            gModel_CameraMove.target.start_x = gModel_CameraMove.target.start_x +
                (dx > 0 ? (dx < 31 ? dx : 30) :
                (dx >= -30 ? dx : -30));
            gModel_CameraMove.target.start_y = gModel_CameraMove.target.start_y +
                (dy > 0 ? (dy < 31 ? dy : 30) :
                (dy >= -30 ? dy : -30));
        }
        if (target_slots[1] >= 0) {
            ModelSlot *base = gModel_aSlots;
            ModelSlot *slot;
            s32 dx;
            s32 dy;
            s32 dz;

            slot = base + target_slots[1];
            dx = (s16)slot->cached_position[0] - gModel_CameraMove.target.end_x;
            dy = (s16)slot->cached_position[1] - gModel_CameraMove.target.end_y;
            dz = (s16)slot->cached_position[2] - gModel_CameraMove.target.end_z;
            gModel_CameraMove.target.end_x = gModel_CameraMove.target.end_x +
                (dx > 0 ? (dx < 31 ? dx : 30) :
                (dx >= -30 ? dx : -30));
            gModel_CameraMove.target.end_y = gModel_CameraMove.target.end_y +
                (dy > 0 ? (dy < 31 ? dy : 30) :
                (dy >= -30 ? dy : -30));
            gModel_CameraMove.target.end_z = gModel_CameraMove.target.end_z +
                (dz > 0 ? (dz < 31 ? dz : 30) :
                (dz >= -30 ? dz : -30));
        }
        if (gModel_CameraMove.flags & 1) {
            gModel_RView.vpx = (gModel_CameraMove.eye.end_x - gModel_CameraMove.eye.start_x) *
                gModel_CameraMove.elapsed / gModel_CameraMove.duration + gModel_CameraMove.eye.start_x;
            gModel_RView.vpy = (gModel_CameraMove.eye.end_y - gModel_CameraMove.eye.start_y) *
                gModel_CameraMove.elapsed / gModel_CameraMove.duration + gModel_CameraMove.eye.start_y;
            gModel_RView.vpz = (gModel_CameraMove.eye.end_z - gModel_CameraMove.eye.start_z) *
                gModel_CameraMove.elapsed / gModel_CameraMove.duration + gModel_CameraMove.eye.start_z;
            if (gModel_RView.vpy >= -100)
                gModel_RView.vpy = -100;
        } else if (gModel_CameraMove.mode) {
            rate = Model_GetFrameStep();
            Model_StepViewOrbit(1, gModel_CameraMove.orbit_step * rate * gModel_bOrbitDirection, 0, 0, 0);
        }
        if (gModel_CameraMove.flags & 2) {
            gModel_RView.vrx = (gModel_CameraMove.target.end_x - gModel_CameraMove.target.start_x) *
                gModel_CameraMove.elapsed / gModel_CameraMove.duration + gModel_CameraMove.target.start_x;
            gModel_RView.vry = (gModel_CameraMove.target.end_y - gModel_CameraMove.target.start_y) *
                gModel_CameraMove.elapsed / gModel_CameraMove.duration + gModel_CameraMove.target.start_y;
            gModel_RView.vrz = (gModel_CameraMove.target.end_z - gModel_CameraMove.target.start_z) *
                gModel_CameraMove.elapsed / gModel_CameraMove.duration + gModel_CameraMove.target.start_z;
            if (gModel_RView.vry >= -100)
                gModel_RView.vry = -100;
        }
        Model_UpdateViewMetrics(0);
        Model_CorrectViewDistance(1);
        if (gModel_CameraMove.elapsed >= gModel_CameraMove.duration)
            Model_RequestCameraMove(1);
        return;
    }
    if (!gModel_CameraMove.mode)
        return;
    key = (Key *)Model_GetCurrentEffectKeyAddress();
    if (eye_slots[1] >= 0) {
        ModelSlot *base = gModel_aSlots;
        ModelSlot *slot;
        s32 dx;
        s32 dy;
        s32 dz;

        slot = base + eye_slots[1];
        dx = (s16)slot->cached_position[0] - gModel_RView.vpx;
        dy = (s16)slot->cached_position[1] - gModel_RView.vpy;
        dz = (s16)slot->cached_position[2] - gModel_RView.vpz;
        gModel_RView.vpx = gModel_RView.vpx +
            (dx > 0 ? (dx < 31 ? dx : 30) :
            (dx >= -30 ? dx : -30));
        gModel_RView.vpy = gModel_RView.vpy +
            (dy > 0 ? (dy < 31 ? dy : 30) :
            (dy >= -30 ? dy : -30));
        gModel_RView.vpz = gModel_RView.vpz +
            (dz > 0 ? (dz < 31 ? dz : 30) :
            (dz >= -30 ? dz : -30));
        if (gModel_RView.vpy >= -100)
            gModel_RView.vpy = -100;
    }
    if (target_slots[1] >= 0) {
        ModelSlot *base = gModel_aSlots;
        ModelSlot *slot;
        s32 dx;
        s32 dy;
        s32 dz;

        slot = base + target_slots[1];
        dx = (s16)slot->cached_position[0] - gModel_RView.vrx;
        dy = (s16)slot->cached_position[1] - gModel_RView.vry;
        dz = (s16)slot->cached_position[2] - gModel_RView.vrz;
        gModel_RView.vrx = gModel_RView.vrx +
            (dx > 0 ? (dx < 31 ? dx : 30) :
            (dx >= -30 ? dx : -30));
        gModel_RView.vry = gModel_RView.vry +
            (dy > 0 ? (dy < 31 ? dy : 30) :
            (dy >= -30 ? dy : -30));
        gModel_RView.vrz = gModel_RView.vrz +
            (dz > 0 ? (dz < 31 ? dz : 30) :
            (dz >= -30 ? dz : -30));
        if (gModel_RView.vry >= -100)
            gModel_RView.vry = -100;
    }
    if (!key || (key->requested[0].kind != 4 && key->requested[1].kind != 4)) {
        rate = Model_GetFrameStep();
        Model_StepViewOrbit(1, gModel_CameraMove.orbit_step * rate * gModel_bOrbitDirection, 0, 0, 0);
        Model_CorrectViewDistance(1);
    }
    if (gModel_CameraMove.follow_countdown) {
        ModelCameraMove *move = &gModel_CameraMove;
        s32 step;

        step = Model_GetFrameStep();
        if (move->follow_countdown - step > 0) {
            step = Model_GetFrameStep();
            progress = move->follow_countdown - step;
        } else {
            progress = 0;
        }
        move->follow_countdown = progress;
    }
}

void Model_CorrectViewPitch(void) {
    GsRVIEW2 *ref_view;
    s32 target_y;
    s32 height_weight;
    s32 pitch_target;
    s32 lower_pitch_limit;
    s32 pitch_sample;
    s32 pitch_work;

    if (*(s16 *)&gModel_wViewPitch >= 0x801) {
        ref_view = &gModel_RView;
        target_y = ref_view->vry;
        height_weight = 0;
        if (target_y >= -0xC7 && ref_view->vpy < target_y) {
            if (target_y < -0x64) {
                height_weight = (-200 - target_y) * MODEL_FIXED_ONE / -100;
            } else {
                height_weight = MODEL_FIXED_ONE;
            }
        }

        pitch_target = (height_weight * 3755 + (MODEL_FIXED_ONE - height_weight) * 3968) /
            MODEL_FIXED_ONE;
        lower_pitch_limit = pitch_target - 0x10;

        do {
            pitch_sample = *(s16 *)&gModel_wViewPitch;
            pitch_work = pitch_target + 0x10;
        } while (0);

        if (pitch_work < pitch_sample) {
            pitch_work = pitch_work - pitch_sample;
            if (pitch_work < -1) {
                pitch_work = -1;
            }
            Model_StepViewOrbit(1, 0, pitch_work, 0, 0);
        } else if (pitch_sample < 0xEAB) {
            Model_StepViewOrbit(1, 0, 0xEAB - pitch_sample, 0, 0);
        } else if (pitch_sample < lower_pitch_limit) {
            pitch_work = lower_pitch_limit - pitch_sample;
            if (pitch_work >= 5) {
                pitch_work = 4;
            }
            Model_StepViewOrbit(1, 0, pitch_work, 0, 0);
        }
    } else {
        Model_StepViewOrbit(1, 0, -4, 0, 0);
    }
}

void Model_CorrectViewDistance(s32 pull_in_request) {
    ModelCameraMove *separation_move;
    ModelCameraMove *relaxation_move;
    s32 target_slot;
    s32 lower_distance;
    s32 upper_distance;
    s32 zero_yaw;
    s32 distance_work;
    s32 distance_sample;

    target_slot = gModel_CameraMove.target.slot;
    lower_distance = 0x2BC;
    upper_distance = 0x3E8;
    zero_yaw = 0;

    if (target_slot >= 0) {
        lower_distance = Model_AdjustCameraDistanceForSlot(target_slot, lower_distance);
        if (lower_distance >= upper_distance) {
            upper_distance = Model_AdjustCameraDistanceForSlot(target_slot, upper_distance);
        }
    }

    /* The Japanese build only clears the flag bit here; it neither steps
       orbit_step up nor, on the other branch, back down towards base_orbit_step. */
    if (Model_SeparateViewFromSlots(1, 0x96, 0) != 0) {
        separation_move = &gModel_CameraMove;
#ifdef VERSION_EUROPE
        /* The European build steps orbit_step by 2 up to 0x200, and back
           down by 2 without passing base_orbit_step. */
        if (separation_move->orbit_step < 0x200) {
            separation_move->orbit_step = separation_move->orbit_step + 2;
        }
#elif !defined(VERSION_JAPAN)
        if (separation_move->orbit_step < 0x10) {
            separation_move->orbit_step = separation_move->orbit_step + 1;
        }
#endif
        separation_move->flags = separation_move->flags & 0xFE;
    }
#ifdef VERSION_EUROPE
    else {
        u16 baseline_step;
        u16 step_candidate;

        relaxation_move = &gModel_CameraMove;
        baseline_step = relaxation_move->base_orbit_step;
        if (baseline_step < relaxation_move->orbit_step) {
            step_candidate = relaxation_move->orbit_step - 2;
            relaxation_move->orbit_step = step_candidate;
            if (step_candidate < baseline_step) {
                relaxation_move->orbit_step = baseline_step;
            }
        }
    }
#elif !defined(VERSION_JAPAN)
    else {
        relaxation_move = &gModel_CameraMove;
        if (relaxation_move->base_orbit_step < relaxation_move->orbit_step) {
            relaxation_move->orbit_step = relaxation_move->orbit_step - 1;
        }
    }
#endif

    Model_CorrectViewPitch();

    distance_sample = *(s16 *)&gModel_wViewDistance;
    if (distance_sample < 0x12C) {
        if (distance_sample < 0x96) {
            distance_work = 0x96 - distance_sample;
            Model_StepViewOrbit(1, zero_yaw, 0, distance_work, 0);
        } else {
            distance_work = 0x12C - distance_sample;
            if (distance_work >= 0xB) {
                distance_work = 0xA;
            }
            Model_StepViewOrbit(1, zero_yaw, 0, distance_work, 0);
        }
    } else if (distance_sample < lower_distance) {
        distance_work = lower_distance - distance_sample;
        if (distance_work >= 6) {
            distance_work = 5;
        }
        Model_StepViewOrbit(1, 0, 0, distance_work, 0);
    }

    if (pull_in_request != 0) {
        distance_work = *(s16 *)&gModel_wViewDistance;
        if (upper_distance < distance_work) {
            goto last;
        }
        if (lower_distance < distance_work) {
            distance_work = lower_distance - distance_work;
            if (distance_work < -0xA) {
                distance_work = -0xA;
            }
            Model_StepViewOrbit(1, 0, 0, distance_work, 0);
        }
    }

    if (upper_distance < *(s16 *)&gModel_wViewDistance) {
last:
        distance_work = upper_distance - *(s16 *)&gModel_wViewDistance;
        if (distance_work < -0x14) {
            distance_work = -0x14;
        }
        Model_StepViewOrbit(1, 0, 0, distance_work, 0);
    }

    Model_CorrectViewPitch();
}

/* Per-frame tint pass over the ten requests at gModel_aTintRequests. A live request
 * interpolates its start colour towards its end colour, redraws the slot with
 * the saved animation selector, then restores the slot and advances the request.
 *
 * What the allocation rests on: the shifted flag word `flag_work` is masked into its
 * own name, `request_animation_row`, ahead of the Model_SetSlotShadowEnabled call, and `request_animation_row`
 * is copied to `draw_animation_row` after it. `request_animation_row` therefore lives and dies inside the one
 * basic block while crossing the call, so local-alloc gives the flag word,
 * shift and mask $s2 and pushes animation_position_snapshot to $s3; `draw_animation_row` then inherits $s2
 * through its
 * copy preference. `flag_work` itself never crosses the call, which is what holds the
 * srl and andi ahead of it where the scheduler would otherwise sink them to
 * the end of the block. The saved current_animation_row is one name, `saved_animation_row`, across both
 * loops. */
void Model_UpdateTintRequests(void)
{
    ModelTintColor saved_tint;
    ModelTintColor draw_tint;
    s32 request_index;
    s32 draw_part_index;
    s32 restore_part_index;
    u32 flag_work;
    s32 saved_animation_row;
    s32 mask_base_address;
    u32 request_animation_row;
    u32 draw_animation_row;
    s32 saved_shadow_state;
    u8 slot_index;
    u16 saved_animation_position;
    s32 request_offset;
    u16 elapsed;
    u16 duration;
    u16 request_animation_position;
    s32 draw_position;
    s32 restore_position;
    ModelSlot *slot_state;
    ModelTintRequest *request;
    ModelTintRequest *requests;

    requests = (ModelTintRequest *)gModel_aTintRequests;
    for (request_index = 0, request_offset = 0; request_index < MODEL_TINT_REQUEST_COUNT; request_offset += 0x18, request_index++) {
        request = &requests[request_index];
        mask_base_address = (s32)((u8 *)requests + 1);
        if ((*(u8 *)request & 1) == 0) {
            continue;
        }
        if (gModel_aSlots[request->slot].is_active == 0) {
            continue;
        }
        if (!(Model_HasInsufficientBufferSpace(request->slot, request->start.blend_mode))) {
            flag_work = *(u16 *)request;
            request_animation_position = request->animation_position_snapshot;
            elapsed = request->elapsed;
            duration = request->duration;
            slot_index = (flag_work >> 1) & 1;
            flag_work = flag_work >> 3;
            slot_state = &gModel_aSlots[slot_index];
            saved_animation_position = slot_state->animation_position;
            request_animation_row = flag_work & 0x1F;
            saved_animation_row = slot_state->current_animation_row;
            saved_shadow_state = Model_SetSlotShadowEnabled(slot_index, 0);
            draw_animation_row = request_animation_row;
            saved_tint = *MODEL_TINT_COLOR_VIEW(slot_state->tint);
            draw_tint.blend_mode = request->start.blend_mode;
            draw_tint.r = request->start.r * (duration - elapsed) / duration + request->end.r * elapsed / duration;
            draw_tint.g = request->start.g * (duration - elapsed) / duration + request->end.g * elapsed / duration;
            draw_tint.b = request->start.b * (duration - elapsed) / duration + request->end.b * elapsed / duration;
            *MODEL_TINT_COLOR_VIEW(slot_state->tint) = draw_tint;

            draw_position = request_animation_position;
            for (draw_part_index = 0; draw_part_index < slot_state->animation_sequence_count; draw_part_index++) {
                s32 part_row = slot_state->current_animation_row;
                if (draw_animation_row != 0) {
                    slot_state->animation_sequences[draw_part_index]->sid = draw_animation_row;
                    part_row = draw_animation_row;
                }
                Model_SeekSlotPartAnimation(slot_state, draw_part_index, part_row, draw_position);
            }
            if (draw_animation_row != 0) {
                slot_state->current_animation_row = draw_animation_row;
            }

            gModel_PartMaskAddress = request_offset + mask_base_address;
            gModel_bDrawMaskedPartsOnly = request->masked_parts_only;
            Model_DrawSlot(slot_index);
            gModel_PartMaskAddress = 0;
            gModel_bDrawMaskedPartsOnly = 0;

            restore_position = saved_animation_position;
            for (restore_part_index = 0; restore_part_index < slot_state->animation_sequence_count; restore_part_index++) {
                s32 part_row = slot_state->current_animation_row;
                if (saved_animation_row != 0) {
                    slot_state->animation_sequences[restore_part_index]->sid = saved_animation_row;
                    part_row = saved_animation_row;
                }
                Model_SeekSlotPartAnimation(slot_state, restore_part_index, part_row, restore_position);
            }
            if (saved_animation_row != 0) {
                slot_state->current_animation_row = saved_animation_row;
            }

            Model_SetSlotShadowEnabled(slot_index, saved_shadow_state);
            *MODEL_TINT_COLOR_VIEW(slot_state->tint) = saved_tint;
        }
        request->elapsed += Model_GetFrameStep();
        if (request->elapsed >= request->duration) {
            request->active = 0;
        }
    }
}

/* Starts a camera move from the live view to model-slot or explicit targets.
 * A zero duration writes the selected endpoints back immediately. */
void Model_StartCameraMove(s32 mode_request, s32 eye_transport, s32 target_transport, s32 timing_work)
{
    SVECTOR eye_start;
    SVECTOR view_work;
    SVECTOR target_copy;
    s32 selected_slot;
    s32 hold_state;
    s32 active_flags;

    memset(&view_work, 0, 8);
    view_work.vx = gModel_RView.vpx;
    view_work.vy = gModel_RView.vpy;
    view_work.vz = gModel_RView.vpz;
    eye_start = view_work;
    memset(&target_copy, 0, 8);
    target_copy.vx = gModel_RView.vrx;
    target_copy.vy = gModel_RView.vry;
    target_copy.vz = gModel_RView.vrz;
    view_work = target_copy;
    hold_state = Model_GetCameraHoldState();
    if (hold_state == 1) {
        if (Model_GetHandlerCallbackState() == hold_state) {
            return;
        }
    }
    if (eye_transport != 0 || mode_request <= 0) {
        if (eye_transport > 0) {
            selected_slot = eye_transport - 1;
            eye_transport = (s32)MODEL_SLOT_BYTES(gModel_aSlots[0].cached_position, selected_slot);
            if (timing_work >= 0) {
                gModel_CameraMove.eye.pair_slot = selected_slot ^ 1;
            } else {
                gModel_CameraMove.eye.pair_slot = -1;
            }
            gModel_CameraMove.eye.slot = selected_slot;
        } else {
            ModelCameraMove *camera_move = &gModel_CameraMove;

            camera_move->eye.slot = -1;
            camera_move->eye.pair_slot = -1;
        }
    }
    if (target_transport != 0 || mode_request <= 0) {
        if (target_transport > 0) {
            selected_slot = target_transport - 1;
            target_transport = (s32)MODEL_SLOT_BYTES(gModel_aSlots[0].cached_position, selected_slot);
            if (timing_work >= 0) {
                gModel_CameraMove.target.pair_slot = selected_slot ^ 1;
            } else {
                gModel_CameraMove.target.pair_slot = -1;
            }
            gModel_CameraMove.target.slot = selected_slot;
        } else {
            ModelCameraMove *camera_move = &gModel_CameraMove;

            camera_move->target.slot = -1;
            camera_move->target.pair_slot = -1;
        }
    }
    if (mode_request >= 0) {
        gModel_CameraMove.mode = mode_request;
    }
    {
        ModelCameraMove *camera_move = &gModel_CameraMove;

        camera_move->follow_countdown = MODEL_DEFAULT_PROJECTION;
#ifndef VERSION_JAPAN
        camera_move->base_orbit_step = 8;
#endif
        camera_move->orbit_step = 8;
        camera_move->flags = 0;
        if (eye_transport != 0) {
            camera_move->flags = 1;
        } else {
            eye_transport = (s32)&eye_start;
        }
    }
    if (target_transport != 0) {
        ModelCameraMove *camera_move = &gModel_CameraMove;

        camera_move->flags = camera_move->flags | 2;
    } else {
        target_transport = (s32)&view_work;
    }
    {
        ModelCameraMove *camera_move = &gModel_CameraMove;

        camera_move->eye.start_x = eye_start.vx;
        camera_move->eye.start_y = eye_start.vy;
        camera_move->eye.start_z = eye_start.vz;
        /* eye_transport/target_transport now hold a slot's cached_position position or &eye_start/&view_work; both
           are SVECTOR-shaped. */
        camera_move->eye.end_x = SVECTOR_VIEW(eye_transport)->vx;
        camera_move->eye.end_y = SVECTOR_VIEW(eye_transport)->vy;
        camera_move->eye.end_z = SVECTOR_VIEW(eye_transport)->vz;
        camera_move->target.start_x = view_work.vx;
        camera_move->target.start_y = view_work.vy;
        camera_move->target.start_z = view_work.vz;
        camera_move->target.end_x = SVECTOR_VIEW(target_transport)->vx;
        camera_move->target.end_y = SVECTOR_VIEW(target_transport)->vy;
        camera_move->target.end_z = SVECTOR_VIEW(target_transport)->vz;
        timing_work = (timing_work < 0 ? -timing_work : timing_work) * 2;
        if (timing_work > 0xFFFE) {
            camera_move->duration = 0xFFFF;
        } else {
            camera_move->duration = timing_work;
        }
    }
    {
        ModelCameraMove *camera_move = &gModel_CameraMove;

        active_flags = camera_move->flags;
        camera_move->elapsed = 0;
        if (active_flags != 0) {
            gModel_bOrbitDirectionTimer = 0;
        }
        if (timing_work != 0) {
            return;
        }
        if (active_flags & 1) {
            gModel_RView.vpx = camera_move->eye.end_x;
            gModel_RView.vpy = camera_move->eye.end_y;
            gModel_RView.vpz = camera_move->eye.end_z;
        }
        if (camera_move->flags & 2) {
            gModel_RView.vrx = camera_move->target.end_x;
            gModel_RView.vry = camera_move->target.end_y;
            gModel_RView.vrz = camera_move->target.end_z;
        }
        if (camera_move->flags & 3) {
            Model_UpdateViewMetrics(0);
        }
        camera_move->flags = 0;
    }
}

void Model_ResetSceneView(void)
{
    int projection_distance = gModel_wProjectionDistance;
    gModel_wBackgroundOffsetY = 0;
    gModel_wBackgroundScrollX = 0;
    gModel_wViewPitch = 0;
    gModel_wViewDistance = 0;
    gModel_wViewYaw = 0xC00;
    GsSetProjection(projection_distance);
    gModel_RView.vpx = 0;
    gModel_RView.vpy = -350;
    gModel_RView.vpz = -1200;
    gModel_RView.vrx = 0;
    gModel_RView.vry = -350;
    gModel_RView.vrz = 0;
    gModel_RView.rz = 0;
    gModel_RView.super = 0;
    Model_StepViewOrbit(1, 0x80, -85, 1200, 0);
    GsSetRefView2(&gModel_RView);
    {
        GsCOORDUNIT *camera_coordinate = &gModel_CameraCoordinateUnit;
        camera_coordinate->rot.vz = 0;
        camera_coordinate->rot.vy = 0;
        camera_coordinate->rot.vx = 0;
        camera_coordinate->matrix.t[2] = 0;
        camera_coordinate->matrix.t[1] = 0;
        camera_coordinate->matrix.t[0] = 0;
        camera_coordinate->super = 0;
        RotMatrix_gte(&camera_coordinate->rot, &camera_coordinate->matrix);
        camera_coordinate->flg = 0;
    }
    gModel_SavedRefView = gModel_RView;
    gModel_ViewSnapshot = *(ModelBytes8 *)&gModel_wViewDistance;
    Model_UpdateBackgroundScroll(1);
    {
        int camera_hold_state = Model_GetCameraHoldState();
        if (camera_hold_state != 1 || Model_GetHandlerCallbackState() != camera_hold_state)
            Model_StartCameraMove(0, 0, 0, 0);
    }
    Model_ResetEffectKeyQueue();
}

/* The Japanese build sets only the slot record, without the three
   gModel_awSlotModelIds/gModelDebug_abSlotAfFlags/gModelDebug_abSlotGfFlags mirrors. */
void Model_SetSlotProperties(s32 idx, ...)
{
    va_list arguments;
    s32 model_id;
    s32 attack_override;
    s32 defense_override;
    s32 guardian_star_option;
    s32 defense_position_option;

    va_start(arguments, idx);
    model_id = va_arg(arguments, s32);
    if (model_id >= 0) {
        gModel_aSlots[idx].model_id = model_id;
#ifndef VERSION_JAPAN
        gModel_awSlotModelIds[idx] = model_id;
#endif
    }
    if (idx < 2) {
        attack_override = va_arg(arguments, s32);
        defense_override = va_arg(arguments, s32);
        guardian_star_option = va_arg(arguments, s32);
        defense_position_option = va_arg(arguments, s32);
        if (attack_override >= 0) {
            gModel_aSlots[idx].attack_compare_override = attack_override;
        }
        if (defense_override >= 0) {
            gModel_aSlots[idx].defense_compare_override = defense_override;
        }
        if (guardian_star_option >= 0) {
            gModel_aSlots[idx].guardian_star_selection = (guardian_star_option != 0);
#ifndef VERSION_JAPAN
            gModelDebug_abSlotAfFlags[idx] = (guardian_star_option != 0);
#endif
        }
        if (defense_position_option >= 0) {
            gModel_aSlots[idx].defense_position = (defense_position_option != 0);
#ifndef VERSION_JAPAN
            gModelDebug_abSlotGfFlags[idx] = (defense_position_option != 0);
#endif
        }
    } else {
        gModel_pBackgroundRecord =
            (ModelBackgroundRecord *)&gModel_abBackgroundRecords[gModel_aSlots[2].model_id * 0xB2];
    }
    gModel_bSceneMode = 15;
}

/* Regional value: the x of the row Model_ResetSceneState clears, 0x200 in the
   US build and 0x280 in the European one, whose wrapper defines its own. */
#ifndef MODEL_TEXTURE_RESET_ROW_X
#define MODEL_TEXTURE_RESET_ROW_X 0x200
#endif

void Model_ResetSceneState(void){s16 table[256];RECT packet;register s16*p=&table[1];register s32 fill=0xffff;register s32 counter=254;s32 i;table[0]=0;do{*p=fill;counter--;p++;}while(counter>=0);packet.x=MODEL_TEXTURE_RESET_ROW_X;packet.y=0xF0;packet.w=0x100;packet.h=1;while(IsIdleGPU(3)){}while(LoadImage2(&packet,(u32 *)table)){}while(IsIdleGPU(3)){}Model_ResetSlot(0);Model_ResetSlot(1);Model_ResetSlot(2);for(i=0;i<MODEL_TINT_REQUEST_COUNT;i++)gModel_aTintRequests[i].active=0;gModel_bDrawMaskedPartsOnly=0;gModel_PartMaskAddress=0;Model_SetScreenYOverride(0x8000);gModel_bSceneMode=0;gModel_bPresentationPhase=-1;}

#include "input.h"

#ifdef VERSION_JAPAN
#define MODEL_DEBUG_CANCEL_MASK 0x40
#else
#define MODEL_DEBUG_CANCEL_MASK 0x20
#endif
ModelDebugState D_8009B004 = {0};
#ifndef VERSION_JAPAN
ModelDebugDisplayState gModelDebug_Display = {0};
#endif

s32 ModelDebug_UpdateController(void)
{
    s32 reload = 0;
#ifndef VERSION_JAPAN
    char *normal = "~cff0";
    char *selected = "~cf00";
#endif
    ModelSlot *slots;

    if (gModel_bPresentationPhase == -2)
        return 1;
    if (gModel_bPresentationPhase >= 40)
        return 0;
    if ((gInput_awPadPressed[0] & MODEL_DEBUG_CANCEL_MASK) ||
        (gInput_awPadPressed[1] & MODEL_DEBUG_CANCEL_MASK)) {
        gModel_bPresentationPhase = 40;
        return 0;
    }
    slots = gModel_aSlots;
    if (!slots[2].is_active) {
        Model_SetFrameStepOverride(1);
#ifndef VERSION_JAPAN
        FntPrint(gModelDebug_szLoadStageFormat, slots[2].model_id, slots[2].load_step);
#endif
        if (slots[2].load_step == 255) {
            if (!((gFile_dwTransferState & 0x2000030) | gFile_dwCancelRequestState))
                Model_LoadMonsterMerge(2, -1, -1, -1, -1, -1, 0);
        } else {
            Model_AdvanceSlotLoad(2);
        }
        if (gModel_aSlots[2].is_active) {
            ModelSlot *stage = gModel_aSlots;
            s32 height;

            stage += 2;
            /* A natural member store moves the stage-pointer load across it;
               the u16 member view preserves the original ordering. */
            *(u16 *)&D_8009B004.fields.height =
                gModel_pBackgroundRecord->stage_height;
            height = D_8009B004.fields.height;
            if (stage->root_coordinate) {
                stage->root_coordinate->rot.vx = 0;
                stage->root_coordinate->rot.vy = 0;
                stage->root_coordinate->rot.vz = 0;
                stage->root_coordinate->matrix.t[0] = 0;
                stage->root_coordinate->matrix.t[1] = height;
                stage->root_coordinate->matrix.t[2] = 0;
            }
            Model_UpdateCoordinateUnitMatrix(stage->root_coordinate, 0);
            gModel_bPresentationPhase = 0;
        }
    } else if (!slots[0].is_active) {
        Model_SetFrameStepOverride(1);
#ifndef VERSION_JAPAN
        FntPrint(gModelDebug_szLoadModel0Format, slots[0].model_id, slots[0].load_step);
#endif
        if (slots[0].load_step == 255) {
            if (!((gFile_dwTransferState & 0x2000030) | gFile_dwCancelRequestState))
                Model_LoadMonsterMerge(0, -1, -1, -1, -1, -1, 0);
        } else {
            Model_AdvanceSlotLoad(0);
        }
        if (gModel_aSlots[0].is_active)
            Model_SetSlotShadowEnabled(0, ((gModel_pBackgroundRecord->flags >> 1) ^ 1) & 1);
    } else if (!slots[1].is_active) {
        Model_SetFrameStepOverride(1);
#ifndef VERSION_JAPAN
        FntPrint(gModelDebug_szLoadModel1Format, slots[1].model_id, slots[1].load_step);
#endif
        if (slots[1].load_step == 255) {
            if (!((gFile_dwTransferState & 0x2000030) | gFile_dwCancelRequestState))
                Model_LoadMonsterMerge(1, -1, -1, -1, -1, -1, 0);
        } else {
            Model_AdvanceSlotLoad(1);
        }
        if (gModel_aSlots[1].is_active)
            Model_SetSlotShadowEnabled(1, ((gModel_pBackgroundRecord->flags >> 1) ^ 1) & 1);
    }
#ifndef VERSION_JAPAN
    if ((gInput_awPadHeld[0] & 0x100) && (gInput_awPadPressed[1] & 0x100))
        gModelDebug_Display.display_enabled ^= 1;
    if (!gModelDebug_Display.display_enabled)
        return 0;
    if (gModel_bPresentationPhase == -2) {
        s32 i = 0;
        s32 excluded;
        s32 value;

        while (1) {
            do {
                value = (rand() >> 8) % 722;
                excluded = 720;
            } while ((u32)value >= 722);
            if ((u32)(value - 300) >= 50 && (u32)(value - 650) >= 50 && value != excluded) {
                gModel_awSlotModelIds[i] = value;
                if ((u16)value != gModel_aSlots[i].model_id) {
                    i++;
                    if (i >= 2)
                        break;
                }
            }
        }
        reload++;
        gModel_awSlotModelIds[2] = (rand() >> 8) % 7;
    }
    if (gInput_awPadPressed[0] & 0x10)
        reload++;
    if (gInput_awPadRepeat[0] & 0x900) {
        if (gInput_awPadRepeat[0] & 0x100)
            gModel_awSlotModelIds[2] = ((u16)gModel_awSlotModelIds[2] + 6) % 7;
        else
            gModel_awSlotModelIds[2] = ((u16)gModel_awSlotModelIds[2] + 8) % 7;
    }
    if (gInput_awPadRepeat[0] & 0xA000) {
        if (gInput_awPadRepeat[0] & 0x8000)
            gModelDebug_bSelectedDigit = (gModelDebug_bSelectedDigit + 5) % 6;
        else
            gModelDebug_bSelectedDigit = (gModelDebug_bSelectedDigit + 7) % 6;
    }
    if (gInput_awPadHeld[0] & 0x80) {
        if (gInput_awPadRepeat[0] & 0x5000) {
            if (gInput_awPadRepeat[0] & 0x1000)
                D_8009B004.halfwords[1] -= 10;
            else
                D_8009B004.halfwords[1] += 10;
            if (gModel_aSlots[2].is_active) {
                ModelSlot *stage = gModel_aSlots;
                s32 height;

                stage += 2;
                height = D_8009B004.fields.height;
                if (stage->root_coordinate) {
                    stage->root_coordinate->rot.vx = 0;
                    stage->root_coordinate->rot.vy = 0;
                    stage->root_coordinate->rot.vz = 0;
                    stage->root_coordinate->matrix.t[0] = 0;
                    stage->root_coordinate->matrix.t[1] = height;
                    stage->root_coordinate->matrix.t[2] = 0;
                }
                Model_UpdateCoordinateUnitMatrix(stage->root_coordinate, 0);
            }
        }
    } else if (gInput_awPadRepeat[0] & 0x5000) {
        s32 side = gModelDebug_bSelectedDigit >= 3u;
        u16 *number = (u16 *)&gModel_awSlotModelIds[side];
        s32 digits[3] = {
            (u16)((u16)(*number / 100u) % 10u),
            (u16)((u16)(*number / 10u) % 10u),
            (u16)(*number % 10u)
        };

        if (gInput_awPadRepeat[0] & 0x1000) {
            digits[(u8)(gModelDebug_bSelectedDigit % 3u)] =
                (digits[(u8)(gModelDebug_bSelectedDigit % 3u)] + 11) % 10;
        } else {
            digits[(u8)(gModelDebug_bSelectedDigit % 3u)] =
                (digits[(u8)(gModelDebug_bSelectedDigit % 3u)] + 9) % 10;
        }
        gModel_awSlotModelIds[side] = 100 * digits[0] + 10 * digits[1] + digits[2];
    }
    if (gInput_awPadPressed[0] & 4)
        gModelDebug_abSlotAfFlags[0] ^= 1;
    if (gInput_awPadPressed[0] & 1)
        gModelDebug_abSlotGfFlags[0] ^= 1;
    if (gInput_awPadPressed[0] & 8)
        gModelDebug_abSlotAfFlags[1] ^= 1;
    if (gInput_awPadPressed[0] & 2)
        gModelDebug_abSlotGfFlags[1] ^= 1;
    FntPrint("%s%d", gModelDebug_bSelectedDigit == 0 ?
        selected : normal, (u16)((u16)((u16)gModel_awSlotModelIds[0] / 100u) % 10u));
    FntPrint("%s%d", gModelDebug_bSelectedDigit == 1 ?
        selected : normal, (u16)((u16)((u16)gModel_awSlotModelIds[0] / 10u) % 10u));
    FntPrint("%s%d", gModelDebug_bSelectedDigit == 2 ?
        selected : normal, (u16)((u16)gModel_awSlotModelIds[0] % 10u));
    FntPrint(normal);
    FntPrint(gModelDebug_szSlotFlagsFormat, gModelDebug_abSlotAfFlags[0], gModelDebug_abSlotGfFlags[0]);
    FntPrint(" VS ");
    FntPrint("%s%d", gModelDebug_bSelectedDigit == 3 ?
        selected : normal, (u16)((u16)((u16)gModel_awSlotModelIds[1] / 100u) % 10u));
    FntPrint("%s%d", gModelDebug_bSelectedDigit == 4 ?
        selected : normal, (u16)((u16)((u16)gModel_awSlotModelIds[1] / 10u) % 10u));
    FntPrint("%s%d", gModelDebug_bSelectedDigit == 5 ?
        selected : normal, (u16)((u16)gModel_awSlotModelIds[1] % 10u));
    FntPrint(normal);
    FntPrint(gModelDebug_szSlotFlagsFormat, gModelDebug_abSlotAfFlags[1], gModelDebug_abSlotGfFlags[1]);
    FntPrint("\n");
    FntPrint(gModelDebug_szStageFormat, (u16)gModel_awSlotModelIds[2], D_8009B004.fields.height);
    if (reload) {
        File_WaitForTransfers();
        Model_ResetSceneState();
        Model_SetSlotProperties(0, (u16)gModel_awSlotModelIds[0], -1, -1,
            gModelDebug_abSlotAfFlags[0], gModelDebug_abSlotGfFlags[0]);
        Model_SetSlotProperties(1, (u16)gModel_awSlotModelIds[1], -1, -1,
            gModelDebug_abSlotAfFlags[1], gModelDebug_abSlotGfFlags[1]);
        Model_SetSlotProperties(2, (u16)gModel_awSlotModelIds[2]);
    }
#endif
    return 0;
}

/*
 * Per-slot model draw pass: selects the slot's ordering table, programs the
 * three flat lights and the ambient colour, steps the slot's colour fade into
 * the shared primitive templates at gModel_dwPolyFt3Template..gModel_bDrawTintBlue, sorts every unit
 * of the slot, then updates the slot's bounding volume, draws the ground
 * shadow fan and runs the palette flash. Matching pure C under
 * gcc_2_8_1_g8_split_comm: no register pins, asm aliases or inline assembly.
 *
 * What the match depends on:
 * - the frame is block-scoped objects. GCC 2.8.1 gives block-scope aggregates
 *   temporary stack slots and hands a closed block's slots to the next block,
 *   so `hidden_matrix` owns 0x20 for the whole function, the unit loop's two matrices
 *   sit at 0x40 and 0x60, and the shadow fan's vertices and primitive and the
 *   flash's rectangles and pixel rows reuse that space in declaration order;
 * - gModel_dwLitColorCacheBase and gModel_bCurrentDrawMode are defined here as common symbols, because
 *   the assembler only pads a load feeding a gp-relative store when the unit
 *   defines the stored symbol (c_symbols.ld overrides the commons, so no
 *   storage is allocated);
 * - the scratch variables are function-scope and reused: `a` and `b` hold
 *   the hidden flag, a unit's x and z and then the shadow's half extents,
 *   `w`, `h` and `d` hold the box extents, the shadow's clamped extents and
 *   the green and blue channels, and the fade-complete block keeps red in
 *   `y`. That reuse is what decides the register each one gets;
 * - the template selector is nested inequality tests, which gives retail's
 *   linear comparison chain with its arms laid out in reverse;
 * - the first matrix loop counts a separate index so loop.c reverses it, and
 *   the GsGetLwsUnit call sits inside a do/while (0): cse only ends a block
 *   at a loop end, and without that boundary the matrix address is shared
 *   with the call argument and kept in a callee-saved register;
 * - the mirrored x and z offsets go through a static inline, whose parameter
 *   copy is retail's `move v1,v0` in the branch delay slot;
 * - the second bounding loop reads slot->render_parent_coordinate directly. The extra load
 *   keeps the loop above loop.c's size threshold, so -1 and the unit count
 *   stay inside the loop while the twice-used D1C load is still hoisted;
 * - the shadow clamps assign a conditional expression back to each vertex
 *   field, the fan table is indexed as x/z pairs so loop.c makes the pointer,
 *   and the shadow colour reads the template bytes directly.
 */
#include "graphics_packet_submission.h"
#include "../game/model_handler_state.h"
#include "../game/model_primitive_templates.h"
#include "../game/model_record_tables.h"
#include "../game/ordering_tables.h"
#include "../game/model_slot_control.h"
#include "../game/color.h"
#include "../game/gpu_packets.h"
#include "../game/model_handler_registry.h"

#define B(p, o) (*((u8 *)(p) + (o)))
#define S(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define W(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define GS_COORD_UNIT_VIEW(unit) ((GsCOORDUNIT *)(unit))
#define VECTOR_VIEW(vector) ((VECTOR *)(vector))

#ifndef MODEL_RENDER_BLEND_MODE_5_WORD
#define MODEL_RENDER_BLEND_MODE_5_WORD 0x3C200000
#endif

/* Defined rather than declared so the assembler pads the loads that feed
 * their gp-relative stores; model_background.c and
 * model_scene_controllers.c hold the initialised definitions these commons merge
 * into, so no storage is allocated here. */
u32 gModel_dwLitColorCacheBase;
u8 gModel_bCurrentDrawMode;

/* Stamps a colour into the ten primitive templates and derives the blend
 * words from the fade mode byte. Both colour blocks share it; a plain braced
 * block rather than do/while (0), because a loop note is a cse boundary. */
#define MODEL_STAMP_TEMPLATES(colour, red, green, blue, fl, mode) \
    { \
    if ((fl) != 0) { \
        (colour) |= 0x02000000; \
    } \
    gModel_dwPolyFt3Template = (colour) | 0x24000000; \
    gModel_dwPolyFt4Template = (colour) | 0x2C000000; \
    gModel_dwPolyGt3Template = (colour) | 0x34000000; \
    gModel_dwPolyGt4Template = (colour) | 0x3C000000; \
    gModel_dwLineF3Template = (colour) | 0x4A000000; \
    gModel_dwLineF4Template = (colour) | 0x4E000000; \
    gModel_dwPolyFt3SemiTemplate = (colour) | 0x26000000; \
    gModel_dwPolyFt4SemiTemplate = (colour) | 0x2E000000; \
    gModel_dwPolyGt3SemiTemplate = (colour) | 0x36000000; \
    gModel_dwPolyGt4SemiTemplate = (colour) | 0x3E000000; \
    B(&gModel_wDrawTintRedGreen, 0) = (red); \
    B(&gModel_wDrawTintRedGreen, 1) = (green); \
    gModel_bDrawTintBlue = (blue); \
    if ((fl) != 2) { \
        if ((fl) != 3) { \
            if ((fl) != 4) { \
                if ((fl) != 5) { \
                    gModel_dwTpageBlendRateBits = 0; \
                } else { \
                    gModel_dwTpageBlendRateBits = 0x200000; \
                } \
            } else { \
                gModel_dwTpageBlendRateBits = 0x600000; \
            } \
        } else { \
            gModel_dwTpageBlendRateBits = 0x400000; \
        } \
    } else { \
        gModel_dwTpageBlendRateBits = 0x200000; \
    } \
    if ((fl) == 5) { \
        gModel_dwSecondPassClutBits = MODEL_RENDER_BLEND_MODE_5_WORD; \
    } else { \
        gModel_dwSecondPassClutBits = 0; \
    } \
    (mode) = (fl) - 1; \
    gModel_bBlendRateActive = (u32)(mode) < 4; \
    if (gModel_bBlendRateActive != 0) { \
        gModel_dwDrawModeTemplate = (((mode) << 5) & 0x9FF) | 0xE1000200; \
    } else { \
        gModel_dwDrawModeTemplate = 0xE1000220; \
    } \
    }

/* The ground shadow is kept inside the +/-0x4B0 duel field. */
#define CLAMP_FIELD(v) ((v) < -0x4B0 ? -0x4B0 : (v) < 0x4B1 ? (v) : 0x4B0)
#define ABS(x) ((x) >= 0 ? (x) : -(x))

#ifndef MODEL_RENDER_EUROPEAN_TEXTURE_LAYOUT
#define MODEL_RENDER_EUROPEAN_TEXTURE_LAYOUT 0
#endif

/* Slot 1 faces slot 0, so its x and z offsets are applied mirrored. */
static inline s32 Model_MirrorOffset(s32 index, s32 value, s32 offset)
{
    if (index > 0) {
        return value - offset;
    }
    return value + offset;
}

/* Neutral grey tint and the default primitive templates. */
static inline void Model_ResetDrawTint(void)
{
    B(&gModel_wDrawTintRedGreen, 0) = 0x80;
    B(&gModel_wDrawTintRedGreen, 1) = 0x80;
    gModel_bDrawTintBlue = 0x80;
    gModel_dwPolyFt3Template = 0x24808080;
    gModel_dwPolyFt4Template = 0x2C808080;
    gModel_dwPolyGt3Template = 0x34808080;
    gModel_dwPolyGt4Template = 0x3C808080;
    gModel_dwLineF3Template = 0x4A808080;
    gModel_dwLineF4Template = 0x4E808080;
    gModel_dwPolyFt3SemiTemplate = 0x26808080;
    gModel_dwPolyFt4SemiTemplate = 0x2E808080;
    gModel_dwPolyGt3SemiTemplate = 0x36808080;
    gModel_dwPolyGt4SemiTemplate = 0x3E808080;
    gModel_dwTpageBlendRateBits = 0;
    gModel_dwSecondPassClutBits = 0;
    gModel_bBlendRateActive = 0;
    gModel_dwDrawModeTemplate = 0xE1000220;
}

void Model_DrawSlot(s32 index)
{
    MATRIX hidden_matrix;
    long sxy_z;
    long projection_flags;
    GsOT *ordering_table;
    ModelSlot *slot;
    s8 *metadata_bytes;
    ModelSlotHeadEntry *head_unit;
    s32 scan_index;
    s32 x;
    s32 y;
    s32 z;
    s32 a;
    s32 b;
    s32 origin_count;
    s32 w;
    s32 h;
    s32 d;
    s32 cached_dimension;
    s32 dimension_step;
#if MODEL_RENDER_EUROPEAN_TEXTURE_LAYOUT
    s32 texture_y;
#endif
    u8 texture_state;

    if (index < 2) {
        ordering_table = gGraphics_apOrderingTables[2];
    } else {
        ordering_table = gGraphics_apOrderingTables[3];
    }
    slot = &gModel_aSlots[index];
    metadata_bytes = (s8 *)&slot->metadata;
#if MODEL_RENDER_EUROPEAN_TEXTURE_LAYOUT
    texture_y = (index << 4) + 0xD0;
#endif
    if (slot->is_active == 0) {
        return;
    }
    if (index == 2 && slot->ambient_rgb_scale[0] == -0x1000 && slot->ambient_rgb_scale[1] == slot->ambient_rgb_scale[0]
        && slot->ambient_rgb_scale[2] == slot->ambient_rgb_scale[1]) {
        return;
    }
    if (slot->draw_mode_request != 1 && slot->tint[3] >= 2 && !(W(slot->tint, 0) & 0xFFFFFF)) {
        return;
    }
    GsSetFlatLight(0, (GsF_LIGHT *)&slot->flat_lights[0]);
    GsSetFlatLight(1, (GsF_LIGHT *)&slot->flat_lights[1]);
    GsSetFlatLight(2, (GsF_LIGHT *)&slot->flat_lights[2]);
    GsSetAmbient(slot->ambient_rgb_scale[0], slot->ambient_rgb_scale[1], slot->ambient_rgb_scale[2]);
    gModel_dwLitColorCacheBase = slot->lit_color_cache_address;
    gModel_bCurrentDrawMode = slot->draw_mode_request;
    if (slot->draw_mode_request != 4) {
        slot->draw_mode_request = 0;
    }
    if (index < 2) {
        ordering_table->length = 0xE;
        if ((slot->tint[7] % 6) && slot->tint[3] == 0 && gModel_bCurrentDrawMode == 0) {
            s32 fade_step;
            s32 packed_template_color;
            s32 fade_red;
            s32 blend_mode;
            s32 blend_mode_index;
            s32 next_red;
            s32 next_green;
            s32 next_blue;

            fade_step = Model_GetFrameStep() * 2;
            h = slot->tint[6];
            w = slot->tint[5];
            fade_red = slot->tint[4];
            packed_template_color = (h << 16) | (w << 8) | fade_red;
            blend_mode = slot->tint[7];
            MODEL_STAMP_TEMPLATES(packed_template_color, fade_red, w, h, blend_mode, blend_mode_index);
            gModel_bCurrentDrawMode = 3;
            next_red = slot->tint[0];
            if (slot->tint[4] + fade_step < next_red) {
                next_red = slot->tint[4] + fade_step;
            }
            slot->tint[4] = next_red;
            next_green = slot->tint[1];
            if (slot->tint[5] + fade_step < next_green) {
                next_green = slot->tint[5] + fade_step;
            }
            slot->tint[5] = next_green;
            next_blue = slot->tint[2];
            if (slot->tint[6] + fade_step < next_blue) {
                next_blue = slot->tint[6] + fade_step;
            }
            slot->tint[6] = next_blue;
            if (slot->tint[4] >= slot->tint[0] && slot->tint[5] >= slot->tint[1]
                && (u32)(next_blue & 0xFF) >= slot->tint[2]) {
                slot->tint[7] = 0;
            }
        } else if (gModel_bCurrentDrawMode != 1) {
            s32 packed_template_color;
            s32 blend_mode;
            s32 blend_mode_index;

            h = slot->tint[2];
            w = slot->tint[1];
            y = slot->tint[0];
            blend_mode = slot->tint[3];
            packed_template_color = h << 16;
            packed_template_color |= w << 8;
            packed_template_color |= y;
            MODEL_STAMP_TEMPLATES(packed_template_color, y, w, h, blend_mode, blend_mode_index);
            if (W(slot->tint, 0) != 0x808080) {
                gModel_bCurrentDrawMode = 3;
            }
            slot->tint[7] = 0;
        } else {
            Model_ResetDrawTint();
        }
    } else {
        Model_ResetDrawTint();
    }

    head_unit = slot->hmd_units;
    if (gModel_bCurrentDrawMode >= 3) {
        gModel_dwLitColorCacheBase = 0;
    }
    for (scan_index = 0; scan_index < slot->hmd_unit_count; scan_index++, head_unit++) {
        MATRIX local_to_screen;
        MATRIX local_to_world;
        GsCOORDUNIT *coordinate_unit;

        a = 0;
        if (gModel_bDrawMaskedPartsOnly != 0 && slot->entry_count < scan_index) {
            continue;
        }
        if (gModel_PartMaskAddress != 0 && scan_index > 0 && !(slot->entry_count < scan_index)) {
            if (!((((u8 *)gModel_PartMaskAddress)[(scan_index - 1) / 8] >> ((scan_index - 1) % 8)) & 1)) {
                a = 1;
                if (gModel_bDrawMaskedPartsOnly != 0) {
                    continue;
                }
            }
        }
        if (head_unit->primitive_data == 0) {
            continue;
        }
        if (slot->render_parent_coordinate != 0 &&
            GS_COORD_UNIT_VIEW(head_unit->coordinate_address)->super == slot->render_parent_coordinate) {
            s32 sequence_index;
            s32 scale_existing_matrix;
            u8 sequence_count;
            s32 coordinate_rewrite_index;

            coordinate_rewrite_index = (GS_COORD_UNIT_VIEW(head_unit->coordinate_address) -
                   GS_COORD_UNIT_VIEW(slot->entries)) * 0x14 + 0x04000001;
            sequence_index = 0;
            scale_existing_matrix = 0;
            sequence_count = slot->animation_sequence_count;
            for (; sequence_index < sequence_count; sequence_index++) {
                if (slot->animation_sequences[sequence_index]->rewrite_idx == coordinate_rewrite_index) {
                    if (slot->animation_sequences[sequence_index]->aframe == 0xFFFF
                        && ((slot->extended_animation_part_mask[sequence_index / 8] >> (sequence_index % 8)) & 1)
                        && slot->animation_duration_rows[slot->current_animation_row].values[sequence_index] != 0) {
                        scale_existing_matrix = 1;
                    }
                    break;
                }
            }
            if (scale_existing_matrix) {
                ScaleMatrix(
                    &GS_COORD_UNIT_VIEW(head_unit->coordinate_address)->matrix,
                    VECTOR_VIEW(&slot->render_scale));
            } else {
                Model_UpdateCoordinateUnitMatrix(
                    GS_COORD_UNIT_VIEW(head_unit->coordinate_address),
                    VECTOR_VIEW(&slot->render_scale));
            }
        }
        if (W(head_unit->primitive_data, 0) != -1 || W(head_unit->primitive_data, 8) != 0) {
            if (a) {
                memset(&hidden_matrix, 0, 0x20);
                GsSetLsMatrix(&hidden_matrix);
            } else if ((coordinate_unit = GS_COORD_UNIT_VIEW(head_unit->coordinate_address)) != 0) {
                if (gModel_bCurrentDrawMode != 0) {
                    s32 matrix_index;
                    s32 coefficient_magnitude;
                    u16 maximum_magnitude;
                    s16 *matrix_halfwords;

                    do {
                        GsGetLwsUnit(coordinate_unit, &local_to_world, &local_to_screen);
                    } while (0);
                    maximum_magnitude = 0;
                    matrix_halfwords = (s16 *)&local_to_world;
                    for (matrix_index = 0; matrix_index < 9; matrix_index++, matrix_halfwords++) {
                        coefficient_magnitude = ABS(*matrix_halfwords);
                        if (maximum_magnitude < coefficient_magnitude) {
                            maximum_magnitude = coefficient_magnitude;
                        }
                    }
                    /* Japanese retail leaves matrix_halfwords advanced when maximum_magnitude is zero. */
#ifndef VERSION_JAPAN
                    matrix_halfwords = (s16 *)&local_to_world;
#endif
                    if (maximum_magnitude != 0) {
#ifdef VERSION_JAPAN
                        matrix_halfwords = (s16 *)&local_to_world;
#endif
                        for (matrix_index = 0; matrix_index < 9; matrix_index++, matrix_halfwords++) {
                            *matrix_halfwords = (*matrix_halfwords << 12) / maximum_magnitude;
                        }
                    } else {
                        matrix_halfwords[8] = 0x1000;
                        matrix_halfwords[4] = 0x1000;
                        matrix_halfwords[0] = 0x1000;
                    }
                    GsSetLightMatrix(&local_to_world);
                } else {
                    GsGetLsUnit(coordinate_unit, &local_to_screen);
                }
                GsSetLsMatrix(&local_to_screen);
            }
            GsSortUnit((GsUNIT *)head_unit, ordering_table, (u32 *)0x1F800000);
        }
    }

    if (gModel_PartMaskAddress != 0) {
        if (index < 2) {
            ordering_table->length = 0xC;
        }
        return;
    }
    if (index < 2 && slot->render_parent_coordinate != 0) {
        s16 min_origin_x;
        s16 min_origin_y;
        s16 min_origin_z;
        s16 max_origin_x;
        s16 max_origin_y;
        s16 max_origin_z;
        GsCOORDUNIT *coordinate_unit;
        u8 *group_bytes;

        min_origin_z = min_origin_y = min_origin_x = 0x7FFF;
        max_origin_z = max_origin_y = max_origin_x = -0x8000;
        origin_count = x = y = z = 0;
        head_unit = slot->hmd_units;
        for (scan_index = 0; scan_index < slot->hmd_unit_count; scan_index++, head_unit++) {
            coordinate_unit = GS_COORD_UNIT_VIEW(head_unit->coordinate_address);
            if (coordinate_unit != 0 && head_unit->primitive_data != 0
                && (W(head_unit->primitive_data, 0) != -1 || W(head_unit->primitive_data, 8) != 0)) {
                origin_count++;
                x += coordinate_unit->workm.t[0];
                y += coordinate_unit->workm.t[1];
                z += coordinate_unit->workm.t[2];
            }
        }
        if (origin_count != 0) {
            x /= origin_count;
            y /= origin_count;
            z /= origin_count;
        } else {
            x = slot->render_parent_coordinate->workm.t[0];
            y = slot->render_parent_coordinate->workm.t[1];
            z = slot->render_parent_coordinate->workm.t[2];
        }
        x = Model_MirrorOffset(index, x, ((ModelSlotMetadataPrefix *)metadata_bytes)->position_offsets.x * 0x10);
        slot->cached_position[0] = x;
        y += ((ModelSlotMetadataPrefix *)metadata_bytes)->position_offsets.y * 0x10;
        slot->cached_position[1] = y;
        z = Model_MirrorOffset(index, z, ((ModelSlotMetadataPrefix *)metadata_bytes)->position_offsets.z * 0x10);
        origin_count = y = 0;
        slot->cached_position[2] = z;
        head_unit = slot->hmd_units;
        for (scan_index = 0; scan_index < slot->hmd_unit_count; scan_index++, head_unit++) {
            coordinate_unit = GS_COORD_UNIT_VIEW(head_unit->coordinate_address);
            if (coordinate_unit != slot->render_parent_coordinate && coordinate_unit != slot->root_coordinate && coordinate_unit != 0 && (group_bytes = head_unit->primitive_data) != 0) {
                s32 unit_origin_y;

                if (coordinate_unit->super == slot->render_parent_coordinate) {
                    origin_count++;
                    y += coordinate_unit->workm.t[1];
                }
                if (W(group_bytes, 0) != -1 || W(group_bytes, 8) != 0) {
                    a = coordinate_unit->workm.t[0];
                    b = coordinate_unit->workm.t[2];
                    unit_origin_y = coordinate_unit->workm.t[1];
                    if (max_origin_x < a) {
                        max_origin_x = a;
                    }
                    if (a < min_origin_x) {
                        min_origin_x = a;
                    }
                    if (max_origin_y < unit_origin_y) {
                        max_origin_y = unit_origin_y;
                    }
                    if (unit_origin_y < min_origin_y) {
                        min_origin_y = unit_origin_y;
                    }
                    if (max_origin_z < b) {
                        max_origin_z = b;
                    }
                    if (b < min_origin_z) {
                        min_origin_z = b;
                    }
                }
            }
        }
        if (origin_count != 0) {
            y /= origin_count;
        } else {
            y = slot->render_parent_coordinate->workm.t[1];
        }
        slot->cached_position[3] = (y - 700) / 3 + ((ModelSlotMetadataPrefix *)metadata_bytes)->position_offsets.y * 0x10;
        w = max_origin_x - min_origin_x;
        h = max_origin_y - min_origin_y;
        d = max_origin_z - min_origin_z;
        if ((s16)slot->cached_dimensions[3] < w) {
            slot->cached_dimensions[3] = w;
        }
        if ((s16)slot->cached_dimensions[3] < d) {
            slot->cached_dimensions[3] = d;
        }
        w += 0x32;
        h += 0x32;
        d += 0x32;
        cached_dimension = (s16)slot->cached_dimensions[0];
        if (cached_dimension != 0) {
            if (w < cached_dimension) {
                dimension_step = cached_dimension - w;
                if (dimension_step >= 0x15) {
                    dimension_step = 0x14;
                }
                slot->cached_dimensions[0] = slot->cached_dimensions[0] - dimension_step;
            } else {
                dimension_step = w - cached_dimension;
                if (dimension_step >= 0x15) {
                    dimension_step = 0x14;
                }
                slot->cached_dimensions[0] = slot->cached_dimensions[0] + dimension_step;
            }
        } else {
            slot->cached_dimensions[0] = w;
        }
        cached_dimension = (s16)slot->cached_dimensions[1];
        if (cached_dimension != 0) {
            if (h < cached_dimension) {
                dimension_step = cached_dimension - h;
                if (dimension_step >= 0x15) {
                    dimension_step = 0x14;
                }
                slot->cached_dimensions[1] = slot->cached_dimensions[1] - dimension_step;
            } else {
                dimension_step = h - cached_dimension;
                if (dimension_step >= 0x15) {
                    dimension_step = 0x14;
                }
                slot->cached_dimensions[1] = slot->cached_dimensions[1] + dimension_step;
            }
        } else {
            slot->cached_dimensions[1] = h;
        }
        cached_dimension = (s16)slot->cached_dimensions[2];
        if (cached_dimension != 0) {
            if (d < cached_dimension) {
                dimension_step = cached_dimension - d;
                if (dimension_step >= 0x15) {
                    dimension_step = 0x14;
                }
                slot->cached_dimensions[2] = slot->cached_dimensions[2] - dimension_step;
            } else {
                dimension_step = d - cached_dimension;
                if (dimension_step >= 0x15) {
                    dimension_step = 0x14;
                }
                slot->cached_dimensions[2] = slot->cached_dimensions[2] + dimension_step;
            }
        } else {
            slot->cached_dimensions[2] = d;
        }
        if (slot->shadow_enabled != 0 && gModel_bCurrentDrawMode != 1) {
            SVECTOR shadow_center;
            SVECTOR next_shadow_edge;
            SVECTOR previous_shadow_edge;
            ModelEffectAdjustment shadow_dimensions;
            POLY_G3 shadow_triangle;
            s32 clamped_shadow_extent;
            s32 shadow_depth;

            setlen(&shadow_triangle, 6);
            setcode(&shadow_triangle, 0x30);
            shadow_triangle.r0 = B(&gModel_wDrawTintRedGreen, 0);
            shadow_triangle.g0 = B(&gModel_wDrawTintRedGreen, 1);
            shadow_triangle.b0 = gModel_bDrawTintBlue;
            shadow_triangle.r1 = (s16)B(&gModel_wDrawTintRedGreen, 0) / 16;
            shadow_triangle.g1 = (s16)B(&gModel_wDrawTintRedGreen, 1) / 16;
            shadow_triangle.b1 = (s16)gModel_bDrawTintBlue / 16;
            shadow_triangle.r2 = (s16)B(&gModel_wDrawTintRedGreen, 0) / 16;
            shadow_triangle.g2 = (s16)B(&gModel_wDrawTintRedGreen, 1) / 16;
            shadow_triangle.b2 = (s16)gModel_bDrawTintBlue / 16;
            GsSetLsMatrix((MATRIX *)Model_GetLightSourceMatrix());
            Model_GetSlotDimensionsWithMinimums(index, &shadow_dimensions);
            w = shadow_dimensions.x;
            d = shadow_dimensions.z;
            if (w >= 0x12C) {
                clamped_shadow_extent = 0x320;
                if (w < 0x321) {
                    clamped_shadow_extent = w;
                }
            } else {
                clamped_shadow_extent = 0x12C;
            }
            w = clamped_shadow_extent;
            if (d >= 0x12C) {
                clamped_shadow_extent = 0x320;
                if (d < 0x321) {
                    clamped_shadow_extent = d;
                }
            } else {
                clamped_shadow_extent = 0x12C;
            }
            d = clamped_shadow_extent;
            if (gModel_wScreenYOverride != 0x8000) {
                y = (s16)gModel_wScreenYOverride;
            } else {
                y = gModel_aSlots[2].root_coordinate->matrix.t[1] + gModel_pBackgroundRecord->shadow_y_offset;
            }
            a = w / 2;
            b = d / 2;
            shadow_center.vx = x;
            shadow_center.vy = y;
            shadow_center.vz = z;
            previous_shadow_edge.vx = x + a;
            previous_shadow_edge.vy = y;
            previous_shadow_edge.vz = z;
            next_shadow_edge.vy = y;
            for (scan_index = 0; scan_index < 8; scan_index++) {
                shadow_center.vx = x;
                shadow_center.vz = z;
                next_shadow_edge.vx = x + ((ModelShadowFanStep *)gModel_aShadowFanSteps)[scan_index].x * a / 4096;
                next_shadow_edge.vz = z + ((ModelShadowFanStep *)gModel_aShadowFanSteps)[scan_index].z * b / 4096;
                if (gModel_pBackgroundRecord->flags & 1) {
                    shadow_center.vx = CLAMP_FIELD(shadow_center.vx);
                    shadow_center.vz = CLAMP_FIELD(shadow_center.vz);
                    next_shadow_edge.vx = CLAMP_FIELD(next_shadow_edge.vx);
                    next_shadow_edge.vz = CLAMP_FIELD(next_shadow_edge.vz);
                    previous_shadow_edge.vx = CLAMP_FIELD(previous_shadow_edge.vx);
                    previous_shadow_edge.vz = CLAMP_FIELD(previous_shadow_edge.vz);
                    if (next_shadow_edge.vx == previous_shadow_edge.vx && ABS(shadow_center.vx) >= ABS(next_shadow_edge.vx)) {
                        goto skip;
                    }
                    if (next_shadow_edge.vz == previous_shadow_edge.vz && ABS(shadow_center.vz) >= ABS(next_shadow_edge.vz)) {
                        goto skip;
                    }
                }
                shadow_depth = RotAverage3(&shadow_center, &next_shadow_edge, &previous_shadow_edge, (long *)&shadow_triangle.x0, (long *)&shadow_triangle.x1, (long *)&shadow_triangle.x2,
                    &sxy_z, &projection_flags);
                sxy_z = NormalClip(*(long *)&shadow_triangle.x0, *(long *)&shadow_triangle.x1, *(long *)&shadow_triangle.x2);
                if (shadow_depth >= 0 && projection_flags >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&shadow_triangle, ordering_table, 0xFFF, 2);
                }
            skip:
                previous_shadow_edge = next_shadow_edge;
            }
        }
    }
    if (gModel_bCurrentDrawMode == 1) {
        if (index < 2) {
            ordering_table->length = 0xC;
        }
        return;
    }
    if (slot->part_command_reset_pending != 0) {
        Model_ControlSlotAnimation(index, -1, 0);
    } else if (slot->active_animation_command_row != 0) {
        if (slot->active_animation_command_row != slot->current_animation_row) {
            Model_SeekSlotAnimation(index, slot->active_animation_command_row, 0);
        } else if (slot->playback_state != 0x23) {
            slot->queued_animation_row_latch = 0;
        }
    }
    texture_state = slot->stone_texture_state;
    if (texture_state != 0 && texture_state != 0xFF) {
        RECT texture_band_rect;
        u16 palette_words[16];
        RECT palette_strip_rect;
        u16 texture_words[0x400];

        if (texture_state == 0xFE) {
            s32 texture_word_index;
            s32 texture_row;
            u16 *pixel_cursor;

#if MODEL_RENDER_EUROPEAN_TEXTURE_LAYOUT
            texture_band_rect.x = 0x280;
            texture_band_rect.y = (index << 4) + 0xD0;
#else
            texture_band_rect.x = index << 8;
            texture_band_rect.y = 0xF0;
#endif
            texture_band_rect.w = 0x100;
            texture_band_rect.h = 4;
#if MODEL_RENDER_EUROPEAN_TEXTURE_LAYOUT
            for (scan_index = 0, texture_row = texture_y; scan_index < 8; scan_index += 4, texture_row += 4) {
#else
            for (scan_index = 0, texture_row = 0xF0; scan_index < 8; scan_index += 4, texture_row += 4) {
#endif
                texture_band_rect.y = texture_row;
                while (IsIdleGPU(3) != 0) {
                }
                while (StoreImage2(&texture_band_rect, (u32 *)texture_words) != 0) {
                }
                while (IsIdleGPU(3) != 0) {
                }
                pixel_cursor = texture_words;
                for (texture_word_index = 0; texture_word_index < 0x400; texture_word_index++, pixel_cursor++) {
                    if (*pixel_cursor != 0) {
                        *pixel_cursor |= 0x8000;
                    }
                }
                texture_band_rect.y = texture_row;
                while (IsIdleGPU(3) != 0) {
                }
                while (LoadImage2(&texture_band_rect, (u32 *)texture_words) != 0) {
                }
                while (IsIdleGPU(3) != 0) {
                }
            }
            Model_SetSlotPlaybackDirection(index, 0);
            slot->stone_texture_state = 0;
        } else {
            u16 *pixel_cursor;

            slot->stone_texture_state = texture_state - 1;
#if MODEL_RENDER_EUROPEAN_TEXTURE_LAYOUT
            palette_strip_rect.x = 0x280 + slot->stone_texture_state * 2;
            palette_strip_rect.y = (index << 4) + 0xD8;
#else
            palette_strip_rect.x = (index << 8) + slot->stone_texture_state * 2;
            palette_strip_rect.y = 0xF8;
#endif
            palette_strip_rect.w = 2;
            palette_strip_rect.h = 8;
            while (IsIdleGPU(3) != 0) {
            }
            while (StoreImage2(&palette_strip_rect, (u32 *)palette_words) != 0) {
            }
            while (IsIdleGPU(3) != 0) {
            }
            for (scan_index = 0, pixel_cursor = palette_words; scan_index < 0x10; scan_index++, pixel_cursor++) {
                *pixel_cursor = Color_TintBgr555Pixel(*pixel_cursor, 6, 0x1000) & 0x7FFF;
            }
#if MODEL_RENDER_EUROPEAN_TEXTURE_LAYOUT
            palette_strip_rect.y = texture_y;
#else
            palette_strip_rect.y = 0xF0;
#endif
            while (IsIdleGPU(3) != 0) {
            }
            while (LoadImage2(&palette_strip_rect, (u32 *)palette_words) != 0) {
            }
            while (IsIdleGPU(3) != 0) {
            }
            if (slot->stone_texture_state == 0) {
                ModelSlot *remap_slot;
                ModelSlotHeadEntry *remap_unit;
                u8 *group_bytes;
                u8 *next_group_bytes;
                s32 (*remap_handler)(s32);

                remap_slot = &gModel_aSlots[index];
                remap_unit = remap_slot->hmd_units;
                remap_handler = Model_MapToBasePrimitiveHandler;
                if (remap_slot->is_active != 0) {
                    for (scan_index = 0; scan_index < remap_slot->hmd_unit_count; scan_index++, remap_unit++) {
                        group_bytes = remap_unit->primitive_data;
                        if (group_bytes != 0) {
                            for (;;) {
                                if (W(group_bytes, 8) != 0) {
                                    Model_RemapPrimitiveHandlers(remap_handler, (ModelHandlerRunEntry *)(group_bytes + 0xC),
                                                  W(group_bytes, 4), W(group_bytes, 8));
                                }
                                next_group_bytes = (u8 *)W(group_bytes, 0);
                                if (next_group_bytes == (u8 *)-1) {
                                    break;
                                }
                                group_bytes = next_group_bytes;
                            }
                        }
                    }
                }
                slot->stone_texture_state = 0xFE;
            }
        }
    }
    if (index < 2) {
        ordering_table->length = 0xC;
    }
}

#define TEXTURE_TINT_RECT ((RECT *)&gModel_sTextureTintRectX)
#define TEXTURE_TINT_WORDS(buffer) ((u32 *)(buffer))

#include "color_constants.h"
#include "color.h"

/*
 * The per-slot model runtime: the per-frame slot animation tick
 * (Model_UpdateSlotAnimation), the per-slot control step (Model_UpdateSlotControl), slot reset and
 * load (Model_ResetSlot, Model_LoadSlot), the staged slot loader from
 * Model_LoadMonsterMerge through Model_LoadSpecialBattlePackageStage, and
 * Model_ControlSlotAnimation.
 */

#include "model_link_slot_animation_rows.h"
#include "model_word_memory.h"
#include "model_slot_row_tables.h"
#include "shared_work_base.h"
#include "file_package_staging.h"
#include "sound_voice_data.h"
#include "script_state.h"
#include "duel_scene.h"

/* Per-frame animation tick for a model slot. When the slot is active and
   has an animation loaded it takes the current animation's length and this
   tick's frame advance, lets the animation-6 idle/loop cases hand off through
   Model_SetSlotPlaybackDirection, runs the sound trigger for the two duelist slots, advances
   or rewinds the frame counter (state 0x3E plays forward and clamps, other
   states play backward and wrap), and when the animation has run out with
   no hold pending it switches to the queued animation through Model_SeekSlotAnimation
   and its follow-ups. Finally, unless a slow 0x23-state animation is on an
   off tick, it updates every attached part through Model_SeekSlotPartAnimation at the
   new frame. */
void Model_UpdateSlotAnimation(s32 index) {
    ModelSlot *slot;
    s32 animation_row;
    s32 row_end_position;
    s32 position_step;
    s32 part_index;
    s32 scaled_position;
    s32 forward_handoff_position;

    slot = &gModel_aSlots[index];
    if (slot->is_active == 0) {
        return;
    }
    if (slot->animation_commands != 0) {
        animation_row = slot->current_animation_row;
        row_end_position = slot->animation_duration_rows[animation_row].max << 4;
        position_step = slot->playback_speed * Model_GetFrameStep();
        if (animation_row == 6) {
            forward_handoff_position = row_end_position - position_step;
            if ((slot->playback_state == 0x3E && slot->animation_position >= forward_handoff_position) ||
                (slot->playback_state == 0x3C && slot->animation_position < 2)) {
                Model_SetSlotPlaybackDirection(index, 0);
            }
        }
        if (index < 2) {
            Model_PlaySlotAnimationSounds(index);
        }
        if (slot->playback_state != 0x23) {
            if (slot->playback_state == 0x3E) {
                s32 forward_position;

                if (slot->animation_position >= row_end_position) {
                    slot->animation_position = 0;
                }
                forward_position = slot->animation_position + position_step;
                if (forward_position >= row_end_position) {
                    slot->animation_position = row_end_position;
                } else {
                    slot->animation_position = forward_position;
                }
            } else {
                s32 reverse_position;

                if (slot->animation_position == 0) {
                    slot->animation_position = row_end_position;
                }
                reverse_position = slot->animation_position - position_step;
                if (reverse_position <= 0) {
                    slot->animation_position = 0;
                } else {
                    slot->animation_position = reverse_position;
                }
            }
        }
        if (slot->part_command_reset_pending == 0) {
            if (slot->playback_state == 0x3E ? slot->animation_position >= row_end_position : slot->animation_position == 0) {
                if (slot->tint[7] == 6) {
                    slot->queued_animation_row = 2;
                }
                if (slot->queued_animation_row != slot->queued_animation_row_latch) {
                    Model_SeekSlotAnimation(index, slot->queued_animation_row, 0);
                    slot->queued_animation_row_latch = slot->queued_animation_row;
                    if (slot->active_animation_command_row != 0) {
                        Model_ControlSlotAnimation(index, 0, 0);
                    }
                    if (slot->tint[7] == 6) {
                        Model_QueueEffectEndpointPair(-1, index + 2, slot->animation_duration_rows[2].max);
                        slot->tint[7] = 0;
                        slot->queued_animation_row = 1;
                    }
                }
            }
        }
    }
    if (slot->playback_state != 0x23) {
        if (slot->playback_speed == 0x10) {
            if (Model_GetFrameStep() == 2) {
                return;
            }
        }
    }
    scaled_position = slot->animation_position;
    if (slot->animation_sequence_count != 0) {
        part_index = 0;
        do {
            Model_SeekSlotPartAnimation(slot, part_index, slot->current_animation_row, scaled_position);
            part_index++;
        } while (part_index < slot->animation_sequence_count);
    }
}

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

#define MODEL_SLOT_CF8_TAIL_VIEW(address) ((ModelSlotCF8TailView *)(address))

/* The state trace printed on the debug font. The Japanese build carries none
   of these prints, so a regional build defines it away. */
#ifndef MODEL_CONTROL_TRACE
#define MODEL_CONTROL_TRACE(message) FntPrint(message)
#endif

void Model_UpdateSlotControl(s32 index)
{
    ModelSlot *slot = &gModel_aSlots[index];
    ModelSlot *other;
    ModelControlHandler handler;
    ModelControlHandler primary_handler;
    u8 *primary = (u8 *)slot->primary_module_data_address;
    u8 *secondary = (u8 *)slot->variant_module_data_address;
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

    if (slot->is_active == 0) {
        return;
    }
    primary_state = ((ModelControlCommandView *)slot)->commands[2];
    gModel_bActiveSlotIndex = index;
    if (primary_state >= 0) {
        if (index != 0) {
            primary_handler = (ModelControlHandler)(gModel_pSlot1PrimaryModule + 4);
        } else {
            primary_handler = (ModelControlHandler)(gModel_pSlot0PrimaryModule + 4);
        }
        Model_SetHandlerCallbackState(1);
        primary_handler(primary, -1);
        Model_SetHandlerCallbackState(0);
    }
    if (slot->animation_control_state == 6) {
        if (slot->current_animation_row == slot->guardian_star_selection + 3
            || slot->model_id == MODEL_SPECIAL_BATTLE_ID) {
            slot->animation_control_state = 7;
        } else {
            slot->animation_control_state = 2;
        }
    }
    state = slot->animation_control_state;
    if (state == 2) {
        return;
    }
    if (((ModelControlCommandView *)slot)->commands[slot->guardian_star_selection] >= 0) {
        if (index != 0) {
            handler = (ModelControlHandler)((u8 *)gModel_Slot1VariantModuleAddress + 4);
        } else {
            handler = (ModelControlHandler)((u8 *)gModel_Slot0VariantModuleAddress + 4);
        }
        if (slot->animation_control_state == 7) {
            Model_SetHandlerCallbackState(1);
            {
                s32 request = ((ModelControlCommandView *)slot)->commands[slot->guardian_star_selection];
                handler(secondary, request % 1000);
            }
            Model_SetHandlerCallbackState(0);
            slot->animation_control_state = 8;
            slot->animation_sound_position = 0;
        }
        if (slot->current_animation_row != slot->guardian_star_selection + 3
            && slot->model_id != MODEL_SPECIAL_BATTLE_ID) {
            s32 eligible = slot->animation_control_state;
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
            do { base = (s32)gModel_aSlots; } while (0);
            other = (ModelSlot *)(other_offset + base);
            own_offset = index * MODEL_SLOT_SIZE;
            stat_base = base + (u32)&((ModelSlot *)0)->metadata;
            own_record = own_offset + stat_base;
            other_record = other_offset + stat_base;
            if (other->defense_position != 0) {
                {
                    u16 own_value =
                        MODEL_SLOT_CF8_TAIL_VIEW(own_record)->prefix.values.attack_compare_value;
                    u16 other_value =
                        MODEL_SLOT_CF8_TAIL_VIEW(other_record)->prefix.values.defense_compare_value;
                    selected = own_value > other_value ? 6 : 8;
                }
            } else {
                u16 own_value =
                    MODEL_SLOT_CF8_TAIL_VIEW(own_record)->prefix.values.attack_compare_value;
                u16 other_value =
                    MODEL_SLOT_CF8_TAIL_VIEW(other_record)->prefix.values.attack_compare_value;
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
        Model_SetHandlerCallbackState(1);
        slot->animation_control_state = handler(secondary, -1);
        Model_SetHandlerCallbackState(0);
        switch (slot->animation_control_state) {
        case 0:
            MODEL_CONTROL_TRACE(gModel_szContinueTrace);
            return;
        case 5:
            MODEL_CONTROL_TRACE("BACK\n");
            if (other->active_animation_command_row == 5 && other->animation_position >= 0x60) {
                Model_ControlSlotAnimation(index ^ 1, 1, 1);
            }
            return;
        case 3:
            MODEL_CONTROL_TRACE("STONE\n");
            if (action == 6) {
                other->stone_texture_state = 0x80;
                Model_ControlSlotAnimation(index ^ 1, 6, 1);
                return;
            }
            if (action & 0x80) {
                other->stone_texture_state = 0xFF;
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
            do { own_record = offset + (s32)&gModel_aSlots[0].metadata; } while (0);
            offset = (index ^ 1) * MODEL_SLOT_SIZE;
            other_record = offset + (s32)&gModel_aSlots[0].metadata;
            amount = slot->playback_speed * (amount + 1);
            if ((((u8 *)&gModel_aSlots[0].metadata) + offset)[MODEL_SLOT_CF8_DFF_OFFSET] != 0) {
                {
                    u32 own_value =
                        MODEL_SLOT_CF8_TAIL_VIEW(own_record)->prefix.values.attack_compare_value;
                    u32 other_value =
                        MODEL_SLOT_CF8_TAIL_VIEW(other_record)->prefix.values.defense_compare_value;
                    reaction = other_value < own_value ? 6 : 8;
                }
            } else {
                u32 own_value =
                    MODEL_SLOT_CF8_TAIL_VIEW(own_record)->prefix.values.attack_compare_value;
                u32 other_value =
                    MODEL_SLOT_CF8_TAIL_VIEW(other_record)->prefix.values.attack_compare_value;
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
            if (slot->animation_control_state == 1) {
                MODEL_CONTROL_TRACE("HIT\n");
            }
            if (slot->animation_control_state == 4) {
                MODEL_CONTROL_TRACE("OUCH\n");
                if (slot->animation_control_state == 4 && command == 6 && other->active_animation_command_row != 6) {
                    command = 5;
                }
            }
            if (other->active_animation_command_row == command) {
                if (other->playback_state != 0x23 && other->animation_position >= 0xA0 &&
                    ((rand() >> 8) & 3) != 0) {
                    other->animation_position -= amount;
                    frame = other->animation_position;
                    for (part = 0; part < other->animation_sequence_count; part++) {
                        Model_SeekSlotPartAnimation(other, part, other->current_animation_row, frame);
                    }
                    if ((rand() >> 8) & 1) {
                        changed++;
                    }
                }
            } else {
                Model_ControlSlotAnimation(index ^ 1, command, 1);
                changed++;
            }
            if (gModel_bSceneMode == 0xF && changed != 0 && command == 8) {
                u32 card_id;
                s32 sound_id;
                do { card_id = other->model_id; } while (0);
                sound_id = ((gDuel_adwCardStats[card_id] >> 26) & 0x1F) == 1 ? 3 : 4;
                SD_SEPlay(sound_id, 0xFF, 0);
            }
            return;
        }
        case 2:
            MODEL_CONTROL_TRACE("TERM\n");
            return;
        default:
            MODEL_CONTROL_TRACE("WHY?\n");
            slot->animation_control_state = 2;
            return;
        }
    }

    {
        s32 fallback_action;
        s32 offset;
        s32 own_record;
        s32 other_record;
        if (slot->current_animation_row == slot->guardian_star_selection + 3) {
            s32 current_frame = slot->animation_position >> 4;
            s32 midpoint = slot->animation_duration_rows[slot->current_animation_row].max >> 1;
            if (current_frame >= midpoint) {
                offset = index * MODEL_SLOT_SIZE;
                own_record = offset + (s32)&gModel_aSlots[0].metadata;
                offset = (index ^ 1) * MODEL_SLOT_SIZE;
                other_record = offset + (s32)&gModel_aSlots[0].metadata;
                if ((((u8 *)&gModel_aSlots[0].metadata) +
                     offset)[MODEL_SLOT_CF8_DFF_OFFSET] != 0) {
                    {
                        u32 own_value =
                            MODEL_SLOT_CF8_TAIL_VIEW(own_record)->
                                prefix.values.attack_compare_value;
                        u32 other_value =
                            MODEL_SLOT_CF8_TAIL_VIEW(other_record)->
                                prefix.values.defense_compare_value;
                        fallback_action = other_value < own_value ? 6 : 8;
                    }
                } else {
                    u32 own_value =
                        MODEL_SLOT_CF8_TAIL_VIEW(own_record)->
                            prefix.values.attack_compare_value;
                    u32 other_value =
                        MODEL_SLOT_CF8_TAIL_VIEW(other_record)->
                            prefix.values.attack_compare_value;
                    fallback_action = other_value < own_value ? 6 : 5;
                    if (own_value == other_value) {
                        fallback_action |= 0x80;
                        if (index != 0) {
                            fallback_action = 6;
                        }
                    }
                }
                Model_ControlSlotAnimation(index ^ 1, fallback_action & 0x7F, 1);
                slot->animation_control_state = 2;
            }
        } else if (state == 8) {
            slot->animation_control_state = 2;
        }
        if (slot->animation_control_state == 7) {
            slot->animation_control_state = 8;
        }
    }
}

/* One model slot's setup: the reset that gives it its defaults (0x8005611C)
   and the per-frame duel-side layout pass that reads them (0x80056250). The
   two are contiguous -- 0x8005611C is 0x134 bytes and ends exactly at
   0x80056250 -- and are bounded above by Model_LoadMonsterMerge, which is
   assembly again.

   They are one initializer and its consumer. The reset writes the mode byte
   at +0xE16 as 0x3E, the pair at +0xE0C/+0xE0D as 7 and 8, the colour scale
   at +0xE0A as COLOR_FIXED_ONE and the busy byte at +0xE1F as 0; the layout
   pass switches on that same +0xE16 (0x3E is one of its three cases), reads
   +0xE0C, +0xE0D and +0xE0A back, and sets +0xE1F to 1. */

void Model_ResetSlot(s32 slot_index)
{
    ModelSlot *slot_state;
    u8 *reset_cursor;
    s32 reset_index;
    s32 default_byte;

    slot_state = &gModel_aSlots[slot_index];
    Model_FillWords((s32 *)slot_state, 0, 0x388);

    slot_state->ambient_rgb_scale[2] = MODEL_FIXED_HALF;
    slot_state->ambient_rgb_scale[1] = MODEL_FIXED_HALF;
    slot_state->ambient_rgb_scale[0] = MODEL_FIXED_HALF;
    slot_state->render_scale.vz = MODEL_FIXED_ONE;
    slot_state->render_scale.vy = MODEL_FIXED_ONE;
    slot_state->render_scale.vx = MODEL_FIXED_ONE;
    slot_state->tint[2] = 0x80;
    slot_state->tint[1] = 0x80;
    slot_state->tint[0] = 0x80;
    slot_state->texture_tint_control = 7;
    slot_state->playback_speed = 8;
    slot_state->load_step = 0xFF;
    slot_state->tint[3] = 0;
    *(s16 *)&slot_state->texture_tint_scale = COLOR_FIXED_ONE;
    slot_state->skip_sound_setup = 0;
    slot_state->guardian_star_selection = slot_index;
    slot_state->defense_position = 0;
    slot_state->defense_compare_override = 0xFFFF;
    slot_state->attack_compare_override = 0xFFFF;

    default_byte = 1;
    reset_index = 3;
    reset_cursor = (u8 *)slot_state + reset_index;
    for (; reset_index >= 0; reset_cursor--, reset_index--) {
        reset_cursor[0xBF4] = default_byte;
    }

    for (reset_index = 0; reset_index < 0x40; reset_index++) {
        *(s32 *)&slot_state->sound_entries[reset_index] &= 0x8000FFFF;
        slot_state->sound_entries[reset_index].sound_id = 0;
        slot_state->sound_entries[reset_index].animation_row = 0;
    }

    slot_state->metadata.first_star_module_command = -1;
    slot_state->metadata.second_star_module_command = -1;
    slot_state->metadata.primary_module_command = -1;
    slot_state->playback_state = 0x3E;
    slot_state->is_active = 0;
    Model_InitLightTriplet(slot_index);
}

/* Duel-side layout pass for one player's model slot: sums the hand's card
 * widths (0x14 for the cards flagged in the +0xBEC bitfield, 0xC otherwise),
 * derives the two cursor limits at +0xDF0/+0xDF4, resets each card object's
 * sprite fields, and applies the mode-dependent horizontal offset through
 * Model_SetSlotPartSpeeds before Model_ApplyTextureTint draws it. */
void Model_LoadSlot(s32 slot_index, u8 *hmd, s32 hmd_size, s32 render_flags) {
    ModelSlot *span_slot;
    ModelSlot *rearm_slot;
    ModelSlotPart **part_sequence;
    ModelSlot *draw_slot;
    ModelSlot *tint_slot;
    s32 span_part_index;
    s32 rearm_part_index;
    s32 snapshot_bytes;
    s32 mask_byte_index;
    s32 cache_or_speed;
    s32 missing_command_index;
    s32 part_count;
    u8 *slot_bytes;
    u8 *tint_slot_bytes;
    s32 part_speed;

    Model_InitSlot(slot_index, hmd, hmd_size, render_flags);
    if (hmd == (u8 *)0) {
        return;
    }
    Model_BuildSlotAnimationTables(slot_index);
    Model_LinkSlotAnimationRows(slot_index);
    span_slot = &gModel_aSlots[slot_index];
    snapshot_bytes = 0;
    if (slot_index < 2) {
        span_part_index = snapshot_bytes;
        if (snapshot_bytes < span_slot->animation_sequence_count) {
            missing_command_index = 0xFFFF;
            part_count = span_slot->animation_sequence_count;
            do {
                if (span_slot->animation_command_indices[1][span_part_index] != missing_command_index) {
                    mask_byte_index = span_part_index / 8;
                    if ((span_slot->extended_animation_part_mask[mask_byte_index] >> (span_part_index - mask_byte_index * 8)) & 1) {
                        snapshot_bytes += 0x14;
                    } else {
                        snapshot_bytes += 0xC;
                    }
                }
                span_part_index++;
            } while (span_part_index < part_count);
        }
    }
    cache_or_speed = (span_slot->lit_color_cache_address = (s32)span_slot->animation_snapshot_buffer + snapshot_bytes);
    span_slot->lit_color_cache_end_address = cache_or_speed + span_slot->lit_color_cache_entry_count * sizeof(u32);
    Model_SetSlotRootTransform(span_slot, 0, 0, 0, slot_index == 1 ? 0x800 : 0);

    rearm_slot = &gModel_aSlots[slot_index];
    part_sequence = rearm_slot->animation_sequences;
    for (rearm_part_index = 0; rearm_part_index < rearm_slot->animation_sequence_count; rearm_part_index++) {
        (*part_sequence)->ii = 0xFFFF;
        (*part_sequence)->ti = (*part_sequence)->start;
        (*part_sequence)->aframe = 0xFFFF;
        (*part_sequence)->sid = (*part_sequence)->start_sid;
        (*part_sequence)->rframe = 0;
        (*part_sequence)->speed = 0x10;
        part_sequence++;
    }

    slot_bytes = (u8 *)gModel_aSlots;
    draw_slot = (ModelSlot *)(slot_index * MODEL_SLOT_SIZE + slot_bytes);
    draw_slot->is_active = 1;
    Model_DrawSlotUnitsOnly(slot_index);
    cache_or_speed = draw_slot->playback_speed * 2;
    part_speed = cache_or_speed;
    switch (draw_slot->playback_state) {
    case 0x23:
        part_speed = 0;
        Model_SetSlotPartSpeeds(slot_index, part_speed);
        break;
    case 0x3E:
        part_speed = cache_or_speed;
        Model_SetSlotPartSpeeds(slot_index, part_speed);
        break;
    case 0x3C:
        part_speed = -part_speed;
        Model_SetSlotPartSpeeds(slot_index, part_speed);
        break;
    }
    if (slot_index < 2) {
        tint_slot_bytes = (u8 *)gModel_aSlots;
        tint_slot = (ModelSlot *)(slot_index * MODEL_SLOT_SIZE + tint_slot_bytes);
        Model_ApplyTextureTint(slot_index, tint_slot->texture_tint_control, tint_slot->texture_tint_scale);
    }
}

/* Starts the asynchronous transfer that fills one model slot with a monster
 * merge record, and records the slot's display properties while the request is
 * in flight.
 *
 * The slot argument carries a flag in bit 0x80: when it is clear the sound
 * driver's voices are keyed off first, and the flag itself is stored at
 * +0xE1D.  A negative model id means "reuse the id already in the slot" and a
 * non-negative one is written back to +0xDF8, exactly as
 * Model_SetSlotProperties does; the remaining properties follow the same
 * "negative means leave alone" convention it uses for +0xDFA to +0xDFF.
 *
 * Duel slots (0 and 1) load out of the model MRG. Each compacted model entry
 * occupies MODEL_MRG_SECTOR_COUNT sectors, so the three id ranges that have no
 * record - MODEL_MRG_FIRST_GAP_START through the byte before
 * MODEL_MRG_FIRST_GAP_END, MODEL_MRG_SECOND_GAP_START through the byte before
 * MODEL_MRG_SECOND_GAP_END, and MODEL_MRG_SINGLE_GAP_ID - are rejected and
 * every id above such a gap is biased down by MODEL_MRG_GAP_SIZE.
 * MODEL_SPECIAL_BATTLE_ID is the one special case and comes from its own
 * file. Any other slot loads MODEL_AUX_SECTOR_COUNT sectors selected
 * through the MODEL_AUX_LOOKUP_RECORD_SIZE-byte table at gModel_abBackgroundRecords.
 *
 * Returns zero once a transfer has been requested and one when the model id
 * has no record to request. */
s32 Model_LoadMonsterMerge(s32 slot, s32 model, s32 attack_override, s32 defense_override, s32 guardian_star_option,
                           s32 defense_position_option, s32 render_flags)
{
    FileTransferDescriptor *transfer;
    s32 skip_sound_setup_mask;
    s32 request_result;

    request_result = 0;
    skip_sound_setup_mask = slot & 0x80;
    slot &= 0x7F;
    if (model < 0) {
        model = gModel_aSlots[slot].model_id;
    } else {
        gModel_aSlots[slot].model_id = model;
    }
    if (skip_sound_setup_mask == 0) {
        SD_KeyOffVoiceSlots();
    }
    File_WaitForTransfers();
    Model_InitSlot(slot, 0, 0, render_flags);
    gModel_aSlots[slot].skip_sound_setup = skip_sound_setup_mask;
    if (slot < 2) {
        if (model == MODEL_SPECIAL_BATTLE_ID) {
            transfer = File_TryRequestAsyncTransfer(
                1, gModel_szSuMrgPath, MODEL_SPECIAL_BATTLE_FILE_START_SECTOR,
                MODEL_SPECIAL_BATTLE_FILE_SECTOR_COUNT,
                Model_LoadSpecialBattlePackageStage, 0, 0
            );
            gFile_dwTransferState = transfer->status_flags
                | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
            gModel_aSlots[slot].load_step = 0;
            return 0;
        }
        if (model < 0 || model >= MODEL_MRG_ID_END
            || (model >= MODEL_MRG_FIRST_GAP_START
                && model < MODEL_MRG_FIRST_GAP_END)
            || (model >= MODEL_MRG_SECOND_GAP_START
                && model < MODEL_MRG_SECOND_GAP_END)
            || model == MODEL_MRG_SINGLE_GAP_ID) {
            request_result++;
        } else {
            if (model >= MODEL_MRG_LAST_ID) {
                model--;
            }
            if (model >= MODEL_MRG_SECOND_GAP_END) {
                model -= MODEL_MRG_GAP_SIZE;
            }
            if (model >= MODEL_MRG_FIRST_GAP_END) {
                model -= MODEL_MRG_GAP_SIZE;
            }
            transfer = File_TryRequestAsyncTransfer(
                2, gFile_szModelMrgPath, model * MODEL_MRG_SECTOR_COUNT,
                MODEL_MRG_SECTOR_COUNT, Model_LoadMonsterMergeStage,
                0, 0
            );
            if (attack_override >= 0) {
                gModel_aSlots[slot].attack_compare_override = attack_override;
            }
            if (defense_override >= 0) {
                gModel_aSlots[slot].defense_compare_override = defense_override;
            }
            if (guardian_star_option >= 0) {
                gModel_aSlots[slot].guardian_star_selection = guardian_star_option != 0;
            }
            if (defense_position_option >= 0) {
                gModel_aSlots[slot].defense_position = defense_position_option != 0;
            }
            transfer->callback_data = (void *)slot;
            transfer->position = gModel_aSlots[slot].guardian_star_selection;
            gFile_dwTransferState = transfer->status_flags
                | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
            gModel_aSlots[slot].load_step = 0;
        }
    } else {
        if (!(model >= 0 && model < 7
              && (model = gModel_abBackgroundRecords[
                      model * MODEL_AUX_LOOKUP_RECORD_SIZE
                      + MODEL_AUX_LOOKUP_VALUE_OFFSET],
                  model >= 0 && model < 7))) {
            request_result++;
        } else {
            transfer = File_TryRequestAsyncTransfer(
                1, gModel_szSuMrgPath,
                model * MODEL_AUX_SECTOR_COUNT + MODEL_AUX_FILE_START_SECTOR,
                MODEL_AUX_SECTOR_COUNT, Model_LoadAuxiliaryPackageStage, 0, 0
            );
            gFile_dwTransferState = transfer->status_flags
                | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
            gModel_aSlots[slot].load_step = 0;
        }
    }
    return request_result;
}

static inline s16 load_phase(ModelSlot *slot)
{
    return slot->load_step;
}

void Model_AdvanceSlotLoad(s32 index)
{
    ModelSlot *slot = &gModel_aSlots[index];
#ifndef VERSION_JAPAN
    s32 start_vblank = VSync(1);
#endif
    s32 initial_load_step = slot->load_step;
    s32 phase_work;
    s32 phase_value;
    s32 part_speed;

    if (initial_load_step == 0 || initial_load_step == 255) {
        return;
    }
    switch (load_phase(slot)) {
    case 1: {
        u8 *hmd_payload = (u8 *)gModel_AuxiliaryPayloadAddress;
        s32 hmd_size = 0xC000;
        switch (index) {
        case 0:
            hmd_payload = gRuntime_pSharedWorkBase;
            break;
        case 1:
            hmd_payload = gModel_pSlot1PayloadBase;
            break;
        }
        if (*(s32 *)hmd_payload != 0) {
            hmd_size = *(s32 *)hmd_payload;
        }
        Model_InitSlot(index, hmd_payload, hmd_size, -1);
        break;
    }
    case 2:
        Model_BuildSlotAnimationTables(index);
        break;
    case 3:
        Model_LinkSlotAnimationRows(index);
        break;
    case 4: {
        ModelSlot *phase_slot = &gModel_aSlots[index];
        s32 snapshot_bytes = 0;
        s32 part_index, part_count, mask_byte_index, missing_command_index;
        if (index < 2) {
            part_index = snapshot_bytes;
            if (snapshot_bytes < phase_slot->animation_sequence_count) {
                missing_command_index = 0xFFFF;
                part_count = phase_slot->animation_sequence_count;
                do {
                    if (phase_slot->animation_command_indices[1][part_index] != missing_command_index) {
                        mask_byte_index = part_index / 8;
                        if ((phase_slot->extended_animation_part_mask[mask_byte_index] >> (part_index - mask_byte_index * 8)) & 1) {
                            snapshot_bytes += 0x14;
                        } else {
                            snapshot_bytes += 0xC;
                        }
                    }
                    part_index++;
                } while (part_index < part_count);
            }
        }
        phase_value = (phase_slot->lit_color_cache_address = (s32)phase_slot->animation_snapshot_buffer + snapshot_bytes);
        phase_slot->lit_color_cache_end_address = phase_value + phase_slot->lit_color_cache_entry_count * 4;
        Model_SetSlotRootTransform(phase_slot, 0, 0, 0, index == 1 ? 0x800 : 0);
        break;
    }
    case 5: {
        ModelSlot *phase_slot = &gModel_aSlots[index];
        ModelSlotPart **part_sequence = phase_slot->animation_sequences;
        s32 part_index;
        for (part_index = 0; part_index < phase_slot->animation_sequence_count; part_index++) {
            (*part_sequence)->ii = 0xFFFF;
            (*part_sequence)->ti = (*part_sequence)->start;
            (*part_sequence)->aframe = 0xFFFF;
            (*part_sequence)->sid = (*part_sequence)->start_sid;
            (*part_sequence)->rframe = 0;
            (*part_sequence)->speed = 0x10;
            part_sequence++;
        }
        break;
    }
    case 6:
        slot->is_active = 1;
        Model_DrawSlotUnitsOnly(index);
        slot->is_active = 0;
        break;
    case 7: {
        ModelSlot *phase_slot = &gModel_aSlots[index];
        phase_value = phase_slot->playback_speed * 2;
        part_speed = phase_value;
        switch (phase_slot->playback_state) {
        case 0x23:
            part_speed = 0;
            Model_SetSlotPartSpeeds(index, part_speed);
            break;
        case 0x3E:
            part_speed = phase_value;
            Model_SetSlotPartSpeeds(index, part_speed);
            break;
        case 0x3C:
            part_speed = -part_speed;
            Model_SetSlotPartSpeeds(index, part_speed);
            break;
        }
        if (index >= 2) {
            slot->is_active = 1;
        }
        break;
    }
    case 8:
        Model_ApplyTextureTint(index, slot->texture_tint_control, slot->texture_tint_scale);
        break;
    case 9: {
        ModelSlot *phase_slot = &gModel_aSlots[index];
        u8 *handler_context;
        ModelControlHandler module_handler;
        s32 selected_row;
        s32 frame_position;
        s32 part_index;
        phase_work = ((ModelControlCommandView *)phase_slot)->commands[2];
        handler_context = (u8 *)phase_slot->primary_module_data_address;
        gModel_bActiveSlotIndex = index;
        if (phase_work >= 0) {
            if (index != 0) {
                module_handler = (ModelControlHandler)(gModel_pSlot1PrimaryModule + 4);
            } else {
                module_handler = (ModelControlHandler)(gModel_pSlot0PrimaryModule + 4);
            }
            Model_SetHandlerCallbackState(1);
            module_handler(handler_context, ((ModelControlCommandView *)phase_slot)->commands[2] % 1000);
            Model_SetHandlerCallbackState(0);
        }
        selected_row = phase_slot->current_animation_row;
        phase_slot->animation_control_state = 6;
        frame_position = phase_slot->animation_position;
        for (part_index = 0; part_index < phase_slot->animation_sequence_count; part_index++) {
            s32 current_row = phase_slot->current_animation_row;
            if (selected_row != 0) {
                phase_slot->animation_sequences[part_index]->sid = selected_row;
                current_row = selected_row;
            }
            Model_SeekSlotPartAnimation(phase_slot, part_index, current_row, frame_position);
        }
        if (selected_row != 0) {
            phase_slot->current_animation_row = selected_row;
        }
        break;
    }
    case 10:
        if (slot->skip_sound_setup == 0) {
            SD_LoadSequenceBankPair(
                index, (u32 *)(gFile_StagingBuffer + (index << 11))
            );
        }
        break;
    case 11:
        if (slot->skip_sound_setup == 0) {
            slot->animation_sound_enabled = 1;
        }
        slot->is_active = 1;
        break;
    }
#ifndef VERSION_JAPAN
    phase_work = VSync(1);
    printf(gModel_szLoadModelTraceFormat, slot->load_step, phase_work - start_vblank);
#endif
    {
        s32 next_load_step;
        if (slot->is_active != 0) {
            next_load_step = 255;
        } else {
            next_load_step = slot->load_step + 1;
        }
        slot->load_step = next_load_step;
    }
}

/*
 * Contiguous model file-transfer phase callbacks. The first handles MRG
 * transfer phases for the two duel slots. The entry slot and
 * alternate-palette selector are captured before dispatch; the finish stage
 * reloads the slot overrides after the copy and optional state callback.
 *
 * All 1,992 text bytes and the 17-entry table match the uniform G8 profile.
 * Separate cleared/reloaded flag captures retain the upload-stage ordering.
 * The direct image argument and post-call source cursor share the same
 * staging buffer without extending the cursor's lifetime across the upload.
 */
#ifndef MODEL_TEXTURE_STAGE3_RECT_X
#define MODEL_TEXTURE_STAGE3_RECT_X(index) ((index) << 8)
#endif
#ifndef MODEL_TEXTURE_STAGE3_RECT_Y
#define MODEL_TEXTURE_STAGE3_RECT_Y(index) 0xF8
#endif
#ifndef MODEL_TEXTURE_PALETTE_RECT_X
#define MODEL_TEXTURE_PALETTE_RECT_X 0x200
#endif
#ifndef MODEL_TEXTURE_TAIL_ROW_Y
#define MODEL_TEXTURE_TAIL_ROW_Y 0xF0
#endif

void Model_LoadMonsterMergeStage(FileTransferDescriptor *transfer, s32 stage)
{
    RECT texture_row_rect;
    RECT palette_row_rect;
    s32 slot_index;
    s32 variant_selection;
    ModelSlot *loaded_slot;

    slot_index = (s32)transfer->callback_data;
    variant_selection = transfer->position;
    loaded_slot = &gModel_aSlots[slot_index];
    switch (stage) {
    case 0: {
        u8 *payload_destination;
        if (slot_index == 0) {
            payload_destination = gRuntime_pSharedWorkBase;
        } else {
            payload_destination = gModel_pSlot1PayloadBase;
        }
        transfer->sector_destination = transfer->alternate_sector_destination = (u32)payload_destination;
        transfer->phase_size = 96 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        transfer->transfer_mode = 1;
        break;
    }
    case 1:
        transfer->field_30.h.counter = slot_index << 8;
        transfer->field_30.h.field_32 = 0x100;
        transfer->w = 0x40;
        transfer->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        transfer->phase_size = 48 * FILE_SECTOR_SIZE;
        gFile_dwTransferState |= 0x10000;
        transfer->transfer_mode = 2;
        transfer->sector_destination = (u32)gFile_TransferBufferBase;
        transfer->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        break;
    case 2:
        transfer->sector_destination = transfer->alternate_sector_destination = (u32)gFile_abPackageStagingBuffer;
        transfer->phase_size = 2 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        transfer->transfer_mode = 1;
        break;
    case 3: {
        u32 transfer_clear_mask;
        u8 *next_sector_source;
        RECT *texture_row_view;
        do { texture_row_view = &texture_row_rect; } while (0);
        texture_row_rect.x = MODEL_TEXTURE_STAGE3_RECT_X(slot_index);
        texture_row_rect.y = MODEL_TEXTURE_STAGE3_RECT_Y(slot_index);
        texture_row_rect.w = 0x100;
        texture_row_rect.h = 8;
        LoadImage2(texture_row_view, (u32 *)gFile_abPackageStagingBuffer);
        next_sector_source = gFile_abPackageStagingBuffer;
        do { transfer_clear_mask = 0xFFDCFFFF; } while (0);
        next_sector_source += 2 * FILE_SECTOR_SIZE;
        transfer->sector_destination = transfer->alternate_sector_destination = (u32)next_sector_source;
        transfer->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= transfer_clear_mask;
        transfer->transfer_mode = 1;
        break;
    }
    case 4:
        if (variant_selection == 0) {
            palette_row_rect.x = MODEL_TEXTURE_PALETTE_RECT_X;
            palette_row_rect.y = slot_index + 0xF2;
            palette_row_rect.w = 0x100;
            palette_row_rect.h = 1;
            LoadImage2(&palette_row_rect, (u32 *)gModel_abTexturePaletteStaging);
            transfer->field_30.h.counter = (slot_index << 8) + 0xC0;
            transfer->w = 0x40;
            transfer->h = 0x10;
            {
                u32 cleared_transfer_state, reloaded_transfer_state, image_transfer_bit;
                cleared_transfer_state = gFile_dwTransferState & 0xFFDDFFFF;
                gFile_dwTransferState = cleared_transfer_state;
                do { reloaded_transfer_state = gFile_dwTransferState; } while (0);
                do { image_transfer_bit = 0x10000; } while (0);
                transfer->field_30.h.field_32 = 0x100;
                gFile_dwTransferState = reloaded_transfer_state | image_transfer_bit;
            }
            transfer->transfer_mode = 2;
            transfer->phase_size = 16 * FILE_SECTOR_SIZE;
            transfer->sector_destination = (u32)gFile_TransferBufferBase;
            transfer->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        } else {
            transfer->phase_size = 16 * FILE_SECTOR_SIZE;
            gFile_dwTransferState &= 0xFFFCFFFF;
            gFile_dwTransferState |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        }
        break;
    case 6:
        if (variant_selection == 1) {
            palette_row_rect.x = MODEL_TEXTURE_PALETTE_RECT_X;
            palette_row_rect.y = slot_index + 0xF2;
            palette_row_rect.w = 0x100;
            palette_row_rect.h = 1;
            LoadImage2(&palette_row_rect, (u32 *)gFile_abPackageStagingBuffer);
            transfer->field_30.h.counter = (slot_index << 8) + 0xC0;
            transfer->w = 0x40;
            transfer->h = 0x10;
            {
                u32 cleared_transfer_state, reloaded_transfer_state, image_transfer_bit;
                cleared_transfer_state = gFile_dwTransferState & 0xFFDDFFFF;
                gFile_dwTransferState = cleared_transfer_state;
                do { reloaded_transfer_state = gFile_dwTransferState; } while (0);
                do { image_transfer_bit = 0x10000; } while (0);
                transfer->field_30.h.field_32 = 0x100;
                gFile_dwTransferState = reloaded_transfer_state | image_transfer_bit;
            }
            transfer->transfer_mode = 2;
            transfer->phase_size = 16 * FILE_SECTOR_SIZE;
            transfer->sector_destination = (u32)gFile_TransferBufferBase;
            transfer->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        } else {
            transfer->phase_size = 16 * FILE_SECTOR_SIZE;
            gFile_dwTransferState &= 0xFFFCFFFF;
            gFile_dwTransferState |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        }
        break;
    case 7:
        if (variant_selection == 0 && slot_index == 0) {
            transfer->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState &= 0xFFDCFFFF;
            transfer->sector_destination = transfer->alternate_sector_destination = gModel_Slot0VariantModuleAddress;
            transfer->transfer_mode = 1;
        } else {
            transfer->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState &= 0xFFFCFFFF;
            gFile_dwTransferState |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        }
        break;
    case 8:
        if (variant_selection == 0 && slot_index == 1) {
            transfer->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState &= 0xFFDCFFFF;
            transfer->sector_destination = transfer->alternate_sector_destination = gModel_Slot1VariantModuleAddress;
            transfer->transfer_mode = 1;
        } else {
            transfer->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState &= 0xFFFCFFFF;
            gFile_dwTransferState |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        }
        break;
    case 9:
        if (variant_selection == 1 && slot_index == 0) {
            transfer->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState &= 0xFFDCFFFF;
            transfer->sector_destination = transfer->alternate_sector_destination = gModel_Slot0VariantModuleAddress;
            transfer->transfer_mode = 1;
        } else {
            transfer->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState &= 0xFFFCFFFF;
            gFile_dwTransferState |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        }
        break;
    case 10:
        if (variant_selection == 1 && slot_index == 1) {
            transfer->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState &= 0xFFDCFFFF;
            transfer->sector_destination = transfer->alternate_sector_destination = gModel_Slot1VariantModuleAddress;
            transfer->transfer_mode = 1;
        } else {
            transfer->phase_size = 10 * FILE_SECTOR_SIZE;
            gFile_dwTransferState &= 0xFFFCFFFF;
            gFile_dwTransferState |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        }
        break;
    case 11:
        if (slot_index == 0) {
            transfer->phase_size = 2 * FILE_SECTOR_SIZE;
            gFile_dwTransferState &= 0xFFDCFFFF;
            transfer->sector_destination = transfer->alternate_sector_destination = (u32)gModel_pSlot0PrimaryModule;
            transfer->transfer_mode = 1;
        } else {
            transfer->phase_size = 2 * FILE_SECTOR_SIZE;
            gFile_dwTransferState &= 0xFFFCFFFF;
            gFile_dwTransferState |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        }
        break;
    case 12:
        if (slot_index == 1) {
            transfer->phase_size = 2 * FILE_SECTOR_SIZE;
            gFile_dwTransferState &= 0xFFDCFFFF;
            transfer->sector_destination = transfer->alternate_sector_destination = (u32)gModel_pSlot1PrimaryModule;
            transfer->transfer_mode = 1;
        } else {
            transfer->phase_size = 2 * FILE_SECTOR_SIZE;
            gFile_dwTransferState &= 0xFFFCFFFF;
            gFile_dwTransferState |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        }
        break;
    case 13:
        if (loaded_slot->skip_sound_setup != 0) {
            transfer->phase_size = FILE_SECTOR_SIZE;
            gFile_dwTransferState &= 0xFFFCFFFF;
            gFile_dwTransferState |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        } else {
            transfer->sector_destination = transfer->alternate_sector_destination =
                (u32)(gFile_StagingBuffer + slot_index * FILE_SECTOR_SIZE);
            transfer->phase_size = FILE_SECTOR_SIZE;
            gFile_dwTransferState &= 0xFFDCFFFF;
            transfer->transfer_mode = 1;
        }
        break;
    case 14:
        if (loaded_slot->skip_sound_setup != 0) {
            u32 transfer_clear_mask;
            do { transfer_clear_mask = 0xFFFCFFFF; } while (0);
            transfer->phase_size = 50 * FILE_SECTOR_SIZE;
            gFile_dwTransferState &= transfer_clear_mask;
            gFile_dwTransferState |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
            transfer->transfer_mode = 1;
        } else {
            transfer->transfer_mode = 3;
            transfer->field_30.word = slot_index * 0x19000 + 0xD810;
            transfer->phase_size = 50 * FILE_SECTOR_SIZE;
            transfer->sector_destination = (u32)gFile_TransferBufferBase;
            transfer->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        }
        break;
    case 5:
    case 15:
        transfer->sector_destination = transfer->alternate_sector_destination = (u32)gFile_abPackageStagingBuffer;
        transfer->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        transfer->transfer_mode = 1;
        break;
    case 16: {
        u8 *metadata_source;
        metadata_source = gFile_abPackageStagingBuffer;
        Model_CopyWords(
            (s32 *)loaded_slot->sound_entries,
            (const s32 *)metadata_source,
            MODEL_SLOT_SOUND_ENTRY_COUNT
        );
        *(ModelSlotMetadataBlockWords *)&loaded_slot->metadata =
            ((ModelTransferMetadata *)metadata_source)->metadata;
        if (loaded_slot->metadata.prefix.values.field_08 & 0x20200000) {
            Model_SetSlotDrawMode(slot_index, 4);
        }
        if (loaded_slot->attack_compare_override < 0xFFFF) {
            loaded_slot->metadata.prefix.values.attack_compare_value = loaded_slot->attack_compare_override;
        }
        if (loaded_slot->defense_compare_override < 0xFFFF) {
            loaded_slot->metadata.prefix.values.defense_compare_value = loaded_slot->defense_compare_override;
        }
        if (loaded_slot->skip_sound_setup != 0) {
            loaded_slot->metadata.first_star_module_command = -1;
            loaded_slot->metadata.second_star_module_command = -1;
            loaded_slot->metadata.primary_module_command = -1;
        }
        loaded_slot->load_step = 1;
        break;
    }
    }
}

/* Two transfer-phase callbacks of the FreeDuel_LoadPackageStage family, kept
   in one translation unit because they are the same routine over different
   assets: each takes the descriptor plus a phase index, and each switch arm
   programs the same fields -- phase_size, the sector_destination/alternate_sector_destination source
   window, done, and the field_30 halfword pair -- while clearing and setting
   the same bits of gFile_dwTransferState.

   Both own a .rodata jump table, and the two tables are adjacent in the image
   (0x1F84 for the six-case switch, 0x1F9C for the eleven-case one). Together
   with Model_LoadMonsterMergeStage's 17-entry table and the retail zero word after it, the
   complete object owns the 0x8C-byte .rodata run at 0x1F3C. Definition order
   is address order and must stay that way. */

void Model_LoadAuxiliaryPackageStage(FileTransferDescriptor *transfer, s32 stage) {
    RECT first_palette_rect;
    RECT second_palette_rect;

    switch (stage) {
    case 0:
        transfer->phase_size = 18 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        transfer->alternate_sector_destination = gModel_AuxiliaryPayloadAddress;
        transfer->sector_destination = gModel_AuxiliaryPayloadAddress;
        transfer->transfer_mode = 1;
        break;

    case 1:
        transfer->field_30.h.counter = 0x200;
        transfer->field_30.h.field_32 = 0x100;
        transfer->w = 0x40;
        transfer->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        gFile_dwTransferState |= 0x10000;
        transfer->transfer_mode = 2;
        transfer->phase_size = 64 * FILE_SECTOR_SIZE;
        transfer->sector_destination = (u32)gFile_TransferBufferBase;
        transfer->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        break;

    case 2:
        transfer->alternate_sector_destination = (s32)gModel_abAuxiliaryPaletteSector;
        transfer->sector_destination = (s32)gModel_abAuxiliaryPaletteSector;
        transfer->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        transfer->transfer_mode = 1;
        break;

    case 3:
        first_palette_rect.x = MODEL_TEXTURE_PALETTE_RECT_X;
        first_palette_rect.y = 0xF1;
        first_palette_rect.w = 0x100;
        first_palette_rect.h = 1;
        LoadImage2(&first_palette_rect, (u32 *)gModel_abAuxiliaryPaletteSector);
        transfer->alternate_sector_destination = (s32)(gModel_abAuxiliaryPaletteSector - FILE_SECTOR_SIZE);
        transfer->sector_destination = (s32)(gModel_abAuxiliaryPaletteSector - FILE_SECTOR_SIZE);
        transfer->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        transfer->transfer_mode = 1;
        break;

    case 4:
        second_palette_rect.x = MODEL_TEXTURE_PALETTE_RECT_X;
        second_palette_rect.y = 0xF4;
        second_palette_rect.w = 0x100;
        second_palette_rect.h = 2;
        LoadImage2(&second_palette_rect, (u32 *)gFile_abPackageStagingBuffer);
        transfer->field_30.h.counter = 0x380;
        transfer->field_30.h.field_32 = 0;
        transfer->w = 0x40;
        transfer->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        gFile_dwTransferState |= 0x10000;
        transfer->transfer_mode = 2;
        transfer->phase_size = 32 * FILE_SECTOR_SIZE;
        transfer->sector_destination = (u32)gFile_TransferBufferBase;
        transfer->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        break;

    case 5:
        gModel_aSlots[2].load_step = 1;
        break;
    }
}

void Model_LoadSpecialBattlePackageStage(FileTransferDescriptor *transfer, s32 stage) {
    RECT texture_row_rect;
    RECT palette_rows_rect;
    ModelSlot *slot_zero;
    u8 *metadata_source;

    switch (stage) {
    case 0:
        transfer->phase_size = 96 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        transfer->alternate_sector_destination = (s32)gRuntime_pSharedWorkBase;
        transfer->sector_destination = (s32)gRuntime_pSharedWorkBase;
        transfer->transfer_mode = 1;
        break;

    case 1:
        transfer->field_30.h.field_32 = 0x100;
        transfer->w = 0x40;
        transfer->field_30.h.counter = 0;
        transfer->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        gFile_dwTransferState |= 0x10000;
        transfer->transfer_mode = 2;
        transfer->phase_size = 96 * FILE_SECTOR_SIZE;
        transfer->sector_destination = (u32)gFile_TransferBufferBase;
        transfer->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        break;

    case 2:
        transfer->alternate_sector_destination = (s32)gFile_abPackageStagingBuffer;
        transfer->sector_destination = (s32)gFile_abPackageStagingBuffer;
        transfer->phase_size = 2 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        transfer->transfer_mode = 1;
        break;

    case 3:
#ifdef VERSION_EUROPE
        /* The European build stores the row's x first. */
        texture_row_rect.x = MODEL_TEXTURE_STAGE3_RECT_X(0);
        texture_row_rect.y = MODEL_TEXTURE_STAGE3_RECT_Y(0);
        texture_row_rect.w = 0x100;
#else
        texture_row_rect.y = MODEL_TEXTURE_STAGE3_RECT_Y(0);
        texture_row_rect.w = 0x100;
        texture_row_rect.x = MODEL_TEXTURE_STAGE3_RECT_X(0);
#endif
        texture_row_rect.h = 8;
        LoadImage2(&texture_row_rect, (u32 *)gFile_abPackageStagingBuffer);
        transfer->phase_size = 10 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        transfer->alternate_sector_destination = gModel_Slot0VariantModuleAddress;
        transfer->sector_destination = gModel_Slot0VariantModuleAddress;
        transfer->transfer_mode = 1;
        break;

    case 4:
        transfer->phase_size = 10 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        transfer->alternate_sector_destination = gModel_Slot1VariantModuleAddress;
        transfer->sector_destination = gModel_Slot1VariantModuleAddress;
        transfer->transfer_mode = 1;
        break;

    case 6:
        palette_rows_rect.x = MODEL_TEXTURE_STAGE3_RECT_X(1);
        palette_rows_rect.y = MODEL_TEXTURE_TAIL_ROW_Y;
        palette_rows_rect.w = 0x100;
        palette_rows_rect.h = 2;
        LoadImage2(&palette_rows_rect, (u32 *)gFile_abPackageStagingBuffer);
        transfer->field_30.h.counter = 0x180;
        transfer->w = 0x40;
        transfer->field_30.h.field_32 = 0x100;
        transfer->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        gFile_dwTransferState |= 0x10000;
        transfer->transfer_mode = 2;
        transfer->phase_size = 8 * FILE_SECTOR_SIZE;
        transfer->sector_destination = (u32)gFile_TransferBufferBase;
        transfer->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        break;

    case 7:
        transfer->alternate_sector_destination = (s32)gFile_StagingBuffer;
        transfer->sector_destination = (s32)gFile_StagingBuffer;
        transfer->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        transfer->transfer_mode = 1;
        break;

    case 8:
        transfer->transfer_mode = 3;
        transfer->field_30.word = 0xD810;
        transfer->phase_size = 50 * FILE_SECTOR_SIZE;
        transfer->sector_destination = (u32)gFile_TransferBufferBase;
        transfer->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        break;

    case 5:
    case 9:
        transfer->alternate_sector_destination = (s32)gFile_abPackageStagingBuffer;
        transfer->sector_destination = (s32)gFile_abPackageStagingBuffer;
        transfer->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        transfer->transfer_mode = 1;
        break;

    case 10:
        slot_zero = gModel_aSlots;
        metadata_source = gFile_abPackageStagingBuffer;
        Model_CopyWords(
            (s32 *)slot_zero->sound_entries,
            (const s32 *)metadata_source,
            MODEL_SLOT_SOUND_ENTRY_COUNT
        );
        *(ModelSlotMetadataBlockWords *)&slot_zero->metadata =
            ((ModelTransferMetadata *)metadata_source)->metadata;
        slot_zero->metadata.first_star_module_command = -1;
        slot_zero->metadata.second_star_module_command = -1;
        slot_zero->metadata.primary_module_command = -1;
        slot_zero->metadata.prefix.values.attack_compare_value = 0;
        slot_zero->metadata.prefix.values.defense_compare_value = 0;
        slot_zero->load_step = 1;
        break;
    }
}

/* Starts, restores or stops one model slot's part animation.
 *
 * -1 clears every part's command index. 0 restores the words the active row
 * saved, three per part or five for parts flagged in extended_animation_part_mask, and clears
 * the active row. Any other row saves the parts' current words, records each
 * part's command index and makes that row active, restoring the previous row
 * first when the caller allows it.
 *
 * Each loop keeps its own word count, row entry and rounded part index: the
 * part bit is found by shifting with the word count while it still holds 3,
 * which is the retail srav/sllv form, and a shared variable would give the
 * word count the slot's callee-saved register. */
void Model_ControlSlotAnimation(s32 index, s32 anim, s32 flag) {
    ModelSlot *slot;
    ModelSlotPart **part_sequences;
    u8 *snapshot_cursor;
    u8 *parameter_base;
    u16 *command_halfwords;
    s32 part_index;
    s32 previous_row;
    s32 restore_word_count;
    s32 save_word_count;
    s32 restore_command_index;
    s32 save_command_index;
    s32 save_rounded_part;
    s32 restore_rounded_part;

    slot = &gModel_aSlots[index];
    snapshot_cursor = slot->animation_snapshot_buffer;
    parameter_base = slot->animation_parameter_base;
    part_sequences = slot->animation_sequences;
    if (slot->is_active == 0) {
        return;
    }
    switch (anim) {
    case -1:
        slot->part_command_reset_pending = 0;
        for (part_index = 0; part_index < slot->animation_sequence_count; part_sequences++, part_index++) {
            (*part_sequences)->ii = 0xFFFF;
        }
        break;
    case 0:
        if (slot->active_animation_command_row == 0) {
            return;
        }
        for (part_index = 0; part_index < slot->animation_sequence_count; part_index++) {
            restore_command_index = slot->animation_command_indices[slot->active_animation_command_row][part_index];
            command_halfwords = (u16 *)&slot->animation_commands[restore_command_index];
            if (restore_command_index != 0xFFFF) {
                restore_rounded_part = part_index;
                if (part_index < 0) {
                    restore_rounded_part = part_index + 7;
                }
                restore_word_count = 3;
                if ((slot->extended_animation_part_mask[restore_rounded_part >> restore_word_count] >>
                     (part_index - ((restore_rounded_part >> restore_word_count) << restore_word_count))) &
                    1) {
                    restore_word_count = 5;
                }
                Model_CopyWords((s32 *)(parameter_base + command_halfwords[0] * 4), (const s32 *)snapshot_cursor,
                                restore_word_count);
                snapshot_cursor += restore_word_count * 4;
            }
        }
        slot->active_animation_command_row = 0;
        break;
    default:
        previous_row = slot->active_animation_command_row;
        if (anim != slot->guardian_star_selection + 3) {
            slot->animation_control_state = 2;
        }
        if (slot->animation_duration_rows[anim].max == 0) {
            return;
        }
        if (previous_row != 0) {
            if (flag == 0) {
                return;
            }
            slot->queued_animation_row_latch = slot->queued_animation_row;
            Model_ControlSlotAnimation(index, 0, 0);
        }
        if (anim == slot->guardian_star_selection + 3
            || slot->model_id == MODEL_SPECIAL_BATTLE_ID) {
            slot->animation_control_state = 7;
        }
        slot->active_animation_command_row = anim;
        if (previous_row == anim) {
            Model_SeekSlotAnimation(index, 0, 0);
            return;
        }
        slot->part_command_reset_pending = 1;
        for (part_index = 0; part_index < slot->animation_sequence_count; part_sequences++, part_index++) {
            save_command_index = slot->animation_command_indices[slot->active_animation_command_row][part_index];
            command_halfwords = (u16 *)&slot->animation_commands[save_command_index];
            if (save_command_index != 0xFFFF) {
                save_rounded_part = part_index;
                if (part_index < 0) {
                    save_rounded_part = part_index + 7;
                }
                save_word_count = 3;
                if ((slot->extended_animation_part_mask[save_rounded_part >> save_word_count] >>
                     (part_index - ((save_rounded_part >> save_word_count) << save_word_count))) &
                    1) {
                    save_word_count = 5;
                }
                (*part_sequences)->ii = ((u8 *)command_halfwords - (u8 *)slot->animation_commands) >> 2;
                Model_CopyWords((s32 *)snapshot_cursor,
                                (const s32 *)(parameter_base + command_halfwords[0] * 4), save_word_count);
                snapshot_cursor += save_word_count * 4;
            }
        }
        break;
    }
}

/* Copies the four cached halfwords at record[idx]+0xDC8 into *out, then
   clamps each axis up to the per-record minimum the slot's
   CF8 prefix carries for it: min_x, min_y and min_z, each a byte multiplied
   by 16, with 0 meaning "no clamp". Finally sets out->max to the largest of
   {x (if positive), y, z}. */
void Model_GetSlotDimensionsWithMinimums(s32 idx, ModelEffectAdjustment *out) {
    ModelSlot *dimension_slot = &gModel_aSlots[idx];
    ModelSlotMetadataPrefix *metadata_prefix = &dimension_slot->metadata.prefix;

    *out = *(ModelEffectAdjustment *)dimension_slot->cached_dimensions;

    if (metadata_prefix->thresholds.min_x != 0) {
        s32 minimum_x = metadata_prefix->thresholds.min_x << 4;
        if (out->x < minimum_x) {
            out->x = minimum_x;
        }
    }
    if (metadata_prefix->thresholds.min_y != 0) {
        s32 minimum_y = metadata_prefix->thresholds.min_y << 4;
        if (out->y < minimum_y) {
            out->y = minimum_y;
        }
    }
    if (metadata_prefix->thresholds.min_z != 0) {
        s32 minimum_z = metadata_prefix->thresholds.min_z << 4;
        if (out->z < minimum_z) {
            out->z = minimum_z;
        }
    }

    out->max = 0;
    if (out->x > 0) {
        out->max = out->x;
    }
    if (out->max < out->y) {
        out->max = out->y;
    }
    if (out->max < out->z) {
        out->max = out->z;
    }
}

/* The first of the two steps of the camera-relative model transform.
 *
 * Model_UpdateViewMetrics recomputes the cached distance, yaw and pitch of
 * the current view. Model_BuildViewRelativeCoordinateUnit combines those
 * angles with a selected coordinate entry, retaining its slot-zero rotation
 * reads and lifetime-sensitive automatic parent link.
 *
 * The view is a GsRVIEW2 and the six offsets read here are its members:
 * 0x00, 0x04 and 0x08 are vpx, vpy and vpz, 0x0C, 0x10 and 0x14 are vrx,
 * vry and vrz. Three things say so -- this function already casts m to
 * GsRVIEW2 * to copy it whole into gModel_RView, camera_view.h declares that
 * global as GsRVIEW2, and the arithmetic reads as the standard camera
 * derivation: the horizontal distance and the yaw both take vp - vr in x
 * and z, and the pitch takes the y pair against that distance.
 *
 * They are read as *(s32 *)&view->vpx rather than view->vpx, and that is
 * measured. The plain member reads keep the instruction count at 226 but
 * rename registers across the whole function. Taking each member's address
 * keeps every read a scalar reference, as the old (m + offset) casts were,
 * and builds byte-identically. It is the same device
 * the two DisplayObject_Render*GouraudQuadList functions use; they too store to
 * fixed addresses between the reads.
 */
#include "model_view_coordinate_helpers.h"

void Model_UpdateViewMetrics(GsRVIEW2 *m) {
    GsRVIEW2 *cached_view;
    GsRVIEW2 *selected_view;

    cached_view = &gModel_RView;
    if (m == 0) {
        m = cached_view;
    } else if (cached_view != 0 && m != cached_view) {
        gModel_RView = *m;
    }
    selected_view = m;

    *(s16 *)&gModel_wViewDistance = SquareRoot0((*(s32 *)&selected_view->vpx - *(s32 *)&selected_view->vrx) * (*(s32 *)&selected_view->vpx - *(s32 *)&selected_view->vrx) + (*(s32 *)&selected_view->vpz - *(s32 *)&selected_view->vrz) * (*(s32 *)&selected_view->vpz - *(s32 *)&selected_view->vrz));

    gModel_wViewYaw = ratan2(*(s32 *)&selected_view->vpz - *(s32 *)&selected_view->vrz,
                        *(s32 *)&selected_view->vpx - *(s32 *)&selected_view->vrx);

    gModel_wViewPitch = ratan2(*(s32 *)&selected_view->vpy - *(s32 *)&selected_view->vry,
                        *(s16 *)&gModel_wViewDistance);

    *(s16 *)&gModel_wViewDistance = SquareRoot0((*(s32 *)&selected_view->vpx - *(s32 *)&selected_view->vrx) * (*(s32 *)&selected_view->vpx - *(s32 *)&selected_view->vrx) + (*(s32 *)&selected_view->vpy - *(s32 *)&selected_view->vry) * (*(s32 *)&selected_view->vpy - *(s32 *)&selected_view->vry) + (*(s32 *)&selected_view->vpz - *(s32 *)&selected_view->vrz) * (*(s32 *)&selected_view->vpz - *(s32 *)&selected_view->vrz));

    gModel_wViewYaw = (*(s16 *)&gModel_wViewYaw + MODEL_ANGLE_FULL_TURN) %
        MODEL_ANGLE_FULL_TURN;
    gModel_wViewPitch = (*(s16 *)&gModel_wViewPitch + MODEL_ANGLE_FULL_TURN) %
        MODEL_ANGLE_FULL_TURN;
}

static __inline__ void setup_rotation(
    SVECTOR *source,
    SVECTOR *copy,
    s32 turn,
    GsCOORDUNIT *unit
)
{
    source->vy = turn - gModel_wViewYaw;
    source->vz = gModel_wViewPitch;
    *copy = *source;

    unit->flg = 0;
    unit->rot.vx =
        turn - ((volatile ModelSlot *)gModel_aSlots)->root_coordinate->rot.vx;
    unit->rot.vy =
        turn - ((volatile ModelSlot *)gModel_aSlots)->root_coordinate->rot.vy;
    unit->rot.vz =
        turn - (u16)((volatile ModelSlot *)gModel_aSlots)->root_coordinate->rot.vz;
    RotMatrixZXY(&unit->rot, &unit->matrix);
}

static __inline__ void build_work_matrix(SVECTOR *rotation, MATRIX *work)
{
    RotMatrix_gte(rotation, work);
}

void Model_BuildViewRelativeCoordinateUnit(s32 index, s32 entry_index, SVECTOR *local_position, GsCOORDUNIT *output_coordinate)
{
    GsCOORDUNIT translation_parent;
    SVECTOR world_position;
    MATRIX entry_world_matrix;
    SVECTOR view_rotation_copy;
    SVECTOR view_rotation;
    MATRIX view_work_matrix;
    u8 transform_flags[8];
    ModelSlot *slot;

    slot = &gModel_aSlots[index];
    if (slot->entry_count < entry_index) {
        entry_index = slot->root_coordinate_index;
    }

    GsGetLwUnit(
        (GsCOORDUNIT *)(slot->entries + entry_index * MODEL_SLOT_DATA_ENTRY_SIZE),
        &entry_world_matrix
    );
    GsSetLsMatrix(&entry_world_matrix);

    RotTransSV(local_position, &world_position, (long *)transform_flags);

    translation_parent.rot.vz = 0;
    translation_parent.rot.vy = 0;
    translation_parent.rot.vx = 0;
    RotMatrix_gte(&translation_parent.rot, &translation_parent.matrix);

    translation_parent.flg = 1;
    translation_parent.matrix.t[0] = world_position.vx;
    translation_parent.matrix.t[1] = world_position.vy;
    translation_parent.matrix.t[2] = world_position.vz;
    translation_parent.workm = translation_parent.matrix;
    translation_parent.super = NULL;
    memset(&view_rotation, 0, sizeof(view_rotation));

    setup_rotation(&view_rotation, &view_rotation_copy, MODEL_ANGLE_FULL_TURN, output_coordinate);

    build_work_matrix(&view_rotation_copy, &view_work_matrix);
    MulMatrix(&output_coordinate->matrix, &view_work_matrix);

    output_coordinate->matrix.t[2] = 0;
    output_coordinate->matrix.t[1] = 0;
    output_coordinate->matrix.t[0] = 0;
    output_coordinate->super = &translation_parent;
}

#ifdef VERSION_EUROPE
void Model_ApplyTextureTint(s32 side_index, s32 tint_control, s32 saturation_scale)
{
    /* The European build keeps each side's two rows at x 0x280, y
       (side_index << 4) + 0xD0 and 0xD8, where the US build has x side_index << 8 and
       y 0xF0 and 0xF8. */
    u16 pixel_buffer[0x400];
    u16 *pixel_cursor;
    s32 band_offset;
    s32 pixel_index;
    s32 destination_base_y;

    if ((u32)side_index < 2) {
        destination_base_y = (side_index << 4) + 0xD0;

        if (tint_control >= COLOR_TINT_KEEP_HUE &&
            (tint_control & COLOR_TINT_INVERT) == 0 && saturation_scale >= COLOR_FIXED_ONE) {
            goto big;
        }

        band_offset = 0;
        gModel_sTextureTintRectX = 0x280;
        gModel_sTextureTintRectY = destination_base_y + 8;
        gModel_sTextureTintRectW = 0x100;
        gModel_sTextureTintRectH = 4;

        do {
            {
                s32 source_row_offset = band_offset + 8;

                gModel_sTextureTintRectY = destination_base_y + source_row_offset;
            }
            while (IsIdleGPU(3) != 0) {
            }
            while (StoreImage2(TEXTURE_TINT_RECT, TEXTURE_TINT_WORDS(pixel_buffer)) != 0) {
            }
            while (IsIdleGPU(3) != 0) {
            }

            pixel_cursor = pixel_buffer;
            for (pixel_index = 0; pixel_index < 0x400; pixel_index++, pixel_cursor++) {
                *pixel_cursor = Color_TintBgr555Pixel(*pixel_cursor, (u8)tint_control, (u16)saturation_scale);
            }

            gModel_sTextureTintRectY = destination_base_y + band_offset;
            while (IsIdleGPU(3) != 0) {
            }
            while (LoadImage2(TEXTURE_TINT_RECT, TEXTURE_TINT_WORDS(pixel_buffer)) != 0) {
            }
            while (IsIdleGPU(3) != 0) {
            }
            band_offset += 4;
        } while (band_offset < 8);
    }

    return;

big:
    gModel_sTextureTintRectX = 0x280;
    gModel_sTextureTintRectY = destination_base_y + 8;
    gModel_sTextureTintRectW = 0x100;
    gModel_sTextureTintRectH = 8;
    MoveImage(TEXTURE_TINT_RECT, 0x280, destination_base_y);
}
#else
void Model_ApplyTextureTint(s32 side_index, s32 tint_control, s32 saturation_scale)
{
    u16 pixel_buffer[0x400];
    u16 *pixel_cursor;
    s32 band_offset;
    s32 pixel_index;

    if ((u32)side_index < 2) {
        band_offset = 0;

        if (tint_control >= COLOR_TINT_KEEP_HUE &&
            (tint_control & COLOR_TINT_INVERT) == 0 && saturation_scale >= COLOR_FIXED_ONE) {
            goto big;
        }

        gModel_sTextureTintRectX = side_index << 8;
        gModel_sTextureTintRectY = 0xF8;
        gModel_sTextureTintRectW = 0x100;
        gModel_sTextureTintRectH = 4;

        do {
            gModel_sTextureTintRectY = band_offset + 0xF8;
            while (IsIdleGPU(3) != 0) {
            }
            while (StoreImage2(TEXTURE_TINT_RECT, TEXTURE_TINT_WORDS(pixel_buffer)) != 0) {
            }
            while (IsIdleGPU(3) != 0) {
            }

            pixel_cursor = pixel_buffer;
            for (pixel_index = 0; pixel_index < 0x400; pixel_index++, pixel_cursor++) {
                *pixel_cursor = Color_TintBgr555Pixel(*pixel_cursor, (u8)tint_control, (u16)saturation_scale);
            }

            gModel_sTextureTintRectY = band_offset + 0xF0;
            while (IsIdleGPU(3) != 0) {
            }
            while (LoadImage2(TEXTURE_TINT_RECT, TEXTURE_TINT_WORDS(pixel_buffer)) != 0) {
            }
            while (IsIdleGPU(3) != 0) {
            }
            band_offset += 4;
        } while (band_offset < 8);
    }

    return;

big:
    gModel_sTextureTintRectY = 0xF8;
    gModel_sTextureTintRectW = 0x100;
    gModel_sTextureTintRectX = side_index << 8;
    gModel_sTextureTintRectH = 8;
    MoveImage(TEXTURE_TINT_RECT, side_index << 8, 0xF0);
}
#endif

/* &gModel_RView.vrx. Keep the interior symbol so the object retains the retail
 * relocation target while the canonical GsRVIEW2 owns the surrounding view. */

void Model_StepViewOrbit(s32 direction_scale, s32 yaw_delta, s32 pitch_delta, s32 distance_delta, s32 unused)
{
    long *pivot_position;
    long *orbit_position;
    s32 cosine;
    s32 sine;
    s32 distance;
    s32 horizontal_or_x;
    s32 vertical_offset;
    s32 z_offset;
    s32 angle_sum;

    /* pivot_position and orbit_position are the two long triples at the head of the reference view,
       vpx..vpz and vrx..vrz: orbit_position is moved around pivot_position. */
    if (direction_scale > 0) {
        pivot_position = &gModel_RView.vrx;
        orbit_position = pivot_position - 3;
    } else {
        pivot_position = &gModel_RView.vpx;
        orbit_position = pivot_position + 3;
    }

    if (*(s16 *)&gModel_wViewDistance + distance_delta >= 0xB) {
        *(u16 *)&gModel_wViewDistance = *(u16 *)&gModel_wViewDistance + distance_delta;
    }

    if ((yaw_delta & MODEL_ANGLE_MASK) != 0) {
        angle_sum = *(s16 *)&gModel_wViewYaw + yaw_delta + MODEL_ANGLE_FULL_TURN;
        gModel_wViewYaw = angle_sum - angle_sum / MODEL_ANGLE_FULL_TURN *
            MODEL_ANGLE_FULL_TURN;
    }

    if ((pitch_delta & MODEL_ANGLE_MASK) != 0) {
        angle_sum = *(s16 *)&gModel_wViewPitch + pitch_delta + MODEL_ANGLE_FULL_TURN;
        gModel_wViewPitch = angle_sum - angle_sum / MODEL_ANGLE_FULL_TURN *
            MODEL_ANGLE_FULL_TURN;
    }

    if (yaw_delta != 0 || pitch_delta != 0 || distance_delta != 0) {
        cosine = rcos(*(s16 *)&gModel_wViewPitch);
        sine = rsin(*(s16 *)&gModel_wViewPitch);
        distance = *(s16 *)&gModel_wViewDistance;
        horizontal_or_x = distance * cosine / MODEL_FIXED_ONE;
        vertical_offset = distance * sine / MODEL_FIXED_ONE;
        cosine = rcos(*(s16 *)&gModel_wViewYaw);
        sine = rsin(*(s16 *)&gModel_wViewYaw);
        z_offset = horizontal_or_x * sine / MODEL_FIXED_ONE;
        horizontal_or_x = horizontal_or_x * cosine / MODEL_FIXED_ONE;
        orbit_position[0] = pivot_position[0] + horizontal_or_x * direction_scale;
        orbit_position[1] = pivot_position[1] + vertical_offset * direction_scale;
        orbit_position[2] = pivot_position[2] + z_offset * direction_scale;
    }
}

#define POSITIONAL_SOUND_PAN_PTR(buffer) ((s8 *)&(buffer)[1])

void SD_ApplyPositionalSound(
    s32 sound_id, s32 play, s32 world_x, s32 world_z)
{
    u8 st[2];
    s8 *pp;
    s32 dx;
    s32 dz;
    s32 a;
    s32 b;
    s32 c;
    s32 d;
    s32 ex;
    s32 ez;
    s32 n;
    s32 r;
    s32 dd;
    s32 v;
    s32 w;

    dx = gModel_RView.vpx - world_x;
    dz = gModel_RView.vpz - world_z;
    pp = POSITIONAL_SOUND_PAN_PTR(st);
    st[0] = 0;
    st[1] = 0;
    r = SquareRoot0(dx * dx + dz * dz);
    b = gModel_RView.vpz;
    c = gModel_RView.vrx;
    d = gModel_RView.vrz;
    ez = d - b;
    a = gModel_RView.vpx;
    ex = a - c;
    n = c * b - a * d;
    dd = SquareRoot0(ez * ez + ex * ex);
    r -= 0x2BC;

    st[0] = 0xFF;
    *pp = 0;
    if (r > 0) {
        v = 0xFF - r / 12;
        if (v < 0x10) {
            v = 0x10;
        }
        st[0] = v;
    }

    if (dd != 0) {
        r = (world_x * ez + world_z * ex + n) / dd;
        w = r / 16;
        /* The conditional form negates w's register instead of v in place. */
        v = __builtin_abs(w);
        if (v >= 0x80) {
            v = 0x7F;
        }
        w = v;
        if (r <= 0) {
            w = -w;
        }
        *pp = w;
    }

    if (play != 0) {
        SD_SEPlay(sound_id & 0xFFFF, st[0], *POSITIONAL_SOUND_PAN_PTR(st));
    } else {
        SD_SESetVolumePan(sound_id & 0xFFFF, st[0],
                     *POSITIONAL_SOUND_PAN_PTR(st));
    }
}

void Model_QueueTintRequestForParts(
    s32 slot, s32 mode, ModelTintColor start, ModelTintColor end, s32 duration, ...)
{
    u8 part_mask[8];
    u8 *argument_cursor;
    s32 part_index;
    s32 mask_byte_index;
    s32 clear_index;
    u8 *clear_cursor;

    clear_index = 7;
    clear_cursor = part_mask + 7;
    for (; clear_index >= 0; clear_index--)
        *clear_cursor-- = 0;
    argument_cursor = (u8 *)&duration + 4;
    while (1) {
        argument_cursor += 4;
        part_index = *(s32 *)(argument_cursor - 4);
        if (part_index < 0)
            break;
        mask_byte_index = part_index >> 3;
        part_mask[mask_byte_index] |= 1 << (part_index - mask_byte_index * 8);
    }
    start.blend_mode = mode & 127;
    Model_QueueTintRequest(slot, mode & 128, start, end, duration, part_mask);
}

void Model_QueueTintRequest(
    s32 slot, s32 selection, ModelTintColor start, ModelTintColor end, s32 duration,
    const u8 *part_mask)
{
    ModelSlot *slot_state;
    ModelTintRequest *request;
    s32 i;
    s32 j;

    slot_state = &gModel_aSlots[slot];
    for (i = 0; i < MODEL_TINT_REQUEST_COUNT; i++) {
        request = &gModel_aTintRequests[i];
        /* The live bit is tested through the record's first byte, as
           Model_UpdateTintRequests also does. */
        if ((*(u8 *)request & 1) == 0) {
            request->active = 1;
            request->slot = slot;
            request->masked_parts_only = selection;
            request->animation_row = slot_state->current_animation_row;
            request->animation_position_snapshot = slot_state->animation_position;
            request->elapsed = 0;
            request->duration = duration * 2;
            request->start = start;
            request->end = end;
            for (j = 0; j < 8; j++) {
                gModel_aTintRequests[i].part_mask[j] = part_mask ? part_mask[j] : 0xFF;
            }
            break;
        }
    }
}
