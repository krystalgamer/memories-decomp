#include "../types.h"
#include "card_constants.h"
#include "save_data.h"
#include "script_op_save_prompt.h"

int Duel_IsPlayerDeckComplete(void)
{
    unsigned short *entry = gDuel_awPlayerDeck;
    int i = 0;

    while (i < DECK_SIZE) {
        if (*entry == 0) {
            return 0;
        }
        i++;
        entry++;
    }
    return 1;
}
