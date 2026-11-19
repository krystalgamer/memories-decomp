#include "../types.h"

/* Zero-initialized storage this unit owns at 0x8009B428-0x8009B430, defined in
 * address order ahead of the headers that declare it (#2602). */
u8 gMain_bFrontendPhase;
static PADDING_VARIABLE(1);
static PADDING_VARIABLE(2);
static u32 D_8009B42C;

#include "main.h"
#include "main_boot_frontend.h"
#include "display_object_system.h"
#include "display_object_layout.h"
#include "display_object.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"
#include "fade.h"
#include "file_transfer.h"
#include "graphics_constants.h"
#include "main_mode_runners.h"
#include "sound_effect.h"
#include "display_asset_banks.h"
#include "duel_effect_states.h"
#include "duel_effect_text.h"
#include "../overlays/main_menu/frontend.h"
#include "../psyq/rand.h"
#include "input.h"
#include "movie_playback_control.h"
#include "sound.h"
#ifdef VERSION_EUROPE
#include "high_memory_addresses.h"
#include "european_text_bank_views.h"
#include "card_preview.h"
#endif

/* The boot and front-end unit (#2602, #39). Main_RunBootSequence drives the
 * boot screen helpers ahead of it: the two transfer callbacks it queues for
 * the boot package and boot image, the crossfade that swaps one full-screen
 * boot sprite for the next, and the hold that keeps a boot screen up until its
 * frame count runs out or Start/confirm skips it once pending transfers have
 * finished. The front-end loop follows the sequence; retail reads
 * gMain_bFrontendPhase gp-relative only from those two functions. Every
 * member compiles at gcc_2_8_1_g8_split. */

/* Japan accepts circle or square as confirm on the boot screens and in the
 * front-end loop. */
#ifndef MAIN_BOOT_CONFIRM_MASK
#ifdef VERSION_JAPAN
#define MAIN_BOOT_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#else
#define MAIN_BOOT_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif
#endif

