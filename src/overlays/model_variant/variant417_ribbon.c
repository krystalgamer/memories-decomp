#include "../../types.h"
#include "variant417_entry.h"

/* MODEL417 builds the MODEL441 ribbon body on its own entry record, whose
 * members sit 0x5AC bytes lower; the MODEL441 entry header is skipped. */
#define MEMORIES_DECOMP_SPANISH_MODEL_VARIANT441_ENTRY_H
#define Variant441EntryState Variant417EntryState
#include "variant441_ribbon.c"
