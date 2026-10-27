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
    u8 *p;
    u8 *q;
    u8 *b;
    u16 *e;
    ModelType2Scratch *r;
    ModelType2Record *rec;
    s32 n;
    s32 i;
    s32 j;
    s32 k;
    s32 f;
    s32 sh;
    s32 v;
    s32 w;
    s32 m;

    m = 0x2000001;
    i = 0;
    r = (ModelType2Scratch *)scratch;
    q = r->records;
    n = ((ModelType2RecordHeader *)q)->record_count;
    q += sizeof(ModelType2RecordHeader);

    if (n != 0) {
        f = model_index < 2;
#ifndef VERSION_EUROPE
        sh = model_index << 8;
#endif
        p = q;

        do {
            q = p + sizeof(ModelType2Record);
            if (unit->type == m) {
                b = r->clut_data;
                if (b != (u8 *)0) {
                    j = 0;
                    rec = MODEL_TYPE2_RECORD_VIEW(q);
                    e = (u16 *)&((u32 *)b)[rec->index_offset];
                    if (rec->row_count > 0) {
                        do {
                            if (f) {
                                *e = 0;
                            }
                            e++;
                            for (
                                k = 1;
                                k < rec->column_count;
                                k++
                            ) {
                                *e |= 0x8000;
                                e++;
                            }
                            j++;
                        } while (j < rec->row_count);
                    }
                    if (f) {
#ifdef VERSION_EUROPE
                        rec->vram_x = 0x280;
                        v = rec->vram_y;
                        if (v >= 8) {
                            rec->vram_y = v % 8;
                        }
                        rec->vram_y += 0xD0 + (model_index << 4);
#else
                        v = rec->vram_x;
                        if ((s16)rec->vram_x >= 0x280) {
                            rec->vram_x = (v - 0x280) + sh;
                        } else {
                            rec->vram_x = sh;
                        }
                        v = rec->vram_y;
                        if (v >= 8) {
                            rec->vram_y = v % 8;
                        }
                        rec->vram_y = rec->vram_y + 0xF8;
#endif
#ifdef VERSION_EUROPE
                    } else {
                        if (rec->vram_y < 0x100) {
                            rec->vram_x = rec->vram_x + 0x280;
                            rec->vram_y =
                                (rec->vram_y & 0xF) + 0xD0;
                        }
                        v = MODEL_TYPE2_RECORD_VIEW(p)->vram_y +
                            MODEL_TYPE2_RECORD_VIEW(p)->row_count - 0x200;
                        if (v > 0) {
                            MODEL_TYPE2_RECORD_VIEW(p)->row_count =
                                MODEL_TYPE2_RECORD_VIEW(p)->row_count - v;
                        }
#endif
                    }
                    q += sizeof(ModelType2Record);
                }
            }
            if (f) {
                w = MODEL_TYPE2_RECORD_VIEW(p)->vram_x;
#ifdef VERSION_EUROPE
                MODEL_TYPE2_RECORD_VIEW(p)->vram_x =
                    (w - 0x280) + (model_index << 8);
#else
                MODEL_TYPE2_RECORD_VIEW(p)->vram_x = (w - 0x280) + sh;
#endif
            }
            i++;
            p = q;

        } while (i < n);
    }

    Model_GetImagePrimitiveHandler(unit->type)(scratch);
    DrawSync(0);
    *(ModelHandler *)unit->ptr = (ModelHandler)GsU_00000000;
}
#endif
