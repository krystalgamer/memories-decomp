#include "../types.h"
#include "scene_image_overlay_tables.h"

/* Initialized data at 0x80090BA8 and 0x80090C00, read only by
   ScriptImage_RebuildObjects
   when it rebuilds a scene-script image record's display objects. Both were
   part of a generated blob until now (#2602).

   D_80090BA8 is indexed b[k * 2] and b[k * 2 + 1] for ids from 0x100 up: the
   first byte is the overlay-present mask and the second is the value written
   to the record at +0x2C when bit 1 is set.

   D_80090C00 is indexed &D_80090C00[n * 6] for ids from 0x200 up. Its six
   bytes are the same mask, then an x and y for each of the two overlays, then
   the +0x2C value. Bit 7 of the mask also puts the second overlay in screen
   space.

   Eighty is not a multiple of six, so the last two bytes are not a row. They
   are retail padding before the script command table that follows. */
u8 D_80090BA8[] = {
    3, 1, 3, 1, 3, 1, 1, 0,
    1, 0, 1, 0, 1, 0, 1, 0,
    1, 0, 1, 0, 1, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 1, 0,
    1, 0, 1, 0, 1, 0, 1, 0,
    1, 0, 0, 0, 1, 0, 0, 0,
    1, 0, 2, 1, 1, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    3, 1, 1, 0, 2, 1, 1, 0,
    1, 0, 1, 0, 3, 0, 3, 1,
    1, 0, 1, 0, 0, 0, 0, 0,
};

u8 D_80090C00[] = {
    130, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 1,
    3, 0, 96, 192, 0, 0,
    3, 0, 0, 0, 0, 1,
    2, 0, 0, 0, 0, 1,
    3, 0, 0, 0, 0, 1,
    3, 0, 0, 0, 0, 1,
    3, 0, 0, 0, 0, 1,
    0, 0, /* Retail padding before the script command table. */
};
