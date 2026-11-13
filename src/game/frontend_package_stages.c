#include "../types.h"
#include "display_asset_banks.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "high_memory_addresses.h"
#include "shared_work_base.h"
#include "file_constants.h"
#include "../overlays/free_duel/free_duel.h"
#include "text_constants.h"
#include "duel_scene.h"
#include "campaign_flags.h"
#include "card_preview.h"
#include "frontend_package_stages.h"

/* The frontend package stages, one translation unit from FreeDuel_LoadPackageStage
 * (US 0x8003B808) through File_RequestGameOverPackage (0x8003C498): the Free
 * Duel, name entry, password, campaign map, Options and Game Over package
 * transfer callbacks with the requests that queue them, Main_InitFreeDuelMenu,
 * and Text_SjisToGlyphCodes, which name entry and the password screen use to
 * turn typed Shift-JIS into glyph codes. The European build adds two earlier
 * frontend package callbacks ahead of the Options stage. Each stage's
 * regional values are #ifndef defaults the regional wrappers override. */

/* Regional values, which the European wrapper defines for itself: the
   sectors mode 0 transfers (32 in the US build, 48 in the European one)
   and where mode 2 uploads its image row, (0, 0xF0) in the US build and
   (0x280, 0xD0) in the European one. */
#ifndef FREE_DUEL_STAGE0_SECTORS
#define FREE_DUEL_STAGE0_SECTORS 32
#endif
#ifndef FREE_DUEL_IMAGE_X
#define FREE_DUEL_IMAGE_X 0
#endif
#ifndef FREE_DUEL_IMAGE_Y
#define FREE_DUEL_IMAGE_Y 0xF0
#endif

void FreeDuel_LoadPackageStage(FileTransferDescriptor *object, s32 mode) {
    switch (mode) {
    case 0:
        object->field_30.h.counter = 0;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        gFile_dwTransferState |= 0x10000;
        object->transfer_mode = 2;
        object->phase_size = FREE_DUEL_STAGE0_SECTORS * FILE_SECTOR_SIZE;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 2:
        object->x = FREE_DUEL_IMAGE_X;
        object->y = FREE_DUEL_IMAGE_Y;
        object->w = 0x100;
        object->h = 4;
        LoadImage2((RECT *)object, (u32 *)gFile_TransferBufferBase);
        object->alternate_sector_destination = (s32)gDisplay_abResourceBank0;
        object->sector_destination = (s32)gDisplay_abResourceBank0;
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->transfer_mode = 1;
        break;

    case 3:
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->phase_size = 48 * FILE_SECTOR_SIZE;
        object->alternate_sector_destination = (s32)gRuntime_pSharedWorkBase;
        object->sector_destination = (s32)gRuntime_pSharedWorkBase;
        object->transfer_mode = 1;
        break;

    case 4:
        object->phase_size = 5 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = (u32)gFile_pScreenBankBase;
        object->sector_destination = (u32)gFile_pScreenBankBase;
        object->transfer_mode = 1;
        break;
    }
}

/* Regional value: the free duel package's sector count (0x57 -> 0x67). The
 * European build (src/game/european/) defines its own. */
#ifndef FREE_DUEL_PACKAGE_SECTOR_COUNT
#define FREE_DUEL_PACKAGE_SECTOR_COUNT FILE_WA_FREE_DUEL_SECTOR_COUNT
#endif

#ifndef FREE_DUEL_PACKAGE_START_SECTOR
#define FREE_DUEL_PACKAGE_START_SECTOR FILE_WA_FREE_DUEL_START_SECTOR
#endif

void Main_InitFreeDuelMenu(void){File_RequestAsyncTransfer(0,0,FREE_DUEL_PACKAGE_START_SECTOR,FREE_DUEL_PACKAGE_SECTOR_COUNT,FreeDuel_LoadPackageStage,0,0);File_WaitForTransfers();FreeDuel_Init(gRuntime_pSharedWorkBase);}

