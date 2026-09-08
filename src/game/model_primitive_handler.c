#include "../types.h"
#include "model_primitive_handler.h"

extern void func_80033DB0(void);
extern void func_80034830(void);
extern void func_800612C0(void);
extern void func_8006151C(void);
extern void func_800617E0(void);
extern void func_80061A84(void);
extern void func_80061DDC(void);
extern void func_80062058(void);
extern void func_8006233C(void);
extern void func_80062600(void);
extern void func_80062978(void);
extern void func_80062BC0(void);
extern void func_80062E70(void);
extern void func_80063100(void);
extern void func_80063444(void);
extern void func_800636AC(void);
extern void func_8006397C(void);
extern void func_80063C2C(void);
extern void func_80063F90(void);
extern void func_80064248(void);
extern void func_80064568(void);
extern void func_80064868(void);
extern void func_80064C1C(void);
extern void func_80064EF4(void);
extern void func_80065234(void);
extern void func_80065554(void);
extern void func_80065928(void);
extern void func_80065BCC(void);
extern void func_80065ED8(void);
extern void func_800661C4(void);
extern void func_80066564(void);
extern void func_80066828(void);
extern void func_80066B54(void);
extern void func_80066E60(void);
extern void GsU_00000000(void);
extern void GsU_02000000(void);
extern void GsU_02000001(void);

void *func_800603DC(unsigned int v){if(v==0x2000000)goto a;if(v==0x2000001)goto b;goto d;a:return GsU_02000000;b:return GsU_02000001;d:return GsU_00000000;}

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
    return GsU_00000000;
block_25:
    return func_800612C0;
block_26:
    return func_8006151C;
block_27:
    return func_800617E0;
block_28:
    return func_80061A84;
block_29:
    return func_80061DDC;
block_30:
    return func_80062058;
block_31:
    return func_8006233C;
block_32:
    return func_80062600;
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
    return func_80033DB0;
block_37:
    return func_80034830;
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
    return func_80062978;
block_54:
    return func_80062BC0;
block_55:
    return func_80062E70;
block_56:
    return func_80063100;
block_57:
    return func_80063444;
block_58:
    return func_800636AC;
block_59:
    return func_8006397C;
block_60:
    return func_80063C2C;
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
    return func_80063F90;
block_77:
    return func_80064248;
block_78:
    return func_80064568;
block_79:
    return func_80064868;
block_80:
    return func_80064C1C;
block_81:
    return func_80064EF4;
block_82:
    return func_80065234;
block_83:
    return func_80065554;
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
    return func_80065928;
block_100:
    return func_80065BCC;
block_101:
    return func_80065ED8;
block_102:
    return func_800661C4;
block_103:
    return func_80066564;
block_104:
    return func_80066828;
block_105:
    return func_80066B54;
block_106:
    return func_80066E60;
block_107:
    return GsU_00000000;
}
