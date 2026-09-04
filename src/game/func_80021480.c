#include "../types.h"

typedef struct{char p[8];u16 flags;}Child;typedef struct{Child*first;Child*children[10];char p2C[8];u8 field34;}Container;
extern Container*D_8009B1E8;extern void func_80040410(Child*,int);extern void*TextBox_Create();extern void func_80039A14(void*);
void func_80021480(int mode)
{
 int i;Child*child;void*object;func_80040410(D_8009B1E8->first,mode);
 if(mode==0){Container*c=D_8009B1E8;for(i=0;i<10;i++){child=c->children[i];if(!child)break;child->flags|=0x40;}}
 else{Container*c=D_8009B1E8;for(i=0;i<10;i++){child=c->children[i];if(!child)break;child->flags&=~0x40;}}
 object=TextBox_Create(0,*(u8*)((char*)D_8009B1E8+mode+0x34),0x1A,0x28,0x120,0x120);func_80039A14(object);
}
