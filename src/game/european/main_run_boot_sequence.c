#include "../../types.h"

/* SLES-03947 build of src/game/main_run_boot_sequence.c, with its European bodies. */

#define VERSION_EUROPE
#define D_8009C02B_IN_DATA
/* The European display height. */
#define GRAPHICS_DEFAULT_HEIGHT 256
/* As in main_init.c. */
#define Main_RunBootSequence func_80043C3C
#define func_800434F4 Main_LoadBootImageStage
/* This build names the loader words directly. */
#define D_8009B0F4_abs D_8009B0F4
#define D_8009B134_abs D_8009B134

#include "../main_run_boot_sequence.c"
