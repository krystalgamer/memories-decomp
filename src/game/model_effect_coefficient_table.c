#include "../types.h"
#include "model.h"

/* Initialized data at 0x80091570: fifteen model-effect coefficient records in
   the order consumed by func_8005F1A4, func_8005F27C and func_8005F5C8. */
ModelEffectCoefficient D_80091570[] = {
    { 700, -512, -256, 0 },
    { 700, 512, -256, 0 },
    { 700, 1536, -256, 0 },
    { 700, 2560, -256, 0 },
    { 700, 0, 0, 0 },
    { 1200, -128, -85, 0 },
    { 1200, 128, -85, 0 },
    { 1200, 1920, -85, 0 },
    { 1200, 2176, -85, 0 },
    { 1800, -256, -256, 0 },
    { 1800, 256, -256, 0 },
    { 1800, 1792, -256, 0 },
    { 1800, 2304, -256, 0 },
    { 700, -512, -512, 0 },
    { 700, 512, -512, 0 },
};