/* The package's first sector in WA.MRG. A regional build defines its own. */
#ifndef NAME_ENTRY_PACKAGE_START_SECTOR
#define NAME_ENTRY_PACKAGE_START_SECTOR FILE_WA_NAME_ENTRY_START_SECTOR
#endif

#ifndef NAME_ENTRY_PACKAGE_IMAGE_X
#define NAME_ENTRY_PACKAGE_IMAGE_X 0x100
#endif

#ifndef NAME_ENTRY_PACKAGE_IMAGE_Y
#define NAME_ENTRY_PACKAGE_IMAGE_Y 0xF0
#endif

/* The Name Entry package load. File_RequestNameEntryPackage queues the
   package transfer with NameEntry_LoadPackageStage as its stage callback. */

void NameEntry_LoadPackageStage(FileTransferDescriptor *object, s32 mode)
{
    s32 one;
    s32 v;
    s32 w;
    s32 d;
    s32 m;
    s32 v2;
    s32 w2;
    s32 d2;
    s32 k;
    s32 c;
    s32 hun;
    s32 n;
    s32 x;
    u8 *y;

    /* Direct mode and y members change old-GCC commoning/register allocation. */
    one = 1;

    switch (mode) {
    case 0:
        m = 0xFFDDFFFF;
        do {
            k = 0x18000;
        } while (0);
        c = 0x100;
        object->field_30.h.counter = c;
        object->field_30.h.field_32 = c;
        v = gFile_dwTransferState;
        object->w = 0x40;
        gFile_dwTransferState = v & m;
        w = gFile_dwTransferState;
        n = 0x10000;
        do {
            *(s32 *)&object->phase_size = k;
            gFile_dwTransferState = w | n;
            object->transfer_mode = 2;
            d = gFile_TransferBufferBase;
            object->h = 0x10;
        } while (0);
        object->sector_destination = d;
        object->alternate_sector_destination = d + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->field_30.h.counter = 0x1C0;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        v2 = gFile_dwTransferState;
        object->h = 0x10;
        gFile_dwTransferState = v2 & 0xFFDDFFFF;
        w2 = gFile_dwTransferState;
        gFile_dwTransferState = w2 | 0x10000;
        object->transfer_mode = 2;
        d2 = gFile_TransferBufferBase;
        *(s32 *)&object->phase_size = 16 * FILE_SECTOR_SIZE;
        object->sector_destination = d2;
        object->alternate_sector_destination = d2 + FILE_SECTOR_SIZE;
        break;

    case 2:
        m = 0xFFDCFFFF;
        *(s32 *)&object->phase_size = FILE_SECTOR_SIZE;
        x = gFile_dwTransferState & m;
        y = gFile_TransferBufferBase;
        gFile_dwTransferState = x;
        object->alternate_sector_destination = (s32)y;
        object->sector_destination = (s32)y;
        object->transfer_mode = one;
        break;

    case 3:
#ifdef VERSION_EUROPE_NAME_ENTRY_PACKAGE_IMAGE
        do {
            hun = NAME_ENTRY_PACKAGE_IMAGE_X;
        } while (0);
        object->x = hun;
        *(u16 *)&object->y = NAME_ENTRY_PACKAGE_IMAGE_Y;
        k = gFile_TransferBufferBase;
        object->w = 0x100;
#else
        do {
            hun = 0x100;
        } while (0);
        *(u16 *)&object->y = 0xF0;
        k = gFile_TransferBufferBase;
        object->x = hun;
        object->w = hun;
#endif
        object->h = 4;
        LoadImage2((RECT *)object, (u32 *)k);
        m = 0xFFDCFFFF;
        *(s32 *)&object->phase_size = 15 * FILE_SECTOR_SIZE;
        x = gFile_dwTransferState & m;
        y = gFile_pScreenBankBase;
        gFile_dwTransferState = x;
        object->alternate_sector_destination = (s32)y;
        object->sector_destination = (s32)y;
        object->transfer_mode = one;
        break;
    }
}

