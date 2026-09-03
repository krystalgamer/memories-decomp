#include "../types.h"

typedef struct{void*p[6];s8 value;}Entry;typedef struct{s8 result,field1;char p2[4];s8 value,zero,random;}State;
extern State D_800EAE88;extern int func_80026D18(Entry**,int,int),func_80026C0C(int),rand(void);
int func_8002712C(void)
{
 Entry*list[6];int count=func_80026D18(list,0,20);
 if(count){int first=list[0]->value;int index;D_800EAE88.result=first%5+11;D_800EAE88.field1=0;D_800EAE88.zero=0;D_800EAE88.random=0;D_800EAE88.value=6;
  index=func_80026C0C(10);if(index>=0){D_800EAE88.random=rand()&1;D_800EAE88.value+=index%5;}return 0;}return 1;
}
