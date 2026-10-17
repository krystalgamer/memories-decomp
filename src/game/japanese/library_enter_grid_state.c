#include "../../types.h"
#include "../library_runtime.h"

void Library_EnterGridState(LibraryState *state)
{
    state->motion.state = 1;
}
