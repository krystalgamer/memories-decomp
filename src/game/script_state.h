#ifndef MEMORIES_DECOMP_SCRIPT_STATE_H
#define MEMORIES_DECOMP_SCRIPT_STATE_H

#include "../types.h"
#include "file_staging_buffer.h"

/* The script engine's current command word.
 *
 * script_command_runner.c fetches it straight from the script stream
 * (gScript_wCommandWord = *gScript_StreamCursor++) and dispatches on its low five bits
 * (gScript_apfnCommandHandlers[gScript_wCommandWord & 0x1F]()), so the low bits carry the opcode and
 * the upper bits carry engine flags: bit 15 is the one-shot busy latch
 * documented in script_command_busy.h, and handlers set and clear 0x4000,
 * 0x2000, 0x1000, 0x800, 0x400, 0x200 and 0x80 around it.
 * Text_StartViewportTween's signed store is a cast at the use. */
extern u16 gScript_wCommandWord;

/* The script instruction cursor: a byte pointer walking the script stream.
 *
 * script_readers.c is the clearest statement of the encoding:
 * Script_ReadByte returns *gScript_StreamCursor++ for a single-byte operand;
 * Script_ReadU16LE returns current[0] | (current[1] << 8) after advancing two.
 * Operands are little-endian and the cursor is byte-granular. Command handlers
 * advance it by two, four or six according to how many operands they take, and
 * script_command_runner.c fetches the next opcode through it into gScript_wCommandWord.
 */
extern u8 *gScript_StreamCursor;

/* The loaded script package, as the script engine reads it: a byte base the
 * cursor above is set from.
 *
 * script_command_runner.c states the layout in one line --
 * `gScript_StreamCursor = gFile_StagingBuffer + *(u16 *)(gFile_StagingBuffer + g * 2)` -- so the head of
 * the package is a table of little-endian u16 byte offsets indexed by script
 * number, and the entry is where that script's code starts. The other three
 * namers jump within the same package: Script_OpStoryFlag,
 * Script_OpJump and Script_OpJumpIfDeckIncomplete all set the cursor to
 * `gFile_StagingBuffer + offset`. They use the byte view in
 * file_staging_buffer.h; card_list_rows.h explains the other subsystems'
 * record views of the same buffer. */

/* The viewport tween target, X then Y, as two consecutive halfwords.
 *
 * Script_OpStageImage and Script_OpViewportTween read them out of the script
 * stream as little-endian operands (cursor[0] | (cursor[1] << 8));
 * Script_OpShowImage copies them straight into gGraphics_sViewportX/Y,
 * and Script_UpdateViewportTween interpolates the viewport towards them,
 * reading them signed for the delta (`*(s16 *)&gScript_ViewportTargetX`, retail lh)
 * and unsigned for the final copy (retail lhu). Retail reaches them
 * gp-relative in all four of those units. */
/* Shared retained halfword: viewport commands use it as their divisor/countdown;
 * Script_OpDuelResult also stages its two-byte operand here. It is not
 * universally a duration. Text_StartViewportTween uses TextStream_ReadU16LE.
 *
 * Consumers read it unsigned except where a site takes it as
 * `*(s16 *)&gScript_SharedOperand` to read the signed divisor; that cast
 * stays at the use, so the declaration is the plain u16. */
extern u16 gScript_SharedOperand;
extern u16 gScript_ViewportTargetX;
extern u16 gScript_ViewportTargetY;

/* Script_UpdateViewportTween's 16.16 X/Y accumulators and per-invocation
 * deltas. The intervening command latch and stream pointer are independent
 * symbols, not padding in a fabricated contiguous tween object. */
extern s32 gScript_ViewportAccumulatorX;
extern s32 gScript_ViewportAccumulatorY;
extern s32 gScript_ViewportDeltaX;
extern s32 gScript_ViewportDeltaY;

/* Signed halfword countdown, decremented even on the initialization call.
 * Keep narrowing/wrap and the existing signed completion test. */
extern s16 gScript_WaitCounter;

/* Script_OpShowImage and Script_OpDuelResult retain an allocated display object
 * here until their later command phase releases it with DisplayObject_ReleaseIfPresent.
 * The stored pointer is not nulled by those releases. */
struct DisplayObject;
extern struct DisplayObject *gScript_TransitionObject;

#define SCRIPT_IMAGE_OPERAND_ID_MASK 0xFFF
#define SCRIPT_IMAGE_OPERAND_SKIP_FADE_IN 0x4000
#define SCRIPT_IMAGE_OPERAND_HAS_VIEWPORT 0x8000

/* The show-image command's halfword operand. Script_OpLoadImageScene and
 * Script_OpStageImage
 * read it from the stream as `cursor[0] | (cursor[1] << 8)`: the low twelve
 * bits are the image index, but the complete word and high flags remain.
 * 0x8000 says a viewport X/Y pair follows (Script_OpLoadImageScene masks
 * only on that optional-coordinate branch and
 * reads the pair into gGraphics_sViewportX/Y; Script_OpStageImage into
 * gScript_ViewportTargetX/Y), and Script_OpShowImage tests 0x4000.
 * Retail is sh/lhu gp-relative at all three, so the plain u16 they already wrote.
 *
 * The text callbacks store TextStream_ReadU16LE's result through $at. Their optional viewport
 * form consumes one byte then one halfword, not two halfwords. */
extern u16 gScript_ImageOperand;

/* Two halfwords the event driver keeps at 0x8009B2A4 and 0x8009B2A6.
 *
 * gScript_EventState is the event selector/state word: Campaign_LoadScenePackage seeds it from
 * its argument, Script_RunTick clears DUEL_EVENT_SCRIPT_FLAG_DIALOG_ACTIVE
 * and ORs in DUEL_EVENT_SCRIPT_FLAG_STARTED (duel_effect.h), and
 * Script_OpShowDialog sets the dialog bit. Before the started bit is set,
 * the original whole word indexes the package table; it is not newly masked.
 * The save-prompt context at
 * gScript_SavePromptSceneOperand is declared by script_op_save_prompt.h, its actual consumer.
 * Retail reaches both gp-relative; historical listings spell the latter
 * %gp_rel(gScript_EventState + 0x2) in the listings (Script_OpSavePrompt.s:27, 324,
 * 350, 361) because nothing references its address directly, so its
 * original address-qualified name recorded a role, not a new allocation.
 * Whether it is its own object or the flag word's second element is not
 * established; the two names record the two roles. */
extern u16 gScript_EventState;

#endif
