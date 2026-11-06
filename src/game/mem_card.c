#include "../types.h"

/* Zero-initialized storage this unit owns at 0x8009B3B8-0x8009B3BC, defined in
 * address order ahead of the headers that declare it (#2602). The European
 * builds keep it inside the memory card dialog's small storage block, which
 * defines it there. */
#ifndef SAVE_DATA_SEQUENCE_IN_MEM_CARD_DIALOG
s32 gSaveDataSequence;
#endif

#ifndef MEM_CARD_DIALOG_STORAGE_ORDER
/* Zero-initialized storage this unit owns at 0x800EF6D0-0x800EFE38. GCC lays
 * it out in first-declaration order, so it is declared here in address order
 * ahead of the headers and defined once they supply the types (#2602). A
 * regional wrapper that lays it out differently declares its own order and
 * defines MEM_CARD_DIALOG_STORAGE_ORDER. */
extern struct MemCardWorkArea gMemCardWork_Area;
extern u8 gMemCardDialog_DirectoryBuffer[];
extern u8 gMemCardDialog_FileNameBuffer[];
#endif

#include "text_box_blocking_build.h"
#include "mem_card_dialog.h"

#ifndef MEM_CARD_DIALOG_REGIONAL_SMALL_STORAGE
/* Zero-initialized storage this unit owns at 0x8009B3BC-0x8009B400, defined in
 * address order ahead of the headers that declare it (#2602). */
s32 gMemCardDialog_SyncStatus;
u8 gSaveData_PairMessageCode;
u8 gDialog_bCallbackState;
u16 gMemCardDialog_TransferSize;
u16 gMemCardDialog_TransferOffset;
u8 gMemCardDialog_MessageCode;
u8 gMemCardWork_OperationMode;
s32 gMemCardWork_nRequestResult;
u16 gMemCardWork_wFlags;
static PADDING_VARIABLE(1);
u8 gMemCardWork_OperationState;
u8 *gMemCard_pPrimaryTransferCursor;
u8 gSaveData_AllowIdentityBypass;
u8 gMemCardWork_bOperationResult;
DisplayObject *gMemCard_pDialogObject;
u8 gMemCardDialog_BlockCount;
u8 gMemCardWork_bSlotIndex;
u8 gMemCardDialog_StepIndex;
u8 *gMemCard_pSecondaryTransferCursor;
s32 gMemCardWork_nCompletedRequest;
static PADDING_VARIABLE(2);
u8 gSaveData_LoadPairState;
u8 gMemCardDialog_OperationState;
u8 gMemCardDialog_RetryCounter;
u8 gSaveData_PairSetupState;
u8 gMemCardDialog_ChannelIndex;
u8 gMemCardDialog_Outcome;
s32 gMemCardDialog_SyncCommand;
s32 gMemCardDialog_SyncResult;
static PADDING_VARIABLE(1);
u8 gMemCardDialog_Port;
u16 gMemCard_wDialogFlags;
static PADDING_VARIABLE(4);
#else
extern s32 gMemCardDialog_SyncStatus;
extern u8 gSaveData_PairMessageCode;
extern u8 gDialog_bCallbackState;
extern u16 gMemCardDialog_TransferSize;
extern u16 gMemCardDialog_TransferOffset;
extern u8 gMemCardDialog_MessageCode;
extern u8 gMemCardWork_OperationMode;
extern s32 gMemCardWork_nRequestResult;
extern u16 gMemCardWork_wFlags;
extern u8 gMemCardWork_OperationState;
extern u8 *gMemCard_pPrimaryTransferCursor;
extern u8 gSaveData_AllowIdentityBypass;
extern u8 gMemCardWork_bOperationResult;
extern DisplayObject *gMemCard_pDialogObject;
extern u8 gMemCardDialog_BlockCount;
extern u8 gMemCardWork_bSlotIndex;
extern u8 gMemCardDialog_StepIndex;
extern u8 *gMemCard_pSecondaryTransferCursor;
extern s32 gMemCardWork_nCompletedRequest;
extern u8 gSaveData_LoadPairState;
extern u8 gMemCardDialog_OperationState;
extern u8 gMemCardDialog_RetryCounter;
extern u8 gSaveData_PairSetupState;
extern u8 gMemCardDialog_ChannelIndex;
extern u8 gMemCardDialog_Outcome;
extern s32 gMemCardDialog_SyncCommand;
extern s32 gMemCardDialog_SyncResult;
extern u8 gMemCardDialog_Port;
extern u16 gMemCard_wDialogFlags;
#endif

#include "save_data.h"
#include "player_name_glyphs.h"
#include "graphics_frame.h"
#include "sound.h"
#include "text_sjis_to_glyph_codes.h"
#include "util_memory.h"

#define SAVE_DATA_PRIMARY_VIEW(data) ((SaveDataPrimaryIntegrity *)(data))
#define SAVE_DATA_SECONDARY_VIEW(data) \
    ((SaveDataSecondaryIntegrity *)((u8 *)(data) + SAVE_DATA_SECONDARY_OFFSET))
#define SAVE_DATA_TERTIARY_VIEW(data) \
    ((SaveDataTertiaryIntegrity *)((u8 *)(data) + SAVE_DATA_TERTIARY_OFFSET))

/* SaveData_ApplyRuntimeState stores the campaign scene index through this
 * absolute address (the literal form is what matches); a regional build names
 * its own. */
#ifndef SAVE_DATA_CAMPAIGN_SCENE_INDEX_ADDRESS
#define SAVE_DATA_CAMPAIGN_SCENE_INDEX_ADDRESS 0x8009B27A
#endif

/* The save payload: sealing it, building it, applying it, and checking it.

   Each of the three payload regions is sealed the same way - a CRC-16 over
   the region, stored twice, then seeding the two-word mask generator from
   that CRC and writing a run of mask words backwards from the region's last
   mask slot. SaveData_WritePrimarySecondaryIntegrity and
   SaveData_WriteTertiaryIntegrity write those seals, SaveData_BuildPayload
   assembles the payload and calls both, and SaveData_ValidateIntegrity
   replays the same three seeds and compares the stored words against the
   generator instead of writing them. SaveData_ApplyRuntimeState is
   BuildPayload's inverse, loading the fields BuildPayload stores back into
   the runtime, and the last pair compares two payload states by duelist
   code and save sequence.

   The manifest used to spread this over six sources, recorded at
   gcc_2_8_1_g8, gcc_2_8_1_g8_split and gcc_2_8_1_g8_no_split. None of those
   differences was a boundary: every member compiles to an identical object
   at gcc_2_8_1_g8, so the unit builds there. */

u32 gSaveData_dwMaskStateLow = 0x55555555;
u32 gSaveData_dwMaskStateHigh = 0x55555555;

/* Advances the two-word save-data mask state and returns the next word. */
u32 SaveData_NextMaskWord(void)
{
    u32 *state = &gSaveData_dwMaskStateLow;
    u32 low;
    u32 next;
    u32 high;
    u32 sum;

    low = state[0];
    high = state[1];
    next = (high << 31) | (low >> 1);
    next ^= low << 12;
    sum = high + (low & 1);
    high += sum;
    state[1] = high;
    return state[0] = next ^ (next >> 20);
}

/* CRC-16/XMODEM (poly 0x1021, zero-initialized) over data[0..len). */
u32 SaveData_CalcCrc16(u8 *data, s32 len)
{
    u16 crc = 0;
    s32 i;

    i = 0;
    if (len > 0) {
        do {
            s32 bit;

            crc ^= data[i] << 8;
            for (bit = 0; bit < 8; bit++) {
                if (crc & SAVE_DATA_CRC16_HIGH_BIT) {
                    crc = (crc << 1) ^ SAVE_DATA_CRC16_POLYNOMIAL;
                } else {
                    crc = crc << 1;
                }
            }
            i++;
        } while (i < len);
    }
    return crc;
}

void SaveData_WritePrimarySecondaryIntegrity(u8 *data)
{
    s32 value = SaveData_CalcCrc16(data, SAVE_DATA_PRIMARY_LENGTH);
    u32 seed = value & SAVE_DATA_CRC16_MASK;
    s32 *output = &SAVE_DATA_PRIMARY_VIEW(data)->mask_words[
        SAVE_DATA_PRIMARY_MASK_WORD_COUNT - 1];
    s32 i = SAVE_DATA_PRIMARY_MASK_WORD_COUNT;

    SAVE_DATA_PRIMARY_VIEW(data)->checksum[1] = value;
    SAVE_DATA_PRIMARY_VIEW(data)->checksum[0] = value;
    gSaveData_dwMaskStateHigh = seed | (seed << SAVE_DATA_CRC16_BITS);
    gSaveData_dwMaskStateLow = seed | (seed << SAVE_DATA_CRC16_BITS);

    do {
        i--;
        *output = SaveData_NextMaskWord();
        output--;
    } while (i != 0);

    value = SaveData_CalcCrc16(
        data + SAVE_DATA_SECONDARY_OFFSET,
        SAVE_DATA_SECONDARY_LENGTH
    );
    seed = value & SAVE_DATA_CRC16_MASK;
    output = &SAVE_DATA_SECONDARY_VIEW(data)->mask_words[
        SAVE_DATA_SECONDARY_MASK_WORD_COUNT - 1];
    i = SAVE_DATA_SECONDARY_MASK_WORD_COUNT;

    SAVE_DATA_SECONDARY_VIEW(data)->checksum[1] = value;
    SAVE_DATA_SECONDARY_VIEW(data)->checksum[0] = value;
    gSaveData_dwMaskStateHigh = seed | (seed << SAVE_DATA_CRC16_BITS);
    gSaveData_dwMaskStateLow = seed | (seed << SAVE_DATA_CRC16_BITS);

    do {
        i--;
        *output = SaveData_NextMaskWord();
        output--;
    } while (i != 0);
}

void SaveData_WriteTertiaryIntegrity(u8 *p)
{
    register s32 i;
    register u32 *dst;
    u16 v = SaveData_CalcCrc16(p + SAVE_DATA_TERTIARY_OFFSET,
                               SAVE_DATA_TERTIARY_LENGTH);

    dst = &SAVE_DATA_TERTIARY_VIEW(p)->mask_words[
        SAVE_DATA_TERTIARY_MASK_WORD_COUNT - 1];
    i = SAVE_DATA_TERTIARY_MASK_WORD_COUNT;
    SAVE_DATA_TERTIARY_VIEW(p)->checksum[1] = v;
    SAVE_DATA_TERTIARY_VIEW(p)->checksum[0] = v;
    {
        u32 seed = v | (v << SAVE_DATA_CRC16_BITS);

        gSaveData_dwMaskStateHigh = seed;
        gSaveData_dwMaskStateLow = seed;
    }
    do {
        --i;
        *dst = SaveData_NextMaskWord();
        dst--;
    } while (i);
}

void SaveData_BuildPayload(SaveDataPayload *data)
{
    u8 *copy;
    s32 output_type;
    s32 value;
    s32 saved_value;

    Util_CopyWords(
        data->header, gSaveData_aHeaderTemplate, SAVE_DATA_HEADER_SIZE
    );

    saved_value = gSaveData_dwVBlankCounter;
    output_type = gSD_bOutputType;
    data->state.field_400 = 0;
    data->state.vblank_counter = saved_value;

    if (output_type < 0) {
        gSD_bOutputType = 0;
    }

    data->state.output_type = gSD_bOutputType;
    copy = (u8 *)&data->state;
    value = gSaveDataSequence + 1;
    data->state.save_sequence = value;
    data->duplicate.save_sequence = value;

    SaveData_WritePrimarySecondaryIntegrity(copy);
    SaveData_WriteTertiaryIntegrity(copy);

    {
        u32 i;

        i = 0;
        do {
            *((u8 *)data + i + SAVE_DATA_RESERVED_TAIL_PAYLOAD_OFFSET) = 0;
            i++;
        } while (i < SAVE_DATA_RESERVED_TAIL_SIZE);
    }

    Util_CopyWords(
        (u8 *)&data->duplicate, (u8 *)&data->state, SAVE_DATA_STATE_SIZE
    );
}

#define gCampaignSceneIndex (*(u8 *)SAVE_DATA_CAMPAIGN_SCENE_INDEX_ADDRESS)

void SaveData_ApplyRuntimeState(SaveDataState *state) {
    Text_SjisToGlyphCodes(gText_aCurrentPlayerNameGlyphs, state->player_name_sjis, SAVE_DATA_PLAYER_NAME_CHAR_COUNT);

    /*
     * These discarded addresses use the retail assembler-temporary form.
     * Symbolic stores allocate ordinary registers and change exact codegen.
     */
    gSaveData_dwVBlankCounter = state->vblank_counter;
    gSaveDataSequence = state->save_sequence;
    gCampaignSceneIndex = state->campaign_scene_index;

    if (gSD_bOutputType < 0) {
        u8 output_type = state->output_type;

        gSD_bOutputType = output_type;
        SD_SetOutputType((s8)output_type);
    }
}

