#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_H

#include "../types.h"
#include "display_object_layout.h"

typedef void (*DisplayObjectCallback)(u8 *);

/* One entry of the DISPLAY_OBJECT_POOL_CAPACITY-entry pool at D_800EFE48.
 *
 * previous and next are pool indices, not pointers: func_800402A0 unlinks an
 * entry by writing -1 to both and then repairs the neighbour's link through
 * D_800EFE48[v].previous, and func_80040814 walks the list through next.
 */
typedef struct DisplayObject {
    s16 previous;
    s16 next;
    u8 pad_04[4];
    u16 flags;
    u8 field_0A[0x1A];
    DisplayObjectCallback update;
    u8 pad_28[DISPLAY_OBJECT_RECORD_SIZE - 0x28];
} DisplayObject;

typedef char DisplayObject_size_must_match_record_size[
    sizeof(DisplayObject) == DISPLAY_OBJECT_RECORD_SIZE ? 1 : -1
];

extern DisplayObject D_800EFE48[DISPLAY_OBJECT_POOL_CAPACITY];

#endif
