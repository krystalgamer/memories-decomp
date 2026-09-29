#include "../../types.h"
#include "runtime.h"

void func_80180004(s32 index)
{
    ModelIntroCue *cue = &D_801805D0[index];
    ModelIntroChannel *state = D_80180654;
    DuelEffectChannel *channel;
    s32 i;
    s32 text;

    for (i = 0; i < 2; state++, i++) {
        if (state->active == 0) {
            state->active = 1;
            state->cue = index;
            state->phase = 0;
            state->delay = 244;
            text = cue->text;
            if (text == 255) {
                return;
            }
            TextBox_DestroyIndex(i);
            channel = TextBox_CreateFlagged(i, text, 16, 40, 512, 256, 6);
            channel->field_54 = 5;
            func_80039A14(channel);
            if (cue->flags & 1) {
                func_800373C8(channel, 8, 0);
            } else {
                func_800373C8(channel, 5, 0);
            }
            DuelEffect_ProcessEntries(channel);
            if (cue->secondary) {
                TextBox_DestroyIndex(i + 2);
                channel = TextBox_CreateFlagged(i + 2, cue->secondary, 16, 40, 512, 256, 6);
                channel->field_54 = 5;
                func_80039A14(channel);
                func_800373C8(channel, 5, 0);
                DuelEffect_ProcessEntries(channel);
            }
            break;
        }
    }
}

s32 func_8018019C(void)
{
    s32 i;
    s32 j;
    s32 next;
    ModelIntroCue *cue;

    D_80180670++;
    i = (D_80180670 >> 1) & 127;
    if (i >= 64) {
        i = 128 - i;
    }
    *(u8 *)&D_8018066C->field_0C = i;
    for (i = 0; i < 4; i++) {
        if (D_80180654[i].active) {
            switch (D_80180654[i].phase) {
            case 0:
                if (DuelEffect_HasActiveEntry(&D_800EB0F8[i]) == 0) {
                    D_80180654[i].delay = 244;
                    D_80180654[i].phase++;
                }
                break;
            case 1:
                if (i < 2) {
                    D_80180654[i].delay -= (u16)D_8009B0D8;
                    if (D_80180654[i].delay <= 0) {
                        next = D_80180654[i].cue + 1;
                        cue = &D_801805D0[next];
                        if (cue->text == 255) {
                            D_80180650 = 1;
                            D_80180654[i].phase = 16;
                        } else {
                            if (cue->flags & 1) {
                                func_800373C8(&D_800EB0F8[i], 9, 0);
                            } else {
                                func_800373C8(&D_800EB0F8[i], 6, 0);
                            }
                            next = D_80180654[i].cue + 1;
                            if (D_801805D0[next].clear_secondary) {
                                for (j = 2; j < 4; j++) {
                                    if (D_800EB0F8[j].flags_34 & 0x8000) {
                                        func_800373C8(&D_800EB0F8[j], 6, 0);
                                        D_80180654[j].phase++;
                                    }
                                }
                            }
                            func_80180004(D_80180654[i].cue + 1);
                            D_80180654[i].phase++;
                        }
                    }
                }
                break;
            case 2:
                if (DuelEffect_HasActiveEntry(&D_800EB0F8[i]) == 0) {
                    if (D_80180654[i].cue) {
                        TextBox_DestroyIndex(i);
                    }
                    D_80180654[i].active = 0;
                }
                break;
            }
        }
    }
    return D_80180650;
}

void func_80180420(void)
{
    DisplayObject *object;
    s32 i;

    for (i = 3; i >= 0; i--) {
        D_80180654[i].active = 0;
    }
    D_80180650 = 0;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
    DisplayObject_SelectOrderingTable3(object);
    *(u8 *)&object->field_0C = 0;
    object->field_4C = (s32)func_801804B0;
    D_8018066C = object;
    D_80180670 = 0;
}

s32 func_801804A0(void)
{
    return D_80180650;
}

void func_801804B0(DisplayObject *object, GsOT *ot)
{
    LINE_F2 line;
    LINE_F2 *packet = &line;
    s32 i;

    setLineF2(packet);
    packet->r0 = packet->g0 = packet->b0 = (u8)object->field_0C;
    packet->x0 = 0;
    packet->x1 = 320;
    i = D_80180670 % 24;
    do {
        packet->y0 = packet->y1 = i;
        i += 24;
        func_8005B260((u32 *)packet, ot, 0, 0);
    } while (i < 256);
    packet->y0 = 0;
    packet->y1 = 256;
    i = D_80180670 % 96;
    i >>= 2;
    do {
        packet->x0 = packet->x1 = i;
        i += 24;
        func_8005B260((u32 *)packet, ot, 0, 0);
    } while (i < 320);
}
