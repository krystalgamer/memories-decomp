#include "../../types.h"
/* SLPM-86398 build of src/game/model_slot_control.c: the symbols below sit at
 * other addresses in the Japanese executable and their US names are taken
 * there, so they are aliased (config/slpm_86398/symbols.txt has the
 * addresses). The US source is included unchanged. */
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009AFA0 gJapanese_D_8009AFA0
#define D_8009AF94 gJapanese_D_8009AF94
#define Model_AdvanceSlotLoad func_80059688
#define func_8005F198 func_8004EBE8
/* Drops the control step's FntPrint trace. */
#define MODEL_CONTROL_TRACE(message)
/* Drops the load-step timing printf. */
#define VERSION_JAPAN
#include "../model_slot_control.c"
