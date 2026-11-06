#include "../../types.h"
#define D_8009B1D5_IS_ABSOLUTE_SCALAR
#include "../view_state.h"
#include "../duel_effect_request.h"
#define DUEL_EFFECT_OBJECT_POOL_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C1D8-0x8009C1EA (#2602); its
 * layout differs from the North American block the shared source defines. */
u8 gDuel_bEffectRequestStatus;
DuelEffectRequest *gDuelEffect_pActiveRequest;
u8 gDuelEffect_bRequestFinished;
static PADDING_VARIABLE(1);
static PADDING_VARIABLE(2);
static PADDING_VARIABLE(4);
static PADDING_VARIABLE(2);

/* The European builds compile the shared unit through this wrapper so that
 * they can give it their own zero-initialized storage (#2602). */
#include "../duel_effect_object_pool.c"
