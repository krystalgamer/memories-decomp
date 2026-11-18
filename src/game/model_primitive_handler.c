#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/stdio.h"
#include "sorted_entry_render.h"
#include "model_primitive_handler.h"
#include "direct_polygon_renderers.h"
#include "shared_primitive_renderers.h"
#include "model_handler_registry.h"

/* Rewrites a type-2 image unit's VRAM and CLUT records for the model slot,
 * then hands the unit to the image driver Model_GetImagePrimitiveHandler
 * selects and retires it with the GsU_00000000 sentinel. */
void Model_ProcessType2Unit(
    s32 model_index, GsTYPEUNIT *unit, GsARGUNIT *scratch)
{
    ModelType2Record *image_record_cursor;
    ModelType2Record *next_record_cursor;
    unsigned long *clut_data;
    u16 *palette_entry;
    GsARGUNIT_IMAGE *image_context;
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
    image_context = (GsARGUNIT_IMAGE *)scratch;
    /* One cursor steps from the stream header to its records; a separate
       stream local takes the header load in another register. */
    next_record_cursor = (ModelType2Record *)image_context->primp;
    record_count = ((ModelType2RecordStream *)next_record_cursor)->header.record_count;
    next_record_cursor = ((ModelType2RecordStream *)next_record_cursor)->records;

    if (record_count != 0) {
        below_two_slot = model_index < 2;
#ifndef VERSION_EUROPE
        slot_texture_x = model_index << 8;
#endif
        image_record_cursor = next_record_cursor;

        do {
            next_record_cursor = image_record_cursor + 1;
            if (unit->type == image_with_clut_kind) {
                clut_data = image_context->cluttop;
                if (clut_data != 0) {
                    palette_row = 0;
                    clut_record = next_record_cursor;
                    palette_entry = (u16 *)&clut_data[clut_record->index_offset];
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
                        coordinate_value = image_record_cursor->vram_y +
                            image_record_cursor->row_count - 0x200;
                        if (coordinate_value > 0) {
                            image_record_cursor->row_count =
                                image_record_cursor->row_count - coordinate_value;
                        }
#endif
                    }
                    next_record_cursor++;
                }
            }
            if (below_two_slot) {
                source_image_x = image_record_cursor->vram_x;
#ifdef VERSION_EUROPE
                image_record_cursor->vram_x =
                    (source_image_x - 0x280) + (model_index << 8);
#else
                image_record_cursor->vram_x = (source_image_x - 0x280) + slot_texture_x;
#endif
            }
            record_index++;
            image_record_cursor = next_record_cursor;

        } while (record_index < record_count);
    }

    Model_GetImagePrimitiveHandler(unit->type)(scratch);
    DrawSync(0);
    *unit->ptr = (unsigned long)GsU_00000000;
}

u32 *(*Model_GetImagePrimitiveHandler(u32 v))(GsARGUNIT *)
{
    switch (v) {
    case 0x2000000:
        return GsU_02000000;
    case 0x2000001:
        return GsU_02000001;
    }
#ifdef VERSION_EUROPE
    printf("unsupported IMAGE primitive 0x%08lx.\n", v);
#endif
    return GsU_00000000;
}

