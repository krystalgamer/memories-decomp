/* The model handler registry and the two maps that search it.
 *
 * gModel_aHandlerKeyRegistry is the eighty-entry table; Model_RegisterHandlerKey fills it and
 * Model_FindHandlerKey reads it back. All three use the static inline
 * Model_LookupHandlerKey; Model_MapToWireframePrimitiveHandler and
 * Model_MapToBasePrimitiveHandler inline that reverse lookup -- same sentinel,
 * same eighty-entry scan, same -1 -- and then dispatch on the packed id it
 * returns, which is why they belong here rather than beside the handlers they
 * name. */
#include "../types.h"
#include "../ygo_types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model.h"

/* Zero-initialized storage this unit owns at 0x800F5918-0x800F5B98, defined in
 * address order ahead of the headers that declare it (#2602). */
ModelHandlerRegistryEntry gModel_aHandlerKeyRegistry[80];

#include "model_handler_registry.h"
#include "direct_polygon_renderers.h"
#include "shared_primitive_renderers.h"
#include "wireframe_renderers.h"
#include "primitive_handler_modes.h"

/* The registry's reverse lookup, shared by both maps and Model_FindHandlerKey:
   the matching entry's key, or -1 for the sentinel or no match after all 80. */
static inline s32 Model_LookupHandlerKey(s32 handler_value)
{
    ModelHandlerRegistryEntry *registry_entry = gModel_aHandlerKeyRegistry;
    s32 registry_index;

    if (handler_value == (s32)GsU_00000000) {
        return -1;
    }
    for (registry_index = 0; registry_index < MODEL_HANDLER_REGISTRY_COUNT; registry_index++, registry_entry++) {
        if (registry_entry->handler_value == handler_value) {
            return registry_entry->key;
        }
    }
    return -1;
}

/* Maps an id to its handler in the second dispatch family: looks the id up
 * in the handler registry at gModel_aHandlerKeyRegistry (GsU_00000000 is the sentinel that
 * skips the search), then dispatches on the high halfword's group and the
 * low halfword's kind. Returns handler_value unchanged when nothing matches. */

s32 Model_MapToWireframePrimitiveHandler(s32 handler_value) {
    s32 type_key;
    u32 type_family;

    type_key = Model_LookupHandlerKey(handler_value);
    type_family = type_key & 0xFFFF0000;

    switch (type_family) {
    case 0:
    case 0x100000:
    case 0x200000:
    case 0x300000:
        switch (type_key & 0xFFFF) {
        case GsUFT3:
            return (s32)Model_DrawFt3Wireframes;
        case GsUTFT3:
            return (s32)Model_DrawFt3WindowRecordWireframes;
        case GsUFT4:
            return (s32)Model_DrawFt4Wireframes;
        case GsUTFT4:
            return (s32)Model_DrawFt4WindowRecordWireframes;
        case GsUGT3:
            return (s32)Model_DrawGt3Wireframes;
        case GsUTGT3:
            return (s32)Model_DrawGt3WindowRecordWireframes;
        case GsUGT4:
            return (s32)Model_DrawGt4Wireframes;
        case GsUTGT4:
            return (s32)Model_DrawGt4WindowRecordWireframes;
        }
        break;
    case 0x1000000:
    case 0x1100000:
    case 0x1200000:
    case 0x1300000:
        switch (type_key & 0xFFFF) {
        case GsUGT3:
            return (s32)Model_DrawSharedGt3Wireframes;
        case GsUTGT3:
            return (s32)Model_DrawSharedGt3WindowRecordWireframes;
        case GsUGT4:
            return (s32)Model_DrawSharedGt4Wireframes;
        case GsUTGT4:
            return (s32)Model_DrawSharedGt4WindowRecordWireframes;
        }
        break;
    }
    return handler_value;
}

/* Maps the registered callback to the handler family selected by the packed
 * registry id. The case order is retail's own arm order in memory. */

s32 Model_MapToBasePrimitiveHandler(s32 handler_value) {
    s32 type_key;
    u32 type_family;

    type_key = Model_LookupHandlerKey(handler_value);
    type_family = type_key & 0xFFFF0000;

    switch (type_family) {
    case 0x200000:
        switch (type_key & 0xFFFF) {
        case GsUFT3:
            return (s32)Model_DrawFt3Primitives;
        case GsUTFT3:
            return (s32)Model_DrawFt3WindowRecordPrimitives;
        case GsUFT4:
            return (s32)Model_DrawFt4Primitives;
        case GsUTFT4:
            return (s32)Model_DrawFt4WindowRecordPrimitives;
        case GsUGT3:
            return (s32)Model_DrawGt3Primitives;
        case GsUTGT3:
            return (s32)Model_DrawGt3WindowRecordPrimitives;
        case GsUGT4:
            return (s32)Model_DrawGt4Primitives;
        case GsUTGT4:
            return (s32)Model_DrawGt4WindowRecordPrimitives;
        }
        break;
    case 0x300000:
        switch (type_key & 0xFFFF) {
        case GsUFT3:
            return (s32)Model_DrawTwoSidedFt3Primitives;
        case GsUTFT3:
            return (s32)Model_DrawTwoSidedFt3WindowRecordPrimitives;
        case GsUFT4:
            return (s32)Model_DrawTwoSidedFt4Primitives;
        case GsUTFT4:
            return (s32)Model_DrawTwoSidedFt4WindowRecordPrimitives;
        case GsUGT3:
            return (s32)Model_DrawTwoSidedGt3Primitives;
        case GsUTGT3:
            return (s32)Model_DrawTwoSidedGt3WindowRecordPrimitives;
        case GsUGT4:
            return (s32)Model_DrawTwoSidedGt4Primitives;
        case GsUTGT4:
            return (s32)Model_DrawTwoSidedGt4WindowRecordPrimitives;
        }
        break;
    case 0x1200000:
        switch (type_key & 0xFFFF) {
        case GsUGT3:
            return (s32)Model_DrawSharedGt3Primitives;
        case GsUTGT3:
            return (s32)Model_DrawSharedGt3WindowRecordPrimitives;
        case GsUGT4:
            return (s32)Model_DrawSharedGt4Primitives;
        case GsUTGT4:
            return (s32)Model_DrawSharedGt4WindowRecordPrimitives;
        }
        break;
    case 0x1300000:
        switch (type_key & 0xFFFF) {
        case GsUGT3:
            return (s32)Model_DrawSharedTwoSidedGt3Primitives;
        case GsUTGT3:
            return (s32)Model_DrawSharedTwoSidedGt3WindowRecordPrimitives;
        case GsUGT4:
            return (s32)Model_DrawSharedTwoSidedGt4Primitives;
        case GsUTGT4:
            return (s32)Model_DrawSharedTwoSidedGt4WindowRecordPrimitives;
        }
        break;
    }
    return handler_value;
}

/* Finds `type_key` in the table; if absent and there is a free slot, claims it
   with (type_key, handler_value). No-op once all 80 slots are taken and no match exists. */
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

/* Reverse lookup: finds the entry whose handler_value matches, returns its key (or -1
   if handler_value is the sentinel, or if no entry matches after scanning all 80). */
s32 Model_FindHandlerKey(s32 handler_value) {
    return Model_LookupHandlerKey(handler_value);
}
