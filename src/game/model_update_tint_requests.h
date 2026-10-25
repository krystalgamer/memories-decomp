#ifndef MEMORIES_DECOMP_MODEL_UPDATE_TINT_REQUESTS_H
#define MEMORIES_DECOMP_MODEL_UPDATE_TINT_REQUESTS_H

#include "../types.h"

/* Per-frame tint pass over the ten requests at gModel_aTintRequests. Each live request
 * interpolates its start colour towards its end colour by elapsed/duration,
 * drops the result into the model slot's tint, redraws the slot through
 * Model_DrawSlot with the request's part id pushed into every part record, then
 * restores the colour, the part records and field_BF5 and advances the
 * request's clock. Requests whose clock reaches their duration clear their
 * live bit.
 *
 * It takes no arguments and returns nothing: the request table and the slots
 * it walks are both reached through globals. Model_UpdateScene in
 * model_scene_states.c is the only consumer and drives it once per frame,
 * between Model_StepBackgroundVramAnimation and the Model_UpdateSlotAnimation slot ticks.
 *
 * The request table is the one Model_QueueTintRequest and
 * Model_QueueTintRequestForParts fill, and every request redraws a model slot
 * through Model_DrawSlot, so the pass is named with the Model_ owner of that
 * queue. */
void Model_UpdateTintRequests(void);

#endif
