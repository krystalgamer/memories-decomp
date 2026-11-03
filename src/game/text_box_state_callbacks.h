#ifndef MEMORIES_DECOMP_TEXT_BOX_STATE_CALLBACKS_H
#define MEMORIES_DECOMP_TEXT_BOX_STATE_CALLBACKS_H

#include "../types.h"
#include "../ygo_types.h"

extern TextBoxStateCallback gTextBox_apfnStateCallbacks[];

/* Expand through the existing Japanese native-name adapter as well. */
#define gTextBox_apfnStateCallbacks gTextBox_apfnStateCallbacks

#endif
