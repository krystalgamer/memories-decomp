#include "../types.h"
#include "model_effect_endpoints.h"

/* Initialized data at 0x80091550: four endpoint presets consumed in pairs by
   func_8005F714. */
ModelEffectEndpoint gModel_aEffectEndpoints[MODEL_EFFECT_ENDPOINT_COUNT] = {
    { 0, 0, 0, 2 },
    { 1, 0, 0, 2 },
    { 0, 0, 0, 3 },
    { 1, 0, 0, 3 },
};
