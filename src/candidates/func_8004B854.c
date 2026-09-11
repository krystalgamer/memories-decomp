/* Reclassified from matching_c (#3859). This was src/game/func_8004B854.c,
 * byte-exact only under gcc_2_8_1_cc_g8_as_g0_split, whose compiler and
 * assembler disagree about small data (GCC -G8, MASPSX -G0), and with 4
 * variables pinned to hard registers. Under gcc_2_8_1_g0, a single
 * threshold, it is 47 of 47 instructions with 3 differing, opcode distance
 * 0. The source below is the match, unchanged apart from its include paths. */
#include "../types.h"
#include "../psyq/libapi.h"
#include "../unmatched.h"
#include "../game/sound.h"
#include "../game/sound_event_runtime.h"

void func_8004B854(void)
{
    long event;

    if (D_8009B458->event_guard)
        return;

    D_8009B458->event_guard = 1;
    EnterCriticalSection();
    StopRCnt(RCntCNT2);
    {
        register unsigned long descriptor asm("$4") = RCntCNT2;
        register long specification asm("$5") = EvSpINT;
        register long mode asm("$6") = EvMdINTR;
        register long (*callback)(void) asm("$7") = SD_SequenceTimerCallback;

        event = OpenEvent(descriptor, specification, mode, callback);
    }
    D_8009B458->event_handle = event;
    EnableEvent(event);
    SetRCnt(RCntCNT2, 0xE000, RCntMdINTR);
    StartRCnt(RCntCNT2);
    ExitCriticalSection();
    D_8009B458->flag_0500 = 0;
    D_8009B458->event_guard = 0;
}
