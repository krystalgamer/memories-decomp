#include "../../types.h"
#include "dispatch.h"

void func_80146258(s32 effect, s32 phase, void *buffer, DuelEffectRequest *context)
{
    s32 i;
    u16 flags = context->field_10;

    setVector(&D_8015B7F8, context->field_00, context->field_02, context->field_04);
    D_8015B800 = flags;
    if (phase >= 0) {
        for (i = 0; i < 21; i++) {
            func_8014F564(D_8015B748.pairs[i], &D_8015A1E4[i], D_8015A430.modes[i]);
        }
    }
    if (effect == 0) {
        D_8015B7F4 = (GsOT *)context->field_0C;
        func_80154688(buffer, phase);
    }
    if (effect == 1) {
        D_8015B7F4 = (GsOT *)context->field_0C;
        func_80157794(buffer, phase);
    }
    if (effect == 2) {
        D_8015B7F4 = (GsOT *)context->field_0C;
        func_80153200(buffer, phase, context->field_12);
    }
    if (effect == 3) {
        D_8015B7F4 = (GsOT *)context->field_0C;
        func_80149F90(buffer, phase);
    }
    if (effect == 4) {
        D_8015B7F4 = (GsOT *)context->field_0C;
        func_80159AAC(buffer, phase);
    }
    if (effect == 5) {
        D_8015B7F4 = (GsOT *)context->field_0C;
        func_80157E10(buffer, phase);
    }
    if (effect == 6) {
        D_8015B7F4 = (GsOT *)context->field_0C;
        func_80154B30(buffer, phase);
    }
    if (effect == 7) {
        D_8015B7F4 = (GsOT *)context->field_0C;
        func_801587D8(buffer, phase);
    }
    if (effect == 8) {
        D_8015B7F4 = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        func_80147B18(buffer, phase);
    }
    if (effect == 9) {
        /* Both stores are present; the callee takes only two arguments. */
        D_8015B7F4 = (GsOT *)context->field_08;
        D_8015B7F4 = (GsOT *)context->field_0C;
        func_801593A8(buffer, phase);
    }
    if (effect == 10) {
        D_8015B7F4 = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        func_8014C8FC(buffer, phase);
    }
    if (effect == 11) {
        D_8015B7F4 = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        func_80146760(buffer, phase);
    }
    if (effect == 12) {
        D_8015B7F4 = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        func_80150E00(buffer, phase);
    }
    if (effect == 13) {
        D_8015B7F4 = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        func_801503F8(buffer, phase, context->field_12);
    }
    if (effect == 14) {
        D_8015B7F4 = (GsOT *)context->field_0C;
        func_801481A8(buffer, phase);
    }
    if (effect == 15) {
        D_8015B7F4 = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        func_8014FF40(buffer, phase);
    }
    if (effect == 16) {
        D_8015B7F4 = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        func_80151558(buffer, phase);
    }
    if (effect == 17) {
        D_8015B7F4 = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        func_80148BA4(buffer, phase);
    }
    if (effect == 18) {
        D_8015B7F4 = (GsOT *)context->field_0C;
        func_80154084(buffer, phase);
    }
    if (effect == 19) {
        D_8015B7F4 = (GsOT *)context->field_0C;
        func_80153ADC(buffer, phase);
    }
    if (effect == 20) {
        D_8015B7F4 = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        if (phase >= 0) {
            func_80151558(buffer, 1);
        } else {
            func_80151558(buffer, phase);
        }
    }
    if (effect == 21) {
        D_8015B7F4 = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        func_8014E3EC(buffer, phase);
    }
    if (effect == 22) {
        D_8015B7F4 = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        func_8014A8E4(buffer, phase);
    }
    if (effect == 23) {
        D_8015B7F4 = (GsOT *)context->field_08;
        SetGeomOffset(160, 108);
        func_80152048(buffer, phase);
    }
    if (effect == 24) {
        D_8015B7F4 = (GsOT *)context->field_0C;
        func_8014D3E8(buffer, phase);
    }
}
