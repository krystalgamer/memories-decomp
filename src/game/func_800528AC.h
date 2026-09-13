#ifndef MEMORIES_DECOMP_FUNC_800528AC_H
#define MEMORIES_DECOMP_FUNC_800528AC_H

#include "../types.h"

/* Per-frame tint pass over the ten requests at D_800F2B50. Each live request
 * interpolates its start colour towards its end colour by elapsed/duration,
 * drops the result into the model slot's field_DC0, redraws the slot through
 * func_800540B4 with the request's part id pushed into every part record, then
 * restores the colour, the part records and field_BF5 and advances the
 * request's clock. Requests whose clock reaches their duration clear their
 * live bit.
 *
 * It takes no arguments and returns nothing: the request table and the slots
 * it walks are both reached through globals. func_80059CE4 in
 * model_scene_states.c is the only consumer and drives it once per frame,
 * between func_8004E9A0 and the func_800556E8 slot ticks.
 *
 * The address-based name is kept. The defining unit describes the pass in
 * detail, but the same description would fit either the model or the effect
 * subsystem, and nothing observed here settles which one owns it. */
void func_800528AC(void);

#endif