void Main_LoadBootPackageStage(FileTransferDescriptor *obj, s32 stage) {
#ifdef VERSION_JAPAN
    /* The Japanese boot package is transferred in three stages rather than
       four: a 32-sector first stage at 0x2C0, and a last stage that uploads
       the image and then points at gFile_pScreenBankBase, folding the US stages 2
       and 3 together. */
    switch (stage) {
    case 0: {
        u32 m;
        s32 w;

        m = 0xFFDDFFFF;
        obj->field_30.h.counter = 0x2C0;
        w = 0x40;
        obj->field_30.h.field_32 = 0;
        obj->rect.w = w;
        gFile_dwTransferState &= m;
        m = 0x10000;
        obj->phase_size = m;
        gFile_dwTransferState |= m;
        obj->transfer_mode = 2;
        obj->rect.h = 0x10;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;
    }

    case 1:
        obj->phase_size = 2 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = gFile_TransferBufferBase;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->transfer_mode = 1;
        break;

    case 2:
        obj->rect.x = 0x200;
        obj->rect.y = 0xF8;
        obj->rect.w = 0x100;
        obj->rect.h = 8;
        LoadImage2(&obj->rect, (u32 *)gFile_TransferBufferBase);
        obj->phase_size = 3 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = gFile_pScreenBankBase;
        obj->sector_destination = gFile_pScreenBankBase;
        obj->transfer_mode = 1;
        break;
    }
#elif defined(VERSION_EUROPE)
    /* The European boot package takes eight stages: three header stages
       (0x280, 0x2C0 and 0x300 in the counter), the US image stages with the
       image at x 0x280, and three last stages that land in the European
       text banks. */
    switch (stage) {
    case 0: {
        u32 cleared;
        u32 loaded;
        u32 enabled;

        obj->field_30.h.counter = 0x280;
        obj->rect.w = 0x40;
        obj->rect.h = 0x10;
        cleared = gFile_dwTransferState & 0xFFDDFFFF;
        gFile_dwTransferState = cleared;
        do { loaded = gFile_dwTransferState; } while (0);
        do { enabled = 0x10000; } while (0);
        obj->field_30.h.field_32 = 0;
        gFile_dwTransferState = loaded | enabled;
        obj->transfer_mode = 2;
        obj->phase_size = 12 * FILE_SECTOR_SIZE;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;
    }

    case 1: {
        u32 cleared;
        u32 loaded;
        u32 enabled;

        obj->field_30.h.counter = 0x2C0;
        obj->rect.w = 0x40;
        obj->rect.h = 0x10;
        cleared = gFile_dwTransferState & 0xFFDDFFFF;
        gFile_dwTransferState = cleared;
        do { loaded = gFile_dwTransferState; } while (0);
        do { enabled = 0x10000; } while (0);
        obj->field_30.h.field_32 = 0;
        gFile_dwTransferState = loaded | enabled;
        obj->transfer_mode = 2;
        obj->phase_size = 13 * FILE_SECTOR_SIZE;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;
    }

    case 2: {
        u32 cleared;
        u32 loaded;
        u32 enabled;

        obj->field_30.h.counter = 0x300;
        obj->rect.w = 0x40;
        obj->rect.h = 0x10;
        cleared = gFile_dwTransferState & 0xFFDDFFFF;
        gFile_dwTransferState = cleared;
        do { loaded = gFile_dwTransferState; } while (0);
        do { enabled = 0x10000; } while (0);
        obj->field_30.h.field_32 = 0;
        gFile_dwTransferState = loaded | enabled;
        obj->transfer_mode = 2;
        obj->phase_size = 13 * FILE_SECTOR_SIZE;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;
    }

    case 3:
        obj->phase_size = 2 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = gFile_TransferBufferBase;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->transfer_mode = 1;
        break;

    case 4:
        obj->rect.x = 0x280;
        obj->rect.y = 0xF8;
        obj->rect.w = 0x100;
        obj->rect.h = 8;
        LoadImage2(&obj->rect, (u32 *)gFile_TransferBufferBase);
        obj->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = gFile_TransferBufferBase + 2 * FILE_SECTOR_SIZE;
        obj->sector_destination = gFile_TransferBufferBase + 2 * FILE_SECTOR_SIZE;
        obj->transfer_mode = 1;
        break;

    case 5:
        obj->rect.x = 0x280;
        obj->rect.y = 0xC0;
        obj->rect.w = 0x10;
        obj->rect.h = 8;
        LoadImage2(&obj->rect, (u32 *)(gFile_TransferBufferBase + 2 * FILE_SECTOR_SIZE));
        obj->phase_size = 30 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = gFile_pBootTextStage5Destination;
        obj->sector_destination = gFile_pBootTextStage5Destination;
        obj->transfer_mode = 1;
        break;

    case 6:
        obj->phase_size = 32 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = gFile_pBootTextStage6Destination;
        obj->sector_destination = gFile_pBootTextStage6Destination;
        obj->transfer_mode = 1;
        break;

    case 7:
        obj->phase_size = 7 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = gFile_pBootTextStage7Destination;
        obj->sector_destination = gFile_pBootTextStage7Destination;
        obj->transfer_mode = 1;
        break;
    }
#else
    switch (stage) {
    case 0:
        obj->field_30.h.counter = 0x280;
        obj->field_30.h.field_32 = 0;
        obj->rect.w = 0x40;
        obj->rect.h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        obj->phase_size = 48 * FILE_SECTOR_SIZE;
        gFile_dwTransferState |= 0x10000;
        obj->transfer_mode = 2;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 1:
        obj->phase_size = 2 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = gFile_TransferBufferBase;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->transfer_mode = 1;
        break;

    case 2:
        obj->rect.x = 0x200;
        obj->rect.y = 0xF8;
        obj->rect.w = 0x100;
        obj->rect.h = 8;
        LoadImage2(&obj->rect, (u32 *)gFile_TransferBufferBase);
        obj->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = gFile_TransferBufferBase + 2 * FILE_SECTOR_SIZE;
        obj->sector_destination = gFile_TransferBufferBase + 2 * FILE_SECTOR_SIZE;
        obj->transfer_mode = 1;
        break;

    case 3:
        obj->rect.x = 0x280;
        obj->rect.y = 0xE8;
        obj->rect.w = 0x10;
        obj->rect.h = 8;
        LoadImage2(&obj->rect, (u32 *)(gFile_TransferBufferBase + 2 * FILE_SECTOR_SIZE));
        obj->phase_size = 3 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = gFile_pScreenBankBase;
        obj->sector_destination = gFile_pScreenBankBase;
        obj->transfer_mode = 1;
        break;
    }
#endif
}

