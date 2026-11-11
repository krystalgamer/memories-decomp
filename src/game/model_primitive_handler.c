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

u32 *(*Model_GetImagePrimitiveHandler(u32 v))(GsARGUNIT *){if(v==0x2000000)goto a;if(v==0x2000001)goto b;goto d;a:return GsU_02000000;b:return GsU_02000001;d:
#ifdef VERSION_EUROPE
printf("unsupported IMAGE primitive 0x%08lx.\n", v);
#endif
return GsU_00000000;}

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
            return (void *)GsU_00000000;
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
    return (void *)GsU_00000000;
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
