#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_RENDER_TABLE_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_RENDER_TABLE_H

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object_layout.h"

/* One renderer per display list. func_80041340 walks the lists in descending
 * order and calls the entry whose list head in D_800EFE38 is non-negative. */
extern void (*D_80090FB0[DISPLAY_OBJECT_LIST_COUNT])(void);

/* Two flat lights: one shining down at level 0x60 and one shining up at
 * 0x30. func_80041340 installs the first as light 0 and the second as both
 * light 1 and light 2. */
extern GsF_LIGHT D_80090FCC[];
extern GsF_LIGHT D_80090FDC[];

#endif