#ifdef VERSION_EUROPE
/* The European boot image takes four stages: the header stage counts 0xC0,
   the image goes to (0x280, 0xE0), and a fourth stage reads three sectors
   to gFile_pScreenBankBase. */
void Main_LoadBootImageStage(FileTransferDescriptor *p, s32 mode)
{
    s32 one;
    s32 v;
    s32 w;
    s32 x;
    s32 m;
    s32 d;
    s32 f;

    one = 1;

    switch (mode) {
    case 0:
        m = 0xFFDDFFFF;
        p->field_30.h.counter = 0xC0;
        p->field_30.h.field_32 = 0x100;
        v = gFile_dwTransferState;
        f = 0x40;
        p->rect.w = f;
        gFile_dwTransferState = v & m;
        w = gFile_dwTransferState;
        m = 0x10000;
        do {
            p->phase_size = m;
            gFile_dwTransferState = w | m;
            p->transfer_mode = 2;
            d = gFile_TransferBufferBase;
            p->rect.h = 0x10;
        } while (0);
        p->sector_destination = d;
        p->alternate_sector_destination = d + FILE_SECTOR_SIZE;
        break;

    case 1:
        p->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState = gFile_dwTransferState & 0xFFDCFFFF;
        p->alternate_sector_destination = gFile_TransferBufferBase;
        p->sector_destination = gFile_TransferBufferBase;
        p->transfer_mode = one;
        break;

    case 2:
        p->rect.x = 0x280;
        p->rect.y = 0xE0;
        p->rect.w = 0x100;
        p->rect.h = 4;
        LoadImage2(&p->rect, (u32 *)gFile_TransferBufferBase);
        p->alternate_sector_destination = gDisplay_abResourceBank0;
        p->sector_destination = gDisplay_abResourceBank0;
        x = gFile_dwTransferState;
        p->phase_size = 0x800;
        gFile_dwTransferState = x & 0xFFDCFFFF;
        p->transfer_mode = one;
        break;

    case 3:
        p->phase_size = 3 * FILE_SECTOR_SIZE;
        gFile_dwTransferState = gFile_dwTransferState & 0xFFDCFFFF;
        p->alternate_sector_destination = gFile_pScreenBankBase;
        p->sector_destination = gFile_pScreenBankBase;
        p->transfer_mode = one;
        break;
    }
}
#else
void Main_LoadBootImageStage(FileTransferDescriptor *p, s32 mode)
{
    s32 one;
    s32 v;
    s32 w;
    u8 *e;
    s32 x;
    s32 m;
    u8 *d;
    s32 f;
    s32 hun;

    one = 1;

    switch (mode) {
    case 0:
        m = 0xFFDDFFFF;
        p->field_30.h.field_32 = 0x100;
        v = gFile_dwTransferState;
        f = 0x40;
        do {
            p->field_30.h.counter = 0;
        } while (0);
        p->rect.w = f;
        gFile_dwTransferState = v & m;
        w = gFile_dwTransferState;
        m = 0x10000;
        do {
            p->phase_size = m;
            gFile_dwTransferState = w | m;
            p->transfer_mode = 2;
            d = gFile_TransferBufferBase;
            p->rect.h = 0x10;
        } while (0);
        p->sector_destination = d;
        p->alternate_sector_destination = d + FILE_SECTOR_SIZE;
        break;

    case 1:
        p->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState = gFile_dwTransferState & 0xFFDCFFFF;
        p->alternate_sector_destination = gFile_TransferBufferBase;
        p->sector_destination = gFile_TransferBufferBase;
        p->transfer_mode = one;
        break;

    case 2:
        do {
            hun = 0x100;
        } while (0);
        /* The RECT halves are stored through the members' addresses as u16.
         * A plain member store, or an s16 cast that fold turns back into one,
         * is an in-struct reference and is reordered against the gFile_TransferBufferBase
         * load; retail keeps these in source order around LoadImage2. */
        *(u16 *)&p->rect.y = 0xF0;
        e = gFile_TransferBufferBase;
        *(u16 *)&p->rect.x = hun;
        *(u16 *)&p->rect.w = hun;
        *(u16 *)&p->rect.h = 4;
        LoadImage2(&p->rect, (u32 *)e);
        p->alternate_sector_destination = gDisplay_abResourceBank0;
        p->sector_destination = gDisplay_abResourceBank0;
        x = gFile_dwTransferState;
        p->phase_size = 0x800;
        gFile_dwTransferState = x & 0xFFDCFFFF;
        p->transfer_mode = one;
        break;
    }
}
#endif