s32 SaveData_ValidateIntegrity(u8 *data)
{
    s32 seed;
    s32 i;
    s32 *word;

    seed = SaveData_CalcCrc16(data, SAVE_DATA_PRIMARY_LENGTH) &
           SAVE_DATA_CRC16_MASK;
    word = &SAVE_DATA_PRIMARY_VIEW(data)->mask_words[
        SAVE_DATA_PRIMARY_MASK_WORD_COUNT - 1];
    i = SAVE_DATA_PRIMARY_MASK_WORD_COUNT;
    gSaveData_dwMaskStateHigh = seed | (seed << SAVE_DATA_CRC16_BITS);
    gSaveData_dwMaskStateLow = seed | (seed << SAVE_DATA_CRC16_BITS);
    do {
        if (*word != SaveData_NextMaskWord()) {
            return 0;
        }
        i--;
        word--;
    } while (i != 0);

    seed = SaveData_CalcCrc16(
        data + SAVE_DATA_SECONDARY_OFFSET,
        SAVE_DATA_SECONDARY_LENGTH
    ) & SAVE_DATA_CRC16_MASK;
    word = &SAVE_DATA_SECONDARY_VIEW(data)->mask_words[
        SAVE_DATA_SECONDARY_MASK_WORD_COUNT - 1];
    i = SAVE_DATA_SECONDARY_MASK_WORD_COUNT;
    gSaveData_dwMaskStateHigh = seed | (seed << SAVE_DATA_CRC16_BITS);
    gSaveData_dwMaskStateLow = seed | (seed << SAVE_DATA_CRC16_BITS);
    do {
        if (*word != SaveData_NextMaskWord()) {
            return 0;
        }
        i--;
        word--;
    } while (i != 0);

    seed = SaveData_CalcCrc16(
        data + SAVE_DATA_TERTIARY_OFFSET,
        SAVE_DATA_TERTIARY_LENGTH
    ) & SAVE_DATA_CRC16_MASK;
    word = (s32 *)&SAVE_DATA_TERTIARY_VIEW(data)->mask_words[
        SAVE_DATA_TERTIARY_MASK_WORD_COUNT - 1];
    i = SAVE_DATA_TERTIARY_MASK_WORD_COUNT;
    gSaveData_dwMaskStateHigh = seed | (seed << SAVE_DATA_CRC16_BITS);
    gSaveData_dwMaskStateLow = seed | (seed << SAVE_DATA_CRC16_BITS);
    do {
        if (*word != SaveData_NextMaskWord()) {
            return 0;
        }
        i--;
        word--;
    } while (i != 0);

    return 1;
}

s32 SaveData_HasSameDuelistCode(SaveDataState *left, SaveDataState *right)
{
    s32 i;

    if (left->duelist_code == right->duelist_code) {
        i = 6;
        while (--i >= 0) {
        }
        return 1;
    }
    return 0;
}

s32 SaveData_MatchesDuelistAndCurrentSequence(
    SaveDataState *left,
    SaveDataState *right)
{
    s32 result;

    if (SaveData_HasSameDuelistCode(left, right)) {
        result = gSaveDataSequence == right->save_sequence;
    } else {
        result = 0;
    }
    return result;
}

#undef gCampaignSceneIndex

#include "duel_effect.h"
#include "mem_card_work_initialize_operation.h"
#include "mem_card_work.h"
#include "mem_card_work_state.h"
#include "../unmatched.h"

void MemCardWork_InitializeOperation(u8 value)
{
    gMemCardWork_Area.root.blocks = 1;
    gMemCardWork_OperationState = 0;
    gDialog_bCallbackState = 0;
    gMemCardWork_wFlags = 0x8000;
    gMemCardWork_bSlotIndex = 0;
    gMemCardWork_OperationMode = value;
}

#include "checkerboard_background.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

#define CHECKERBOARD_CELL_WIDTH 0x40
#define CHECKERBOARD_CELL_HEIGHT 0x20
#define CHECKERBOARD_SCREEN_WIDTH 0x140
/* Regional values: the European build (src/game/european/) defines its own. */
#ifndef CHECKERBOARD_SCREEN_HEIGHT
#define CHECKERBOARD_SCREEN_HEIGHT 0xF0
#endif
/* GsSPRITE cx: the CLUT x of the first cell colour; the second is 0x10 on. */
#ifndef CHECKERBOARD_CLUT_X
#define CHECKERBOARD_CLUT_X 0x2C0
#endif
#define GS_SPRITE_VIEW(sprite) ((GsSPRITE *)(sprite))

void func_8003D32C(void)
{
}

/* Tiles the 320x240 background with alternating 64x32 sprite cells. */
void Checkerboard_DrawBackground(
    CheckerboardState *state, GsOT *ordering_table)
{
    CheckerboardSprite *sprite = (CheckerboardSprite *)0x1F800000;
    s32 y;
    s32 flip;
    s32 mode;
    s32 x;
    s32 attribute;
    s32 rgb;

    attribute = state->attribute;
    sprite->wh = 0x200020;
    sprite->attribute = attribute;
    rgb = state->rgb;
    sprite->tpage = 0xB;
    sprite->cx = CHECKERBOARD_CLUT_X;
    sprite->cy = 0xFC;
    mode = state->ot_depth;
    sprite->uv.word = 0x3030;
    sprite->rgb = rgb;

    flip = 0;
    y = 0;
    do {
        x = 0;
        sprite->x = flip << 5;
        sprite->uv.bytes.u = 0x30;
        sprite->y = y;
        sprite->cx = CHECKERBOARD_CLUT_X;
        do {
            GsSortFastSprite(GS_SPRITE_VIEW(sprite), ordering_table,
                             mode & 0xFFFF);
            x += CHECKERBOARD_CELL_WIDTH;
            sprite->x =
                sprite->x + CHECKERBOARD_CELL_WIDTH;
        } while (x < CHECKERBOARD_SCREEN_WIDTH);

        x = 0;
        sprite->x = (flip ^ 1) << 5;
        sprite->uv.bytes.u = 0x50;
        sprite->cx = CHECKERBOARD_CLUT_X + 0x10;
        do {
            GsSortFastSprite(GS_SPRITE_VIEW(sprite), ordering_table,
                             mode & 0xFFFF);
            x += CHECKERBOARD_CELL_WIDTH;
            sprite->x =
                sprite->x + CHECKERBOARD_CELL_WIDTH;
        } while (x < CHECKERBOARD_SCREEN_WIDTH);

        y += CHECKERBOARD_CELL_HEIGHT;
        flip ^= 1;
    } while (y < CHECKERBOARD_SCREEN_HEIGHT);
}

#include "text_box_blocking_build.h"
#include "duel_effect.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "display_object_helpers.h"
#include "display_object_depth_bases.h"
#include "duel_effect_create_channel.h"
#include "dialog_layout.h"
#include "mem_card_work.h"

/* Regional value: the channel flags it sets (0x1008 -> 0x1010). The European
 * build (src/game/european/) defines its own. */
#ifndef DUEL_EFFECT_CHANNEL_CREATE_FLAGS
#define DUEL_EFFECT_CHANNEL_CREATE_FLAGS 0x1008
#endif

#ifndef DIALOG_CHOICE_ADDRESS
#define DIALOG_CHOICE_ADDRESS 0x8009B34D
#endif

#ifndef DUEL_EFFECT_CHANNEL_HEIGHT
#define DUEL_EFFECT_CHANNEL_HEIGHT 0x40
#endif

#define gDialog_bChoice (*(s8 *)DIALOG_CHOICE_ADDRESS)

DuelEffectChannel *DuelEffect_CreateChannel(s32 value, s32 set_flags) {
    DuelEffectChannel *channel;

    /* A symbolic store changes the retail assembler-temporary address form. */
    gDialog_bChoice = -1;
    channel = TextBox_Create(
        gMemCardWork_Area.root.text_index,
        value & 0x7FFF,
        DIALOG_BOX_X,
        0x50,
        DIALOG_BOX_WIDTH,
        DUEL_EFFECT_CHANNEL_HEIGHT
    );
    channel->depth_offset = *(u8 *)&gDisplayObject_awOrderingTableDepthBases[1] - 1;

    if (set_flags != 0) {
        channel->flags_34 |= DUEL_EFFECT_CHANNEL_CREATE_FLAGS;
    } else if (value & 0x8000) {
        TextBox_BuildBlocking(channel);
    }

    return channel;
}

#undef gDialog_bChoice

#include "../unmatched.h"
#include "display_object.h"
#include "display_object_interpolation.h"
#include "display_object_core.h"

#include "duel_effect.h"
#include "text_channel_state.h"
#include "display_object_helpers.h"
#include "display_object_depth_bases.h"
#include "text_box_runtime.h"
#include "text_box_update_channels.h"
#include "duel_effect_create_channel.h"
#include "dialog_transition.h"
#include "dialog_update_message_transition.h"
#include "dialog_layout.h"
#include "dialog_choice.h"

#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))
#define DISPLAY_OBJECT_POSITION_VIEW(object) \
    ((DisplayObjectPosition *)(object))
#define DISPLAY_OBJECT_SNAPSHOT_VIEW(object) \
    ((DisplayObjectSnapshot *)(object))

/* Regional values, which the European wrappers define for themselves: the
   spotlight's centre y (120 in the US build, 128 in the European one) and
   the channel flag Dialog_UpdateOpenTransition sets while its text box is up (4 in the US
   build, 8 in the European one). */
#ifndef DIALOG_TRANSITION_SPOTLIGHT_Y
#define DIALOG_TRANSITION_SPOTLIGHT_Y 120
#endif
#ifndef DIALOG_TRANSITION_CHANNEL_FLAG
#define DIALOG_TRANSITION_CHANNEL_FLAG 4
#endif

