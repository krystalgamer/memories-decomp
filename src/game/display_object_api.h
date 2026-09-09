#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_API_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_API_H

#include "../types.h"

s32 func_8004002C(void);
s32 func_8004006C(void);
void *func_800400AC(s32 index, s32 key);
void *func_80042B40(s32 value);
void func_8004036C(void *object);
/* Unlinks one pool entry from the list it is on and clears its flags, which is
 * what frees it -- func_8004036C is the null-tolerant wrapper, and
 * func_800402A0 calls it to unlink an entry before relinking it on another
 * list, saving and restoring the flags word around the call for exactly that
 * reason. It also bumps the D_8009B410 unlink counter.
 *
 * This one takes the typed entry rather than the `void *` the rest of this
 * header uses, because it is the function that walks the links: previous, next
 * and field_1E are pool indices it reads and repairs. The struct tag is
 * forward declared so this header stays independent of display_object.h; the
 * two callers each held a different flattened view, `void *` and s32, and cast
 * or pass the pointer they already have. */
struct DisplayObject;
void func_8004020C(struct DisplayObject *slot);
void func_800404CC(
    void *object, s32 x, s32 y, s32 field_67, s32 field_68,
    s32 field_69, s32 color, s32 texture
);

#endif