/* Regional value: pivot.y beside pivot.x = 0xA0: half the screen height
 * (0x78 -> 0x80). The European wrapper defines its own. */
#ifndef DISPLAY_OBJECT_TRANSITION_HALF_HEIGHT
#define DISPLAY_OBJECT_TRANSITION_HALF_HEIGHT 0x78
#endif

void DisplayObject_CrossfadeReplace(DisplayObject *a, DisplayObject *b)
{
    DisplayObject *x;
    DisplayObject *y;
    s32 i;
    s32 hi;
    s32 c;
    s32 v;

    x = 0;

    if (a != 0) {
        x = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(x, 0, 0, 0, 0, a->resource_variant, a->texture_page, 0x20D,
                      gDisplay_abResourceBank0);
        x->attribute = a->attribute;
        DisplayObject_SetDepthOffset(x, -1);
        x->flags = x->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        x->attribute = x->attribute | (GsALON | GsATWO);
        a->attribute = a->attribute | (GsALON | GsAONE);
    }

    b->field_48.pivot.x = 0xA0;
    b->field_48.pivot.y = DISPLAY_OBJECT_TRANSITION_HALF_HEIGHT;
    b->attribute = b->attribute & ~GsROTOFF;

    y = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPositionWithResource(y, 0, 0, 0, 0, b->resource_variant, b->texture_page, 0x20D, gDisplay_abResourceBank0);
    y->attribute = b->attribute;
    DisplayObject_SetDepthOffset(y, -1);
    y->flags = y->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    y->attribute = y->attribute | (GsALON | GsATWO);
    y->field_48.word = b->field_48.word;
    b->attribute = b->attribute | (GsALON | GsAONE);

    i = 0;
    hi = 0x80;

    do {
        c = ((i << 16) | (i << 8)) | i;
        b->rgb.word = c;
        y->rgb.word = c;
        if (a != 0) {
            c = hi - i;
            v = c * 0x60 + 0x1000;
            c = c | ((c << 16) | (c << 8));
            y->field_44.scale.y = v;
            y->field_44.scale.x = v;
            b->field_44.scale.y = v;
            b->field_44.scale.x = v;
            a->rgb.word = c;
            x->rgb.word = c;
        }
        i += 8;
        Main_AdvanceFrame();
    } while (i < 0x81);

    b->field_44.word = 0x10001000;
    b->attribute = (b->attribute | GsROTOFF) & ~(GsALON | GsATWO | GsAONE);
    DisplayObject_ReleaseIfPresent(a);
    DisplayObject_ReleaseIfPresent(x);
    DisplayObject_ReleaseIfPresent(y);
}

void Main_HoldBootScreen(s32 count)
{
    s32 found = 0;

    for (;;) {
        Main_AdvanceFrame();
        if (!found &&
            (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
              gFile_dwCancelRequestState) == 0))
            found = 1;
        if ((gInput_awPadPressed[0] &
             (PAD_BUTTON_START | MAIN_BOOT_CONFIRM_MASK)) && found)
            count = 0;
        count--;
        if (count >= 0)
            continue;
        if (!found) {
            count = 0;
            continue;
        }
        break;
    }
}

#if defined(VERSION_EUROPE)
/* The European boot sequence factors the debug-font setup and the boot
   package request out into two helpers: the package is one 110-sector
   package per text language, from 0x1962, and the main path asks for
   BUILD_LANGUAGE_INDEX once the logos are done. */
