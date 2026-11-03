#ifndef GAME_FILE_STAGING_BUFFER_H
#define GAME_FILE_STAGING_BUFFER_H

#include "../types.h"

/* The shared package staging buffer at 0x801A8000 (JP 0x801B8000), as raw
 * bytes. File-transfer stages store its address into transfer descriptors,
 * and the script engine reads the loaded script package from it. Other
 * users cast it to the record type they load there (see card_list_rows.h). */
extern u8 gFile_StagingBuffer[];

#endif
