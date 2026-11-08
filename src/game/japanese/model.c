#include "../../types.h"
/* SLPM-86398 build of src/game/model.c. VERSION_JAPAN drops the debug
 * display data and traces and selects the Japanese draw and debug paths. */
#define VERSION_JAPAN
/* Drops the control step's FntPrint trace. */
#define MODEL_CONTROL_TRACE(message)
#include "../model.c"
