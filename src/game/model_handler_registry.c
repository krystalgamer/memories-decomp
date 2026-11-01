/* The model handler registry and the two maps that search it.
 *
 * gModel_aHandlerKeyRegistry is the eighty-entry table; Model_RegisterHandlerKey fills it and
 * Model_FindHandlerKey reads it back. Model_MapToWireframePrimitiveHandler and Model_MapToBasePrimitiveHandler open-code
 * that same reverse lookup -- same sentinel, same eighty-entry scan, same -1 --
 * and then dispatch on the packed id it returns, which is why they belong here
 * rather than beside the handlers they name. */
#include "../types.h"
#include "../ygo_types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "func_8006151C.h"
#include "func_80061A84.h"
#include "model.h"

/* Zero-initialized storage this unit owns at 0x800F5918-0x800F5B98, defined in
 * address order ahead of the headers that declare it (#2602). */
ModelHandlerRegistryEntry tent_ModelHandlerRegistry[80];

#define FUNC_800603DC_RETURNS_HANDLER
#include "model_primitive_handler.h"
#include "model_handler_registry.h"
#include "direct_polygon_renderers.h"
#include "shared_primitive_renderers.h"
#include "wireframe_renderers.h"
#include "primitive_handler_modes.h"
#include "../unmatched.h"

#define MODEL_TYPE2_RECORD_VIEW(record) ((ModelType2Record *)(record))

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MODEL_HANDLER_DISPATCH)
/* Maps an id to its handler in the second dispatch family: looks the id up
 * in the handler registry at gModel_aHandlerKeyRegistry (GsU_00000000 is the sentinel that
 * skips the search), then dispatches on the high halfword's group and the
 * low halfword's kind. Returns handler_value unchanged when nothing matches. */

s32 Model_MapToWireframePrimitiveHandler(s32 handler_value) {
    ModelHandlerRegistryEntry *registry_entry;
    s32 registry_index;
    s32 type_key;
    u32 type_family;

    registry_entry = gModel_aHandlerKeyRegistry;
    registry_index = 0;

    if (handler_value == (s32)GsU_00000000) {
        type_key = -1;
    } else {
        do {
            if (registry_entry->handler_value == handler_value) {
                type_key = registry_entry->key;
                goto have;
            }
            registry_index++;
            registry_entry++;
        } while (registry_index < MODEL_HANDLER_REGISTRY_COUNT);
        type_key = -1;
    }

have:
    type_family = type_key & 0xFFFF0000;

    switch (type_family) {
    case 0:
    case 0x100000:
    case 0x200000:
    case 0x300000:
        switch (type_key & 0xFFFF) {
        case 9:
            return (s32)Model_DrawFt3Wireframes;
        case 0x209:
            return (s32)Model_DrawFt3WindowRecordWireframes;
        case 0x11:
            return (s32)Model_DrawFt4Wireframes;
        case 0x211:
            return (s32)Model_DrawFt4WindowRecordWireframes;
        case 0xD:
            return (s32)Model_DrawGt3Wireframes;
        case 0x20D:
            return (s32)Model_DrawGt3WindowRecordWireframes;
        case 0x15:
            return (s32)Model_DrawGt4Wireframes;
        case 0x215:
            return (s32)Model_DrawGt4WindowRecordWireframes;
        }
        break;
    case 0x1000000:
    case 0x1100000:
    case 0x1200000:
    case 0x1300000:
        switch (type_key & 0xFFFF) {
        case 0xD:
            return (s32)Model_DrawSharedGt3Wireframes;
        case 0x20D:
            return (s32)Model_DrawSharedGt3WindowRecordWireframes;
        case 0x15:
            return (s32)Model_DrawSharedGt4Wireframes;
        case 0x215:
            return (s32)Model_DrawSharedGt4WindowRecordWireframes;
        }
        break;
    }
    return handler_value;
}

/* Maps the registered callback to the handler family selected by the packed
 * registry id. The case order is retail's own arm order in memory. */

