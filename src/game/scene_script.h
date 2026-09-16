#ifndef MEMORIES_DECOMP_SCENE_SCRIPT_H
#define MEMORIES_DECOMP_SCENE_SCRIPT_H

#include "../types.h"
#include "../ygo_types.h"

/* The scene script's slot table at D_800EAE98.
 *
 * Campaign_LoadScenePackage initializes the fixed scene presentation package
 * and clears
 * this table by subscripting it; Script_OpLoadImageScene is a script command
 * that runs ScriptImage_ReleaseObjects over it before reading its next operand
 * from the script
 * stream. Main_RunCampaign is one of the consumers.
 *
 * Five records of 0x14 bytes, and the shape is measured rather than assumed.
 * Two translation units declare the table as a pointer array and write
 * indices 0, 5 and 10, which are byte offsets 0, 0x14 and 0x28 -- the first
 * word of records 0, 1 and 2. A third declares it u8[100], and 100 is
 * 5 * 0x14. Our matching tree reads exactly those two words at +0x14 and
 * +0x28 in byte-exact functions.
 *
 * unk00 holds a display object: Script_OpDuelResult creates one and stores it into
 * records 0, 1 and 2, and func_8002FED8 reads records 0 and 1 back. It is
 * left s32 here on purpose -- typing it is a second claim on top of the
 * layout, and the struct those files call `Object` is itself a private view.
 *
 * script_op_duel_result.c takes this declaration too. Its three stores are
 * the first word of slots 0, 1 and 2, written D_800EAE98[i].unk00; that
 * spelling builds the unit's .text, relocations and data sections identical
 * to the pointer-array view it used to declare privately. (A cast of the
 * table to `Object **` indexed [0], [5] and [10] was the spelling that
 * differed from byte 0x8002F780.) SceneScriptSlot's measured layout is
 * defined once in ygo_types.h.
 */
#define SCENE_SCRIPT_SLOT_SIZE 0x14
#define SCENE_SCRIPT_SLOT_COUNT 5

/* Unsized: a declared size is a -G input for this toolchain, and no consumer
 * needs the bound. The count is above, with what it was derived from.
 *
 * Open question, not resolved by fiat: our matching tree reads a byte at
 * +0x64, which is record 5 -- one past the five the arithmetic here gives --
 * and there is no symbol at 0x800EAEFC for it to belong to instead. */
extern SceneScriptSlot D_800EAE98[];

/* The per-tick sweep over the first three slots. It dispatches each live
 * slot through D_80090CAC using the low byte at +4 as the callback index. */
void SceneScript_UpdateSlots(SceneScriptSlot *records);

#endif
