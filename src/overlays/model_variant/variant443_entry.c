#include "../../types.h"
#include "../../game/func_80058E1C.h"
/* Header 443 is the North American build of the French and Spanish header-460
 * entry: the same source under gcc 2.7.2, one word longer, with the CLUT
 * column at 512 and the helpers and data at their North American addresses. */
#define MODEL_VARIANT460_CLUT_X 512
#define func_8013BCFC func_8013BD00
#define func_8013C7EC func_8013C808
#define D_8013D8CC D_8013D888
#include "../french_model_variant/variant460_entry.c"
