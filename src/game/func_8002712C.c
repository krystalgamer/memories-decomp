#include "../types.h"
#include "../psyq/rand.h"
#include "card_constants.h"
#include "duel_card.h"

typedef struct{void*p[6];s8 value;}Entry;typedef struct{s8 result,field1;char p2[4];s8 value,zero,random;}State;
extern State D_800EAE88;extern int Duel_CollectFieldCardsByType(Entry**,int,int),func_80026C0C(int);
int func_8002712C(void)
{
 Entry*list[6];int count=Duel_CollectFieldCardsByType(list,0,CARD_TYPE_MAGIC);
 if(count){int first=list[0]->value;int index;D_800EAE88.result=first%DUEL_FIELD_ROW_SIZE+11;D_800EAE88.field1=0;D_800EAE88.zero=0;D_800EAE88.random=0;D_800EAE88.value=6;
  index=func_80026C0C(DUEL_FIELD_SIDE_ZONE_COUNT);if(index>=0){D_800EAE88.random=rand()&1;D_800EAE88.value+=index%DUEL_FIELD_ROW_SIZE;}return 0;}return 1;
}