void Dialog_UpdateSpotlightContraction(MenuRecord *record)
{
    DisplayObject *object;
    s32 flags;

    if ((gDialog_bCallbackState & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        gDialog_bCallbackState |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
        object->xy.h.x = 160;
        object->xy.h.y = DIALOG_TRANSITION_SPOTLIGHT_Y;
        object->field_48.spotlight.inner = 128;
        object->field_48.spotlight.outer = 224;
        DisplayObject_SelectOrderingTable1(object);
        DisplayObject_SetDepthOffset(object, (s8)(*(u8 *)&gDisplayObject_awOrderingTableDepthBases[1] - 3));
        object->field_4C = (s32)DisplayObject_RenderSpotlightMask;
        record->grid[0][1] = (s32)object;
    }
    flags = gDialog_bCallbackState;
    object = DISPLAY_OBJECT_VIEW(record->grid[0][1]);
    if (flags & 64) {
        gDialog_bCallbackState = 0;
    } else {
        s32 x = *(u16 *)&object->field_48.spotlight.inner - 8;
        s32 y = *(u16 *)&object->field_48.spotlight.outer - 8;

        object->field_48.spotlight.outer = y;
        object->field_48.spotlight.inner = x;
        if ((s16)x <= 0) {
            gDialog_bCallbackState = flags | 64;
            object->field_48.spotlight.inner = 0;
            object->field_48.spotlight.outer = 64;
        }
    }
}

void Dialog_UpdateCloseTransition(MenuRecord *record)
{
    DisplayObject *object;
    DuelEffectChannel *entry;

    if (!(gDialog_bCallbackState & DUEL_EFFECT_STATE_FLAG_INITIALIZED)) {
        object = DISPLAY_OBJECT_VIEW(record->grid[0][0]);
        gDialog_bCallbackState |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        DisplayObject_SavePosition(DISPLAY_OBJECT_SNAPSHOT_VIEW(object));
        object->field_60 = 0x400;
    }
    /* The dialog channel's index: the byte at 0x1A, inside grid's third
       row, which MenuRecord does not name. */
#ifdef DIALOG_TRANSITION_EFFECT_CHANNEL_STRIDE
    entry = (DuelEffectChannel *)((u8 *)gText_aChannels +
        ((u8 *)record)[0x1A] * DIALOG_TRANSITION_EFFECT_CHANNEL_STRIDE);
#else
    entry = &gText_aChannels[((u8 *)record)[0x1A]];
#endif
    object = DISPLAY_OBJECT_VIEW(record->grid[0][0]);
    if (object) {
        *(u16 *)&object->field_60 -= 0x40;
        if (object->field_60 <= 0) {
            DisplayObject_ReleaseIfPresent(object);
            record->grid[0][0] = 0;
        } else {
            Widget_SlideSine(
                DISPLAY_OBJECT_POSITION_VIEW(object),
                DIALOG_BOX_X,
                -0x40,
                object->field_60
            );
            TextBox_SetPos(
                entry,
                *(s16 *)&object->xy.h.x,
                *(s16 *)&object->xy.h.y
            );
        }
    }
    object = DISPLAY_OBJECT_VIEW(record->grid[0][1]);
    if (object) {
        *(u16 *)&object->field_48.spotlight.outer += 8;
        *(u16 *)&object->field_48.spotlight.inner += 8;
        if (object->field_48.spotlight.inner >= 0xC0) {
            DisplayObject_ReleaseIfPresent(object);
            record->grid[0][1] = 0;
        }
    }
    if (!record->grid[0][0] && !record->grid[0][1])
        gDialog_bCallbackState = 0;
}

void Dialog_UpdateOpenTransition(MenuRecord *record)
{
    DisplayObject *p;
    DuelEffectChannel *r;
    s32 f;
    s32 v;
    s32 t;
    s32 a;
    s32 g;

    if ((gDialog_bCallbackState & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        gDialog_bCallbackState =
            gDialog_bCallbackState | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        p = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
        p->xy.h.x = 0xA0;
        p->xy.h.y = DIALOG_TRANSITION_SPOTLIGHT_Y;
        p->field_48.spotlight.inner = 0x80;
        p->field_48.spotlight.outer = 0xE0;
        DisplayObject_SelectOrderingTable1(p);
        DisplayObject_SetDepthOffset(p, (s8)(*(u8 *)&gDisplayObject_awOrderingTableDepthBases[1] - 3));
        p->field_4C = (s32)DisplayObject_RenderSpotlightMask;
        record->grid[0][1] = (s32)p;
        p = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(p, DIALOG_BOX_X, -0x40, 3, 2, 0, 0xB, 0x20C);
        p->flags = p->flags | DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                   DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SelectOrderingTable1(p);
        DisplayObject_SetDepthOffset(p, (s8)(*(u8 *)&gDisplayObject_awOrderingTableDepthBases[1] - 2));
        record->grid[0][0] = (s32)p;
        DisplayObject_SavePosition(DISPLAY_OBJECT_SNAPSHOT_VIEW(p));
        a = gMemCardWork_OperationMode;
        p->field_60 = -0x400;
        r = DuelEffect_CreateChannel((a & 1) | 0xD0, 0);
        r->flags_34 = r->flags_34 | DIALOG_TRANSITION_CHANNEL_FLAG;
        do {
            TextBox_UpdateChannels();
        } while (r->prompt_object == 0);
        TextBox_SetPos(r, *(s16 *)&p->xy.h.x,
                       *(s16 *)&p->xy.h.y);
    }
    f = gDialog_bCallbackState;
    if ((f & 0x20) != 0) {
        gDialog_bCallbackState = 0;
        return;
    }
    /* The dialog channel's index: the byte at 0x1A, inside grid's third
       row, which MenuRecord does not name. */
#ifdef DIALOG_TRANSITION_EFFECT_CHANNEL_STRIDE
    r = (DuelEffectChannel *)((u8 *)gText_aChannels +
        ((u8 *)record)[0x1A] * DIALOG_TRANSITION_EFFECT_CHANNEL_STRIDE);
#else
    r = &gText_aChannels[((u8 *)record)[0x1A]];
#endif
    if ((f & 0x40) != 0) {
        TextBox_UpdateChannels();
        if ((r->flags_34 & TEXT_BOX_FLAG_DONE) != 0) {
            gDialog_bCallbackState = gDialog_bCallbackState | 0x20;
            if (gDialog_bChoice == 0) {
                DuelEffect_CreateChannel(0x80D4, 0);
                gDialog_bChoice = 0;
            }
        }
        return;
    }
    p = DISPLAY_OBJECT_VIEW(record->grid[0][0]);
    t = *(u16 *)&p->field_60;
    if (p->field_60 < 0) {
        v = t + 0x40;
        p->field_60 = v;
        if ((s16)v >= 0) {
            p->xy.h.x = DIALOG_BOX_X;
            p->field_60 = 0;
            p->xy.h.y = 0x50;
        } else {
            Widget_SlideSine(
                DISPLAY_OBJECT_POSITION_VIEW(p), DIALOG_BOX_X, 0x50, (s16)v);
        }
        TextBox_SetPos(r, *(s16 *)&p->xy.h.x,
                       *(s16 *)&p->xy.h.y);
    }
    p = DISPLAY_OBJECT_VIEW(record->grid[0][1]);
    p->field_48.spotlight.outer = *(u16 *)&p->field_48.spotlight.outer - 8;
    p->field_48.spotlight.inner = *(u16 *)&p->field_48.spotlight.inner - 8;
    if (p->field_48.spotlight.inner <= 0) {
        p->field_48.spotlight.inner = 0;
        p->field_48.spotlight.outer = 0x40;
        if (DISPLAY_OBJECT_VIEW(record->grid[0][0])->field_60 == 0) {
            g = gDialog_bCallbackState;
            r->flags_34 = r->flags_34 & (0xFFFF ^ DIALOG_TRANSITION_CHANNEL_FLAG);
            gDialog_bCallbackState = g | 0x40;
        }
    }
}

/* Regional value: the flags_34 bit set and cleared on the channel (0x4 ->
 * 0x8). The European build (src/game/european/) defines its own. */
#ifndef FUNC_8003DA40_CHANNEL_FLAG
#define FUNC_8003DA40_CHANNEL_FLAG 4
#endif

void Dialog_UpdateMessageTransition(MenuRecord *record)
{
    DisplayObject *e;
    DuelEffectChannel *q;
    s32 f;
    s32 g;
    s32 v;
    s32 h;

    f = gDialog_bCallbackState;
    if ((f & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        gDialog_bCallbackState = f | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        e = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(e, DIALOG_BOX_X, -0x40, 3, 2, 0, 0xB, 0x20C);
        e->flags = e->flags | DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                   DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SelectOrderingTable1(e);
        DisplayObject_SetDepthOffset(e, (s8)(*(u8 *)&gDisplayObject_awOrderingTableDepthBases[1] - 2));
        record->grid[0][0] = (s32)e;
        DisplayObject_SavePosition((DisplayObjectSnapshot *)e);
        e->field_60 = -0x400;
        q = DuelEffect_CreateChannel(0xD0, 0);
        q->flags_34 = q->flags_34 | FUNC_8003DA40_CHANNEL_FLAG;
        do {
            TextBox_UpdateChannels();
        } while (q->prompt_object == 0);
        TextBox_SetPos(q,
                       *(s16 *)&e->xy.h.x,
                       *(s16 *)&e->xy.h.y);
    }

    e = (DisplayObject *)record->grid[0][0];
    /* The dialog channel's index. It is the byte at 0x1A, inside grid's
       third row, and dialog_transition.c reads it the same way; nothing
       matched yet writes it, so MenuRecord has no name for it. */
#ifdef DIALOG_TRANSITION_EFFECT_CHANNEL_STRIDE
    q = (DuelEffectChannel *)((u8 *)gText_aChannels +
        ((u8 *)record)[0x1A] * DIALOG_TRANSITION_EFFECT_CHANNEL_STRIDE);
#else
    q = &gText_aChannels[((u8 *)record)[0x1A]];
#endif
    g = gDialog_bCallbackState;

    if ((g & 0x40) != 0) {
        TextBox_UpdateChannels();
        if ((q->flags_34 & 0x2000) != 0) {
            gDialog_bCallbackState = 0;
        }
    } else {
        v = *(u16 *)&e->field_60 + 0x20;
        e->field_60 = v;
        if ((s16)v >= 0) {
            *(s16 *)&e->xy.h.x = DIALOG_BOX_X;
            *(s16 *)&e->xy.h.y = 0x50;
            h = q->flags_34;
            gDialog_bCallbackState = g | 0x40;
            q->flags_34 = h & (0xFFFF ^ FUNC_8003DA40_CHANNEL_FLAG);
        } else {
            Widget_SlideSine((DisplayObjectPosition *)e, DIALOG_BOX_X, 0x50, (s16)v);
        }
        TextBox_SetPos(q,
                       *(s16 *)&e->xy.h.x,
                       *(s16 *)&e->xy.h.y);
    }
}

/* The memory-card dialog and the save-transfer runtime built on it: 22
 * contiguous functions from the low-level card controller MemCardWork_UpdateOperation
 * (0x8003DC1C) through SaveData_RequestTradeWrite. The first fifteen drive
 * the card controller and the load/save/trade dialog; the last seven
 * request, poll and apply single-player and two-player save transfers
 * through that dialog. */

#include "display_object.h"
#include "display_object_interpolation.h"
#include "display_object_lifecycle.h"
#include "display_object_core.h"
#include "../psyq/libmcrd.h"
#include "../psyq/strings.h"
#include "data_transfer_request.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "mem_card.h"
#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
/* The Japanese and PAL executables also link the PocketStation library
 * (LIBMCX); the save dialog brackets its card session with McxStartCom and
 * McxStopCom, and its save state 2 polls McxCardType, McxGetApl and
 * McxSync. See notes/psyq.md. */
#include "../psyq/libmcx.h"
#endif
#include "save_data.h"
#include "save_pair_workspace.h"
#include "text_staging.h"
#include "text_message_staging.h"
#include "file_transfer.h"
#include "text_box_lifecycle.h"
#include "display_object_helpers.h"
#include "text_box_runtime.h"
#include "text_box_update_channels.h"
#include "duel_effect_mark_object_if_active.h"
#include "mem_card_dialog_steps.h"
#include "dialog_layout.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/stdio.h"
#include "mem_card_directory.h"
#include "mem_card_directory_patterns.h"
#include "mem_card_work.h"
#include "mem_card_work_state.h"
#include "mem_card_diagnostic_strings.h"
#include "io_event_helpers.h"
#include "duel_effect_create_channel.h"
#include "dialog_choice.h"
#include "file_constants.h"
#include "util_memory.h"
#include "card_constants.h"
#include "input.h"
#include "save_data_update_trade_load.h"
#include "save_data_update_load_pair.h"
#include "text_sjis_to_glyph_codes.h"
#include "two_player_save_setup.h"
#include "sound.h"
#include "../unmatched.h"
#include "paired_player_name_glyphs.h"
#include "save_pair_transfer_views.h"

MemCardWorkArea gMemCardWork_Area;
u8 gMemCardDialog_DirectoryBuffer[600];
u8 gMemCardDialog_FileNameBuffer[32];

/* Initialized data this unit owns at 0x80090F9C: the memory card dialog step table.
 *
 * mem_card_dialog.c calls `gMemCardDialog_apfnSteps[gMemCardDialog_StepIndex]()`, where
 * gMemCardDialog_StepIndex is a u8 written by that runtime's request API. Five entries against
 * an unmasked byte index, so the array stays unsized for the same reason as
 * the command table above. */

void (*gMemCardDialog_apfnSteps[])(void) = {
    MemCardDialog_StepLoad,
    MemCardDialog_StepLoadUnprompted,
    MemCardDialog_StepSave,
#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
    MemCardDialog_StepAlternateSave,
#else
    MemCardDialog_StepNone,
#endif
    MemCardDialog_UpdateTradeSave,
};

/* Regional value: the completion mask it tests (0x2008 -> 0x2010). The
 * European build (src/game/european/) defines its own. */
#ifndef MEM_CARD_MESSAGE_COMPLETION_MASK
#define MEM_CARD_MESSAGE_COMPLETION_MASK 0x2008
#endif

void MemCardWork_UpdateOperation(void)
{
    MemCardWorkRoot *root = &gMemCardWork_Area.root;
    MemCardWorkSlot *slot = &gMemCardWork_Area.slots[gMemCardWork_bSlotIndex];
    s32 zero = 0;
    s32 status;
    s32 index;
    s32 offset;
    s32 remaining;
    u8 *cursor;
    s32 message;
    s32 checksum;
    u8 *name;

    if (gMemCardWork_wFlags & 0x2000) {
        index = zero;
        if (index < gMemCard_nDirEntries) {
            do {
#ifndef VERSION_EUROPE
                FntPrint(gMemCard_szDirectoryEntryFormat, &gMemCard_pDirEntries[index]);
#endif
                index++;
            } while (index < gMemCard_nDirEntries);
        }
#ifndef VERSION_EUROPE
        FntPrint(gMemCard_szFreeBlockCountFormat, slot->free_blocks);
#endif
    }
    status = -1;
    if (gMemCardWork_wFlags & 0x4000)
        status = MemCard_ProcessRequest(1, &gMemCardWork_nCompletedRequest, &gMemCardWork_nRequestResult);
    if ((gMemCardWork_wFlags & 0x1000) && status != 0 && status == 1) {
        switch (gMemCardWork_nCompletedRequest) {
        case 2:
            gMemCardWork_wFlags |= 0x2000;
            switch (gMemCardWork_nRequestResult) {
            case 3:
                gMemCardWork_wFlags |= 0x800;
            case 0:
                slot->free_blocks = (u8)gMemCard_nFreeBlocks;
                goto success;
            case 4:
                gMemCardWork_wFlags |= 2;
                gMemCardWork_bOperationResult = 1;
                break;
            case 1:
            default:
                gMemCardWork_bOperationResult = 2;
                break;
            }
            break;
        case 3:
            if (gMemCardWork_nRequestResult == 0) {
                index = root->chunk;
                remaining = (u16)root->remaining - index;
                root->remaining = remaining;
                if ((s32)((u32)remaining << 16) > 0) {
                    slot->cursor += index;
                    root->offset += index;
                    if (root->remaining < root->chunk)
                        root->chunk = (u16)root->remaining;
                } else {
                    gMemCardWork_bOperationResult = 0;
                }
            } else {
                gMemCardWork_bOperationResult = 1;
            }
            break;
        case 4:
            if (gMemCardWork_nRequestResult == 0) {
                index = root->chunk;
                remaining = (u16)root->remaining - index;
                root->remaining = remaining;
                if ((s32)((u32)remaining << 16) > 0) {
                    slot->cursor += index;
                    root->offset += index;
                    if (root->remaining < root->chunk)
                        root->chunk = (u16)root->remaining;
                } else {
                    gMemCardWork_bOperationResult = 0;
                }
            } else {
                gMemCardWork_bOperationResult = 1;
            }
            break;
        case 8:
        case 11:
        case 12:
            gMemCardWork_bOperationResult = 1;
            if (gMemCardWork_nRequestResult)
                break;
success:
            gMemCardWork_bOperationResult = 0;
            break;
        }
    }
    if (gDialog_bCallbackState & 0xF) {
        status = 1;
        gDialog_apfnTransitionSteps[gDialog_bCallbackState & 0xF](root, slot);
    }
    if (status >= 0)
        return;
    if (gMemCardWork_wFlags & 0x400) {
#ifdef DIALOG_TRANSITION_EFFECT_CHANNEL_STRIDE
        DuelEffectChannel *text = (DuelEffectChannel *)(
            (u8 *)gText_aChannels +
            root->text_index * DIALOG_TRANSITION_EFFECT_CHANNEL_STRIDE);
#else
        DuelEffectChannel *text = &gText_aChannels[root->text_index];
#endif
        TextBox_UpdateChannels();
        if ((*(u32 *)&text->flags_34 & MEM_CARD_MESSAGE_COMPLETION_MASK) == 0x2000)
            gMemCardWork_wFlags &= ~0x400;
        return;
    }
    switch (gMemCardWork_OperationState & 0xF) {
    case 0:
        if (!(gMemCardWork_OperationState & 0x80)) {
            gMemCardWork_OperationState |= 0x80;
            gDialog_bCallbackState = 1;
            break;
        }
        if (gDialog_bChoice) {
            gDialog_bCallbackState = 2;
            break;
        }
        MemCard_Init(1);
        MemCard_InitIOEvents();
        gMemCardWork_OperationState = 1;
        gMemCardWork_wFlags |= 0x4000;
    case 1:
        if (!(gMemCardWork_OperationState & 0x80)) {
            gMemCardWork_OperationState |= 0x80;
            gMemCardWork_wFlags |= 0x1000;
            MemCard_ReqLoadDirectory(zero);
            break;
        }
        gMemCardWork_OperationState = 4;
    case 4:
        if (!(gMemCardWork_wFlags & 0x2000))
            break;
        gMemCardWork_wFlags &= ~0x1000;
        if (gMemCardWork_bOperationResult == 2) {
            message = 0xD2;
            goto show_message;
        }
        if (!(gMemCardWork_OperationMode & 1))
            gMemCardWork_OperationState = 5;
        else
            gMemCardWork_OperationState = 7;
        break;
    case 5:
        if (!(gMemCardWork_OperationState & 0x80)) {
            gMemCardWork_OperationState |= 0x80;
            index = ((MemCardFindLoadedEntryResult)MemCard_FindLoadedEntry)(gMemCard_szSaveFileName);
            if (index < 0 || (gMemCardWork_wFlags & 2)) {
                message = 0xD3;
                goto show_message;
            }
            MemCard_ReqReadSector(zero, MEM_CARD_WORK_FRAME_ADDRESS,
                gMemCard_pDirEntries[index].head / 64);
            break;
        }
        if (gMemCardWork_bOperationResult) {
            message = 0xD8;
            goto show_message;
        }
        gMemCardWork_OperationState = 6;
    case 6:
        if (!(gMemCardWork_OperationState & 0x80)) {
            gMemCardWork_OperationState |= 0x80;
            name = gMemCard_szSaveFileName;
            index = ((MemCardFindLoadedEntryResult)MemCard_FindLoadedEntry)(name);
            message = 0xD3;
            if (index < 0 || (gMemCardWork_wFlags & 2))
                goto show_message;
            DuelEffect_CreateChannel(0x80D5, 0);
            slot->entry_index = index;
            gMemCardWork_wFlags |= 0x1000;
            MemCard_ReqReadFile(
                zero,
                (s32)name,
                MEM_CARD_WORK_READ_ADDRESS,
                SAVE_DATA_HEADER_SIZE,
                MEM_CARD_BLOCK_SIZE - SAVE_DATA_HEADER_SIZE
            );
            break;
        }
        message = 0xD7;
        if (gMemCardWork_bOperationResult == 1)
            message = 0xD8;
        goto show_message;
    case 7: {
        s32 wait;
        s32 result;
        u8 *file;
        if (!(gMemCardWork_OperationState & 0x80)) {
            gMemCardWork_OperationState |= 0x80;
            if (gMemCardWork_wFlags & 2) {
                gMemCardWork_OperationState = 9;
                break;
            }
            file = gMemCard_szSaveFileName;
            if (((MemCardFindLoadedEntryResult)MemCard_FindLoadedEntry)(file) < 0) {
                gMemCardWork_OperationState = 10;
                break;
            }
            DuelEffect_CreateChannel(0x80D6, 0);
            Util_CopyWords(
                (u8 *)MEM_CARD_WORK_WRITE_ADDRESS,
                gSaveData_aHeaderTemplate,
                SAVE_DATA_HEADER_SIZE
            );
            Util_FillMemory(
                (u8 *)MEM_CARD_WORK_WRITE_BODY_ADDRESS,
                0x86,
                FILE_SECTOR_SIZE
            );
            MemCard_ReqWriteFile(
                zero,
                (s32)file,
                MEM_CARD_WORK_WRITE_ADDRESS,
                0,
                SAVE_DATA_HEADER_SIZE + FILE_SECTOR_SIZE
            );
            gMemCardWork_wFlags |= 0x1000;
            break;
        }
        result = gMemCardWork_bOperationResult;
        if (result == 1) {
            message = 0xD9;
            wait = result;
        } else {
            message = 0xD7;
            wait = 1;
            gMemCardWork_OperationState = 8;
        }
        DuelEffect_CreateChannel(message, wait);
        remaining = gMemCardWork_wFlags | 0x400;
        gMemCardWork_wFlags = remaining;
        remaining = 8;
        gMemCardWork_OperationState = remaining;
    }
    case 8:
        if (!(gMemCardWork_OperationState & 0x80)) {
            s32 i;
            s32 sector;
            gMemCardWork_OperationState |= 0x80;
            index = ((MemCardFindLoadedEntryResult)MemCard_FindLoadedEntry)(gMemCard_szSaveFileName);
            checksum = 0;
            i = 0;
            D_8021007E = 1;
            D_8021007D = 255;
            D_8021007C = 255;
            D_8021007B = 255;
            D_8021007A = 255;
            do {
                checksum ^= MEM_CARD_WORK_FRAME_BYTES[i];
                i++;
            } while (i < 0x7F);
            sector = gMemCard_pDirEntries[index].head;
            gMemCardWork_bFrameChecksum = checksum;
            if (sector < 0)
                sector += 63;
            MemCard_ReqWriteSector(zero, MEM_CARD_WORK_FRAME_ADDRESS, sector >> 6);
            break;
        }
        if (!gMemCardWork_bOperationResult)
            break;
        message = 0xD9;
show_message:
        DuelEffect_CreateChannel(message, 1);
        gMemCardWork_wFlags |= 0x400;
        break;
    case 9:
    case 10:
        if (!(gMemCardWork_OperationState & 0x80)) {
            gMemCardWork_OperationState |= 0x80;
            if (slot->free_blocks >= root->blocks)
                MemCard_ReqCreateFile(zero, (s32)gMemCard_szSaveFileName, root->blocks);
            break;
        }
#ifndef VERSION_EUROPE
        if (!gMemCardWork_bOperationResult)
            printf(gMemCard_szMakeFileTrace);
        printf(gMemCard_szFailFileTrace);
#endif
        break;
    }
}

/* The load half of the memory-card dialog's operations: the message helper,
   the load state machine and the two load step callbacks gMemCardDialog_apfnSteps selects.
   The save state machine and its step callback follow them. */

/* The two stores below are deliberate: retail writes the masked value and
   then the value with the new bits set. Without volatile the first store is
   dead and GCC drops it, so this function reaches the word through a volatile
   lvalue rather than forcing volatile on every reader in mem_card.h. */
void MemCardDialog_SetMessage(s32 value, s32 bits)
{
    u16 flags = *(volatile u16 *)&gMemCard_wDialogFlags;

    bits |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
    gMemCardDialog_MessageCode = value;
    flags &= 0xFF87;
    *(volatile u16 *)&gMemCard_wDialogFlags = flags;
    *(volatile u16 *)&gMemCard_wDialogFlags = flags | bits;
}

/* Memory-card load dialog state machine.

   Levers that mattered here:
   - gMemCardDialog_CardNumberArg must be declared as an unsized array. As a plain s32 extern
     the -G8 build puts it in small data and the store becomes one gp-relative
     word; the target materialises %hi in its own register, which also keeps
     $a0 busy across the message value and is why the message ends up in $a1
     and is copied into $a0 at the call.
   - gDialog_bChoice is read through a .data section attribute so it keeps the
     %hi/%lo pair the target uses instead of a gp-relative byte load.
   - The gMemCard_wDialogFlags |= 0x1000 tail is written once inside case 3
     and entered by goto from case 1, matching the retail block order.
   - Cases 0/1 and 2/3 fall through, and cases 6 and 8 are spelled as separate
     bodies: retail keeps two distinct jump-table targets for them.
*/

void MemCardDialog_UpdateLoad(void)
{
    s32 files;
    s32 message;

    switch (gMemCardDialog_OperationState & 0xF) {
    case 0:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xC8, 0x20);
            break;
        }
        gMemCardDialog_OperationState = 1;
        if (gDialog_bChoice != 0) {
            gMemCardDialog_OperationState = 9;
            break;
        }
        /* fallthrough */
    case 1:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            gMemCardDialog_CardNumberArg[0] = (gMemCardDialog_Port >> 4) + 1;
            message = 0xD4;
            if ((gMemCard_wDialogFlags & 0x200) != 0) {
                message = 0xC0;
            }
            MemCardDialog_SetMessage(message, 0);
            do {
            } while (MemCardAccept(gMemCardDialog_Port) == 0);
            goto io_pending;
        }
        switch (gMemCardDialog_SyncResult) {
        case 0:
        case 3:
            gMemCardDialog_OperationState = 2;
            break;
        case 1:
            gMemCardDialog_OperationState = 5;
            break;
        case 2:
        case 4:
            gMemCardDialog_OperationState = 6;
            break;
        }
        break;
    case 2:
        if (MemCardGetDirentry(gMemCardDialog_Port, (char *)gMemCardDialog_FileNameBuffer,
                               (struct DIRENTRY *)gMemCardDialog_DirectoryBuffer, (long *)&files, 0,
                               MEM_CARD_BLOCK_COUNT) != 0) {
            gMemCardDialog_OperationState = 6;
            break;
        }
        if (files == 0) {
            gMemCardDialog_OperationState = 6;
            break;
        }
        gMemCardDialog_OperationState = 3;
        /* fallthrough */
    case 3:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xD5, 0);
            MemCardReadFile(gMemCardDialog_Port, (char *)gMemCardDialog_FileNameBuffer,
                            (unsigned long *)gMemCard_pPrimaryTransferCursor,
                            gMemCardDialog_TransferOffset,
                            gMemCardDialog_TransferSize);
        io_pending:
            gMemCard_wDialogFlags |= MEM_CARD_DIALOG_FLAG_IO_PENDING;
            break;
        }
        switch (gMemCardDialog_SyncResult) {
        case 0:
            gMemCardDialog_OperationState = 7;
            if (SaveData_ValidateIntegrity(gMemCard_pPrimaryTransferCursor) != 0) {
                break;
            }
            gMemCardDialog_OperationState = 4;
            if ((gMemCard_wDialogFlags & 0x400) != 0) {
                MemCardDialog_SetMessage(0xBD, 0x18);
                break;
            }
            gMemCard_wDialogFlags |= 0x400;
            break;
        case 1:
            gMemCardDialog_OperationState = 5;
            break;
        case 2:
        case 3:
            gMemCardDialog_OperationState = 8;
            break;
        case 4:
            break;
        case 5:
            gMemCardDialog_OperationState = 6;
            break;
        }
        break;
    case 4:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xBD, 0x10);
            break;
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            MemCardDialog_SetMessage(0xBC, 0x20);
            break;
        }
        gMemCardDialog_OperationState = 9;
        if (gDialog_bChoice != 0) {
            break;
        }
        gMemCardDialog_OperationState = 3;
        gMemCardDialog_TransferOffset += SAVE_DATA_STATE_SIZE;
        break;
    case 5:
        MemCardDialog_SetMessage(0xC1, 0x18);
        break;
    case 6:
        MemCardDialog_SetMessage(0xD3, 0x18);
        break;
    case 7:
        gMemCardDialog_Outcome = 1;
        if ((gMemCard_wDialogFlags & 0x200) != 0) {
            MemCardDialog_SetMessage(0xD0, 8);
            break;
        }
        gSaveData_AllowIdentityBypass = 0;
        MemCardDialog_SetMessage(0xD0, 0x18);
        break;
    case 8:
        MemCardDialog_SetMessage(0xD3, 0x18);
        break;
    case 9:
        gMemCardDialog_Outcome = 3;
        MemCardDialog_SetMessage(0xC7, 0x18);
        break;
    }
}

