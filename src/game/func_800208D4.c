#include "../types.h"
#include "duel_side_state.h"
#include "duel_grid.h"
#include "func_80022D94.h"
#include "../unmatched.h"
#include "func_800208D4.h"

void func_800208D4(void){if(!(D_8009B23A&0x8000)){D_8009B23A|=0x8000;func_80022D94(0x30,0x258,0x100,D_8009B1D5?0x400:0xC00,0);D_8009B162=0x10;}
 if(!D_8009B162){D_8009B1D5^=1;D_8009B23A=2;D_8009B1C8=(DuelSideState *)((u8 *)D_800E9FF0+D_8009B1D5*sizeof(DuelSideState));D_8009B22C=D_800907D8+D_8009B1D5*DUEL_FIELD_SIDE_GRID_SLOT_COUNT;}}
