#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "display_object_api.h"

#include "display_object_helpers.h"
#include "func_8002A9C0.h"

DisplayObject*func_8002ABB4(DisplayObject*src,int add){DisplayObject*o=func_800400AC(func_8004002C(),6);o->field_48.word=src->field_48.word;*(s32*)&o->field_18=*(s32*)&src->field_18;o->field_60=0x100;*(s16*)&o->field_3C.h.field_3C=*(s16*)&o->field_18*2;*(s16*)&o->field_3C.h.field_3E=*(s16*)&o->field_1A*2;o->field_30.word=src->field_30.word;o->field_20.word=src->field_20.word;o->field_44.word=src->field_44.word;o->field_0C=0;func_80042918(o);func_800428EC((u8*)o,(signed char)(*(u8*)&src->field_16+add));o->field_10=1;o->field_4C=(s32)func_8002A9C0;o->attribute=(o->attribute|(GsALON | GsAONE))&~GsROTOFF;return o;}
