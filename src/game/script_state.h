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
 * The text callbacks select the historical array and signed scalar views;
 * TextBox_BuildStep selects the absolute unsigned scalar. These arms keep
 * the measured addressing without private declarations in the consumers.
 */
#if defined(SCRIPT_STATE_TEXT_CALLBACK_VIEWS)
extern u16 D_8009B27C[];
extern s16 D_8009B27C_scalar asm("D_8009B27C")
    __attribute__((section(".data")));
#elif defined(SCRIPT_STATE_COMMAND_IN_DATA)
extern u16 D_8009B27C __attribute__((section(".data")));
#else
extern u16 D_8009B27C;
#endif

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

/* The loaded script package, as the script engine reads it: a byte base the
 * cursor above is set from.
 *
 * script_run_tick.c states the layout in one line --
 * `D_8009B290 = D_801A8000 + *(u16 *)(D_801A8000 + g * 2)` -- so the head of
 * the package is a table of little-endian u16 byte offsets indexed by script
 * number, and the entry is where that script's code starts. The other three
 * namers jump within the same package: src/game/func_8002E918.c,
 * script_stream_commands.c and func_8002F9D4.c all set the cursor to
 * `D_801A8000 + offset`. All four spelled it `extern u8 D_801A8000[]`, which
 * is the spelling kept here.
 *
 * This is the script engine's view of the address, not a claim to own it.
 * The same buffer is read as 24-byte records through
 * `CardListRowSet D_801A8000[]` in card_list_rows.h, and the overlays reach
 * it as `u32 []` and `MainMenuState []`. None of those files includes this
 * header and none of the four here includes card_list_rows.h, so the
 * spellings never meet.
 *
 * The four file-transfer units that store `(s32)D_801A8000` into a transfer
 * object's fields keep their own declarations: they never read through the
 * base, which is a different reading from this one. */
extern u8 D_801A8000[];

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
 * The text callback arm preserves both the unsigned arrays and the signed
 * absolute scalar aliases. Base2_8009B2A8/AA are distinct linker identifiers
 * at the same addresses: they stop GCC retaining addresses across calls. */
/* A sixteen-bit operand the script engine assembles from the stream a byte at
 * a time in script_flag_commands.c and func_8002F630.c. The text callback
 * func_8003771C sets it from func_80036D3C's return.
 *
 * The default-arm consumers read it unsigned except one site, which takes it as
 * `*(s16 *)&D_8009B29C` to get a signed value out of the same halfword. That
 * cast stays at the use, so the declaration here is the plain u16 all three
 * consumers already wrote.
 *
 * The text callback arm keeps its signed, absolute spelling. */
#ifdef SCRIPT_STATE_TEXT_CALLBACK_VIEWS
extern s16 D_8009B29C __attribute__((section(".data")));
extern u16 D_8009B2A8[];
extern u16 D_8009B2AA[];
extern u16 Base2_8009B2A8[];
extern u16 Base2_8009B2AA[];
extern s16 D_8009B2A8_scalar asm("D_8009B2A8")
    __attribute__((section(".data")));
extern s16 D_8009B2AA_scalar asm("D_8009B2AA")
    __attribute__((section(".data")));
#else
extern u16 D_8009B29C;
extern u16 D_8009B2A8;
extern u16 D_8009B2AA;
#endif

/* Script_UpdateViewportTween's 16.16 X/Y accumulators and per-frame
 * deltas. The intervening command latch and stream pointer are independent
 * symbols, not padding in a fabricated contiguous tween object. */
extern s32 D_8009B284;
extern s32 D_8009B288;
extern s32 D_8009B294;
extern s32 D_8009B298;

/* func_8002F968 loads this signed countdown from its two-byte operand. */
extern s16 D_8009B278;

/* Script_OpShowImage and func_8002F630 retain an allocated display object
 * here until their later command phase releases it with func_8004036C. */
struct DisplayObject;
extern struct DisplayObject *D_8009B280;

/* The show-image command's halfword operand. func_8002E470 and func_8002E6B8
 * read it from the stream as `cursor[0] | (cursor[1] << 8)`: the low twelve
 * bits are the image index func_8002DF2C is handed, 0x8000 says a viewport
 * X/Y pair follows (func_8002E470 masks the index down to `& 0xFFF` and
 * reads the pair into gGraphics_sViewportX/Y; func_8002E6B8 into
 * D_8009B2A8/D_8009B2AA), and Script_OpShowImage tests 0x4000. Retail is
 * sh/lhu gp-relative at all three, so the plain u16 they already wrote.
 *
 * The text callbacks store func_80036D3C's result through $at, requiring
 * the unsized-array arm rather than a small scalar. */
#ifdef SCRIPT_STATE_TEXT_CALLBACK_VIEWS
extern u16 D_8009B270[];
#else
extern u16 D_8009B270;
#endif

/* Two halfwords the event driver keeps at 0x8009B2A4 and 0x8009B2A6.
 *
 * D_8009B2A4 is the event-script flag word: Campaign_LoadScenePackage seeds it from
 * its argument, Script_RunTick clears DUEL_EVENT_SCRIPT_FLAG_DIALOG_ACTIVE
 * and ORs in DUEL_EVENT_SCRIPT_FLAG_STARTED (duel_effect.h), and
 * func_8002E5AC sets the dialog bit. D_8009B2A6 is the scene index the
 * save-prompt command (func_8002EE94) reads out of the script stream and
 * copies into workspace +0x7DC (the scene byte and its following byte;
 * see save_data.h's halfword arm) and, as a byte, into gCampaignSceneIndex.
 * Retail reaches both gp-relative; the second is spelled
 * %gp_rel(D_8009B2A4 + 0x2) in the listings (func_8002EE94.s:27, 324,
 * 350, 361) because nothing references its address directly, so its
 * name here is the tree's address form, not a symbol splat produced.
 * Whether it is its own object or the flag word's second element is not
 * established; the two names record the two roles. */
extern u16 D_8009B2A4;
extern u16 D_8009B2A6;

#endif
