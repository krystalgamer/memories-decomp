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
#include "../types.h"
#include "graphics_packet_submission.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/memory.h"
#include "../game/model.h"
#include "../game/model_graphics_state.h"
#include "../game/model_background.h"
#include "../game/model_handler_state.h"
#include "../game/model_primitive_templates.h"
#include "primitive_handler_modes.h"
#include "../game/model_record_tables.h"
#define ORDERING_TABLE_SLOT2_ARRAY
#include "../game/ordering_tables.h"
#include "../game/model_get_frame_step.h"
#include "../game/model_slot_control.h"
#include "../game/model_get_slot_dimensions_with_minimums.h"
#include "../game/model_update_coordinate_unit_matrix.h"
#include "../game/model_slot_state_updates.h"
#include "../game/model_remap_primitive_handlers.h"
#include "../game/color.h"
#include "../game/gpu_packets.h"
#include "../game/screen_projection.h"
#include "../game/model_handler_registry.h"
#include "../game/model_draw_slot.h"
#include "graphics_ordering_table_views.h"

#define B(p, o) (*((u8 *)(p) + (o)))
#define S(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define W(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define GS_COORD_UNIT_VIEW(unit) ((GsCOORDUNIT *)(unit))
#define VECTOR_VIEW(vector) ((VECTOR *)(vector))

#ifndef MODEL_RENDER_BLEND_MODE_5_WORD
#define MODEL_RENDER_BLEND_MODE_5_WORD 0x3C200000
#endif

/* Defined rather than declared so the assembler pads the loads that feed
 * their gp-relative stores; model_primitive_templates.c and
 * model_handler_state.c hold the initialised definitions these commons merge
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

void Model_DrawSlot(s32 index)
{
    MATRIX hidden_matrix;
    PSXLONG sxy_z;
    PSXLONG projection_flags;
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
        ordering_table = gGraphics_pOrderingTable2[0];
    } else {
        ordering_table = gGraphics_pOrderingTable2[1];
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
            goto neutral;
        }
    } else {
    neutral:
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
        if (slot->shadow_enabled != 0) {
            SVECTOR shadow_center;
            SVECTOR next_shadow_edge;
            SVECTOR previous_shadow_edge;
            ModelEffectAdjustment shadow_dimensions;
            POLY_G3 shadow_triangle;
            s32 clamped_shadow_extent;
            s32 shadow_depth;

            if (gModel_bCurrentDrawMode == 1) {
                goto finish_ot;
            }
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
                shadow_depth = RotAverage3(&shadow_center, &next_shadow_edge, &previous_shadow_edge, (PSXLONG *)&shadow_triangle.x0, (PSXLONG *)&shadow_triangle.x1, (PSXLONG *)&shadow_triangle.x2,
                    &sxy_z, &projection_flags);
                sxy_z = NormalClip(*(PSXLONG *)&shadow_triangle.x0, *(PSXLONG *)&shadow_triangle.x1, *(PSXLONG *)&shadow_triangle.x2);
                if (shadow_depth >= 0 && projection_flags >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&shadow_triangle, ordering_table, 0xFFF, 2);
                }
            skip:
                previous_shadow_edge = next_shadow_edge;
            }
        }
    }
    if (gModel_bCurrentDrawMode == 1) {
    finish_ot:
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
