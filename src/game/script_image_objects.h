#ifndef MEMORIES_DECOMP_SCRIPT_IMAGE_OBJECTS_H
#define MEMORIES_DECOMP_SCRIPT_IMAGE_OBJECTS_H

#include "../types.h"
#include "../ygo_types.h"

/* Three of the scene script's image slots at D_800EAE98, as
 * ScriptImage_ReleaseObjects
 * releases them: a display-object pointer and the image id that filled it.
 * This is not the whole slot table -- the same function writes the mode
 * halfword at +0x3C, past these three entries -- and the callers hold their
 * own wider views under their own names. */
typedef struct {
    void *pointer;
    s16 value;
    u8 pad_06[14];
} ScriptImageEntry;

/* Transfer callback for one script image. The loader supplies its
 * FileTransferDescriptor and advances the callback stage through the setup
 * phases. */
void ScriptImage_TransferCallback(FileTransferDescriptor *transfer, s32 mode);

/* Starts the async read of one script image. `value` is packed BCD: the low
 * two nibbles are the index, and the third selects one of three tables with
 * its own sector stride (0x21, 0x51 or 0x71) -- any other mode returns without
 * requesting anything. `owner` may be null; when it is not, the id is recorded
 * at its +0x3C first. The parameter is volatile in the definition, so that
 * store is not merged with the caller's own writes to the same record. */
void ScriptImage_RequestTransfer(volatile u8 *owner, s32 value);

/* Releases the three image slots: each pointer goes to func_8004036C and both
 * words are zeroed, and the mode halfword at +0x3C is set to -1 first so the
 * next request treats the record as empty. */
void ScriptImage_ReleaseObjects(ScriptImageEntry *entries);

/* Builds one display object for a script image and stores it at `owner`, with
 * `size` the object's request size and `mode` selecting the blend arms: mode 2
 * takes GsALON | GsAONE and sets the record's +4 flag, anything else takes
 * 0x01000000 and clears it. ScriptImage_RebuildObjects is the only caller outside this
 * unit, and called it with no prototype at all before this header. */
void ScriptImage_CreateObject(u8 *owner, s32 size, s32 mode);

#endif
