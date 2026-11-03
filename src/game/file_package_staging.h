#ifndef GAME_FILE_PACKAGE_STAGING_H
#define GAME_FILE_PACKAGE_STAGING_H

#include "../types.h"

/* Shared staging/upload buffer used by the resident transfer-phase callbacks.
 * Every C consumer treats it as an unsized byte buffer, either publishing its
 * address through a FileTransferDescriptor or passing it to LoadImage2. */
extern u8 gFile_abPackageStagingBuffer[];

/* The palette staged immediately after the model's first image payload. */
extern u8 gModel_abTexturePaletteStaging[];

#endif
