#ifndef MEMORIES_DECOMP_WIDGET_UPDATE_PULSE_COLOUR_H
#define MEMORIES_DECOMP_WIDGET_UPDATE_PULSE_COLOUR_H

#include "../types.h"

struct DisplayObject;

/* Drives a widget's pulsing highlight from the frame counter. It folds
 * D_8009B09C into a triangle wave, gates each colour channel on the enable
 * bytes at +0x0C, +0x0D and +0x0E, and writes the result into the six colour
 * words at 0x2C, 0x34, 0x3C, 0x44, 0x4C and 0x54 -- the stride-8 run
 * display_object.h documents -- with the four dimmed to a quarter.
 *
 * Dialog_UpdateChoice is the only consumer: it installs this as the
 * object's DisplayObjectCallback, casting it to that u8 * hook type. */
void Widget_UpdatePulseColour(struct DisplayObject *object);

#endif