void MemCardDialog_StepLoad(void)
{
    if ((gDialog_bCallbackState & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        gDialog_bCallbackState |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        gMemCardDialog_OperationState = 0;
    }
    MemCardDialog_UpdateLoad();
}

void MemCardDialog_StepLoadUnprompted(void)
{
    if (!(gDialog_bCallbackState & DUEL_EFFECT_STATE_FLAG_INITIALIZED)) {
        gDialog_bCallbackState |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        gMemCardDialog_OperationState = 1;
        gMemCard_wDialogFlags |= 0x200;
    }
    MemCardDialog_UpdateLoad();
}

#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
/* The Japanese save dialog and European derivative. Against the US one below: with
   gMemCard_wDialogFlags & 0x100 set, six states show a different message and
   state 1 goes to state 2 instead of reading the directory; state 2 polls
   three calls the US build does not have, retrying three times; state 4's
   choice is the other way round; state 5 formats without accepting the card
   again; and state 7 skips the read-back when the flag is set. Europe keeps
   the message, state 2 and read-back differences, but restores the port
   marker, state 4 choice direction and state 5 flow used by the US build. */
void MemCardDialog_UpdateSave(void)
{
    long cmds;
    long result;
    long apl;
    s32 files;
    s32 needed;
    s32 polled;
    s32 mode;
    s32 status;
    s32 message;

    switch (gMemCardDialog_OperationState & 0xF) {
    case 0:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            message = 0xC9;
            if (gMemCard_wDialogFlags & 0x100) {
                message = 0xCA;
            }
            MemCardDialog_SetMessage(message, 0x20);
            break;
        }
        gMemCardDialog_OperationState = 1;
        if (gDialog_bChoice != 0) {
            gMemCardDialog_OperationState = 0xC;
            break;
        }
        /* fallthrough */
    case 1:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
#ifdef VERSION_EUROPE
            gMemCardDialog_CardNumberArg[0] = (gMemCardDialog_Port >> 4) + 1;
#endif
            message = 0xD4;
            if (gMemCard_wDialogFlags & 0x100) {
                message = 0xBA;
            }
            MemCardDialog_SetMessage(message, 0);
            do {
            } while (MemCardAccept(0) == 0);
            goto io_pending;
        }
        switch (gMemCardDialog_SyncResult) {
        case 0:
        case 3:
            gMemCardDialog_OperationState = 3;
            if (gMemCard_wDialogFlags & 0x100) {
                gMemCardDialog_OperationState = 2;
            }
            break;
        case 4:
            gMemCardDialog_OperationState = 4;
            if (gMemCard_wDialogFlags & 0x100) {
                break;
            }
            if (gSaveData_AllowIdentityBypass != 0) {
                break;
            }
            gMemCardDialog_OperationState = 0xE;
            break;
        case 1:
            gMemCardDialog_OperationState = 9;
            break;
        case 2:
            gMemCardDialog_OperationState = 0xD;
            break;
        }
        break;
    case 2:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            gMemCardDialog_RetryCounter = 3;
            McxCardType(0);
            break;
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) != 0) {
            MemCardDialog_SetMessage(0xB8, 0x18);
            break;
        }
        polled = McxSync(1, &cmds, &result);
        if (polled != 1) {
            break;
        }
        if (result == 0) {
            McxGetApl(0, &apl);
            if (McxSync(0, &cmds, &result) != polled) {
                MemCardDialog_SetMessage(0xBB, 0x18);
                break;
            }
            if (apl != 0) {
                gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
                MemCardDialog_SetMessage(0xB9, 0x10);
                break;
            }
            gMemCardDialog_OperationState = 3;
            break;
        }
        if (--gMemCardDialog_RetryCounter != 0) {
            McxCardType(0);
            break;
        }
        MemCardDialog_SetMessage(0xBB, 0x18);
        break;
    case 3:
        if (MemCardGetDirentry(0, (char *)gMemCardDialog_szAllFilesPattern,
                               (struct DIRENTRY *)gMemCardDialog_DirectoryBuffer, (long *)&files, 0,
                               MEM_CARD_BLOCK_COUNT) != 0) {
            gMemCardDialog_OperationState = 0xD;
            break;
        }
        if (MemCard_FindEntry(gMemCardDialog_FileNameBuffer, (struct DIRENTRY *)gMemCardDialog_DirectoryBuffer, files) >= 0) {
            gMemCardDialog_OperationState = 7;
            break;
        }
        if ((gMemCard_wDialogFlags & 0x100) == 0 && gSaveData_AllowIdentityBypass == 0) {
            gMemCardDialog_OperationState = 0xE;
            break;
        }
        message = MemCard_CalcFreeBlocks((struct DIRENTRY *)gMemCardDialog_DirectoryBuffer, files);
        needed = gMemCardDialog_BlockCount;
        if (message >= needed) {
            gMemCardDialog_OperationState = 6;
            goto create;
        }
        gText_StagedValues[0].blocks.used = MEM_CARD_BLOCK_COUNT - message;
        gText_StagedValues[0].blocks.needed = needed;
        MemCardDialog_SetMessage(0xDB, 0x18);
        break;
    case 4:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xDE, 0x10);
            break;
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            MemCardDialog_SetMessage(0xDF, 0x20);
            break;
        }
