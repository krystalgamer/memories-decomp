#include "../../types.h"
#include "../../game/model_get_frame_step.h"
/* Header 443 is the North American build of the French and Spanish header-460
 * entry: the same source under gcc 2.7.2, one word longer, with the CLUT
 * column at 512 and the helpers and data at their North American addresses. */
#define MODEL_VARIANT460_CLUT_X 512
#include "../french_model_variant/variant460_entry.c"
