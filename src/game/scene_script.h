#ifndef MEMORIES_DECOMP_SCENE_SCRIPT_H
#define MEMORIES_DECOMP_SCENE_SCRIPT_H

#include "../types.h"

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
 * script_op_duel_result.c keeps its own `extern Object *D_800EAE98[]` and does not
 * take this declaration. Writing its three stores as
 * ((Object **)D_800EAE98)[0], [5] and [10] is the same arithmetic against
 * the same offsets, and it builds to the right size and differs from byte
 * 0x8002F780, inside that function. Reverting that one file and nothing else
 * restores the match, so the pointer-array declaration is load-bearing there
 * rather than a stray spelling.
 */
typedef struct {
    s32 unk00;
    s16 unk04;
    s16 unk06;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
} SceneScriptSlot;

#define SCENE_SCRIPT_SLOT_SIZE 0x14
#define SCENE_SCRIPT_SLOT_COUNT 5

typedef char SceneScriptSlot_size_must_be_0x14[
    sizeof(SceneScriptSlot) == SCENE_SCRIPT_SLOT_SIZE ? 1 : -1
];

/* Unsized: a declared size is a -G input for this toolchain, and no consumer
 * needs the bound. The count is above, with what it was derived from.
 *
 * Open question, not resolved by fiat: our matching tree reads a byte at
 * +0x64, which is record 5 -- one past the five the arithmetic here gives --
 * and there is no symbol at 0x800EAEFC for it to belong to instead. */
extern SceneScriptSlot D_800EAE98[];

/* The per-tick sweep over the first three slots. It takes the table as bytes
 * rather than as SceneScriptSlot *, strides 0x14 by hand, and dispatches
 * through D_80090CAC on the byte at +4, so the record view above is the
 * layout it walks rather than the type it is written against.
 * Main_RunCampaign is the only caller and already casts the table to u8 *. */
void func_8002FFD4(u8 *records);

#endif
