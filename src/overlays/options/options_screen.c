#include "../../types.h"
#include "../../game/display_asset_banks.h"
#include "../../game/display_object_system.h"
#include "../../game/fade.h"
#include "../../game/file_transfer.h"
#include "../../game/input.h"
#include "../../game/main.h"
#include "../../game/scratchpad.h"
#include "../../game/sound.h"
#include "../../game/sprite_primitive.h"
#include "helpers.h"

/* Per strip: texture u, v and CLUT row (relative to 0x280). */
u8 gPalOptions_aStripTextures[5][3] = {
    { 0x00, 0x40, 0x10 },
    { 0x00, 0x60, 0x20 },
    { 0x50, 0x00, 0x30 },
    { 0x50, 0x20, 0x40 },
    { 0x80, 0x00, 0x50 },
};

/* The screen state, zero-initialized and common (this unit builds with
 * gcc_2_8_1_g0_split_comm); each layout's options_screen_commons subsegment
 * places it at the image's addresses. */
s16 gPalOptions_wStripScroll;
u8 gPalOptions_bMode;
s8 gPalOptions_bSoundOutputType;
s16 gPalOptions_wStripScrollTarget;
DisplayObject *gPalOptions_pLanguageSprite;
DisplayObject *gPalOptions_pSoundTypeSprite;
s32 gPalOptions_aWaveOffsets[5][9];
s8 gPalOptions_bSelectedRow;
DisplayObject *gPalOptions_pLabelSprite;
DisplayObject *gPalOptions_pCursor;
s8 gPalOptions_bLanguage;
s32 gPalOptions_dwWavePhase;
u32 gPalOptions_aWaveColors[5][9];
u8 gPalOptions_bState;

void PalOptions_SetTextColorSlots(s32 selection)
{
    u8 *colors = gText_abColorSlots;

    colors[0] = 4;
    colors[1] = 4;
    colors[2] = 4;
    colors[3] = 4;
    colors[4] = 4;
    colors[selection] = 0;
    if (selection != 0) {
        colors[3] = 2;
    } else {
        colors[4] = 2;
    }
}

void PalOptions_PositionCursor(s32 selection)
{
    gPalOptions_pCursor->xy.h.x = gPalOptions_bSoundOutputType * 112 + 60;
    gPalOptions_pCursor->xy.h.y = 80;
    if (selection != 0) {
        gPalOptions_pCursor->xy.h.y = 112;
    }
    DisplayObject_UpdateResourceVariant(gPalOptions_pSoundTypeSprite, gPalOptions_bSoundOutputType);
}

s32 PalOptions_EaseStripY(s32 position)
{
    s32 distance;
    s32 result;

    if (position < 60) {
        distance = (60 - position) >> 2;
        distance *= distance;
        distance >>= 4;
        result = position + distance;
    } else if (position < 93) {
        result = position;
    } else {
        distance = (position - 92) >> 2;
        distance *= distance;
        distance >>= 4;
        result = position - distance;
    }
    return result;
}

void PalOptions_DrawScrollingStrips(DisplayObject *object, GsOT *ordering_table)
{
    POLY_FT4 *packet = (POLY_FT4 *)SCRATCH_ADDR(0x344);
    s32 index = 0;
    s32 position;
    s32 start;

    setPolyFT4(packet);
    packet->r0 = packet->g0 = packet->b0 = 0x80;
    start = (s16)(gPalOptions_wStripScroll % 160);
    packet->tpage = 0x11;
    packet->x0 = packet->x2 = 0x30;
    packet->x1 = packet->x3 = 0x60;
    position = start + 60;
    start = position;
    for (;;) {
        packet->y0 = packet->y1 = PalOptions_EaseStripY(position);
        position += 32;
        packet->y2 = packet->y3 = PalOptions_EaseStripY(position);
        if (packet->y0 >= 152) {
            break;
        }
        packet->clut = getClut(0x280 + gPalOptions_aStripTextures[index][2], 0xE0);
        packet->u0 = gPalOptions_aStripTextures[index][0];
        packet->v0 = gPalOptions_aStripTextures[index][1];
        packet->u1 = gPalOptions_aStripTextures[index][0] + 0x30;
        packet->v1 = gPalOptions_aStripTextures[index][1];
        packet->u2 = gPalOptions_aStripTextures[index][0];
        packet->v2 = gPalOptions_aStripTextures[index][1] + 0x20;
        packet->u3 = gPalOptions_aStripTextures[index][0] + 0x30;
        packet->v3 = gPalOptions_aStripTextures[index][1] + 0x20;
        GsSortPoly(packet, ordering_table, object->ot_depth);
        index++;
        if (index >= 5) {
            index = 0;
        }
    }
    index = 4;
    position = start - 32;
    for (;;) {
        packet->y0 = packet->y1 = PalOptions_EaseStripY(position);
        packet->y2 = packet->y3 = PalOptions_EaseStripY(position + 32);
        if (packet->y2 < 8) {
            break;
        }
        packet->clut = getClut(0x280 + gPalOptions_aStripTextures[index][2], 0xE0);
        packet->u0 = gPalOptions_aStripTextures[index][0];
        packet->v0 = gPalOptions_aStripTextures[index][1];
        packet->u1 = gPalOptions_aStripTextures[index][0] + 0x30;
        packet->v1 = gPalOptions_aStripTextures[index][1];
        packet->u2 = gPalOptions_aStripTextures[index][0];
        packet->v2 = gPalOptions_aStripTextures[index][1] + 0x20;
        packet->u3 = gPalOptions_aStripTextures[index][0] + 0x30;
        packet->v3 = gPalOptions_aStripTextures[index][1] + 0x20;
        GsSortPoly(packet, ordering_table, object->ot_depth);
        index--;
        if (index < 0) {
            index = 4;
        }
        position -= 32;
    }
}

