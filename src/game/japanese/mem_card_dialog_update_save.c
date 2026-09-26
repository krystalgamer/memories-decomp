#include "../../types.h"

#define VERSION_JAPAN
#define VERSION_JAPAN_MEM_CARD_DIALOG_UPDATE_SAVE
#define D_8009B3EB gJapanese_MemCardDialogStepState
#define D_8009B3F4 D_8009B2E4
#define D_8009B3EC D_8009B2DC
#define D_8009B3DC gJapanese_MemCardBlockCount
#define D_8009AF70 D_8009AEB8
#define D_800EFE18 gJapanese_MemCardFileNameBuffer
#define D_800EFBC0 D_800ED298
#define gMemCard_pPrimaryTransferCursor gJapanese_PrimaryTransferCursor
#define D_8009B3C4 gJapanese_MemCardTransferOffset
#define D_8009B3C2 gJapanese_MemCardTransferSize
#define gSaveDataSequence gJapanese_SaveDataSequence
#define MemCardAccept func_8008A880
#define MemCardGetDirentry func_8008B668
#define MemCardReadFile func_8008B228
#define MemCardWriteFile func_8008B448
#define MemCardCreateFile func_8008B9F4
#define MemCardFormat func_8008BAF0
#define MemCardDialog_UpdateSave func_8003DDF8

/* Japanese-only library calls that state 2 polls; the US build has none. */
void func_8008D070(long chan);
long func_8008DCB4(long mode, long *cmds, long *result);
void func_8008CF10(long chan, long *apl);

#include "../mem_card_dialog_load_save.c"
