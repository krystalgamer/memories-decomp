#include "../types.h"
#include "duel_init_model_scene.h"
#include "model_slot_setup.h"
#include "model_scene_setup.h"

#define HIGH_MEMORY_ADDRESSES_MODEL_PREFIX
#include "high_memory_addresses.h"

void Duel_InitModelScene(void)
{
    func_800530C4();
    func_800533D8();
    func_80056250(2, D_80010000[0].payload_bases[0], 0x63000, 4);
}
