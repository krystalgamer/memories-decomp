#include "../../types.h"
#include "../../ygo_types.h"
#include "module_state.h"

PasswordModuleState gPassword_ModuleState
    __attribute__((section(".data"))) = {0};