void File_RequestNameEntryPackage(void){File_RequestAsyncTransfer(0,0,NAME_ENTRY_PACKAGE_START_SECTOR,FILE_WA_NAME_ENTRY_SECTOR_COUNT,NameEntry_LoadPackageStage,0,0);File_WaitForTransfers();}

void Text_SjisToGlyphCodes(u8 *dst, const u8 *src, s32 count)
{
    const u16 *input = (const u16 *)src;
    s32 i;

    for (i = count - 1; i >= 0; i--) {
        if (input[i] != 0) {
            count = i + 1;
            break;
        }
    }
    for (i = 0; i < count; i++, input++) {
        s32 value = *input;
        s32 index = value != 0;
        TextGlyphLookupEntry *entry = &gText_aGlyphLookup[index];
        u32 word = entry->packed;

        /* The constant switch enters the lookup scan at its word test, as
           retail does. The empty do/while stops the loop expander rotating
           the two-byte break to the bottom; loop.c then moves that block in
           front of the scan. */
        switch (0) {
            for (;;) {
                do {
                } while (0);
                if (entry->halves.key == value) {
                    if (index >= TEXT_SINGLE_BYTE_GLYPH_LIMIT) {
                        dst[0] = (index >> 8) | -16;
                        dst[1] = index;
                        dst += 2;
                        break;
                    }
                    *dst = index;
                } else {
                    entry++;
                    word = entry->packed;
                    index++;
        case 0:
                    if (word != 0) {
                        continue;
                    }
                    *dst = 0;
                }
                dst++;
                break;
            }
        }
    }
    *dst = TEXT_STRING_TERMINATOR;
}

/* The package's first sector in WA.MRG. A regional build defines its own. */
#ifndef PASSWORD_PACKAGE_START_SECTOR
#define PASSWORD_PACKAGE_START_SECTOR FILE_WA_PASSWORD_START_SECTOR
#endif

#ifndef PASSWORD_PACKAGE_IMAGE_X
#define PASSWORD_PACKAGE_IMAGE_X 0x100
#endif

#ifndef PASSWORD_PACKAGE_IMAGE_Y
#define PASSWORD_PACKAGE_IMAGE_Y 0xF0
#endif

/* The Password package load. File_RequestPasswordPackage queues the package
   transfer with Password_LoadPackageStage as its stage callback. */

void Password_LoadPackageStage(FileTransferDescriptor *object, s32 mode) {
    switch (mode) {
    case 0:
        object->field_30.h.counter = 0x300;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        gFile_dwTransferState |= 0x10000;
        object->transfer_mode = 2;
        object->phase_size = 64 * FILE_SECTOR_SIZE;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->phase_size = 4 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 2:
        object->x = PASSWORD_PACKAGE_IMAGE_X;
        object->y = PASSWORD_PACKAGE_IMAGE_Y;
        object->w = 0x100;
        object->h = 0x10;
        LoadImage2((RECT *)object, (u32 *)gFile_TransferBufferBase);
        object->alternate_sector_destination = (s32)gFile_StagingBuffer;
        object->sector_destination = (s32)gFile_StagingBuffer;
        object->phase_size = 3 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->transfer_mode = 1;
        break;

    case 3:
        object->phase_size = 15 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = (u32)gFile_pScreenBankBase;
        object->sector_destination = (u32)gFile_pScreenBankBase;
        object->transfer_mode = 1;
        break;
    }
}

void File_RequestPasswordPackage(void){File_RequestAsyncTransfer(0,0,PASSWORD_PACKAGE_START_SECTOR,FILE_WA_PASSWORD_SECTOR_COUNT,Password_LoadPackageStage,0,0);File_WaitForTransfers();}

