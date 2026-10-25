#ifndef MEMORIES_DECOMP_SCRIPT_IMAGE_OBJECTS_H
#define MEMORIES_DECOMP_SCRIPT_IMAGE_OBJECTS_H

#include "../types.h"
#include "../ygo_types.h"

typedef struct {
    ScriptImageEntry entries[3];
    s16 image_id;
} ScriptImageObjectSet;

typedef char ScriptImageObjectSet_image_id_offset_must_be_0x3C[
    (u32)&((ScriptImageObjectSet *)0)->image_id == 0x3C ? 1 : -1
];

/* Three of the scene script's image slots at gSceneScript_Slots, as
 * ScriptImage_ReleaseObjects
 * releases them: a display-object pointer and callback-control halfword.
 * This is not the whole slot table -- the same function writes the mode
 * halfword in ScriptImageObjectSet after these three entries -- and the
 * callers hold their own wider views under their own names. The shared
 * ScriptImageEntry layout is defined in ygo_types.h. */
/* Transfer callback for one script image. The loader supplies its
 * FileTransferDescriptor and advances the callback stage through the setup
 * phases. */
void ScriptImage_TransferCallback(FileTransferDescriptor *transfer, s32 mode);

/* Starts the async read of one script image. `value` is packed BCD: the low
 * two nibbles are the index, and the third selects one of three tables with
 * its own sector stride (0x21, 0x51 or 0x71) -- any other mode returns without
 * requesting anything. `owner` may be null; when it is not, the id is recorded
 * in image_id first. Localized builds remap ids 0x10 and 0x11 before recording
 * the id and use an ordinary store; other builds preserve the volatile store. */
void ScriptImage_RequestTransfer(
    volatile ScriptImageObjectSet *owner, s32 value);

/* Releases the three image slots, setting image_id to -1 first.
 * Each pointer goes to DisplayObject_ReleaseIfPresent before the pointer
 * and callback-control halfword are zeroed. */
void ScriptImage_ReleaseObjects(ScriptImageObjectSet *set);

/* Builds one display object for a script image and stores it at `owner`, a
 * ScriptImageEntry, with `size` the object's request size and `mode`
 * selecting the blend arms: mode 2 takes GsALON | GsAONE and sets the entry's
 * `callback_control` halfword to 1; anything else takes 0x01000000 and clears
 * it. It also sets field_10. ScriptImage_RebuildObjects is the only
 * caller outside this unit, and called it with no prototype at all before
 * this header. */
#ifdef VERSION_EUROPE
extern void ScriptImage_CreateObject(ScriptImageEntry *entry, s32 size, s32 mode)
    asm("func_8002E10C");
#else
void ScriptImage_CreateObject(ScriptImageEntry *entry, s32 size, s32 mode);
#endif

#endif