#ifdef VERSION_EUROPE
        if (gDialog_bChoice == 0) {
#else
        if (gDialog_bChoice != 0) {
#endif
            gMemCardDialog_OperationState = 0xC;
            break;
        }
        gMemCardDialog_OperationState = 5;
        MemCardDialog_SetMessage(0xBE, 0);
        break;
    case 5:
#ifdef VERSION_EUROPE
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            do {
            } while (MemCardAccept(0) == 0);
            goto io_pending;
        }
        if (gMemCardDialog_SyncCommand != 2) {
            break;
        }
        switch (gMemCardDialog_SyncResult) {
        case 0:
            MemCardDialog_SetMessage(0xC3, 0x18);
            break;
        case 3:
            MemCardDialog_SetMessage(0xC3, 0x18);
            break;
        case 2:
            gMemCardDialog_OperationState = 0xD;
            break;
        case 4:
            if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
                gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
                if (MemCardFormat(0) != 0) {
                    MemCardDialog_SetMessage(0xDD, 0x18);
                    break;
                }
                MemCardDialog_SetMessage(0xBF, 0x10);
                break;
            }
            gMemCardDialog_OperationState = 1;
            break;
        case 1:
            gMemCardDialog_OperationState = 9;
            break;
        }
#else
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            break;
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            if (MemCardFormat(0) != 0) {
                MemCardDialog_SetMessage(0xDD, 0x18);
                break;
            }
            MemCardDialog_SetMessage(0xBF, 0x10);
            break;
        }
        gMemCardDialog_OperationState = 1;
#endif
        break;
    case 6:
    create:
        if (MemCardCreateFile(0, (char *)gMemCardDialog_FileNameBuffer, gMemCardDialog_BlockCount) != 0) {
            gMemCardDialog_OperationState = 0xB;
            break;
        }
        if ((gMemCard_wDialogFlags & 0x100) == 0) {
            gMemCardDialog_TransferOffset = 0;
            gMemCard_pPrimaryTransferCursor -= SAVE_DATA_HEADER_SIZE;
            gMemCardDialog_TransferSize += SAVE_DATA_HEADER_SIZE;
        }
        gMemCardDialog_OperationState = 8;
        goto write;
    case 7:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            if (gMemCard_wDialogFlags & 0x100) {
                gMemCardDialog_OperationState = 8;
                break;
            }
            gMemCardDialog_RetryCounter = 0;
            MemCardReadFile(0, (char *)gMemCardDialog_FileNameBuffer,
                            (unsigned long *)gLibrary_aCardArtRecord,
                            gMemCardDialog_TransferOffset, 0x480);
            goto io_pending;
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            mode = gMemCardDialog_SyncResult;
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            if (mode != 0) {
                gMemCardDialog_OperationState = 0xD;
                if (mode == 1) {
                    gMemCardDialog_OperationState = 9;
                }
            } else {
                if (gSaveData_AllowIdentityBypass != 0) {
                    goto message_cf;
                }
                if (SaveData_MatchesDuelistAndCurrentSequence(
                        (SaveDataState *)gMemCard_pPrimaryTransferCursor,
                        (SaveDataState *)gLibrary_aCardArtRecord) != 0) {
                    goto message_cf;
                }
                if (gMemCardDialog_RetryCounter != 0) {
                    goto state_e;
                }
                gMemCardDialog_RetryCounter++;
                gMemCardDialog_OperationState &= ~MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
                MemCardReadFile(0, (char *)gMemCardDialog_FileNameBuffer,
                                (unsigned long *)gLibrary_aCardArtRecord,
                                gMemCardDialog_TransferOffset + SAVE_DATA_STATE_SIZE, 0x480);
                goto io_pending;
            }
            break;
        state_e:
            gMemCardDialog_OperationState = 0xE;
            break;
        message_cf:
            MemCardDialog_SetMessage(0xCF, 0x20);
            break;
        }
        if (gDialog_bChoice == 0) {
            gMemCardDialog_OperationState = 8;
            goto write;
        }
        gMemCardDialog_OperationState = 0xC;
        break;
    case 8:
    write:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            message = 0xD6;
            if (gMemCard_wDialogFlags & 0x100) {
                message = 0xD7;
            }
            MemCardDialog_SetMessage(message, 0);
            MemCardWriteFile(0, (char *)gMemCardDialog_FileNameBuffer,
                             (unsigned long *)gMemCard_pPrimaryTransferCursor,
                             gMemCardDialog_TransferOffset,
                             gMemCardDialog_TransferSize);
        io_pending:
            gMemCard_wDialogFlags |= MEM_CARD_DIALOG_FLAG_IO_PENDING;
            break;
        }
        status = gMemCardDialog_SyncResult;
        if (status != 0) {
            gMemCardDialog_OperationState = 0xB;
            if (status == 1) {
                gMemCardDialog_OperationState = 9;
            }
            break;
        }
        gMemCardDialog_OperationState = 0xA;
        break;
    case 9:
        message = 0xD2;
        if (gMemCard_wDialogFlags & 0x100) {
            message = 0xBB;
        }
        MemCardDialog_SetMessage(message, 0x18);
        break;
    case 10:
        gMemCardDialog_Outcome = 1;
        message = 0xCC;
        if ((gMemCard_wDialogFlags & 0x100) == 0) {
            message = 0xD1;
            gSaveData_AllowIdentityBypass = 0;
            gSaveDataSequence = (s32)((u32)gSaveDataSequence + 1);
        }
        MemCardDialog_SetMessage(message, 0x18);
        break;
    case 11:
        message = 0xD9;
        if (gMemCard_wDialogFlags & 0x100) {
            message = 0xDC;
        }
        MemCardDialog_SetMessage(message, 0x18);
        break;
    case 12:
        message = 0xCD;
        if (gMemCard_wDialogFlags & 0x100) {
            message = 0xCB;
        }
        MemCardDialog_SetMessage(message, 0x18);
        gMemCardDialog_Outcome = 3;
        break;
    case 13:
        MemCardDialog_SetMessage(0xDA, 0x18);
        break;
    case 14:
        MemCardDialog_SetMessage(0xCE, 0x18);
        break;
    }
}
#else
/* Memory-card save dialog state machine, the third of the family beside
   MemCardDialog_UpdateLoad and MemCardDialog_UpdateTradeSave.

   It walks the card from "no save present" to "save written": accept the
   card, read the directory, look the file up, compare free blocks against
   the size the save needs, offer and run a format, create the file, read
   back and check the existing state, and finally write it. The low nibble of
   gMemCardDialog_OperationState is the state and the 0x80/0x40 bits latch the first pass
   through each one; gMemCardDialog_SyncResult carries the previous call's result and
   gMemCardDialog_OperationState is set to the message state on every failure.

   Levers that mattered here:
   - The three jump tables follow the load table in this unit's contiguous
     rodata run starting at 0xC10.
   - The two inner switches are written in the target's block-layout order,
     not numeric order, because gcc emits switch arms in source order. The
     arms that merge into blocks elsewhere in the function (state 9 and state
     0xD) go last.
   - State 7 and state 8 each need their OWN local for gMemCardDialog_SyncResult. Sharing
     one gave the two "0xD/9" and "0xB/9" tails the same register, and
     cross-jumping then merged the compare as well; retail merges only the
     final store because its two tails are in $a0 and $v1.
   - The three early exits inside state 7 are gotos to tails written at the
     end of that arm. As inline `{ ...; break; }` bodies gcc leaves the calls
     in line and the shared blocks retail keeps between the two halves of the
     0x40 test never form.
   - One local carries both the free-block count and the state-10 message
     id. Its longer life keeps it out of $a0, so retail's $a2 copy into $a0 at
     the shared MemCardDialog_SetMessage call survives; MemCardDialog_SetMessage
     takes full words, so the count's unknown upper bits need no mask there. */