/* Where case 5 uploads its image row: (0x100, 0xF0) in the US build and
   (0x280, 0xE0) in the European one, whose wrapper defines its own. */
#ifndef CAMPAIGN_MAP_IMAGE_X
#define CAMPAIGN_MAP_IMAGE_X 0x100
#endif
#ifndef CAMPAIGN_MAP_IMAGE_Y
#define CAMPAIGN_MAP_IMAGE_Y 0xF0
#endif

/* The package's first sector in WA.MRG. A regional build defines its own. */
#ifndef CAMPAIGN_MAP_PACKAGE_START_SECTOR
#define CAMPAIGN_MAP_PACKAGE_START_SECTOR FILE_WA_EGYPT_OVERWORLD_START_SECTOR
#endif

/* The Campaign Map package load. File_RequestEgyptOverworldPackage queues
   the Egypt overworld package with CampaignMap_LoadPackageStage as its stage
   callback, starting one package length further on the disc once
   CAMPAIGN_FLAG_TOURNAMENT_COMPLETE is set. */

void CampaignMap_LoadPackageStage(FileTransferDescriptor *object, s32 mode) {
    switch (mode) {
    case 0:
        object->phase_size = 6 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = (u32)gFile_pScreenBankBase;
        object->sector_destination = (u32)gFile_pScreenBankBase;
        object->transfer_mode = 1;
        break;

    case 1:
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->phase_size = 134 * FILE_SECTOR_SIZE;
        object->alternate_sector_destination = (s32)gRuntime_pSharedWorkBase;
        object->sector_destination = (s32)gRuntime_pSharedWorkBase;
        object->transfer_mode = 1;
        break;

    case 2:
        object->alternate_sector_destination = (s32)gDisplay_abResourceBank0;
        object->sector_destination = (s32)gDisplay_abResourceBank0;
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->transfer_mode = 1;
        break;

    case 3:
        object->field_30.h.counter = 0x1C0;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        gFile_dwTransferState |= 0x10000;
        object->transfer_mode = 2;
        object->phase_size = 16 * FILE_SECTOR_SIZE;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 4:
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 5:
        object->x = CAMPAIGN_MAP_IMAGE_X;
        object->y = CAMPAIGN_MAP_IMAGE_Y;
        object->w = 0x100;
        object->h = 4;
        LoadImage2((RECT *)object, (u32 *)gFile_TransferBufferBase);
        break;
    }
}

void File_RequestEgyptOverworldPackage(void){int x=0;if(Campaign_TestStoryFlag(CAMPAIGN_FLAG_TOURNAMENT_COMPLETE))x=FILE_WA_EGYPT_OVERWORLD_SECTOR_COUNT;File_RequestAsyncTransfer(0,0,x+CAMPAIGN_MAP_PACKAGE_START_SECTOR,FILE_WA_EGYPT_OVERWORLD_SECTOR_COUNT,CampaignMap_LoadPackageStage,0,0);File_WaitForTransfers();}

#ifdef VERSION_EUROPE
/* The first of two earlier PAL frontend package callbacks: 640x224 upload
   coordinates and two- and six-sector later phases.
   It is the four-mode Options_LoadPackageStage body (frontend_package_stages.c)
   with its own constants. No caller or address reference is known in the
   European executables. */
void Frontend_LoadPackageStageVariantA(FileTransferDescriptor *object, s32 mode)
{
    switch (mode) {
    case 0:
        object->field_30.h.field_32 = 0x100;
        object->field_30.h.counter = 0;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        object->phase_size = 32 * FILE_SECTOR_SIZE;
        gFile_dwTransferState |= 0x10000;
        object->transfer_mode = 2;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 2:
        object->x = 0x280;
        object->y = 0xE0;
        object->w = 0x100;
        object->h = 4;
        LoadImage2((RECT *)object, (u32 *)gFile_TransferBufferBase);
        object->alternate_sector_destination = (s32)gDisplay_abResourceBank0;
        object->sector_destination = (s32)gDisplay_abResourceBank0;
        object->phase_size = 2 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->transfer_mode = 1;
        break;

    case 3:
        object->phase_size = 6 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = (s32)gFile_pScreenBankBase;
        object->sector_destination = (s32)gFile_pScreenBankBase;
        object->transfer_mode = 1;
        break;
    }
}

