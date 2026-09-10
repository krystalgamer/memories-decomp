#ifndef MEMORIES_DECOMP_WIDGET_UPDATE_PULSE_COLOUR_H
#define MEMORIES_DECOMP_WIDGET_UPDATE_PULSE_COLOUR_H

#include "../types.h"

/* Drives a widget's pulsing highlight from the frame counter. It folds
 * gMain_dwVBlankTick into a triangle wave, gates each colour channel on the
 * bytes at +0x0C, +0x0D and +0x0E, and writes the result into the six colour
 * words at 0x2C, 0x34, 0x3C, 0x44, 0x4C and 0x54 -- the stride-8 run
 * display_object.h documents -- with the four dimmed to a quarter.
 *
 * dialog_update_choice.c is the only consumer and already spelled it this
 * way. The parameter is u8 * because the function reaches those offsets as
 * bytes rather than through a named record. */
void Widget_UpdatePulseColour(u8 *arg0);

#endif