void *Model_GetPrimitiveHandler(u32 primitive_code) {
    s32 fog_type;
    u32 plain_type;
    u32 two_sided_type;
    u32 semi_type;
    u32 two_sided_semi_type;
    u32 family_word;

    family_word = primitive_code & 0xFFFF0000;
    switch (family_word) {
    case 0:
        plain_type = primitive_code & 0xFFFF;
        switch (plain_type) {
        case 0:
            return GsU_00000000;
        case 9:
            return Model_DrawFt3Primitives;
        case 0xD:
            return Model_DrawGt3Primitives;
        case 0x11:
            return Model_DrawFt4Primitives;
        case 0x15:
            return Model_DrawGt4Primitives;
        case 0x209:
            return Model_DrawFt3WindowRecordPrimitives;
        case 0x20D:
            return Model_DrawGt3WindowRecordPrimitives;
        case 0x211:
            return Model_DrawFt4WindowRecordPrimitives;
        case 0x215:
            return Model_DrawGt4WindowRecordPrimitives;
        }
        break;
    case 0x20000:
        fog_type = primitive_code & 0xFFFF;
        switch (fog_type) {
        case 0xD:
            return Model_DrawFogTexturedTriangles;
        case 0x15:
            return Model_DrawFogTexturedQuads;
        }
        break;
    case 0x100000:
        two_sided_type = primitive_code & 0xFFFF;
        switch (two_sided_type) {
        case 9:
            return Model_DrawTwoSidedFt3Primitives;
        case 0xD:
            return Model_DrawTwoSidedGt3Primitives;
        case 0x11:
            return Model_DrawTwoSidedFt4Primitives;
        case 0x15:
            return Model_DrawTwoSidedGt4Primitives;
        case 0x209:
            return Model_DrawTwoSidedFt3WindowRecordPrimitives;
        case 0x20D:
            return Model_DrawTwoSidedGt3WindowRecordPrimitives;
        case 0x211:
            return Model_DrawTwoSidedFt4WindowRecordPrimitives;
        case 0x215:
            return Model_DrawTwoSidedGt4WindowRecordPrimitives;
        }
        break;
    case 0x200000:
        semi_type = primitive_code & 0xFFFF;
        switch (semi_type) {
        case 9:
            return Model_DrawSemiFt3Primitives;
        case 0xD:
            return Model_DrawSemiGt3Primitives;
        case 0x11:
            return Model_DrawSemiFt4Primitives;
        case 0x15:
            return Model_DrawSemiGt4Primitives;
        case 0x209:
            return Model_DrawSemiFt3WindowRecordPrimitives;
        case 0x20D:
            return Model_DrawSemiGt3WindowRecordPrimitives;
        case 0x211:
            return Model_DrawSemiFt4WindowRecordPrimitives;
        case 0x215:
            return Model_DrawSemiGt4WindowRecordPrimitives;
        }
        break;
    case 0x300000:
        two_sided_semi_type = primitive_code & 0xFFFF;
        switch (two_sided_semi_type) {
        case 9:
            return Model_DrawTwoSidedSemiFt3Primitives;
        case 0xD:
            return Model_DrawTwoSidedSemiGt3Primitives;
        case 0x11:
            return Model_DrawTwoSidedSemiFt4Primitives;
        case 0x15:
            return Model_DrawTwoSidedSemiGt4Primitives;
        case 0x209:
            return Model_DrawTwoSidedSemiFt3WindowRecordPrimitives;
        case 0x20D:
            return Model_DrawTwoSidedSemiGt3WindowRecordPrimitives;
        case 0x211:
            return Model_DrawTwoSidedSemiFt4WindowRecordPrimitives;
        case 0x215:
            return Model_DrawTwoSidedSemiGt4WindowRecordPrimitives;
        }
        break;
    }
#ifdef VERSION_EUROPE
    printf("unsupported POLYGON primitive 0x%08lx.\n", primitive_code);
#endif
    return GsU_00000000;
}

void Model_BindPrimitiveHandler(ModelHandlerObject *object)
{
    *object->handler = Model_GetPrimitiveHandler(object->key);
    Model_RegisterHandlerKey(object->key, (int)*object->handler);
}

s32 Model_GetSharedPrimitiveHandler(s32 primitive_code) {
    u32 family_word;
    u32 primitive_type;

    family_word = primitive_code & 0xFFFF0000;
    switch (family_word) {
    case 0x1000000:
        primitive_type = primitive_code & 0xFFFF;
        switch (primitive_type) {
        case 0x0:
            return (s32)Model_PrepareSharedPrimitiveCaches;
        case 0xD:
            return (s32)Model_DrawSharedGt3Primitives;
        case 0x15:
            return (s32)Model_DrawSharedGt4Primitives;
        case 0x20D:
            return (s32)Model_DrawSharedGt3WindowRecordPrimitives;
        case 0x215:
            return (s32)Model_DrawSharedGt4WindowRecordPrimitives;
        }
        break;
    case 0x1100000:
        primitive_type = primitive_code & 0xFFFF;
        switch (primitive_type) {
        case 0xD:
            return (s32)Model_DrawSharedTwoSidedGt3Primitives;
        case 0x15:
            return (s32)Model_DrawSharedTwoSidedGt4Primitives;
        case 0x20D:
            return (s32)Model_DrawSharedTwoSidedGt3WindowRecordPrimitives;
        case 0x215:
            return (s32)Model_DrawSharedTwoSidedGt4WindowRecordPrimitives;
        }
        break;
    case 0x1200000:
        primitive_type = primitive_code & 0xFFFF;
        switch (primitive_type) {
        case 0xD:
            return (s32)Model_DrawSharedSemiGt3Primitives;
        case 0x15:
            return (s32)Model_DrawSharedSemiGt4Primitives;
        case 0x20D:
            return (s32)Model_DrawSharedSemiGt3WindowRecordPrimitives;
        case 0x215:
            return (s32)Model_DrawSharedSemiGt4WindowRecordPrimitives;
        }
        break;
    case 0x1300000:
        primitive_type = primitive_code & 0xFFFF;
        switch (primitive_type) {
        case 0xD:
            return (s32)Model_DrawSharedTwoSidedSemiGt3Primitives;
        case 0x15:
            return (s32)Model_DrawSharedTwoSidedSemiGt4Primitives;
        case 0x20D:
            return (s32)Model_DrawSharedTwoSidedSemiGt3WindowRecordPrimitives;
        case 0x215:
            return (s32)Model_DrawSharedTwoSidedSemiGt4WindowRecordPrimitives;
        }
        break;
    }
#ifdef VERSION_EUROPE
    printf("unsupported SHARED primitive 0x%08lx.\n", primitive_code);
#endif
    return (s32)GsU_00000000;
}

void Model_BindSharedPrimitiveHandler(ModelHandlerObject *object)
{
    *object->handler = (void *)Model_GetSharedPrimitiveHandler(object->key);
    Model_RegisterHandlerKey(object->key, (int)*object->handler);
}