/* The second earlier PAL frontend package callback: like the first, with a
   48-sector opening phase.
   It is the four-mode Options_LoadPackageStage body (frontend_package_stages.c)
   with its own constants. No caller or address reference is known in the
   European executables. */
void Frontend_LoadPackageStageVariantB(FileTransferDescriptor *object, s32 mode)
{
    switch (mode) {
    case 0:
        object->field_30.h.field_32 = 0x100;
        object->field_30.h.counter = 0;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        object->phase_size = 48 * FILE_SECTOR_SIZE;
        gFile_dwTransferState |= 0x10000;
        object->transfer_mode = 2;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 2:
        object->x = 0x280;
        object->y = 0xE0;
        object->w = 0x100;
        object->h = 4;
        LoadImage2((RECT *)object, (u32 *)gFile_TransferBufferBase);
        object->alternate_sector_destination = (s32)gDisplay_abResourceBank0;
        object->sector_destination = (s32)gDisplay_abResourceBank0;
        object->phase_size = 2 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->transfer_mode = 1;
        break;

    case 3:
        object->phase_size = 6 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = (s32)gFile_pScreenBankBase;
        object->sector_destination = (s32)gFile_pScreenBankBase;
        object->transfer_mode = 1;
        break;
    }
}
#endif

#ifndef OPTIONS_PACKAGE_STAGE_MODE_2_X
#define OPTIONS_PACKAGE_STAGE_MODE_2_X 0x100
#endif

#ifndef OPTIONS_PACKAGE_STAGE_MODE_0_SECTORS
#define OPTIONS_PACKAGE_STAGE_MODE_0_SECTORS 32
#endif

#ifndef OPTIONS_PACKAGE_STAGE_MODE_2_Y
#define OPTIONS_PACKAGE_STAGE_MODE_2_Y 0xF0
#endif

#ifndef OPTIONS_PACKAGE_STAGE_MODE_2_SECTORS
#define OPTIONS_PACKAGE_STAGE_MODE_2_SECTORS 1
#endif

#ifndef OPTIONS_PACKAGE_STAGE_MODE_3_SECTORS
#define OPTIONS_PACKAGE_STAGE_MODE_3_SECTORS 16
#endif

#ifndef OPTIONS_PACKAGE_STAGE_MODE_3_DESTINATION
#define OPTIONS_PACKAGE_STAGE_MODE_3_DESTINATION 0x80140000
#endif

#ifndef OPTIONS_PACKAGE_STAGE_MODE_3_COMMON_TAIL
#define OPTIONS_PACKAGE_STAGE_MODE_3_COMMON_TAIL 0
#endif

#ifndef GAME_OVER_PACKAGE_IMAGE_X
#define GAME_OVER_PACKAGE_IMAGE_X 0
#endif

#ifndef GAME_OVER_PACKAGE_IMAGE_Y
#define GAME_OVER_PACKAGE_IMAGE_Y 0xF0
#endif

/* The Options package load. File_RequestOptionsPackage queues the package
   transfer with Options_LoadPackageStage as its stage callback, then loads
   a second 0x10-sector block. */

