#include "../../types.h"

#define VERSION_JAPAN
#define VERSION_JAPAN_MEM_CARD_DIALOG_UPDATE
#define D_800EB0F8 gJapanese_D_800EB0F8
#define D_8009B3EE D_8009B2DE
#define gMemCard_pDialogObject D_8009B2C8
#define MemCardDialog_StepSlide func_8003E994
#define MemCardSync func_8008B8D8
#define D_80090F9C D_80090E4C
#define D_8009B3F0 D_8009B2E0
#define D_8009B3F4 D_8009B2E4
#define D_8009B3BC D_8009B2AC
#define MEM_CARD_DIALOG_CHANNEL(index) \
    ((DuelEffectChannel *)((u8 *)D_800EB0F8 + (index) * 0x60))
/* Unidentified card-library calls: two where the US build calls MemCardStart
   and two where it calls MemCardStop. */
void func_8008A44C(void);
void func_8008D680(void);
void func_8008D6F0(void);
void func_8008A4BC(void);
#define MEM_CARD_DIALOG_START_IO() (func_8008A44C(), func_8008D680())
#define MEM_CARD_DIALOG_STOP_IO() (func_8008D6F0(), func_8008A4BC())
#include "../mem_card_dialog_runtime.c"
