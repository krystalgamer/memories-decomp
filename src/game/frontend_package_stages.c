#include "../types.h"
#include "display_asset_banks.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "duel_effect_resource_setup.h"
#include "../unmatched.h"

#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
#define VERSION_REGIONAL_FRONTEND_PACKAGE_STAGES
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

#if !defined(VERSION_REGIONAL_FRONTEND_PACKAGE_STAGES) || \
    defined(VERSION_JAPAN_OPTIONS_LOAD_PACKAGE_STAGE) || \
    defined(VERSION_EUROPE_OPTIONS_LOAD_PACKAGE_STAGE)
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
        D_8009B0F4 &= 0xFFDDFFFF;
        object->phase_size =
            OPTIONS_PACKAGE_STAGE_MODE_0_SECTORS * FILE_SECTOR_SIZE;
        D_8009B0F4 |= 0x10000;
        object->done = 2;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->value_0C = D_8009B118;
        object->value_08 = D_8009B118;
        object->done = 1;
        break;

    case 2:
        object->x = OPTIONS_PACKAGE_STAGE_MODE_2_X;
        object->y = OPTIONS_PACKAGE_STAGE_MODE_2_Y;
        object->w = 0x100;
        object->h = 4;
        LoadImage2((RECT *)object, (u32 *G32)D_8009B118);
        object->value_0C = (s32)D_801AF000;
        object->value_08 = (s32)D_801AF000;
        object->phase_size = OPTIONS_PACKAGE_STAGE_MODE_2_SECTORS * FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 3:
#if OPTIONS_PACKAGE_STAGE_MODE_3_COMMON_TAIL
        object->phase_size = OPTIONS_PACKAGE_STAGE_MODE_3_SECTORS * FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->value_0C = (s32)OPTIONS_PACKAGE_STAGE_MODE_3_DESTINATION;
        object->value_08 = (s32)OPTIONS_PACKAGE_STAGE_MODE_3_DESTINATION;
#else
        object->value_0C = (s32)OPTIONS_PACKAGE_STAGE_MODE_3_DESTINATION;
        object->value_08 = (s32)OPTIONS_PACKAGE_STAGE_MODE_3_DESTINATION;
        object->phase_size = OPTIONS_PACKAGE_STAGE_MODE_3_SECTORS * FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
#endif
        object->done = 1;
        break;
    }
}
#endif

#ifndef OPTIONS_PACKAGE_START_SECTOR
#define OPTIONS_PACKAGE_START_SECTOR 0x2115
#endif

#ifndef OPTIONS_PACKAGE_SECTOR_COUNT
#define OPTIONS_PACKAGE_SECTOR_COUNT 0x32
#endif

#ifndef OPTIONS_PACKAGE_LOAD_SECOND_BLOCK
#define OPTIONS_PACKAGE_LOAD_SECOND_BLOCK 1
#endif

#if !defined(VERSION_REGIONAL_FRONTEND_PACKAGE_STAGES) || \
    defined(VERSION_JAPAN_FILE_REQUEST_OPTIONS_PACKAGE) || \
    defined(VERSION_EUROPE_FILE_REQUEST_OPTIONS_PACKAGE)
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

#endif

#if !defined(VERSION_REGIONAL_FRONTEND_PACKAGE_STAGES) || \
    defined(VERSION_JAPAN_GAME_OVER_LOAD_PACKAGE_STAGE) || \
    defined(VERSION_EUROPE_GAME_OVER_LOAD_PACKAGE_STAGE)
void GameOver_LoadPackageStage(FileTransferDescriptor *object, s32 mode)
{
    switch (mode) {
    case 0:
        object->field_30.h.field_32 = 0x100;
        object->field_30.h.counter = 0;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4 &= 0xFFDDFFFF;
        object->phase_size = 48 * FILE_SECTOR_SIZE;
        D_8009B0F4 |= 0x10000;
        object->done = 2;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->value_0C = D_8009B118;
        object->value_08 = D_8009B118;
        object->done = 1;
        break;

    case 2:
        object->x = GAME_OVER_PACKAGE_IMAGE_X;
        object->y = GAME_OVER_PACKAGE_IMAGE_Y;
        object->w = 0x100;
        object->h = 4;
        LoadImage2((RECT *)object, (u32 *G32)D_8009B118);
        object->value_0C = (s32)D_801AF000;
        object->value_08 = (s32)D_801AF000;
        object->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->done = 1;
        break;
    }
}
#endif