void MemCardDialog_UpdateSave(void)
{
    s32 files;
    s32 needed;
    s32 result;
    s32 mode;
    s32 message;

    switch (gMemCardDialog_OperationState & 0xF) {
    case 0:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            gMemCardDialog_CardNumberArg[0] = (gMemCardDialog_Port >> 4) + 1;
            MemCardDialog_SetMessage(0xC9, 0x20);
            break;
        }
        gMemCardDialog_OperationState = 1;
        if (gDialog_bChoice != 0) {
            gMemCardDialog_OperationState = 0xC;
            break;
        }
        /* fallthrough */
    case 1:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xD4, 0);
            do {
            } while (MemCardAccept(0) == 0);
            goto io_pending;
        }
        switch (gMemCardDialog_SyncResult) {
        case 0:
        case 3:
            gMemCardDialog_OperationState = 3;
            break;
        case 4:
            gMemCardDialog_OperationState = 4;
            if (gMemCard_wDialogFlags & 0x100) {
                break;
            }
            if (gSaveData_AllowIdentityBypass != 0) {
                break;
            }
            gMemCardDialog_OperationState = 0xE;
            break;
        case 1:
            gMemCardDialog_OperationState = 9;
            break;
        case 2:
            gMemCardDialog_OperationState = 0xD;
            break;
        }
        break;
    case 2:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            gMemCardDialog_RetryCounter = 3;
            break;
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            break;
        }
        MemCardDialog_SetMessage(0xB8, 0x18);
        break;
    case 3:
        if (MemCardGetDirentry(0, (char *)gMemCardDialog_szAllFilesPattern,
                               (struct DIRENTRY *)gMemCardDialog_DirectoryBuffer, (long *)&files, 0,
                               MEM_CARD_BLOCK_COUNT) != 0) {
            gMemCardDialog_OperationState = 0xD;
            break;
        }
        if (MemCard_FindEntry(gMemCardDialog_FileNameBuffer, (struct DIRENTRY *)gMemCardDialog_DirectoryBuffer, files) >= 0) {
            gMemCardDialog_OperationState = 7;
            break;
        }
        if ((gMemCard_wDialogFlags & 0x100) == 0 && gSaveData_AllowIdentityBypass == 0) {
            gMemCardDialog_OperationState = 0xE;
            break;
        }
        message = MemCard_CalcFreeBlocks((struct DIRENTRY *)gMemCardDialog_DirectoryBuffer, files);
        needed = gMemCardDialog_BlockCount;
        if (message >= needed) {
            gMemCardDialog_OperationState = 6;
            goto create;
        }
        gText_StagedValues[0].blocks.used = MEM_CARD_BLOCK_COUNT - message;
        gText_StagedValues[0].blocks.needed = needed;
        MemCardDialog_SetMessage(0xDB, 0x18);
        break;
    case 4:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xDE, 0x10);
            break;
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            MemCardDialog_SetMessage(0xDF, 0x20);
            break;
        }
        if (gDialog_bChoice == 0) {
            gMemCardDialog_OperationState = 0xC;
            break;
        }
        gMemCardDialog_OperationState = 5;
        MemCardDialog_SetMessage(0xBE, 0);
        break;
    case 5:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            do {
            } while (MemCardAccept(0) == 0);
            goto io_pending;
        }
        if (gMemCardDialog_SyncCommand != 2) {
            break;
        }
        switch (gMemCardDialog_SyncResult) {
        case 0:
            MemCardDialog_SetMessage(0xC3, 0x18);
            break;
        case 3:
            MemCardDialog_SetMessage(0xC3, 0x18);
            break;
        case 2:
            gMemCardDialog_OperationState = 0xD;
            break;
        case 4:
            if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
                gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
                if (MemCardFormat(0) != 0) {
                    MemCardDialog_SetMessage(0xDD, 0x18);
                    break;
                }
                MemCardDialog_SetMessage(0xBF, 0x10);
                break;
            }
            gMemCardDialog_OperationState = 1;
            break;
        case 1:
            gMemCardDialog_OperationState = 9;
            break;
        }
        break;
    case 6:
    create:
        if (MemCardCreateFile(0, (char *)gMemCardDialog_FileNameBuffer, gMemCardDialog_BlockCount) != 0) {
            gMemCardDialog_OperationState = 0xB;
            break;
        }
        if ((gMemCard_wDialogFlags & 0x100) == 0) {
            gMemCardDialog_TransferOffset = 0;
            gMemCard_pPrimaryTransferCursor -= SAVE_DATA_HEADER_SIZE;
            gMemCardDialog_TransferSize += SAVE_DATA_HEADER_SIZE;
        }
        gMemCardDialog_OperationState = 8;
        goto write;
    case 7:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            gMemCardDialog_RetryCounter = 0;
            MemCardReadFile(0, (char *)gMemCardDialog_FileNameBuffer,
                            (unsigned long *)gLibrary_aCardArtRecord,
                            gMemCardDialog_TransferOffset, 0x480);
            goto io_pending;
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            mode = gMemCardDialog_SyncResult;
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            if (mode != 0) {
                gMemCardDialog_OperationState = 0xD;
                if (mode == 1) {
                    gMemCardDialog_OperationState = 9;
                }
            } else {
                if (gSaveData_AllowIdentityBypass != 0) {
                    goto message_cf;
                }
                if (SaveData_MatchesDuelistAndCurrentSequence(
                        (SaveDataState *)gMemCard_pPrimaryTransferCursor,
                        (SaveDataState *)gLibrary_aCardArtRecord) != 0) {
                    goto message_cf;
                }
                if (gMemCardDialog_RetryCounter != 0) {
                    goto state_e;
                }
                gMemCardDialog_RetryCounter++;
                gMemCardDialog_OperationState &= ~MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
                MemCardReadFile(0, (char *)gMemCardDialog_FileNameBuffer,
                                (unsigned long *)gLibrary_aCardArtRecord,
                                gMemCardDialog_TransferOffset + SAVE_DATA_STATE_SIZE, 0x480);
                goto io_pending;
            }
            break;
        state_e:
            gMemCardDialog_OperationState = 0xE;
            break;
        message_cf:
            MemCardDialog_SetMessage(0xCF, 0x20);
            break;
        }
        if (gDialog_bChoice == 0) {
            gMemCardDialog_OperationState = 8;
            goto write;
        }
        gMemCardDialog_OperationState = 0xC;
        break;
    case 8:
    write:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            MemCardDialog_SetMessage(0xD6, 0);
            MemCardWriteFile(0, (char *)gMemCardDialog_FileNameBuffer,
                             (unsigned long *)gMemCard_pPrimaryTransferCursor,
                             gMemCardDialog_TransferOffset,
                             gMemCardDialog_TransferSize);
        io_pending:
            gMemCard_wDialogFlags |= MEM_CARD_DIALOG_FLAG_IO_PENDING;
            break;
        }
        result = gMemCardDialog_SyncResult;
        if (result != 0) {
            gMemCardDialog_OperationState = 0xB;
            if (result == 1) {
                gMemCardDialog_OperationState = 9;
            }
            break;
        }
        gMemCardDialog_OperationState = 0xA;
        break;
    case 9:
        MemCardDialog_SetMessage(0xD2, 0x18);
        break;
    case 10:
        gMemCardDialog_Outcome = 1;
        message = 0xCC;
        if ((gMemCard_wDialogFlags & 0x100) == 0) {
            message = 0xD1;
            gSaveData_AllowIdentityBypass = 0;
            gSaveDataSequence = (s32)((u32)gSaveDataSequence + 1);
        }
        MemCardDialog_SetMessage(message, 0x18);
        break;
    case 11:
        MemCardDialog_SetMessage(0xD9, 0x18);
        break;
    case 12:
        MemCardDialog_SetMessage(0xCD, 0x18);
        gMemCardDialog_Outcome = 3;
        break;
    case 13:
        MemCardDialog_SetMessage(0xDA, 0x18);
        break;
    case 14:
        MemCardDialog_SetMessage(0xCE, 0x18);
        break;
    }
}
#endif

