#include "../types.h"
#include "card_constants.h"
#include "duel_card_layout.h"

extern int gDuel_adwCardStats[];
int func_8002827C(unsigned char*object){if(*(unsigned short*)(object+0x16)&DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2){int*table=gDuel_adwCardStats;int index=*(short*)(object+0xC)-1;return(table[index]>>CARD_STAT_GUARDIAN_STAR_2_SHIFT)&CARD_STAT_GUARDIAN_STAR_MASK;}else{int*table=gDuel_adwCardStats;int index=*(short*)(object+0xC)-1;return(table[index]>>CARD_STAT_GUARDIAN_STAR_1_SHIFT)&CARD_STAT_GUARDIAN_STAR_MASK;}}
