#ifndef MEMORIES_DECOMP_PASSWORD_NAME_ENTRY_STATE_H
#define MEMORIES_DECOMP_PASSWORD_NAME_ENTRY_STATE_H

#include "../../types.h"

/* State shared by the name-entry screen's translation units.
 *
 * name_entry_frame.h and name_entry_keyboard.h already cover this screen's
 * drawing prefix and its keyboard entry points; neither is a home for the
 * screen's data, which is what six sources here were each re-declaring.
 *
 * What the uses show, without renaming anything:
 *
 *   D_8016D400  A flag byte. Tested with `& 0x10`, set with `| 4`, cleared
 *               on setup.
 *   D_8016D402  The keyboard row, cleared on setup, stepped and wrapped
 *               through a signed compare, and used to index D_8016ABC0.
 *               Every declarer spells it u8 and casts (s8) at each use.
 *   D_8016D426  A saved copy of D_8016D402, restored back into it.
 *   D_8016D408  A counter handed to each spawned glyph as obj->f106 and
 *               then incremented.
 *   D_8016D434  The cursor's target x, D_8016D401 * 20 + 22.
 *   D_8016D436  The cursor's target y, D_8016D402 * 18 + 24, from which the
 *               tween step (target - current) << 8 / 8 is built.
 *   D_8016D43C  The object the length adjuster works on.
 *
 *   D_8016D401  The keyboard column, stepped and wrapped at 15.
 *   D_8016D42C  The caret position: scaled by 2 to index the name buffer, by
 *               16 for the glyph x, and shifted left 4 for the glyph lookup.
 *   D_8016D4D2  The pending dialog id, masked with 0xFFF at its one reader.
 *   D_8016D418  The name buffer, pointed at gSaveData_aPlayerNameSjis.
 *
 * Four of the five neighbours this header used to list as unsettled are the
 * four above. They were never really in dispute: name_entry_setup.c is the
 * only source that spelled them differently, and it only ever writes them
 * (`= 0`, `= 244`, and one pointer assignment), so it never constrained the
 * type. D_8016D418's third spelling was name_entry_runtime.c's
 * `u16 *`, which it casts to s32 before doing any arithmetic, so that one
 * never used its pointee type either. A declarer that only stores, or only
 * takes an address, abstains rather than votes.
 *
 * NOT HERE, ON PURPOSE
 *
 * D_8016D404 is the one real disagreement of the five. name_entry_setup.c
 * assigns it, but name_entry_runtime.c and
 * name_entry_spawn_glyph_sprite.c both dereference it through structs of
 * their own -- `W` and `Fixed` -- and two dereferencing readers with
 * different types is the canonical-versus-local problem the password digit
 * cursor had, not a spelling to pick. It wants the same union treatment in
 * its own change.
 */
extern u8 D_8016D400;
extern u8 D_8016D402;
extern u8 D_8016D408;
extern u8 D_8016D426;
extern s16 D_8016D434;
extern s16 D_8016D436;
extern u8 *D_8016D43C;
extern s8 D_8016D401;
extern s8 D_8016D42C;
extern u16 D_8016D4D2;
extern u8 *D_8016D418;

#endif
