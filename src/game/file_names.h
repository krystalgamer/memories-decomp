#ifndef MEMORIES_DECOMP_FILE_NAMES_H
#define MEMORIES_DECOMP_FILE_NAMES_H

#include "../ygo_types.h"

/* The seven ISO9660 paths the loader opens, and the null-terminated table
   File_SetPositionTable walks. Defined by src/game/file_names.c, which owns
   the initialized data at 0x80090704. */
extern u8 gFile_szDiscWaMrgPath[20];
extern u8 gFile_szDiscSuMrgPath[16];
extern u8 gFile_szDiscModelMrgPath[20];
extern u8 gFile_szDiscMovieStrPath[20];
extern u8 gFile_szDiscSdSeDatPath[20];
extern u8 gFile_szDiscSdBgmDatPath[20];
extern u8 gFile_szDiscMasterXaPath[20];
extern u8 *gFile_apszName[8];

/* Diagnostic format used while publishing each file's sector position. */
extern u8 D_80010038[4] __attribute__((section(".data")));

#endif
