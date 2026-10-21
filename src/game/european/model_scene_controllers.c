#include "../../types.h"

/* SLES-03947..51 build of src/game/model_scene_controllers.c: the whole unit
 * in one object. The symbols below sit at other addresses in the European
 * executables and their US names are taken there, so they are aliased
 * (config/sles_03947/symbols.txt has the addresses). */
#define VERSION_EUROPE
#define gModel_apfnEffectHandlers D_80011414
#define D_8009AFEC D_8009BF74
#define D_8009AFFC D_8009BF84
#define D_8009B0F4_abs D_8009B0F4
#define D_8009B134_abs D_8009B134
#define D_8009B004 D_8009BF8C
/* As in model_debug_controller's wrapper: the splat names of the European
 * addresses. */
#define func_801807B0 func_80180420
#define func_80181C4C func_80180004
#define func_80180A24 func_8018019C

/* Values that differ in the European release; the US source names each
 * with an #ifndef default. */
#define MODEL_INTRO_PACKAGE_START_SECTOR 0x6E7
#define MODEL_TEXTURE_RESET_ROW_X 0x280

#include "../model_scene_controllers.c"
