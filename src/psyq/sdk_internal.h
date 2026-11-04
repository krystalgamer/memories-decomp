#ifndef MEMORIES_DECOMP_PSYQ_SDK_INTERNAL_H
#define MEMORIES_DECOMP_PSYQ_SDK_INTERNAL_H

#include "libgs.h"

/* Sixteen bytes of embedded SDK-classified assembly that return
 * gGraphics_apOrderingTables[2], the ordering table DisplayObject_AcquireSlot
 * gives every new object (ot_index 2). Every caller passes the result straight
 * to a GsSort* call as its GsOT argument. */
GsOT *Graphics_GetDefaultOrderingTable(void);

/* Unidentified SDK entry used to install the graphics packet work pointer.
 * Its library identity and return semantics remain unresolved. */
void func_800862C0(void *work);

#endif
