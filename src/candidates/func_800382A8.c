/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 4 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/duel_effect_command.c.
 */
#include "../types.h"
#include "../game/card_constants.h"
#include "../game/func_80036C14.h"
#include "../game/duel_card.h"
#include "../game/text_constants.h"
#include "../game/duel_effect.h"
#include "../game/text_encode_decimal_digits.h"
#include "../game/func_80036D70.h"
#include "../game/func_80036D3C.h"
#include "../game/duel_effect_command.h"

/* Entries 0 through 12 of the secondary text-command table D_80090EAC,
   the handlers the F8 escape reaches, together with func_80038024, the
   helper two of them share. Each takes the text channel and reads its
   operands from the channel's live stream. Entry 13, Text_StartCampaignDuel,
   is next in both the table and the image, but it only builds at
   gcc_2_8_1_g0 and stays its own unit.

   The eight former sources were recorded at gcc_2_8_1_g8_split,
   gcc_2_8_1_g8, gcc_2_8_1_g0 and gcc_2_8_1_g0_split, and every member
   compiles to an identical object at gcc_2_8_1_g8_split. Bounded below by
   the primary handlers Text_ExtendGlyphCode and Text_SetStateFromStream in
   text_stream_commands.c. */

void func_800382A8(u8*o){register u8*obj asm("$5")=o;register u8**p asm("$3");register u8*q asm("$2");register unsigned v asm("$4");*(u16*)(obj+0x34)&=0xfeff;p=&((u8**)obj)[*(s8*)(obj+0x58)];q=*p;v=*q++;*p=q;switch(v){case 1:obj[0x5a]=8;obj[0x5b]=8;break;case 2:obj[0x5a]=8;obj[0x5b]=12;break;}if(v==1)*(u16*)(obj+0x34)|=0x100;}

