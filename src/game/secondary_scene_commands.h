#ifndef MEMORIES_DECOMP_SECONDARY_SCENE_COMMANDS_H
#define MEMORIES_DECOMP_SECONDARY_SCENE_COMMANDS_H

#include "../types.h"

struct DuelEffectChannel;

void Text_NoOpSecondary12(void);
void Text_NoOpSecondary13(void);
void Text_OpenCampaignMap(struct DuelEffectChannel *object);

/* One byte at 0x8009B363, used by four files that share nothing else:
 * three writers and one reader.
 *
 * debug_menu.c clears it, and duel_effect_sound_commands.c and
 * script_op_open_campaign_map.c store the unsigned script operand into it.
 * Main_RunCampaignMap in main_run_selection_menus.c only reads it and passes
 * it to CampaignMap_SetLocation.
 *
 * It is a single byte: c_symbols.ld has gDuel_bTerrain at 0x8009B364,
 * immediately after it. Declared as the byte it is, it is small data at -G8,
 * which is what gives -msplit-addresses users (script_op_open_campaign_map.c,
 * debug_menu.c) retail's one-instruction macro store; the earlier
 * incomplete-array spelling only escaped %gp_rel while the assembler ran at
 * -G8, and forced those two units off the split profile. */
extern u8 gCampaignMap_bEntryLocation;

#endif
