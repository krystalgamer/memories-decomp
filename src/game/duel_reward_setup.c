#include "../types.h"
#include "duel_reward_setup.h"
#include "card_constants.h"
#include "file_transfer.h"
#include "../unmatched.h"

void func_80032328(void)
{
    File_RequestAsyncTransfer(0, 0, 0x2189, 0x4C, func_80032184, 0, 0);
    File_WaitForTransfers();
}

