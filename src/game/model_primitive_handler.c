#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/stdio.h"
#include "hmd_fog_textured_drivers.h"
#include "func_8006151C.h"
#include "func_80061A84.h"
#include "model_primitive_handler.h"
#include "direct_polygon_renderers.h"
#include "model_handler_registry.h"
#include "../unmatched.h"

void *Model_GetImagePrimitiveHandler(unsigned int v){if(v==0x2000000)goto a;if(v==0x2000001)goto b;goto d;a:return (void*)GsU_02000000;b:return (void*)GsU_02000001;d:
#ifdef VERSION_EUROPE
printf("unsupported IMAGE primitive 0x%08lx.\n", v);
#endif
return (void*)GsU_00000000;}

void *Model_GetPrimitiveHandler(u32 arg0) {
    s32 temp_a0_2;
    u32 temp_a0;
    u32 temp_a0_3;
    u32 temp_a0_4;
    u32 temp_a0_5;
    u32 temp_v1;

    temp_v1 = arg0 & 0xFFFF0000;
    if (temp_v1 == 0x100000) {
        goto block_38;
    }
    if (temp_v1 > 0x100000U) {
        goto block_5;
    }
    if (temp_v1 == 0) {
        goto block_8;
    }
    if (temp_v1 == 0x20000) {
        goto block_33;
    }
    goto block_107;
block_5:
    if (temp_v1 == 0x200000) {
        goto block_61;
    }
    if (temp_v1 == 0x300000) {
        goto block_84;
    }
    goto block_107;
block_8:
    temp_a0 = arg0 & 0xFFFF;
    if (temp_a0 == 0x15) {
        goto block_28;
    }
    if (temp_a0 >= 0x16U) {
        goto block_17;
    }
    if (temp_a0 == 9) {
        goto block_25;
    }
    if (temp_a0 >= 0xAU) {
        goto block_14;
    }
    if (temp_a0 == 0) {
        goto block_24;
    }
    goto block_107;
block_14:
    if (temp_a0 == 0xD) {
        goto block_26;
    }
    if (temp_a0 == 0x11) {
        goto block_27;
    }
    goto block_107;
block_17:
    if (temp_a0 == 0x20D) {
        goto block_30;
    }
    if (temp_a0 >= 0x20EU) {
        goto block_21;
    }
    if (temp_a0 == 0x209) {
        goto block_29;
    }
    goto block_107;
block_21:
    if (temp_a0 == 0x211) {
        goto block_31;
    }
    if (temp_a0 == 0x215) {
        goto block_32;
    }
    goto block_107;
block_24:
    return (void *)GsU_00000000;
block_25:
    return Model_DrawFt3Primitives;
block_26:
    return Model_DrawGt3Primitives;
block_27:
    return Model_DrawFt4Primitives;
block_28:
    return Model_DrawGt4Primitives;
block_29:
    return Model_DrawFt3WindowRecordPrimitives;
block_30:
    return Model_DrawGt3WindowRecordPrimitives;
block_31:
    return Model_DrawFt4WindowRecordPrimitives;
block_32:
    return Model_DrawGt4WindowRecordPrimitives;
block_33:
    temp_a0_2 = arg0 & 0xFFFF;
    if (temp_a0_2 == 0xD) {
        goto block_36;
    }
    if (temp_a0_2 == 0x15) {
        goto block_37;
    }
    goto block_107;
block_36:
    return Model_DrawFogTexturedTriangles;
block_37:
    return Model_DrawFogTexturedQuads;
block_38:
    temp_a0_3 = arg0 & 0xFFFF;
    if (temp_a0_3 == 0x15) {
        goto block_56;
    }
    if (temp_a0_3 >= 0x16U) {
        goto block_46;
    }
    if (temp_a0_3 == 0xD) {
        goto block_54;
    }
    if (temp_a0_3 >= 0xEU) {
        goto block_44;
    }
    if (temp_a0_3 == 9) {
        goto block_53;
    }
    goto block_107;
block_44:
    if (temp_a0_3 == 0x11) {
        goto block_55;
    }
    goto block_107;
block_46:
    if (temp_a0_3 == 0x20D) {
        goto block_58;
    }
    if (temp_a0_3 >= 0x20EU) {
        goto block_50;
    }
    if (temp_a0_3 == 0x209) {
        goto block_57;
    }
    goto block_107;
block_50:
    if (temp_a0_3 == 0x211) {
        goto block_59;
    }
    if (temp_a0_3 == 0x215) {
        goto block_60;
    }
    goto block_107;
block_53:
    return Model_DrawTwoSidedFt3Primitives;
block_54:
    return Model_DrawTwoSidedGt3Primitives;
block_55:
    return Model_DrawTwoSidedFt4Primitives;
block_56:
    return Model_DrawTwoSidedGt4Primitives;
block_57:
    return Model_DrawTwoSidedFt3WindowRecordPrimitives;
block_58:
    return Model_DrawTwoSidedGt3WindowRecordPrimitives;
block_59:
    return Model_DrawTwoSidedFt4WindowRecordPrimitives;
block_60:
    return Model_DrawTwoSidedGt4WindowRecordPrimitives;
block_61:
    temp_a0_4 = arg0 & 0xFFFF;
    if (temp_a0_4 == 0x15) {
        goto block_79;
    }
    if (temp_a0_4 >= 0x16U) {
        goto block_69;
    }
    if (temp_a0_4 == 0xD) {
        goto block_77;
    }
    if (temp_a0_4 >= 0xEU) {
        goto block_67;
    }
    if (temp_a0_4 == 9) {
        goto block_76;
    }
    goto block_107;
block_67:
    if (temp_a0_4 == 0x11) {
        goto block_78;
    }
    goto block_107;
block_69:
    if (temp_a0_4 == 0x20D) {
        goto block_81;
    }
    if (temp_a0_4 >= 0x20EU) {
        goto block_73;
    }
    if (temp_a0_4 == 0x209) {
        goto block_80;
    }
    goto block_107;
block_73:
    if (temp_a0_4 == 0x211) {
        goto block_82;
    }
    if (temp_a0_4 == 0x215) {
        goto block_83;
    }
    goto block_107;
block_76:
    return Model_DrawSemiFt3Primitives;
block_77:
    return Model_DrawSemiGt3Primitives;
block_78:
    return Model_DrawSemiFt4Primitives;
block_79:
    return Model_DrawSemiGt4Primitives;
block_80:
    return Model_DrawSemiFt3WindowRecordPrimitives;
block_81:
    return Model_DrawSemiGt3WindowRecordPrimitives;
block_82:
    return Model_DrawSemiFt4WindowRecordPrimitives;
block_83:
    return Model_DrawSemiGt4WindowRecordPrimitives;
block_84:
    temp_a0_5 = arg0 & 0xFFFF;
    if (temp_a0_5 == 0x15) {
        goto block_102;
    }
    if (temp_a0_5 >= 0x16U) {
        goto block_92;
    }
    if (temp_a0_5 == 0xD) {
        goto block_100;
    }
    if (temp_a0_5 >= 0xEU) {
        goto block_90;
    }
    if (temp_a0_5 == 9) {
        goto block_99;
    }
    goto block_107;
block_90:
    if (temp_a0_5 == 0x11) {
        goto block_101;
    }
    goto block_107;
block_92:
    if (temp_a0_5 == 0x20D) {
        goto block_104;
    }
    if (temp_a0_5 >= 0x20EU) {
        goto block_96;
    }
    if (temp_a0_5 == 0x209) {
        goto block_103;
    }
    goto block_107;
block_96:
    if (temp_a0_5 == 0x211) {
        goto block_105;
    }
    if (temp_a0_5 == 0x215) {
        goto block_106;
    }
    goto block_107;
block_99:
    return Model_DrawTwoSidedSemiFt3Primitives;
block_100:
    return Model_DrawTwoSidedSemiGt3Primitives;
block_101:
    return Model_DrawTwoSidedSemiFt4Primitives;
block_102:
    return Model_DrawTwoSidedSemiGt4Primitives;
block_103:
    return Model_DrawTwoSidedSemiFt3WindowRecordPrimitives;
block_104:
    return Model_DrawTwoSidedSemiGt3WindowRecordPrimitives;
block_105:
    return Model_DrawTwoSidedSemiFt4WindowRecordPrimitives;
block_106:
    return Model_DrawTwoSidedSemiGt4WindowRecordPrimitives;
block_107:
#ifdef VERSION_EUROPE
    printf("unsupported POLYGON primitive 0x%08lx.\n", arg0);
#endif
    return (void *)GsU_00000000;
}

