#include "../types.h"
#include "file_names.h"

/* Initialized data at 0x80090704. The paths are padded to a word boundary
   the way the compiler pads them, and gFile_apszName is the null-terminated
   table File_SetPositionTable reads. */
u8 gFile_szDiscWaMrgPath[20] = "\\DATA\\WA_MRG.MRG;1";
u8 gFile_szDiscSuMrgPath[16] = "\\DATA\\SU.MRG;1";
u8 gFile_szDiscModelMrgPath[20] = "\\DATA\\MODEL.MRG;1";
u8 gFile_szDiscMovieStrPath[20] = "\\DATA\\MOVIE.STR;1";
u8 gFile_szDiscSdSeDatPath[20] = "\\DATA\\SD_SE.DAT;1";
u8 gFile_szDiscSdBgmDatPath[20] = "\\DATA\\SD_BGM.DAT;1";
u8 gFile_szDiscMasterXaPath[20] = "\\DATA\\MASTER.XA;1";

u8 *gFile_apszName[8] = {
    gFile_szDiscWaMrgPath,
    gFile_szDiscSuMrgPath,
    gFile_szDiscModelMrgPath,
    gFile_szDiscMovieStrPath,
    gFile_szDiscSdSeDatPath,
    gFile_szDiscSdBgmDatPath,
    gFile_szDiscMasterXaPath,
    (u8 *)0,
};