s32 Model_MapToBasePrimitiveHandler(s32 handler_value) {
    ModelHandlerRegistryEntry *registry_entry;
    s32 registry_index;
    s32 type_key;
    u32 type_family;

    registry_entry = gModel_aHandlerKeyRegistry;
    registry_index = 0;

    if (handler_value == (s32)GsU_00000000) {
        type_key = -1;
    } else {
        do {
            if (registry_entry->handler_value == handler_value) {
                type_key = registry_entry->key;
                goto have;
            }
            registry_index++;
            registry_entry++;
        } while (registry_index < MODEL_HANDLER_REGISTRY_COUNT);
        type_key = -1;
    }

have:
    type_family = type_key & 0xFFFF0000;

    switch (type_family) {
    case 0x200000:
        switch (type_key & 0xFFFF) {
        case 0x9:
            return (s32)Model_DrawFt3Primitives;
        case 0x209:
            return (s32)Model_DrawFt3WindowRecordPrimitives;
        case 0x11:
            return (s32)Model_DrawFt4Primitives;
        case 0x211:
            return (s32)Model_DrawFt4WindowRecordPrimitives;
        case 0xD:
            return (s32)Model_DrawGt3Primitives;
        case 0x20D:
            return (s32)Model_DrawGt3WindowRecordPrimitives;
        case 0x15:
            return (s32)Model_DrawGt4Primitives;
        case 0x215:
            return (s32)Model_DrawGt4WindowRecordPrimitives;
        }
        break;
    case 0x300000:
        switch (type_key & 0xFFFF) {
        case 0x9:
            return (s32)Model_DrawTwoSidedFt3Primitives;
        case 0x209:
            return (s32)Model_DrawTwoSidedFt3WindowRecordPrimitives;
        case 0x11:
            return (s32)Model_DrawTwoSidedFt4Primitives;
        case 0x211:
            return (s32)Model_DrawTwoSidedFt4WindowRecordPrimitives;
        case 0xD:
            return (s32)Model_DrawTwoSidedGt3Primitives;
        case 0x20D:
            return (s32)Model_DrawTwoSidedGt3WindowRecordPrimitives;
        case 0x15:
            return (s32)Model_DrawTwoSidedGt4Primitives;
        case 0x215:
            return (s32)Model_DrawTwoSidedGt4WindowRecordPrimitives;
        }
        break;
    case 0x1200000:
        switch (type_key & 0xFFFF) {
        case 0xD:
            return (s32)Model_DrawSharedGt3Primitives;
        case 0x20D:
            return (s32)Model_DrawSharedGt3WindowRecordPrimitives;
        case 0x15:
            return (s32)Model_DrawSharedGt4Primitives;
        case 0x215:
            return (s32)Model_DrawSharedGt4WindowRecordPrimitives;
        }
        break;
    case 0x1300000:
        switch (type_key & 0xFFFF) {
        case 0xD:
            return (s32)Model_DrawSharedTwoSidedGt3Primitives;
        case 0x20D:
            return (s32)Model_DrawSharedTwoSidedGt3WindowRecordPrimitives;
        case 0x15:
            return (s32)Model_DrawSharedTwoSidedGt4Primitives;
        case 0x215:
            return (s32)Model_DrawSharedTwoSidedGt4WindowRecordPrimitives;
        }
        break;
    }
    return handler_value;
}
#endif

/* Finds `type_key` in the table; if absent and there is a free slot, claims it
   with (type_key, handler_value). No-op once all 80 slots are taken and no match exists. */
#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MODEL_REGISTER_HANDLER_KEY)
void Model_RegisterHandlerKey(s32 type_key, s32 handler_value) {
    s32 registry_index;
    ModelHandlerRegistryEntry *registry_entry = gModel_aHandlerKeyRegistry;
    for (registry_index = 0; registry_index < MODEL_HANDLER_REGISTRY_COUNT; registry_index++, registry_entry++) {
        if (registry_entry->key == type_key) {
            return;
        }
        if (registry_entry->key != 0) {
            continue;
        }
        if (registry_entry->handler_value != 0) {
            continue;
        }
        registry_entry->key = type_key;
        registry_entry->handler_value = handler_value;
        return;
    }
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MODEL_FIND_HANDLER_KEY)
/* Reverse lookup: finds the entry whose handler_value matches, returns its key (or -1
   if handler_value is the sentinel, or if no entry matches after scanning all 80). */