void Model_BindPrimitiveHandler(ModelHandlerObject *object)
{
    *object->handler = Model_GetPrimitiveHandler(object->key);
    Model_RegisterHandlerKey(object->key, (int)*object->handler);
}

s32 Model_GetSharedPrimitiveHandler(s32 arg0) {
    u32 t;
    u32 u;

    t = arg0 & 0xFFFF0000;
    switch (t) {
    case 0x1000000:
        u = arg0 & 0xFFFF;
        switch (u) {
        case 0x0:
            return (s32)func_80067220;
        case 0xD:
            return (s32)func_80067354;
        case 0x15:
            return (s32)func_8006759C;
        case 0x20D:
            return (s32)func_80067858;
        case 0x215:
            return (s32)func_80067ABC;
        }
        break;
    case 0x1100000:
        u = arg0 & 0xFFFF;
        switch (u) {
        case 0xD:
            return (s32)func_80067D94;
        case 0x15:
            return (s32)func_80067FD0;
        case 0x20D:
            return (s32)func_8006825C;
        case 0x215:
            return (s32)func_800684B4;
        }
        break;
    case 0x1200000:
        u = arg0 & 0xFFFF;
        switch (u) {
        case 0xD:
            return (s32)func_8006875C;
        case 0x15:
            return (s32)func_80068A00;
        case 0x20D:
            return (s32)func_80068D18;
        case 0x215:
            return (s32)func_80068FD8;
        }
        break;
    case 0x1300000:
        u = arg0 & 0xFFFF;
        switch (u) {
        case 0xD:
            return (s32)func_8006930C;
        case 0x15:
            return (s32)func_800695A4;
        case 0x20D:
            return (s32)func_8006988C;
        case 0x215:
            return (s32)func_80069B40;
        }
        break;
    }
#ifdef VERSION_EUROPE
    printf("unsupported SHARED primitive 0x%08lx.\n", arg0);
#endif
    return (s32)GsU_00000000;
}

void Model_BindSharedPrimitiveHandler(ModelHandlerObject *object)
{
    *object->handler = (void *)Model_GetSharedPrimitiveHandler(object->key);
    Model_RegisterHandlerKey(object->key, (int)*object->handler);
}