void PalOptions_DrawWaveSprite(DisplayObject *object, GsOT *ordering_table)
{
    SpritePrim *sprite;
    POLY_GT4 *packet;
    s32 row;
    s32 column;
    s32 x;
    s32 y;
    s32 color;

    SetPolyGT4((POLY_GT4 *)SCRATCH_ADDR(0x344));
    sprite = (SpritePrim *)SCRATCH_ADDR(0x320);
    sprite->attribute = object->attribute;
    sprite->xy.h.x = object->xy.h.x;
    sprite->xy.h.y = object->xy.h.y;
    sprite->extent.wh.w.word = object->field_3C.size.w;
    sprite->extent.wh.h = object->field_3C.size.h;
    sprite->rgb.word = object->rgb.word;
    sprite->cxcy.word = object->field_40.word;
    sprite->uv.word = object->texture_uv.word;
    sprite->tpage = object->texture_page;
    packet = (POLY_GT4 *)SCRATCH_ADDR(0x344);
    if (gPalOptions_bLanguage == object->kind_index.language) {
        packet->tpage = sprite->tpage;
        packet->clut = getClut(sprite->cxcy.h.cx, sprite->cxcy.h.cy);
        for (row = 0; row < 4; row++) {
            x = (s16)sprite->xy.h.x;
            y = (s16)sprite->xy.h.y + row * 8;
            packet->u0 = sprite->uv.b.lo;
            packet->v0 = packet->v1 = sprite->uv.b.hi + row * 8;
            packet->v2 = packet->v3 = packet->v0 + 8;
            for (column = 0; column < 8; x += 6, column++) {
                packet->x0 = packet->x2 = x;
                packet->x1 = packet->x3 = x + 6;
                packet->u2 = packet->u0;
                packet->u1 = packet->u3 = packet->u0 + 6;
                packet->y0 = y + gPalOptions_aWaveOffsets[row][column];
                packet->y1 = y + gPalOptions_aWaveOffsets[row][column + 1];
                packet->y2 = y + (s16)(gPalOptions_aWaveOffsets[row + 1][column] + 8);
                packet->y3 = y + (s16)(gPalOptions_aWaveOffsets[row + 1][column + 1] + 8);
                color = gPalOptions_aWaveColors[row][column];
                packet->r0 = color;
                packet->g0 = color >> 8;
                packet->b0 = color >> 16;
                /* Packed colour words; a DisplayObjectGT4Packet view reorders the loop. */
                *(u32 *)&packet->r1 = gPalOptions_aWaveColors[row][column + 1];
                *(u32 *)&packet->r2 = gPalOptions_aWaveColors[row + 1][column];
                *(u32 *)&packet->r3 = gPalOptions_aWaveColors[row + 1][column + 1];
                GsSortPoly(packet, ordering_table, 0);
                packet->u0 += 6;
            }
        }
    } else {
        /* SpritePrim is GsSPRITE field for field (sprite_primitive.h). */
        GsSortFastSprite((GsSPRITE *)sprite, ordering_table, 0);
    }
}

void PalOptions_NoOpLanguageHook(s32 language)
{
}

