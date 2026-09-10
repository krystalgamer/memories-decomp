#ifndef MEMORIES_DECOMP_SCRIPT_STATE_H
#define MEMORIES_DECOMP_SCRIPT_STATE_H

#include "../types.h"

/* The script engine's current command word.
 *
 * script_run_tick.c fetches it straight from the script stream
 * (D_8009B27C = *D_8009B290++) and dispatches on its low five bits
 * (D_80090C50[D_8009B27C & 0x1F]()), so the low bits carry the opcode and
 * the upper bits carry engine flags: bit 15 is the one-shot busy latch
 * documented in script_command_busy.h, and handlers set and clear 0x4000,
 * 0x2000, 0x1000, 0x800, 0x400, 0x200 and 0x80 around it.
 *
 * Two translation units deliberately keep their own spelling and must not
 * be switched to this declaration:
 *
 *   duel_effect_interaction_states.c declares it as an array plus a scalar
 *   asm() alias, because, as its own comment records, separate linker names
 *   are what stop GCC retaining the address across a call.
 *
 *   text_box_build_step.c pins it into .data with an attribute, which is
 *   what decides gp-relative versus absolute addressing under -G8.
 */
extern u16 D_8009B27C;

/* The script instruction cursor: a byte pointer walking the script stream.
 *
 * script_readers.c is the clearest statement of the encoding -- one reader
 * returns *D_8009B290++ for a single byte operand, and a second returns
 * current[0] | (current[1] << 8) after advancing two, so operands are
 * little-endian and the cursor is byte-granular. Command handlers advance it
 * by two, four or six according to how many operands they take, and
 * script_run_tick.c fetches the next opcode through it into D_8009B27C.
 */
extern u8 *D_8009B290;

/* The viewport tween target, X then Y, as two consecutive halfwords.
 *
 * func_8002E6B8 and Script_OpViewportTween read them out of the script
 * stream as little-endian operands (cursor[0] | (cursor[1] << 8));
 * Script_OpShowImage copies them straight into gGraphics_sViewportX/Y,
 * and Script_UpdateViewportTween interpolates the viewport towards them,
 * reading them signed for the delta (`*(s16 *)&D_8009B2A8`, retail lh)
 * and unsigned for the final copy (retail lhu). Retail reaches them
 * gp-relative in all four of those units.
 *
 * duel_effect_interaction_states.c keeps its own spelling for the same
 * reason as D_8009B27C above -- an array plus a scalar asm() alias, with
 * its comment on why -- and must not be switched to these. */
/* A sixteen-bit operand the script engine assembles from the stream a byte at
 * a time -- script_flag_commands.c and script_update_viewport_tween.c both
 * build it as `cursor[0] | (cursor[1] << 8)` -- and which func_8002F630.c
 * also sets from func_80036D3C's return.
 *
 * Read unsigned everywhere except one site, which takes it as
 * `*(s16 *)&D_8009B29C` to get a signed value out of the same halfword. That
 * cast stays at the use, so the declaration here is the plain u16 all three
 * consumers already wrote.
 *
 * duel_effect_interaction_states.c is the fourth namer and keeps its own
 * spelling, `s16` with a .data section attribute. That is the same
 * arrangement this header already records for D_8009B27C above: it does not
 * include this header, so the two never meet. */
extern u16 D_8009B29C;

extern u16 D_8009B2A8;
extern u16 D_8009B2AA;

/* The show-image command's halfword operand. func_8002E470 and func_8002E6B8
 * read it from the stream as `cursor[0] | (cursor[1] << 8)`: the low twelve
 * bits are the image index func_8002DF2C is handed, 0x8000 says a viewport
 * X/Y pair follows (func_8002E470 masks the index down to `& 0xFFF` and
 * reads the pair into gGraphics_sViewportX/Y; func_8002E6B8 into
 * D_8009B2A8/D_8009B2AA), and Script_OpShowImage tests 0x4000. Retail is
 * sh/lhu gp-relative at all three, so the plain u16 they already wrote.
 *
 * duel_effect_interaction_states.c is the fourth namer and stores
 * func_80036D3C's result into it through $at; it keeps its own `u16 []`
 * spelling for the same reason this header records for D_8009B27C and
 * D_8009B29C above: it does not include this header. */
extern u16 D_8009B270;

/* Two halfwords the event driver keeps at 0x8009B2A4 and 0x8009B2A6.
 *
 * D_8009B2A4 is the event-script flag word: func_8002FD10 seeds it from
 * its argument, Script_RunTick clears DUEL_EVENT_SCRIPT_FLAG_DIALOG_ACTIVE
 * and ORs in DUEL_EVENT_SCRIPT_FLAG_STARTED (duel_effect.h), and
 * func_8002E5AC sets the dialog bit. D_8009B2A6 is the scene index the
 * save-prompt command (func_8002EE94) reads out of the script stream and
 * copies into D_801D0000[0x3EE] and, as a byte, into gCampaignSceneIndex.
 * Retail reaches both gp-relative; the second is spelled
 * %gp_rel(D_8009B2A4 + 0x2) in the listings (func_8002EE94.s:27, 324,
 * 350, 361) because nothing references its address directly, so its
 * name here is the tree's address form, not a symbol splat produced.
 * Whether it is its own object or the flag word's second element is not
 * established; the two names record the two roles. */
extern u16 D_8009B2A4;
extern u16 D_8009B2A6;

#endif
