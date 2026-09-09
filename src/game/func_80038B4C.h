#ifndef MEMORIES_DECOMP_FUNC_80038B4C_H
#define MEMORIES_DECOMP_FUNC_80038B4C_H

#include "../types.h"

/* D_80090EAC entry, and the dispatcher for the whole table: it reads one byte
 * from the object's current stream, advances the cursor past it, and calls
 * D_80090EAC[byte] with the same object -- so an entry can chain into another,
 * and the table's bound is a property of the script data rather than of any
 * call site. */
void func_80038B4C(u8 *object);

#endif
