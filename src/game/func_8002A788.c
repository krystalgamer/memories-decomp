/*
 * Preserve the post-stepper argument: `func_8002A788`
 *
 * The 568-byte library navigation handler matches under the existing uniform
 * `gcc_2_8_1_g8` profile using the canonical signed card-grid positions and
 * the existing input header's pressed DATA and held DATA_VOLATILE views.
 * Replacing the held-input view with nonvolatile DATA shrinks the function to
 * 484 bytes; each tested direction path must retain retail's input reloads.
 *
 * The call to `func_8002A6B8` explicitly restores the state pointer in `$a0`
 * after the motion stepper. Its card-id definition does not consume that value,
 * but using its normal no-argument prototype replaces this restore at `+0x3C`
 * with a nop. `FUNC_8002A6B8_STATE_ARGUMENT` selects the argument-taking call
 * view only for this handler; the definition and earlier no-argument callers
 * retain their existing view. No dead expression, register pin, inline assembly
 * or new profile is needed.
 *
 * The handler preserves selection/cancel returns, ten-row jumps, bounded
 * single-cell movement, fourfold wrap duration and the final scroll update.
 * Its mode-byte copy uses unsized absolute-address byte declarations in the
 * handler's shared header. The original canonical rows and six-row terminal
 * refinement history remain unchanged; a post-terminal resolution records the
 * new call-view and source evidence.
 */
#define FUNC_8002A6B8_STATE_ARGUMENT
#define GINPUT_PAD1_PRESSED_IN_DATA
#define GINPUT_PAD1_HELD_IN_DATA_VOLATILE
#define D_8009B269_AS_ARRAY
#define D_8009B26C_AS_ARRAY
#include "../types.h"
#include "../unmatched.h"
#include "input.h"
#include "card_grid.h"
#include "func_8002A3CC.h"
#include "func_80029EB0.h"
#include "func_8002A660.h"
#include "func_8002A788.h"

void func_8002A788(u8 *state)
{
    s32 card_id;
    s32 column;
    s32 row;
    s32 frames;
    s32 distance;

    if (func_8002A3CC() == 0) {
        if ((gInput_wPad1Pressed & PAD_BUTTON_CROSS) != 0) {
            card_id = func_8002A6B8(state);
            if ((func_80029EB0(state, card_id) & 0x80) != 0) {
                *(s16 *)(state + 6) = card_id;
                state[0] = 2;
                return;
            }
        }
        if ((gInput_wPad1Pressed & PAD_BUTTON_CANCEL) != 0) {
            D_8009B26C[0] = D_8009B269[0];
            return;
        }
        if ((gInput_wPad1Held & (PAD_DIRECTION_MASK | PAD_BUTTON_L1_R1_MASK)) != 0) {
            column = gCardGrid_bCursorColumn;
            row = gCardGrid_bCursorRow;
            frames = 6;
            if ((gInput_wPad1Held & PAD_BUTTON_SQUARE) != 0) {
                frames = 2;
            }
            if ((gInput_wPad1Held & PAD_BUTTON_L1_R1_MASK) != 0) {
                if ((gInput_wPad1Held & PAD_BUTTON_R1) != 0) {
                    row += 0xA;
                    if (row >= 0x28) {
                        row = 0x27;
                    }
                } else {
                    row -= 0xA;
                    if (row < 0) {
                        row = 0;
                    }
                }
                distance = row - gCardGrid_bCursorRow;
                if (distance >= 0) {
                    frames = distance * 2;
                } else {
                    frames = (gCardGrid_bCursorRow - row) * 2;
                }
            } else {
                if ((gInput_wPad1Held & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
                    if ((gInput_wPad1Held & PAD_DIRECTION_RIGHT) != 0) {
                        column += 1;
                        if (column >= 0x14) {
                            column = 0x13;
                            if (row < 0x27) {
                                column = 0;
                                row += 1;
                                goto wrap_delay;
                            }
                        }
                    } else {
                        column -= 1;
                        if (column < 0) {
                            column = 0;
                            if (row != 0) {
                                column = 0x13;
                                row -= 1;
wrap_delay:
                                frames *= 4;
                            }
                        }
                    }
                }
                if ((gInput_wPad1Held & PAD_DIRECTION_VERTICAL_MASK) != 0) {
                    if ((gInput_wPad1Held & PAD_DIRECTION_DOWN) != 0) {
                        row += 1;
                        if (row >= 0x28) {
                            row = 0x27;
                        }
                    } else {
                        row -= 1;
                        if (row < 0) {
                            row = 0;
                        }
                    }
                }
            }
            func_8002A4A8(column, row, frames);
        }
    }
    func_8002A660(state);
}
