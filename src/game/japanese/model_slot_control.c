#include "../../types.h"
/* SLPM-86398 build of src/game/model_slot_control.c: the symbols below sit at
 * other addresses in the Japanese executable and their US names are taken
 * there, so they are aliased (config/slpm_86398/symbols.txt has the
 * addresses). The US source is included unchanged. */
#define D_8009B0F4_abs gJapanese_FileTransferFlags
/* Drops the control step's FntPrint trace. */
#define MODEL_CONTROL_TRACE(message)
/* Drops the load-step timing printf. */
#define VERSION_JAPAN
#include "../model_slot_control.c"