void MemCardDialog_StepSave(void)
{
    if ((gDialog_bCallbackState & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        gDialog_bCallbackState |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        gMemCardDialog_OperationState = 0;
    }
    MemCardDialog_UpdateSave();
}

#ifndef MEM_CARD_DIALOG_EFFECT_CHANNEL_STRIDE
#define MEM_CARD_DIALOG_EFFECT_CHANNEL_STRIDE sizeof(DuelEffectChannel)
#endif

#ifndef MEM_CARD_DIALOG_TRADE_SAVE_INITIAL_MESSAGE
#define MEM_CARD_DIALOG_TRADE_SAVE_INITIAL_MESSAGE 0xC0
#endif

/* The empty callback, trade write-back operation, and modal runtime that
   drive the operation table. MemCardDialog_Update dispatches the save and
   trade operations through gMemCardDialog_apfnSteps; all paths share the dialog flags,
   result words, active channel, and request outcome. */

#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
void MemCardDialog_StepNone(void)
{
}
#else
/* The Japanese and European builds have this step where the US build has
   MemCardDialog_StepNone: MemCardDialog_StepSave's one-time setup, plus
   clearing the transfer offset and raising dialog flag 0x100, before the
   save update runs. */
void MemCardDialog_StepAlternateSave(void)
{
    if ((gDialog_bCallbackState & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        gDialog_bCallbackState |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        gMemCardDialog_TransferOffset = 0;
        gMemCardDialog_OperationState = 0;
        gMemCard_wDialogFlags |= 0x100;
    }
    MemCardDialog_UpdateSave();
}
#endif

void MemCardDialog_UpdateTradeSave(void)
{
    s32 files;
    u8 *record;

    if ((gDialog_bCallbackState & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        gDialog_bCallbackState |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        MemCardDialog_SetMessage(MEM_CARD_DIALOG_TRADE_SAVE_INITIAL_MESSAGE, 0);
        gMemCardDialog_OperationState = 0;
    }
    switch (gMemCardDialog_OperationState & 0xF) {
    case 0:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            gMemCardDialog_CardNumberArg[0] = (gMemCardDialog_Port >> 4) + 1;
            do {
            } while (MemCardAccept(gMemCardDialog_Port) == 0);
            goto io_pending;
        }
        switch (gMemCardDialog_SyncResult) {
        case 1:
            MemCardDialog_SetMessage(0xC1, 0x18);
            break;
        case 0:
        case 3:
            gMemCardDialog_OperationState = 1;
            gMemCardDialog_Port ^= 0x10;
            if (gMemCardDialog_Port != 0) {
                gMemCardDialog_OperationState = 0;
            }
            break;
        case 2:
            MemCardDialog_SetMessage(0xDA, 0x18);
            break;
        case 4:
            MemCardDialog_SetMessage(0xC3, 0x18);
            break;
        }
        break;
    case 1:
        gMemCardDialog_CardNumberArg[0] = (gMemCardDialog_Port >> 4) + 1;
        if (MemCardGetDirentry(gMemCardDialog_Port, (char *)gMemCardDialog_FileNameBuffer,
                               (struct DIRENTRY *)gMemCardDialog_DirectoryBuffer, (long *)&files, 0,
                               MEM_CARD_BLOCK_COUNT) != 0) {
            MemCardDialog_SetMessage(0xDA, 0x18);
            break;
        }
        if (files == 0) {
            MemCardDialog_SetMessage(0xC3, 0x18);
            break;
        }
        gMemCardDialog_Port ^= 0x10;
        if (gMemCardDialog_Port != 0) {
            gMemCardDialog_OperationState = 1;
            break;
        }
        gMemCardDialog_OperationState = 2;
        /* fallthrough */
    case 2:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            gMemCardDialog_RetryCounter = 0;
            gMemCardDialog_CardNumberArg[0] = (gMemCardDialog_Port >> 4) + 1;
            MemCardReadFile(gMemCardDialog_Port, (char *)gMemCardDialog_FileNameBuffer,
                            (unsigned long *)gLibrary_aCardArtRecord,
                            gMemCardDialog_TransferOffset, 0x480);
            goto io_pending;
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            if (gMemCardDialog_SyncResult != 0) {
                MemCardDialog_SetMessage(0xDA, 0x18);
                break;
            }
        }
        record = gMemCard_pPrimaryTransferCursor;
        if (gMemCardDialog_Port != 0) {
            record = gMemCard_pSecondaryTransferCursor;
        }
        if (SaveData_HasSameDuelistCode(
                (SaveDataState *)record,
                (SaveDataState *)gLibrary_aCardArtRecord) == 0) {
            if (gMemCardDialog_RetryCounter != 0) {
                MemCardDialog_SetMessage(0xC3, 0x18);
                break;
            }
            gMemCardDialog_RetryCounter++;
            gMemCardDialog_OperationState &= ~MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            MemCardReadFile(gMemCardDialog_Port, (char *)gMemCardDialog_FileNameBuffer,
                            (unsigned long *)gLibrary_aCardArtRecord,
                            gMemCardDialog_TransferOffset + SAVE_DATA_STATE_SIZE, 0x480);
            goto io_pending;
        }
        gMemCardDialog_Port ^= 0x10;
        if (gMemCardDialog_Port != 0) {
            gMemCardDialog_OperationState = 2;
            break;
        }
        gMemCardDialog_OperationState = 3;
        /* fallthrough */
    case 3:
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_READY
                          | MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            MemCardDialog_SetMessage(0xC4, 0);
        }
        if ((gMemCardDialog_OperationState & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            goto step;
        }
    write:
        gMemCardDialog_OperationState &= ~MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
        if (gMemCardDialog_Port != 0) {
            MemCardWriteFile(gMemCardDialog_Port, (char *)gMemCardDialog_FileNameBuffer,
                             (unsigned long *)gMemCard_pSecondaryTransferCursor,
                             gMemCardDialog_TransferOffset, 0x80);
        } else {
            MemCardWriteFile(gMemCardDialog_Port, (char *)gMemCardDialog_FileNameBuffer,
                             (unsigned long *)gMemCard_pPrimaryTransferCursor,
                             gMemCardDialog_TransferOffset, 0x80);
        }
    io_pending:
        gMemCard_wDialogFlags |= MEM_CARD_DIALOG_FLAG_IO_PENDING;
        break;
    step:
        if (gMemCardDialog_SyncResult != 0) {
            MemCardDialog_SetMessage(0xC5, 0x18);
            break;
        }
        gMemCardDialog_OperationState |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
        gMemCardDialog_Port ^= 0x10;
        if (gMemCardDialog_Port == 0) {
            gMemCard_pPrimaryTransferCursor += 0x80;
            gMemCard_pSecondaryTransferCursor += 0x80;
            gMemCardDialog_TransferOffset += 0x80;
            gMemCardDialog_TransferSize -= 0x80;
            if (gMemCardDialog_TransferSize == 0) {
                gMemCardDialog_Outcome = 1;
                MemCardDialog_SetMessage(0xC6, 0x18);
                break;
            }
        }
        goto write;
    }
}

s32 MemCardDialog_StepSlide(DisplayObject *object, s32 arg1, s32 arg2,
                            s32 index)
{
    s32 saved_index = index;
    s32 value;

    if (DisplayObject_MarkInitialized((DisplayObjectLifecycle *)object) == 0) {
        DisplayObject_SavePosition((DisplayObjectSnapshot *)object);
    }

    value = object->field_60;
    if (value < 0) {
        value += 0x40;
        if (value >= 0) {
            object->field_6C = 0;
            value = 0;
        }
    } else {
        value -= 0x40;
        if (value <= 0) {
            object->field_6C = 0;
            value = 0;
        }
    }
    object->field_60 = value;

    Widget_SlideSine((DisplayObjectPosition *)object, arg1, arg2, value);

    if (saved_index >= 0) {
        TextBox_SetPos(
#ifdef VERSION_JAPAN
            (DuelEffectChannel *)((u8 *)gText_aChannels +
                                  saved_index * MEM_CARD_DIALOG_EFFECT_CHANNEL_STRIDE),
#else
            &gText_aChannels[saved_index],
#endif
            (s16)object->xy.h.x,
            (s16)object->xy.h.y);
    }

    return object->field_6C;
}

void MemCardDialog_CreateObject(void)
{
    s32 i = 0;
    DuelEffectChannel *p = gText_aChannels;
    DisplayObject *o;

    gMemCardDialog_ChannelIndex = 0;

    for (; i < DUEL_EFFECT_CHANNEL_COUNT; i++) {
        if (!(p->flags_34 & DUEL_EFFECT_CHANNEL_FLAG_ACTIVE)) {
            gMemCardDialog_ChannelIndex = i;
            break;
        }
        p = (DuelEffectChannel *)(
            (u8 *)p + MEM_CARD_DIALOG_EFFECT_CHANNEL_STRIDE
        );
    }

    o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(o, DIALOG_BOX_X, -0x40, 3, 2, 1, 0xB, 0x20C);
    o->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    DisplayObject_SelectOrderingTable1(o);
    DisplayObject_SetDepthOffset(o, 0xF);
    gMemCard_pDialogObject = o;
}

void MemCardDialog_Update(void)
{
    DuelEffectChannel *p;
    s32 f;
    s32 t;
    s32 u;
    s32 c;

    f = gMemCard_wDialogFlags;
    if ((f & MEM_CARD_DIALOG_FLAG_CLOSING) != 0) {
        if (gMemCard_pDialogObject == (DisplayObject *)0) {
            gMemCard_wDialogFlags = 0;
            return;
        }
        if (MemCardDialog_StepSlide(
                gMemCard_pDialogObject, DIALOG_BOX_X, 0x100, gMemCardDialog_ChannelIndex
            ) == 0) {
#ifdef VERSION_JAPAN
            TextBox_Destroy((DuelEffectChannel *)(
                (u8 *)gText_aChannels +
                gMemCardDialog_ChannelIndex * MEM_CARD_DIALOG_EFFECT_CHANNEL_STRIDE));
#else
            TextBox_Destroy(&gText_aChannels[gMemCardDialog_ChannelIndex]);
#endif
            DisplayObject_ReleaseIfPresent(gMemCard_pDialogObject);
            gMemCard_pDialogObject = (DisplayObject *)0;
        }
        return;
    }
    if ((f & (MEM_CARD_DIALOG_FLAG_OPENED |
              MEM_CARD_DIALOG_FLAG_RESULT_READY)) ==
        (MEM_CARD_DIALOG_FLAG_OPENED | MEM_CARD_DIALOG_FLAG_RESULT_READY)) {
        if ((f & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            gMemCard_wDialogFlags =
                f | MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            p = TextBox_Create(
                gMemCardDialog_ChannelIndex, gMemCardDialog_MessageCode, DIALOG_BOX_X, 0x50, DIALOG_BOX_WIDTH, 0x30
            );
            DuelEffect_MarkObjectIfActive(p);
            p->depth_offset = 0x10;
            if ((gMemCard_wDialogFlags & 0x20) != 0) {
                do {
                    TextBox_UpdateChannels();
                } while (p->prompt_object == 0);
                return;
            }
            if ((gMemCard_wDialogFlags & 0x10) != 0) {
#ifdef VERSION_EUROPE
                p->flags_34 = p->flags_34 | 0x1010;
#else
                p->flags_34 = p->flags_34 | 0x1008;
#endif
                return;
            }
            TextBox_BuildBlocking(p);
            goto b14;
        }
        TextBox_UpdateChannels();
#ifdef VERSION_JAPAN
        p = (DuelEffectChannel *)(
            (u8 *)gText_aChannels +
            gMemCardDialog_ChannelIndex * MEM_CARD_DIALOG_EFFECT_CHANNEL_STRIDE);
#else
        p = &gText_aChannels[gMemCardDialog_ChannelIndex];
#endif
#ifdef VERSION_EUROPE
        if ((*(s32 *)&p->flags_34 & 0x2010) !=
#else
        if ((*(s32 *)&p->flags_34 & TEXT_BOX_COMPLETION_MASK) !=
#endif
            TEXT_BOX_FLAG_DONE) {
            return;
        }
    b14:
        t = gMemCard_wDialogFlags;
        u = t & 8;
        gMemCard_wDialogFlags =
            t & ~MEM_CARD_DIALOG_FLAG_RESULT_READY;
        if (u == 0) {
            return;
        }
        gMemCard_wDialogFlags = 0;
        goto b25;
    }
    if ((f & MEM_CARD_DIALOG_FLAG_OPENED) == 0) {
        if ((f & MEM_CARD_DIALOG_FLAG_STARTED) == 0) {
            gMemCard_wDialogFlags = f | MEM_CARD_DIALOG_FLAG_STARTED;
            MemCardStart();
#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
            McxStartCom();
#endif
            gMemCardDialog_Outcome = 2;
            MemCardDialog_CreateObject();
            gMemCard_pDialogObject->field_60 = -0x400;
            return;
        }
        if (MemCardDialog_StepSlide(
                gMemCard_pDialogObject, DIALOG_BOX_X, 0x50, -1
            ) == 0) {
            gMemCard_wDialogFlags =
                gMemCard_wDialogFlags | MEM_CARD_DIALOG_FLAG_OPENED;
        }
        return;
    }
    if ((f & MEM_CARD_DIALOG_FLAG_IO_PENDING) != 0) {
        c = MemCardSync(
            1, (long *)&gMemCardDialog_SyncCommand, (long *)&gMemCardDialog_SyncResult
        );
        gMemCardDialog_SyncStatus = c;
        if (c != 1) {
            return;
        }
        gMemCard_wDialogFlags =
            gMemCard_wDialogFlags & ~MEM_CARD_DIALOG_FLAG_IO_PENDING;
    }
    gMemCardDialog_apfnSteps[gMemCardDialog_StepIndex]();
    if (gMemCard_wDialogFlags != 0) {
        return;
    }
b25:
    gMemCard_wDialogFlags =
        gMemCard_wDialogFlags | MEM_CARD_DIALOG_FLAG_CLOSING;
    gMemCard_pDialogObject->field_60 = 0x400;
#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
    McxStopCom();
#endif
    MemCardStop();
}

int MemCardDialog_Poll(void)
{
    MemCardDialog_Update();
    if (gMemCard_wDialogFlags != 0) {
        return 0;
    }
    return gMemCardDialog_Outcome;
}

void MemCardDialog_Start(s32 step)
{
    gMemCard_wDialogFlags = MEM_CARD_DIALOG_FLAG_ACTIVE;
    gMemCardDialog_StepIndex = step;
    gDialog_bCallbackState = 0;
}

void MemCardDialog_Request(u8 *buf, s32 size, u8 *name, s32 step)
{
    strcpy(gMemCardDialog_FileNameBuffer, name);
    gMemCardDialog_Port = 0;
    gMemCardDialog_TransferSize = size;
    gMemCardDialog_TransferOffset = 0x200;
    gMemCardDialog_BlockCount =
        (size + MEM_CARD_BLOCK_SIZE - 1) / MEM_CARD_BLOCK_SIZE;
    gMemCard_pPrimaryTransferCursor = buf;
    MemCardDialog_Start(step);
}

/* Regional values: the load box's flags (0x1008 -> 0x1010) and the channel
 * flag bit it waits on (0x8 -> 0x10). The European build
 * (src/game/european/) defines its own. */
#ifndef SAVE_DATA_LOAD_BOX_FLAGS
#define SAVE_DATA_LOAD_BOX_FLAGS 0x1008
#endif
#ifndef SAVE_DATA_LOAD_CHANNEL_FLAG
#define SAVE_DATA_LOAD_CHANNEL_FLAG 8
#endif

/* The complete single-player and two-player save-transfer runtime. The first
   three functions request, poll, apply, build, and write the resident save;
   the remaining state machine loads both card slots, then the two wrappers add
   trade or duel setup, and the final request writes the paired trade result
   back. */

#ifndef SAVE_DATA_LOAD_STATUS_ADDRESS
#define SAVE_DATA_LOAD_STATUS_ADDRESS 0x8009B0D1
#endif

#ifndef SAVE_DATA_LOAD_PAIR_CANCEL_BUTTON
#define SAVE_DATA_LOAD_PAIR_CANCEL_BUTTON PAD_BUTTON_CANCEL
#endif

#ifndef SAVE_DATA_LOAD_PAIR_CONFIRM_BUTTON
#define SAVE_DATA_LOAD_PAIR_CONFIRM_BUTTON PAD_BUTTON_CROSS
#endif

void SaveData_RequestLoad(void)
{
    /* The symbolic store changes the target $at scheduling and relocation. */
    *(u8 *)SAVE_DATA_LOAD_STATUS_ADDRESS = 0;
    MemCardDialog_Request(
        gSaveData_aTransferBuffer,
        SAVE_DATA_STATE_SIZE,
        gMemCard_szSaveFileName,
        0
    );
}

s32 SaveData_PollLoad(void) {
    s32 r = MemCardDialog_Poll();
    if (r != 0) {
        if (r == 1) {
            u8 *p = (u8 *)gDuel_awPlayerDeck;
            Util_CopyWords(p, gSaveData_aTransferBuffer, SAVE_DATA_STATE_SIZE);
            SaveData_ApplyRuntimeState((SaveDataState *)p);
        }
        return r;
    }
    return 0;
}

void SaveData_RequestWrite(void){Util_CopyWords(gSaveData_aTransferBuffer,(u8 *)gDuel_awPlayerDeck,SAVE_DATA_STATE_SIZE);SaveData_BuildPayload((SaveDataPayload *)(gSaveData_aTransferBuffer-SAVE_DATA_HEADER_SIZE));MemCardDialog_Request(gSaveData_aTransferBuffer,SAVE_DATA_REPLICATED_STATE_SIZE,gMemCard_szSaveFileName,2);}

#ifdef VERSION_JAPAN
/* Japanese only, with no US counterpart: opens the memory-card dialog at
   step 3 for the 0x8000-byte buffer at 0x80140000, under the file name the
   Japanese image keeps at 0x8001032C ("BISLPMP86398-YUGIOH"). */
void SaveData_RequestOptionsWrite(void)
{
    MemCardDialog_Request((u8 *)0x80140000, 0x8000, gMemCard_szOptionsFileName, 3);
}
#endif

#ifdef VERSION_JAPAN
#define SAVE_DATA_LOAD_PAIR_CHANNEL(base, index) \
    ((DuelEffectChannel *)((u8 *)(base) + (index) * 0x60))
#else
#define SAVE_DATA_LOAD_PAIR_CHANNEL(base, index) (&(base)[index])
#endif

/* The two-player load, validation and write-back runtime. The state machine
   first loads both card slots, then the two wrappers add trade or duel setup,
   and the final request writes the paired trade result back. */

/* MATCH 2026-09-06, first-day function from the m2c draft (257 instructions,
 * the memory-card save state machine: format, load, verify, retry). Jump
 * table on the mode nibble, so only the full build proves it. Levers, in
 * order: the scalar arm for gSaveData_LoadPairState; gInput_awPadPressed[0]
 * volatile (retail reloads it for the second bit test);
 * the call arguments gMemCardDialog_ChannelIndex and gSaveData_PairMessageCode hoisted into locals BEFORE the
 * +0x60 store that precedes the sprite call (-6 -> -1); ONE name for the
 * sprite pointer in cases 0 and 0xA, which is what puts it in $s0 (its live
 * range crosses the wait loop's calls in 0xA); the record passed to
 * TextBox_Destroy as its ADDRESS, index-first (`&gText_aChannels[i]`; m2c had read
 * it as a load); case 3's `v = 0` before the gMemCardDialog_Port store reaching a
 * SHARED `return v` through a goto, so the zero stays a variable; and the
 * flags read through a block-local base `q = gText_aChannels` so the 0x34 is a
 * load displacement rather than folded into %lo. */

s32 SaveData_UpdateLoadPair(void)
{
    DuelEffectChannel *o;
    s32 v;
    s32 m;
    DuelEffectChannel *q;
    s32 a;
    s32 b;

    m = gSaveData_LoadPairState & 0xF;
    switch (m) {
    case 0:
        if ((gSaveData_LoadPairState & 0x80) == 0) {
            gSaveData_LoadPairState |= 0x80;
            MemCardDialog_CreateObject();
            a = gMemCardDialog_ChannelIndex;
            gMemCard_pDialogObject->field_60 = -0x400;
            o = TextBox_Create(a, 0xC2, DIALOG_BOX_X, 0x50, DIALOG_BOX_WIDTH, 0x30);
            o->depth_offset = 0x10;
            TextBox_BuildBlocking(o);
        }
        if (MemCardDialog_StepSlide(
                gMemCard_pDialogObject, DIALOG_BOX_X, 0x50, gMemCardDialog_ChannelIndex) == 0) {
            gSaveData_LoadPairState = 1;
        }
        return 0;
    case 1:
        if (gInput_awPadPressed[0] & SAVE_DATA_LOAD_PAIR_CANCEL_BUTTON) {
            SD_SEPlayFull(8);
            gSaveData_LoadPairState = 0x82;
            gMemCard_pDialogObject->field_60 = 0x400;
        } else if (gInput_awPadPressed[0] & SAVE_DATA_LOAD_PAIR_CONFIRM_BUTTON) {
            SD_SEPlayFull(7);
            gSaveData_LoadPairState = 2;
            gMemCard_pDialogObject->field_60 = 0x400;
        }
        return 0;
    case 2:
        if (MemCardDialog_StepSlide(
                gMemCard_pDialogObject, DIALOG_BOX_X, 0x100, gMemCardDialog_ChannelIndex) == 0) {
            TextBox_Destroy(
                SAVE_DATA_LOAD_PAIR_CHANNEL(gText_aChannels, gMemCardDialog_ChannelIndex));
            DisplayObject_ReleaseIfPresent(gMemCard_pDialogObject);
            gMemCard_pDialogObject = (DisplayObject *)0;
            if ((gSaveData_LoadPairState & 0x80) == 0) {
                gSaveData_LoadPairState = 3;
                MemCardDialog_Request(
                    gSaveData_aPairWorkspace,
                    SAVE_DATA_STATE_SIZE,
                    gMemCard_szSaveFileName,
                    1
                );
                return 0;
            }
            return 2;
        }
        return 0;
    case 3:
        v = MemCardDialog_Poll();
        if (v != 0) {
            if (v == 1) {
                if (gSaveData_LoadPairState & 0x40) {
                    if (SaveData_HasSameDuelistCode(
                            (SaveDataState *)gSaveData_aPairWorkspace,
                            (SaveDataState *)(gSaveData_aPairWorkspace + 0x1000)) == 0) {
                        return 1;
                    }
                    gSaveData_LoadPairState = 0xA;
                    return 0;
                }
                gSaveData_LoadPairState |= 0x40;
                MemCardDialog_Request(
                    gSaveData_aSecondPlayerState,
                    SAVE_DATA_STATE_SIZE,
                    gMemCard_szSaveFileName,
                    1
                );
                v = 0;
                gMemCardDialog_Port = 0x10;
                goto done;
            }
            return v;
        }
    done:
        return v;
    case 0xA:
        if ((gSaveData_LoadPairState & 0x80) == 0) {
            gSaveData_LoadPairState |= 0xC0;
            MemCardDialog_CreateObject();
            a = gMemCardDialog_ChannelIndex;
            b = gSaveData_PairMessageCode;
            gMemCard_pDialogObject->field_60 = -0x400;
            o = TextBox_CreateFlagged(a, b, DIALOG_BOX_X, 0x50, DIALOG_BOX_WIDTH, 0x30, SAVE_DATA_LOAD_BOX_FLAGS);
            o->depth_offset = 0x10;
            do {
                TextBox_UpdateChannels();
            } while ((o->flags_34 & TEXT_BOX_FLAG_DONE) == 0);
        }
        if (gSaveData_LoadPairState & 0x40) {
            if (MemCardDialog_StepSlide(
                    gMemCard_pDialogObject, DIALOG_BOX_X, 0x50, gMemCardDialog_ChannelIndex) == 0) {
                gSaveData_LoadPairState &= 0xBF;
            }
            return 0;
        }
        TextBox_UpdateChannels();
        q = gText_aChannels;
        if ((SAVE_DATA_LOAD_PAIR_CHANNEL(q, gMemCardDialog_ChannelIndex)->flags_34 & SAVE_DATA_LOAD_CHANNEL_FLAG) == 0) {
            gSaveData_LoadPairState = 0xB;
        }
        return 0;
    case 0xB:
        if ((gSaveData_LoadPairState & 0x80) == 0) {
            gSaveData_LoadPairState |= 0x80;
            gMemCard_pDialogObject->field_60 = 0x400;
        }
        if (MemCardDialog_StepSlide(
                gMemCard_pDialogObject, DIALOG_BOX_X, 0x100, gMemCardDialog_ChannelIndex) == 0) {
            TextBox_Destroy(
                SAVE_DATA_LOAD_PAIR_CHANNEL(gText_aChannels, gMemCardDialog_ChannelIndex));
            DisplayObject_ReleaseIfPresent(gMemCard_pDialogObject);
            gMemCard_pDialogObject = (DisplayObject *)0;
            return 2;
        }
        return 0;
    }
    return 0;
}
#undef SAVE_DATA_LOAD_PAIR_CHANNEL

s32 SaveData_UpdateTradeLoad(void)
{
    if ((gSaveData_PairSetupState & 0x80) == 0) {
        gSaveData_PairSetupState |= 0x80;
        gSaveData_PairMessageCode = 0x29;
    }
    return SaveData_UpdateLoadPair();
}

s32 SaveData_UpdateDuelLoad(void)
{
    s32 result;
    s32 i;
    u8 *left;
    u8 *right;
    TextStagingValues *state;

    if ((gSaveData_PairSetupState & 128) == 0) {
        gSaveData_PairSetupState |= 128;
        gSaveData_PairMessageCode = 40;
    }
    result = SaveData_UpdateTradeLoad();
    if (result == 1) {
        left = gSaveData_aPairWorkspace;
        right = gSaveData_aPairWorkspace + TWO_PLAYER_SAVE_SLOT_STRIDE;
        i = 0;
        state = gText_StagedValues;
        gSaveData_LoadPairState = 10;
        gSaveData_PairMessageCode = 36;
        for (; i < DECK_SIZE; i++, left += 2, right += 2) {
            if (*(u16 *)left == 0) {
                state->deck_validation.invalid_side = 1;
                return 0;
            }
            if (*(u16 *)right == 0) {
                state->deck_validation.invalid_side = 2;
                return 0;
            }
        }
        Text_SjisToGlyphCodes(
            gText_aFirstPlayerNameGlyphs,
            gSaveData_aFirstPlayerNameSjis,
            SAVE_DATA_PLAYER_NAME_CHAR_COUNT
        );
        Text_SjisToGlyphCodes(
            gText_aSecondPlayerNameGlyphs,
            gSaveData_aFirstPlayerNameSjis + TWO_PLAYER_SAVE_SLOT_STRIDE,
            SAVE_DATA_PLAYER_NAME_CHAR_COUNT
        );
    }
    return result;
}

void SaveData_RequestTradeWrite(void)
{
    u8 *p = gSaveData_aTradeWriteStaging;
    u8 *q = p + TWO_PLAYER_SAVE_SLOT_STRIDE;

    SaveData_WritePrimarySecondaryIntegrity(p);
    SaveData_WritePrimarySecondaryIntegrity(q);
    gMemCard_pSecondaryTransferCursor = q;
    MemCardDialog_Request(
        p,
        TWO_PLAYER_SAVE_TRANSFER_SIZE,
        gMemCard_szSaveFileName,
        4
    );
}

#include "save_data.h"

void SaveData_SetMaskSeed(u32 value)
{
    gSaveData_dwMaskStateHigh = value;
    gSaveData_dwMaskStateLow = value;
}
