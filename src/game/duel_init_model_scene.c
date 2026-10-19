#include "../types.h"
#include "duel_init_model_scene.h"
#include "model_slot_setup.h"
#include "model_scene_controllers.h"

#define HIGH_MEMORY_ADDRESSES_MODEL_PREFIX
#include "high_memory_addresses.h"

#ifndef DUEL_MODEL_SCENE_BUFFER_SIZE
#define DUEL_MODEL_SCENE_BUFFER_SIZE 0x63000
#endif

void Duel_InitModelScene(void)
{
    Model_ResetSceneView();
    Model_ResetSceneState();
    Model_LoadSlot(
        2,
        D_80010000[0].payload_bases[0],
        DUEL_MODEL_SCENE_BUFFER_SIZE,
        4
    );
}
