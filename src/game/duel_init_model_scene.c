#include "../types.h"
#include "duel_init_model_scene.h"
#include "model_slot_control.h"
#include "model_scene_controllers.h"

#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#include "shared_work_base.h"

#ifndef DUEL_MODEL_SCENE_BUFFER_SIZE
#define DUEL_MODEL_SCENE_BUFFER_SIZE 0x63000
#endif

void Duel_InitModelScene(void)
{
    Model_ResetSceneView();
    Model_ResetSceneState();
    Model_LoadSlot(
        2,
        gRuntime_pSharedWorkBase,
        DUEL_MODEL_SCENE_BUFFER_SIZE,
        4
    );
}
