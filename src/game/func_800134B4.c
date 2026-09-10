#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libmcrd.h"
#include "../psyq/rand.h"
#include "fade.h"
#include "file_set_position_table.h"
#include "func_800136D4.h"
#include "func_80041340.h"
#include "graphics_constants.h"
#include "graphics_frame.h"
#include "main_frame.h"
#include "input.h"
#include "rand_constants.h"
#include "main_services.h"

/* The resident system layer's reset of the callback registry the per-frame
   service pump walks. It is the last of four contiguous functions that are
   the only run in the region built with gcc_2_8_1_g8_split - their
   neighbours on both sides use other profiles. The pump, func_8001306C, is
   in main_services.c and shares the D_800E9DB0 slots and D_8009B0B8 with this
   reset. The two between them, the boot-time graphics and input start-up
   that installs the pump and the pad-driven screen-offset adjustment loop,
   are now candidates in src/candidates/func_80013154.c and
   src/candidates/func_80013360.c. */

/* Zeroes D_800E9DB0[0..3] and D_8009B0B8. */
void func_800134B4(void) {
    void (**v0)(void);
    int v1;
    v1 = 3;
    v0 = &D_800E9DB0[v1];
    do {
        *v0 = 0;
        v1 -= 1;
        v0 -= 1;
    } while (v1 >= 0);
    D_8009B0B8 = 0;
}
