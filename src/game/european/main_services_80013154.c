#include "../../types.h"

/* SLES-03947 build of src/game/main_services.c: only the functions enabled below. */

#define VERSION_EUROPE
#define VERSION_EUROPE_FUNC_80013154

/* The European display height. */
#define GRAPHICS_DEFAULT_HEIGHT 256

/* The display environment and GsDefDispBuff, under the names #6384 gave
 * their European addresses. */
#define D_800FE0A8 gEuropean_GsDISPENV
#define GsDefDispBuff func_80085424

#include "../main_services.c"
