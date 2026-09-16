#include "../types.h"
#include "func_80016784.h"
#include "duel_draw_card_output_position.h"

void Duel_DrawCardAtOutputPosition(DisplayObjectPosition *object, s32 arg1)
{
    func_80016784((DisplayObject *)object, arg1, object->out_x, object->out_y);
}
