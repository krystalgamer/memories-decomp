/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 4 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/script_flag_commands.c.
 */
#include "../types.h"
#include "../game/campaign_flags.h"
#include "../game/script_state.h"
#include "../game/script_flag_commands.h"

void func_8002E918(void)
{
    register u8 *cursor __asm__("$5") = D_8009B290;
    register u8 *next __asm__("$6") = cursor + 2;
    register int high __asm__("$2");
    register int command __asm__("$3");
    int offset;

    D_8009B290 = next;
    high = cursor[1];
    command = cursor[0];
    command |= high << 8;
    if (command & CAMPAIGN_FLAG_COMMAND_WRITE) {
        Library_UpdateCardUsedFlag(command & CAMPAIGN_FLAG_COMMAND_PAYLOAD_MASK);
    } else {
        int low;

        D_8009B290 = cursor + 4;
        high = next[1];
        low = cursor[2];
        offset = low | (high << 8);
        if (Campaign_TestStoryFlag(command)) {
            D_8009B290 = D_801A8000 + offset;
        }
    }
    D_8009B27C = 0;
}

