#include "../types.h"

extern u8 *D_8009B458;
extern void func_800739EC(unsigned int);
extern void SD_ProcessSequenceTracks(void);
extern void func_8004C84C(void);
extern void func_8004AAFC(void);

long SD_SequenceTimerCallback(void)
{
    u8 *state = D_8009B458;
    int i;
    if (state[0x814] == 0)
        return 1;
    if (state[0x500] != 0)
        return 1;
    if (state[0x509] != 0)
        return 1;
    if (state[0x501] != 0)
        return 0;
    func_800739EC(0xF2000002);
    D_8009B458[0x501] = 1;
    for (i = 0; i < 8; i++) {
        void (*callback)(void);
        SD_ProcessSequenceTracks();
        D_8009B458[0x508]++;
        state = D_8009B458;
        if (state[0x508] >= 11) {
            state[0x508] = 0;
            func_8004C84C();
            func_8004AAFC();
            callback = *(void (**)(void))(D_8009B458 + 0x50C);
            if (callback != 0)
                callback();
        }
    }
    {
        register u8 *final asm("$2") = D_8009B458;
        final[0x501] = 0;
    }
    return 0;
}