void Options_Init(s32 mode)
{
    RECT *rect;
    u8 *buffer;
    s32 index;
    DisplayObject *object;
    u32 music;

    buffer = gFile_TransferBufferBase;
    gPalOptions_bMode = mode;
    gPalOptions_bLanguage = gText_bLanguageIndex;
    rect = &gGraphics_aImageTransferRects[1];
    rect->x = 0x290;
    rect->y = 0;
    rect->w = 0x30;
    rect->h = 0x10;
    StoreImage(rect, (u32 *)(buffer + 0x2000));
    DrawSync(0);
    {
        RECT *first = rect - 1;

        first->x = 0x290;
        first->y = 0xC0;
        first->w = 0x30;
        first->h = 0x10;
        StoreImage(first, (u32 *)gFile_TransferBufferBase);
    }
    DrawSync(0);
    buffer = gFile_TransferBufferBase;
    index = 0x60;
    while (buffer[index] == buffer[index + 0x2000]) {
        if (++index >= 0x5A0) {
            gPalOptions_bLanguage = gFile_TransferBufferBase[0];
            break;
        }
    }
    if (mode != 0) {
        PalOptions_NoOpLanguageHook(gPalOptions_bLanguage);
        gPalOptions_bState = 2;
        music = 0x7370;
    } else {
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(
            object, 0, 0, 5, gPalOptions_bLanguage, 2, 0x10, 0x100, gDisplay_abResourceBank0);
        DisplayObject_SetDepthOffset(object, -5);
        object->flags |= 0x28;

        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(
            object, 0, 0, 5, gPalOptions_bLanguage, 3, 0x10, 0x100, gDisplay_abResourceBank0);
        DisplayObject_SetDepthOffset(object, -4);
        gPalOptions_pLanguageSprite = object;
        object->flags |= 0x28;
        gPalOptions_bSelectedRow = 0;

        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(
            object, 0, 0, 5, gPalOptions_bLanguage, 4, 0x10, 0x100, gDisplay_abResourceBank0);
        DisplayObject_SetDepthOffset(object, -4);
        object->flags |= 0x28;
        gPalOptions_pLabelSprite = object;
        gPalOptions_bSoundOutputType = gSD_bOutputType;
        if (gPalOptions_bSoundOutputType < 0) {
            gPalOptions_bSoundOutputType = 0;
        }

        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(
            object, 0, 0, 5, gPalOptions_bLanguage, gPalOptions_bSoundOutputType, 0x10, 0x100, gDisplay_abResourceBank0);
        DisplayObject_SetDepthOffset(object, -4);
        gPalOptions_pSoundTypeSprite = object;
        object->flags |= 0x28;
        gPalOptions_bState = 2;

        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(object, 20, 60, 3, 0, 2, 0xB, 0x20C);
        gPalOptions_pCursor = object;
        object->flags |= 0x28;
        PalOptions_PositionCursor(gPalOptions_bSelectedRow);
        gPalOptions_dwWavePhase = 0;
        music = 0x7350;
    }
    SD_BGMPlay(music);
}

