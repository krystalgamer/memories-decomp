#ifndef MEMORIES_DECOMP_MODEL_BACKGROUND_H
#define MEMORIES_DECOMP_MODEL_BACKGROUND_H

#include "../types.h"
#include "../psyq/libgte.h"

typedef struct {
    SVECTOR values[2];
} BackgroundNormals;

/* Draws from model slot 2 and the selected 0xB2-byte model record.
 * Both the active record and slot coordinate must be valid before the
 * active-byte guard. Texture depth must be 0..2, a visible tile period
 * nonzero, and ordering-table length below 32. See
 * notes/model-background-renderer.md for the measured input contract. */
void func_8004DE24(void);

#endif