s32 Model_FindHandlerKey(s32 handler_value) {
    ModelHandlerRegistryEntry *registry_entry = gModel_aHandlerKeyRegistry;
    s32 registry_index;
    if (handler_value == (s32) GsU_00000000) {
        return -1;
    }
    for (registry_index = 0; registry_index < MODEL_HANDLER_REGISTRY_COUNT; registry_index++, registry_entry++) {
        if (registry_entry->handler_value == handler_value) {
            return registry_entry->key;
        }
    }
    return -1;
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MODEL_PROCESS_TYPE2_UNIT)
void Model_ProcessType2Unit(
    s32 model_index, ModelTypeUnit *unit, u8 *scratch)
{
    u8 *image_record_cursor;
    u8 *next_record_cursor;
    u8 *clut_data;
    u16 *palette_entry;
    ModelType2Scratch *image_context;
    ModelType2Record *clut_record;
    s32 record_count;
    s32 record_index;
    s32 palette_row;
    s32 palette_column;
    s32 below_two_slot;
    s32 slot_texture_x;
    s32 coordinate_value;
    s32 source_image_x;
    s32 image_with_clut_kind;

    image_with_clut_kind = 0x2000001;
    record_index = 0;
    image_context = (ModelType2Scratch *)scratch;
    next_record_cursor = image_context->records;
    record_count = ((ModelType2RecordHeader *)next_record_cursor)->record_count;
    next_record_cursor += sizeof(ModelType2RecordHeader);

    if (record_count != 0) {
        below_two_slot = model_index < 2;
#ifndef VERSION_EUROPE
        slot_texture_x = model_index << 8;
#endif
        image_record_cursor = next_record_cursor;

        do {
            next_record_cursor = image_record_cursor + sizeof(ModelType2Record);
            if (unit->type == image_with_clut_kind) {
                clut_data = image_context->clut_data;
                if (clut_data != (u8 *)0) {
                    palette_row = 0;
                    clut_record = MODEL_TYPE2_RECORD_VIEW(next_record_cursor);
                    palette_entry = (u16 *)&((u32 *)clut_data)[clut_record->index_offset];
                    if (clut_record->row_count > 0) {
                        do {
                            if (below_two_slot) {
                                *palette_entry = 0;
                            }
                            palette_entry++;
                            for (
                                palette_column = 1;
                                palette_column < clut_record->column_count;
                                palette_column++
                            ) {
                                *palette_entry |= 0x8000;
                                palette_entry++;
                            }
                            palette_row++;
                        } while (palette_row < clut_record->row_count);
                    }
                    if (below_two_slot) {
#ifdef VERSION_EUROPE
                        clut_record->vram_x = 0x280;
                        coordinate_value = clut_record->vram_y;
                        if (coordinate_value >= 8) {
                            clut_record->vram_y = coordinate_value % 8;
                        }
                        clut_record->vram_y += 0xD0 + (model_index << 4);
#else
                        coordinate_value = clut_record->vram_x;
                        if ((s16)clut_record->vram_x >= 0x280) {
                            clut_record->vram_x = (coordinate_value - 0x280) + slot_texture_x;
                        } else {
                            clut_record->vram_x = slot_texture_x;
                        }
                        coordinate_value = clut_record->vram_y;
                        if (coordinate_value >= 8) {
                            clut_record->vram_y = coordinate_value % 8;
                        }
                        clut_record->vram_y = clut_record->vram_y + 0xF8;
#endif
#ifdef VERSION_EUROPE
                    } else {
                        if (clut_record->vram_y < 0x100) {
                            clut_record->vram_x = clut_record->vram_x + 0x280;
                            clut_record->vram_y =
                                (clut_record->vram_y & 0xF) + 0xD0;
                        }
                        coordinate_value = MODEL_TYPE2_RECORD_VIEW(image_record_cursor)->vram_y +
                            MODEL_TYPE2_RECORD_VIEW(image_record_cursor)->row_count - 0x200;
                        if (coordinate_value > 0) {
                            MODEL_TYPE2_RECORD_VIEW(image_record_cursor)->row_count =
                                MODEL_TYPE2_RECORD_VIEW(image_record_cursor)->row_count - coordinate_value;
                        }
#endif
                    }
                    next_record_cursor += sizeof(ModelType2Record);
                }
            }
            if (below_two_slot) {
                source_image_x = MODEL_TYPE2_RECORD_VIEW(image_record_cursor)->vram_x;
#ifdef VERSION_EUROPE
                MODEL_TYPE2_RECORD_VIEW(image_record_cursor)->vram_x =
                    (source_image_x - 0x280) + (model_index << 8);
#else
                MODEL_TYPE2_RECORD_VIEW(image_record_cursor)->vram_x = (source_image_x - 0x280) + slot_texture_x;
#endif
            }
            record_index++;
            image_record_cursor = next_record_cursor;

        } while (record_index < record_count);
    }

    Model_GetImagePrimitiveHandler(unit->type)(scratch);
    DrawSync(0);
    *(ModelHandler *)unit->ptr = (ModelHandler)GsU_00000000;
}
#endif
