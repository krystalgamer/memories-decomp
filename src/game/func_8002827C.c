#include "../types.h"
#include "card_constants.h"

extern int gDuel_adwCardStats[];
int func_8002827C(unsigned char*object){if(*(unsigned short*)(object+0x16)&0x200){int*table=gDuel_adwCardStats;int index=*(short*)(object+0xC)-1;return(table[index]>>CARD_STAT_GUARDIAN_STAR_2_SHIFT)&CARD_STAT_GUARDIAN_STAR_MASK;}else{int*table=gDuel_adwCardStats;int index=*(short*)(object+0xC)-1;return(table[index]>>CARD_STAT_GUARDIAN_STAR_1_SHIFT)&CARD_STAT_GUARDIAN_STAR_MASK;}}
