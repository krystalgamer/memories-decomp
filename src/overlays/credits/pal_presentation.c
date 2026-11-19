#include "../../types.h"
#include "pal_presentation.h"

/* Per cue: text id (0xFF ends the run), secondary text, clear-secondary flag
 * and flags. */
CreditsCue gCredits_aCues[32] = {
    { 0x00, 0x00, 0x00, 0x00 },
    { 0x90, 0x00, 0x00, 0x00 },
    { 0x91, 0x00, 0x00, 0x00 },
    { 0x92, 0x00, 0x00, 0x00 },
    { 0x93, 0x00, 0x00, 0x00 },
    { 0x94, 0x00, 0x00, 0x00 },
    { 0x95, 0x00, 0x00, 0x00 },
    { 0x96, 0x00, 0x00, 0x00 },
    { 0x97, 0x00, 0x00, 0x00 },
    { 0x98, 0xB0, 0x00, 0x00 },
    { 0x99, 0x00, 0x00, 0x01 },
    { 0x9A, 0x00, 0x00, 0x01 },
    { 0x9B, 0x00, 0x00, 0x01 },
    { 0x9C, 0xB1, 0xFF, 0x00 },
    { 0x9D, 0x00, 0x00, 0x01 },
    { 0x9E, 0x00, 0x00, 0x01 },
    { 0x9F, 0x00, 0xFF, 0x00 },
    { 0xA0, 0x00, 0x00, 0x00 },
    { 0xA1, 0x00, 0x00, 0x00 },
    { 0xA2, 0x00, 0x00, 0x00 },
    { 0xA3, 0x00, 0x00, 0x00 },
    { 0xA4, 0x00, 0x00, 0x00 },
    { 0xA5, 0xB2, 0x00, 0x00 },
    { 0xA6, 0x00, 0x00, 0x01 },
    { 0xA7, 0x00, 0x00, 0x01 },
    { 0xA8, 0x00, 0x00, 0x01 },
    { 0xA9, 0x00, 0x00, 0x01 },
    { 0xAA, 0x00, 0x00, 0x01 },
    { 0xAB, 0x00, 0xFF, 0x00 },
    { 0xAC, 0x00, 0x00, 0x00 },
    { 0xAD, 0x00, 0x00, 0x00 },
    { 0xFF, 0x00, 0x00, 0x00 },
};

/* The presentation state, zero-initialized and common (this unit builds with
 * gcc_2_8_1_g0_split_comm); each layout's pal_presentation_commons
 * subsegment places it at the image's addresses. */
u8 gCredits_bFinished;
CreditsChannel gCredits_aChannels[4];
DisplayObject *gCredits_pGridObject;
u32 gCredits_dwFrameCounter;

void Credits_StartLineGroup(s32 index)
{
    CreditsCue *cue = &gCredits_aCues[index];
    CreditsChannel *state = gCredits_aChannels;
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
            channel->color_index = 5;
            TextBox_BuildBlocking(channel);
            if (cue->flags & 1) {
                DuelEffect_SetEntryControlBytes(channel, 8, 0);
            } else {
                DuelEffect_SetEntryControlBytes(channel, 5, 0);
            }
            DuelEffect_ProcessEntries(channel);
            if (cue->secondary) {
                TextBox_DestroyIndex(i + 2);
                channel = TextBox_CreateFlagged(i + 2, cue->secondary, 16, 40, 512, 256, 6);
                channel->color_index = 5;
                TextBox_BuildBlocking(channel);
                DuelEffect_SetEntryControlBytes(channel, 5, 0);
                DuelEffect_ProcessEntries(channel);
            }
            break;
        }
    }
}

s32 Credits_UpdateLines(void)
{
    s32 i;
    s32 j;
    s32 next;
    CreditsCue *cue;

    gCredits_dwFrameCounter++;
    i = (gCredits_dwFrameCounter >> 1) & 127;
    if (i >= 64) {
        i = 128 - i;
    }
    gCredits_pGridObject->rgb.c.r = i;
    for (i = 0; i < 4; i++) {
        if (gCredits_aChannels[i].active) {
            switch (gCredits_aChannels[i].phase) {
            case 0:
                if (DuelEffect_HasActiveEntry(&gText_aChannels[i]) == 0) {
                    gCredits_aChannels[i].delay = 244;
                    gCredits_aChannels[i].phase++;
                }
                break;
            case 1:
                if (i < 2) {
                    gCredits_aChannels[i].delay -= (u16)gGraphics_nBaseFrameStep;
                    if (gCredits_aChannels[i].delay <= 0) {
                        next = gCredits_aChannels[i].cue + 1;
                        cue = &gCredits_aCues[next];
                        if (cue->text == 255) {
                            gCredits_bFinished = 1;
                            gCredits_aChannels[i].phase = 16;
                        } else {
                            if (cue->flags & 1) {
                                DuelEffect_SetEntryControlBytes(&gText_aChannels[i], 9, 0);
                            } else {
                                DuelEffect_SetEntryControlBytes(&gText_aChannels[i], 6, 0);
                            }
                            next = gCredits_aChannels[i].cue + 1;
                            if (gCredits_aCues[next].clear_secondary) {
                                for (j = 2; j < 4; j++) {
                                    if (gText_aChannels[j].status.f.flags & DUEL_EFFECT_CHANNEL_FLAG_ACTIVE) {
                                        DuelEffect_SetEntryControlBytes(&gText_aChannels[j], 6, 0);
                                        gCredits_aChannels[j].phase++;
                                    }
                                }
                            }
                            Credits_StartLineGroup(gCredits_aChannels[i].cue + 1);
                            gCredits_aChannels[i].phase++;
                        }
                    }
                }
                break;
            case 2:
                if (DuelEffect_HasActiveEntry(&gText_aChannels[i]) == 0) {
                    if (gCredits_aChannels[i].cue) {
                        TextBox_DestroyIndex(i);
                    }
                    gCredits_aChannels[i].active = 0;
                }
                break;
            }
        }
    }
    return gCredits_bFinished;
}

void Credits_InitPresentation(void)
{
    DisplayObject *object;
    s32 i;

    for (i = 3; i >= 0; i--) {
        gCredits_aChannels[i].active = 0;
    }
    gCredits_bFinished = 0;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
    DisplayObject_SelectOrderingTable3(object);
    object->rgb.c.r = 0;
    object->field_4C.draw = Credits_DrawGrid;
    gCredits_pGridObject = object;
    gCredits_dwFrameCounter = 0;
}

s32 Credits_IsFinished(void)
{
    return gCredits_bFinished;
}

void Credits_DrawGrid(DisplayObject *object, GsOT *ot)
{
    LINE_F2 line;
    LINE_F2 *packet = &line;
    s32 i;

    setLineF2(packet);
    packet->r0 = packet->g0 = packet->b0 = object->rgb.c.r;
    packet->x0 = 0;
    packet->x1 = 320;
    i = gCredits_dwFrameCounter % 24;
    do {
        packet->y0 = packet->y1 = i;
        i += 24;
        Graphics_SubmitDrawModePacket(packet, ot, 0, 0);
    } while (i < 256);
    packet->y0 = 0;
    packet->y1 = 256;
    i = gCredits_dwFrameCounter % 96;
    i >>= 2;
    do {
        packet->x0 = packet->x1 = i;
        i += 24;
        Graphics_SubmitDrawModePacket(packet, ot, 0, 0);
    } while (i < 320);
}
