#include "../types.h"
#include "sound.h"

extern s32 gFile_anLba[];
extern s8 gSD_bOutputType;
extern u32 gSD_dwCurrentBgmCommand;

extern void func_80012D4C(void);
extern void func_80045334(u32);
extern void func_80046990(s32, s32, s32);
extern u32 func_8004703C(void);
extern void func_80047314(u32);
extern void func_800473CC(u32);
extern void func_80047430(s32, s32);
extern void SD_KeyOffVoiceSlots(void);
extern void SD_SEPlay(u32, s32, s32);

void Sound_InitFrontend(void)
{
    register volatile s32 *lbas = gFile_anLba;

    gSD_bOutputType = -1;
    func_80046990(lbas[4], lbas[5], lbas[6]);
    while (func_8004703C() & 8) {
        func_80012D4C();
    }
}

void SD_SEPlayFull(u32 value)
{
    SD_SEPlay(value & SD_COMMAND_VALUE_MASK, 0xFF, 0);
}

void SD_BGMPlay(u32 value)
{
    u32 command = value | SD_BGM_COMMAND_BASE;

    func_80047314(command & SD_COMMAND_VALUE_MASK);
    gSD_dwCurrentBgmCommand = command;
}

void SD_BGMFadeOut(void)
{
    func_80047430(-8, 0);
}

void SD_BGMFadeOutWithStep(s32 value)
{
    if (value > 0)
        value = -value;
    func_80047430((s16)value, 0);
}

void func_8003FF88(u32 value)
{
    SD_SEPlay((value & SD_COMMAND_VALUE_MASK) | 0x8000, 0xFF, 0);
}

void func_8003FFB4(u32 value)
{
    func_80045334((value & SD_COMMAND_VALUE_MASK) | 0x8000);
}

void func_8003FFD8(u32 value)
{
    func_80047314((value & SD_COMMAND_VALUE_MASK) | 0xA000);
}

void SD_StopAll(void)
{
    func_800473CC(0);
    func_800473CC(0x8000);
    SD_KeyOffVoiceSlots();
}