void Options_LoadPackageStage(FileTransferDescriptor *object, s32 mode)
{
    switch (mode) {
    case 0:
        object->field_30.h.field_32 = 0x100;
        object->field_30.h.counter = 0;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        object->phase_size =
            OPTIONS_PACKAGE_STAGE_MODE_0_SECTORS * FILE_SECTOR_SIZE;
        gFile_dwTransferState |= 0x10000;
        object->transfer_mode = 2;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 2:
        object->x = OPTIONS_PACKAGE_STAGE_MODE_2_X;
        object->y = OPTIONS_PACKAGE_STAGE_MODE_2_Y;
        object->w = 0x100;
        object->h = 4;
        LoadImage2((RECT *)object, (u32 *)gFile_TransferBufferBase);
        object->alternate_sector_destination = (s32)gDisplay_abResourceBank0;
        object->sector_destination = (s32)gDisplay_abResourceBank0;
        object->phase_size = OPTIONS_PACKAGE_STAGE_MODE_2_SECTORS * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->transfer_mode = 1;
        break;

    case 3:
#if OPTIONS_PACKAGE_STAGE_MODE_3_COMMON_TAIL
        object->phase_size = OPTIONS_PACKAGE_STAGE_MODE_3_SECTORS * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = (s32)OPTIONS_PACKAGE_STAGE_MODE_3_DESTINATION;
        object->sector_destination = (s32)OPTIONS_PACKAGE_STAGE_MODE_3_DESTINATION;
#else
        object->alternate_sector_destination = (s32)OPTIONS_PACKAGE_STAGE_MODE_3_DESTINATION;
        object->sector_destination = (s32)OPTIONS_PACKAGE_STAGE_MODE_3_DESTINATION;
        object->phase_size = OPTIONS_PACKAGE_STAGE_MODE_3_SECTORS * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
#endif
        object->transfer_mode = 1;
        break;
    }
}

#ifndef OPTIONS_PACKAGE_START_SECTOR
#define OPTIONS_PACKAGE_START_SECTOR 0x2115
#endif

#ifndef OPTIONS_PACKAGE_SECTOR_COUNT
#define OPTIONS_PACKAGE_SECTOR_COUNT 0x32
#endif

#ifndef OPTIONS_PACKAGE_LOAD_SECOND_BLOCK
#define OPTIONS_PACKAGE_LOAD_SECOND_BLOCK 1
#endif

void File_RequestOptionsPackage(void)
{
    File_RequestAsyncTransfer(
        0,
        0,
        OPTIONS_PACKAGE_START_SECTOR,
        OPTIONS_PACKAGE_SECTOR_COUNT,
        Options_LoadPackageStage,
        0,
        0
    );
    File_WaitForTransfers();
#if OPTIONS_PACKAGE_LOAD_SECOND_BLOCK
    File_RequestAsyncTransfer(0, 0, 0x2147, 0x10, 0, 0, (int)0x80140000);
    File_WaitForTransfers();
#endif
}

void GameOver_LoadPackageStage(FileTransferDescriptor *object, s32 mode)
{
    switch (mode) {
    case 0:
        object->field_30.h.field_32 = 0x100;
        object->field_30.h.counter = 0;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        object->phase_size = 48 * FILE_SECTOR_SIZE;
        gFile_dwTransferState |= 0x10000;
        object->transfer_mode = 2;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 2:
        object->x = GAME_OVER_PACKAGE_IMAGE_X;
        object->y = GAME_OVER_PACKAGE_IMAGE_Y;
        object->w = 0x100;
        object->h = 4;
        LoadImage2((RECT *)object, (u32 *)gFile_TransferBufferBase);
        object->alternate_sector_destination = (s32)gDisplay_abResourceBank0;
        object->sector_destination = (s32)gDisplay_abResourceBank0;
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->transfer_mode = 1;
        break;
    }
}

/* The package's first sector in WA.MRG. A regional build defines its own. */
#ifndef GAME_OVER_PACKAGE_START_SECTOR
#define GAME_OVER_PACKAGE_START_SECTOR 0x2157
#endif

void File_RequestGameOverPackage(void){File_RequestAsyncTransfer(0,0,GAME_OVER_PACKAGE_START_SECTOR,0x32,GameOver_LoadPackageStage,0,0);File_WaitForTransfers();}