void Main_InitDebugFont(void)
{
    FntLoad(0x2C0, 0);
    SetDumpFnt(FntOpen(
        0x10, 0x10, GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT, 0, 1000
    ));
}

void Main_RequestLanguageBootPackage(s32 language, s32 wait)
{
    File_RequestAsyncTransfer(
        0, 0, language * 110 + 0x1962, 110, Main_LoadBootPackageStage, 0, 0
    );
    if (wait) {
        File_WaitForTransfers();
        Main_InitDebugFont();
    }
}

void Main_RunBootSequence(s32 mode)
{
    DisplayObject *object;
    DisplayObject *first;

    gMain_bFrontendPhase = 0;
    gGraphics_wFlags = 0;
    if (mode != 0) {
        Main_RequestLanguageBootPackage(gText_bLanguageIndex, 1);
        SD_ReplaceValueLink(2);
        Main_AdvanceFrames(4);
    } else {
        File_RequestAsyncTransfer(0, 0, 0x2503, 0x25, Main_LoadBootImageStage, 0, 0);
        File_WaitForTransfers();
        Fade_InitIn();
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 0, 0x13, 0x100,
                      gDisplay_abResourceBank0);
        object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                         DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_CrossfadeReplace(0, object);
        first = object;
        Main_HoldBootScreen(0xB4);
        CdFlush();
        SafetyInit();
        while (SafetySync(1) != 0) {
        }
        DsInit();
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 1, 0x13, 0x100,
                      gDisplay_abResourceBank0);
        object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                         DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_CrossfadeReplace(first, object);
        SD_ReplaceValueLink(2);
        Main_AdvanceFrames(4);
        Main_HoldBootScreen(0xB4);
        gText_bLanguageIndex = BUILD_LANGUAGE_INDEX;
        Main_RequestLanguageBootPackage(BUILD_LANGUAGE_INDEX, 0);
        Fade_WaitInitOut();
        Main_ResetFrontendRuntime();
        while (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                gFile_dwCancelRequestState) != 0) {
            Main_AdvanceFrame();
        }
        Main_InitDebugFont();
        File_RequestMainMenuPackage();
    }
    File_WaitForTransfers();
}
#elif defined(VERSION_JAPAN)
void Main_RunBootSequence(s32 mode)
{
    DisplayObject *object;
    DisplayObject *first;
    gMain_bFrontendPhase = 0;
    if (mode == 0) {
        File_RequestAsyncTransfer(0, 0, 0x1F90, 0x22, Main_LoadBootImageStage, 0, 0);
        File_WaitForTransfers();
    }
    File_RequestAsyncTransfer(
        0, 0, 0x1690, 0x25, Main_LoadBootPackageStage, 0, 0
    );
    if (mode != 0) {
        int display;
        File_WaitForTransfers();
        FntLoad(0x2C0, 0);
        display = FntOpen(
            0x10, 0x10, GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT,
            0, 1000
        );
        SetDumpFnt(display);
        gGraphics_wFlags = 0;
        SD_ReplaceValueLink(2);
        Main_AdvanceFrames(4);
        File_WaitForTransfers();
        return;
    }
    Fade_InitIn();
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 0, 0x10, 0x100,
                  gDisplay_abResourceBank0);
    first = object;
    first->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                    DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    DisplayObject_CrossfadeReplace(0, first);
    Main_HoldBootScreen(4);
    FntLoad(0x2C0, 0);
    SetDumpFnt(FntOpen(
        0x10, 0x10, GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT,
        0, 1000
    ));
    gGraphics_wFlags = 0;
    CdFlush();
    SafetyInit();
    while (SafetySync(1) != 0) {
    }
    DsInit();
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 1, 0x10, 0x100,
                  gDisplay_abResourceBank0);
    object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                     DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    DisplayObject_CrossfadeReplace(first, object);
    first = object;
    SD_ReplaceValueLink(2);
    Main_AdvanceFrames(4);
    Main_HoldBootScreen(0xB4);
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 2, 0x10, 0x100,
                  gDisplay_abResourceBank0);
    object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                     DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    DisplayObject_CrossfadeReplace(first, object);
    File_RequestMainMenuPackage();
    Main_HoldBootScreen(0xB4);
    Fade_WaitInitOut();
    Main_ResetFrontendRuntime();
}
#else
void Main_RunBootSequence(s32 mode)
{
    register DisplayObject *object;
    register DisplayObject *first;
    gMain_bFrontendPhase = 0;
    if (mode == 0) {
        File_RequestAsyncTransfer(0, 0, 0x1F85, 0x22, Main_LoadBootImageStage, 0, 0);
        File_WaitForTransfers();
    }
    File_RequestAsyncTransfer(
        0, 0, 0x1690, 0x36, Main_LoadBootPackageStage, 0, 0
    );
    if (mode != 0) {
        int display;
        File_WaitForTransfers();
        FntLoad(0x2C0, 0);
        display = FntOpen(
            0x10, 0x10, GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT,
            0, 1000
        );
        SetDumpFnt(display);
        gGraphics_wFlags = 0;
        SD_ReplaceValueLink(2);
        Main_AdvanceFrames(4);
        File_WaitForTransfers();
        return;
    }
    Fade_InitIn();
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 0, 0x10, 0x100,
                  gDisplay_abResourceBank0);
    object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                     DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    DisplayObject_CrossfadeReplace(0, object);
    Main_HoldBootScreen(4);
    FntLoad(0x2C0, 0);
    SetDumpFnt(FntOpen(
        0x10, 0x10, GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT,
        0, 1000
    ));
    gGraphics_wFlags = 0;
    CdFlush();
    first = object;
    SafetyInit();
    while (SafetySync(1) != 0) {
    }
    DsInit();
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 0, 0, 1, 0x10, 0x100,
                  gDisplay_abResourceBank0);
    object->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                     DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    DisplayObject_CrossfadeReplace(first, object);
    SD_ReplaceValueLink(2);
    Main_AdvanceFrames(4);
    File_RequestMainMenuPackage();
    Main_HoldBootScreen(0xB4);
    Fade_WaitInitOut();
    Main_ResetFrontendRuntime();
}
#endif

