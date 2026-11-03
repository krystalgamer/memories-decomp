#ifndef MEMORIES_DECOMP_SECONDARY_SCENE_COMMANDS_H
#define MEMORIES_DECOMP_SECONDARY_SCENE_COMMANDS_H

#include "../types.h"

struct DuelEffectChannel;

void Text_NoOpSecondary12(void);
void Text_NoOpSecondary13(void);
void Text_OpenCampaignMap(struct DuelEffectChannel *object);

/* One byte at 0x8009B363, written by four files that share nothing else.
 *
 * frontend_scene_states.c clears it, and duel_effect_sound_commands.c,
 * script_op_open_campaign_map.c and main_run_selection_menus.c each store a value into it;
 * the last also reads it back to pass to func_8016866C.
 *
 * The array spelling is a lever, not a size. Every one of the four is
 * compiled and assembled at -G8, where a byte-sized global would otherwise be
 * reached %gp_rel, and an array is what escapes that -- the table in
 * notes/build.md, second row. All four already write the same incomplete-array
 * form, so this declaration reproduces it exactly and no arm is needed.
 *
 * It is only ever indexed at [0], in all four files, so nothing here claims
 * the object is longer than one byte. c_symbols.ld agrees: gDuel_bTerrain
 * begins at 0x8009B364, immediately after it. */
extern u8 gCampaignMap_bEntryLocation[];

#endif
