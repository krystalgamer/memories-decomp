#include "../../types.h"

/* SLES-03947 build of src/game/func_80052D2C.c: the symbols below sit at other addresses in the
 * European executable and their US names are taken there, so they are aliased
 * (config/sles_03947/symbols.txt has the addresses). The US source is included
 * as is. */
#define D_8009AF94 gEuropean_D_8009AF94
/* As in model_debug_controller's wrapper: the splat names of the European
 * addresses. */
#define D_8009B488 D_8009C3F8
#define D_8009B48E D_8009C3FE
#define D_8009B490 D_8009C400

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define MODEL_TEXTURE_RESET_ROW_X 0x280

#include "../func_80052D2C.c"
