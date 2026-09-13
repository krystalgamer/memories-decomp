#include "../types.h"
#include "view_state.h"
#include "func_80022D94.h"
#include "../unmatched.h"

void func_80022D94(int frames,int x,int z,int y,int value)
{
 s16 oldX=D_800F2848.field_00;int dx=((x-oldX)<<16)/frames;
 s16 oldZ=D_800F2848.field_04;int dz=((z-oldZ)<<16)/frames;
 s16 oldY=D_800F2848.angle;int dy=((y-oldY)<<16)/frames;
 int oldValue=D_800F2848.view.vrz;int dv=((value-oldValue)<<16)/frames;
 D_8009B204=frames;D_8009B1EE=x;D_8009B192=z;D_8009B190=y;
 D_8009B166=value;D_8009B1C4=(oldX<<16)|0x8000;D_8009B15C=(oldZ<<16)|0x8000;
 D_8009B158=(oldY<<16)|0x8000;D_8009B224=(oldValue<<16)|0x8000;
 D_8009B1FC=dx;D_8009B198=dz;D_8009B194=dy;D_8009B168=dv;
}
