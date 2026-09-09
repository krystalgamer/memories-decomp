#ifndef MEMORIES_DECOMP_MEM_CARD_H
#define MEMORIES_DECOMP_MEM_CARD_H

#include "../types.h"

#define MEM_CARD_DIRECTORY_ENTRY_SIZE 40
#define MEM_CARD_BLOCK_SIZE 8192
#define MEM_CARD_BLOCK_COUNT 15
#define MEM_CARD_DIRECTORY_RETRIES 5

#define MEM_CARD_DIALOG_FLAG_RESULT_CREATED 0x40
#define MEM_CARD_DIALOG_FLAG_RESULT_READY 0x80
#define MEM_CARD_DIALOG_FLAG_CLOSING 0x800
#define MEM_CARD_DIALOG_FLAG_IO_PENDING 0x1000
#define MEM_CARD_DIALOG_FLAG_STARTED 0x2000
#define MEM_CARD_DIALOG_FLAG_OPENED 0x4000
#define MEM_CARD_DIALOG_FLAG_ACTIVE 0x8000

/* The result of the card's asynchronous IO, set from the callbacks
 * mem_card_io_result_callbacks.h declares and polled by the request state
 * machines. func_800440B4 resets it to -1 before starting a request.
 *
 * Three of the five consumers declare it volatile, and they are the ones
 * that read it several times in a row while waiting -- without the
 * qualifier gcc commons those reads into one register and the poll cannot
 * observe the callback. The other two write it once or read it once. */
/* The memory-card I/O event handles opened at init and closed on teardown.
   Five sources declared it identically, and all five spell the element type
   `long` rather than s32; that spelling is preserved rather than normalised,
   because DeliverEvent/OpenEvent hand these back as long and nothing here has
   measured the difference. */
extern long gMemCard_aIOEventHandles[];

#ifdef GMEMCARD_NIORESULT_IS_VOLATILE

extern volatile s32 gMemCard_nIOResult;
#else
extern s32 gMemCard_nIOResult;
#endif

/* The memory-card channel a request was issued on. func_800440B4 stores its
 * first parameter here before it resets gMemCard_nIOResult; every
 * caller passes it the int it was itself handed, and func_8004413C passes
 * that same int to _card_info(long chan). The readers hand it back to
 * _card_info, _card_clear and _card_load, and to func_80044470's s32 first
 * parameter. Retail stores it with sb and reads it with lbu at ten sites,
 * all gp-relative into $a0, five of the reads in func_80044838, still
 * assembly; so it is one unsigned byte, and the one char spelling was the
 * writer's, where a store shows no sign. */
extern u8 D_8009B437;

extern u8 gMemCard_szSaveFileName[];

/* The filename buffer every memory card request is issued against.
 *
 * data_transfer_request.c strcpy()s a name into it, and the create, load and
 * save paths then pass it as the (char *) filename to MemCardCreateFile,
 * MemCardGetDirentry, MemCardReadFile and MemCardWriteFile. All four already
 * include this header, so it belongs here rather than in unmatched.h: that
 * header is for symbols with no identified owner, and this one has an obvious
 * one.
 *
 * The incomplete-array spelling is the one all four consumers already wrote
 * and is kept. c_symbols.ld names D_800EFE38 thirty-two bytes later, but
 * nothing reads or writes through a bound, so no size is asserted here. */
extern u8 D_800EFE18[];

/* The create-state message value must stay wide so GCC keeps the retail
 * register-to-argument move; the definition and the other callers use the
 * measured byte/halfword contract. */
#ifdef GMEMCARD_RESULT_USES_WIDE_ARGS
void func_8003E46C(s32 value, s32 bits);
#else
void func_8003E46C(u8 value, u16 bits);
#endif

/* The memory-card dialog's flag word.
 *
 * Declared plain here because the readers need it plain: every other user
 * only tests bits or does a read-modify-write, and marking the object
 * volatile forces reloads that grow .text by 40 bytes.
 *
 * func_8003E46C needs the opposite and reaches the same word through its own
 * volatile linker name; see the comment there.
 */
extern u16 gMemCard_wDialogFlags;

/* The second block of IO event handles, sixteen bytes past
 * gMemCard_aIOEventHandles at 0x800F2AE0, so four `long` handles apart. Both
 * are handed to func_80043D48, and all three sources that name this one
 * already include this header and already spell the element `long`. That
 * spelling is preserved here for the same reason it is above: the event API
 * hands these back as long and nothing has measured the difference. */
extern long D_800F2AF0[];

/* The request state machines' shared state. Every symbol below was declared
 * identically by each of its users, all of which already include this header.
 *
 *   D_8009B3EF  The request's outcome, set to 1, 2 or 3 by the create, load
 *               and save paths and read back by the dialog runtime.
 *   D_8009B3DC  The block count a save needs; computed by
 *               data_transfer_request.c and passed as MemCardCreateFile's
 *               third argument.
 *   D_8009B3DE  The dialog step index. mem_card_dialog_runtime.c calls
 *               D_80090F9C[D_8009B3DE]() and data_transfer_request.c writes
 *               it; the existing note beside that call records the five
 *               entries.
 *   D_8009B3EC  A retry counter: cleared, tested and incremented by the
 *               create and save paths.
 *   D_8009B3F0  Polled against 2, and passed as (long *)&D_8009B3F0 next to
 *               (long *)&D_8009B3F4, so the two are consecutive words handed
 *               to the same call.
 *   D_800EFBC0  The directory buffer, cast to (struct DIRENTRY *) at each
 *               use and passed with the file count beside it.
 *
 * D_801D5648 keeps its unsized spelling, and it is load-bearing: the note in
 * mem_card_save_state.c records that as a plain s32 extern the -G8 build puts
 * it in small data and the store collapses to one gp-relative word, where
 * retail materialises the %hi half in its own register. Declaring it here
 * changes where the spelling lives, not the spelling. */
extern u8 D_8009B3EF;
/* The dialog message the save path raises: func_8003F8D4.c and
 * func_8003FCD8.c set it to 0x29, 40 and 36 at different failures and
 * two_player_save_setup.c reads it back. All three spell it plain u8. */
extern u8 D_8009B3C0;
extern u8 D_8009B3DC;
extern u8 D_8009B3DE;
extern u8 D_8009B3EC;
extern s32 D_8009B3F0;
extern u8 D_800EFBC0[];
extern s32 D_801D5648[];

#endif