void PalOptions_HandleInput(void)
{
    s32 selection = gPalOptions_bSelectedRow;

    if (selection == 0) {
        if ((gInput_awPadPressed[0] & PAD_DIRECTION_RIGHT) != 0 && gPalOptions_bSoundOutputType == 0) {
            gSD_bOutputType = 1;
            gPalOptions_bSoundOutputType = 1;
            SD_SetOutputType(1);
            SD_SEPlayFull(SD_SE_CHANGE);
        }
        if ((gInput_awPadPressed[0] & PAD_DIRECTION_LEFT) != 0 && gPalOptions_bSoundOutputType == 1) {
            gSD_bOutputType = 0;
            gPalOptions_bSoundOutputType = 0;
            SD_SetOutputType(0);
            SD_SEPlayFull(SD_SE_CHANGE);
        }
    } else {
        if ((gInput_awPadRepeat[0] & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
            selection = gPalOptions_bLanguage;
            if ((gInput_awPadRepeat[0] & PAD_DIRECTION_RIGHT) != 0) {
                selection++;
                if (selection >= 5) {
                    selection = 0;
                }
            } else {
                selection--;
                if (selection < 0) {
                    selection = 4;
                }
            }
            gPalOptions_bLanguage = selection;
            DisplayObject_SetResourceVariant(gPalOptions_pLanguageSprite, selection);
            gPalOptions_pLabelSprite->resource_branch_index = selection;
            DisplayObject_SetResourceVariant(gPalOptions_pLabelSprite, gPalOptions_pLabelSprite->resource_variant);
            gPalOptions_pSoundTypeSprite->resource_branch_index = selection;
            DisplayObject_SetResourceVariant(gPalOptions_pSoundTypeSprite, gPalOptions_pSoundTypeSprite->resource_variant);
            SD_SEPlayFull(SD_SE_CHANGE);
        }
    }
    if ((gInput_awPadPressed[0] & (PAD_BUTTON_CIRCLE | PAD_BUTTON_CROSS)) != 0) {
        gPalOptions_bState = 3;
        SD_SEPlayFull(SD_SE_CANCEL);
    }
}

void PalOptions_UpdateWaveTables(void)
{
    s32 row;
    s32 column;
    s32 amplitude;
    s32 phase;
    s32 value;

    gPalOptions_dwWavePhase += 0x100;
    for (row = 0; row < 5; row++) {
        amplitude = 0x200;
        phase = gPalOptions_dwWavePhase + (row << 8);
        for (column = 0; column < 9; column++) {
            value = 0;
            if (column == 0) {
                gPalOptions_aWaveOffsets[row][column] = 0;
            } else {
                gPalOptions_aWaveOffsets[row][column] = value =
                    ((amplitude >> 8) * rcos(phase & 0xFFF)) / 4096;
                if (value < 0) {
                    value <<= 4;
                } else {
                    value <<= 5;
                }
            }
            value += 0x80;
            if (value < 0) {
                value = 0;
            }
            if (value >= 0x100) {
                value = 0xFF;
            }
            gPalOptions_aWaveColors[row][column] = value | (value << 8) | (value << 16);
            amplitude += 0x60;
            phase -= 0x200;
        }
    }
}

void PalOptions_RequestLanguage(s32 language)
{
    if (gText_bLanguageIndex != language) {
        gText_bLanguageIndex = language;
        Main_RequestLanguageBootPackage(gText_bLanguageIndex, 0);
    }
}

void PalOptions_StoreLanguageImage(void)
{
    RECT *rect;

    Main_InitDebugFont();
    rect = gGraphics_aImageTransferRects;
    rect->x = 0x290;
    rect->y = 0;
    rect->w = 0x30;
    rect->h = 0x10;
    StoreImage(rect, (u32 *)gFile_TransferBufferBase);
    DrawSync(0);
    gFile_TransferBufferBase[0] = gText_bLanguageIndex;
    rect[1].x = 0x290;
    {
        RECT *destination = &rect[1];

        destination->y = 0xC0;
        destination->w = 0x30;
        destination->h = 0x10;
        LoadImage(destination, (u32 *)gFile_TransferBufferBase);
    }
    DrawSync(0);
}

s32 Options_Update(void)
{
    switch (gPalOptions_bState & 0xF) {
    case 1:
        PalOptions_HandleInput();
        break;
    case 2:
        if ((gPalOptions_bState & 0x80) == 0) {
            gPalOptions_bState |= 0x80;
            Fade_StartIn();
        }
        if ((gFade_State.flags & FADE_FLAG_ACTIVE) == 0) {
            gPalOptions_bState = 1;
        }
        break;
    case 3:
        if ((gPalOptions_bState & 0x80) == 0) {
            gPalOptions_bState |= 0x80;
            PalOptions_RequestLanguage(gPalOptions_bLanguage);
            SD_BGMFadeOut();
            Fade_StartOut();
        }
        if ((gFade_State.flags & FADE_FLAG_ACTIVE) == 0 &&
            ((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) | gFile_dwCancelRequestState) == 0) {
            PalOptions_StoreLanguageImage();
            gPalOptions_bState = 0;
        }
        break;
    }
    PalOptions_PositionCursor(gPalOptions_bSelectedRow);
    if (gPalOptions_bState != 0) {
        return -1;
    }
    return gPalOptions_bLanguage;
}

s32 PalOptions_StepStripScroll(void)
{
    s32 next;
    s32 direction;
    s32 difference;

    if (gPalOptions_wStripScroll != gPalOptions_wStripScrollTarget) {
        /* The unoptimized counterpart retains this unused frame store. */
        difference = gPalOptions_wStripScrollTarget - gPalOptions_wStripScroll;
        if (gPalOptions_wStripScroll < gPalOptions_wStripScrollTarget) {
            direction = 1;
            if (gPalOptions_wStripScrollTarget - gPalOptions_wStripScroll > 32767) {
                direction = -1;
            }
        } else {
            direction = -1;
            if (gPalOptions_wStripScroll - gPalOptions_wStripScrollTarget > 32767) {
                direction = 1;
            }
        }
        next = gPalOptions_wStripScroll;
        if (direction >= 0) {
            next += 4;
            if (gPalOptions_wStripScrollTarget - next <= 0) {
                next = gPalOptions_wStripScrollTarget;
            }
        } else {
            next -= 4;
            if (gPalOptions_wStripScrollTarget - next >= 0) {
                next = gPalOptions_wStripScrollTarget;
            }
        }
        gPalOptions_wStripScroll = next;
    }
    return gPalOptions_wStripScroll == gPalOptions_wStripScrollTarget;
}

s32 PalOptions_GetLanguage(void)
{
    return gPalOptions_bLanguage;
}
