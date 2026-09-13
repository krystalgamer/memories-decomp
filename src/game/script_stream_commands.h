#ifndef MEMORIES_DECOMP_SCRIPT_STREAM_COMMANDS_H
#define MEMORIES_DECOMP_SCRIPT_STREAM_COMMANDS_H

#include "../types.h"

/* Three D_80090C50 command handlers that read their operands out of the script
 * stream at D_8009B290 and leave it advanced past them.
 *
 * Script_OpSound is the widest: bits 0-5 of its first byte pick between
 * replaying, latching or copying the current BGM command, a clear low six bits
 * instead take a 16-bit id from the stream and bit 6 sends that id to
 * SD_SEPlayFull rather than SD_BGMPlay, and bit 7 keeps the command busy for
 * another tick instead of clearing D_8009B27C.
 *
 * Script_OpFadeBgm fades the BGM out, with the low seven bits as the step and
 * zero meaning the default, and carries the same bit 7. Script_OpJump is the script
 * jump: it takes a 16-bit offset and repoints D_8009B290 into D_801A8000. */
void Script_OpSound(void);
void Script_OpFadeBgm(void);
void Script_OpJump(void);

#endif