s32 Main_RunFrontendLoop(void) {
    s32 r;
    u32 f;
    s32 g;

#ifdef VERSION_EUROPE
    /* The European loop resets the frontend runtime before the fade. */
    Main_ResetFrontendRuntime();
#endif
    Fade_WaitInitIn();

    for (;;) {
        gMain_bFrontendPhase = 0;
        for (;;) {
            rand();
            Main_AdvanceFrame();
            f = gMain_bFrontendPhase;
            if ((f & 1) == 0) {
                if ((f & 0x80) == 0) {
                    gMain_bFrontendPhase = f | 0x80;
                    Main_ResetFrontendRuntime();
                    Movie_Play(0);
                    gGraphics_bClearBlue = 0xFF;
                    gGraphics_bClearGreen = 0xFF;
                    gGraphics_bClearRed = 0xFF;
                    continue;
                }
                if (f & 0x40) {
                    if (gInput_awPadPressed[0] &
                        (PAD_BUTTON_START | MAIN_BOOT_CONFIRM_MASK)) {
                        TextBox_Destroy(gText_aChannels);
                        gMain_bFrontendPhase = 1;
                    }
                    continue;
                }
                g = gMovie_bPlaybackState;
                if (g & MOVIE_PLAYBACK_STATE_PLAYING) {
                    continue;
                }
                gMain_bFrontendPhase = 1;
                if ((g & MOVIE_PLAYBACK_STATE_STOP_REQUESTED) == 0) {
                    Fade_CutToWhite();
                }
                Fade_WaitInitOut();
                continue;
            }
            if ((f & 0x80) == 0) {
                gMain_bFrontendPhase = f | 0x80;
                Main_ResetFrontendRuntime();
                MainMenu_InitFrontendMenu(0, 0);
                Fade_StartIn();
            }
            r = MainMenu_UpdateFrontendMenu();
            if (r != -1) {
                break;
            }
        }
        SD_BGMFadeOut();
        Fade_WaitOut();
        MainMenu_DestroyFrontendMenu();
        Main_ResetFrontendRuntime();
        if (r != -2) {
            return r;
        }
    }
}
