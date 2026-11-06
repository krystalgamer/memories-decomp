#include "../../types.h"
#define INPUT_PADS_STORAGE_ORDER
/* This build lays out the shared source's zero-initialized storage at
 * 0x800F05D0-0x800F0638 in a different order from the North American one;
 * the shared source defines it in first-declaration order (#2602). */
extern u8 gInput_abRepeatTimers[];
extern struct InputRawPadPacket gInput_abRawPadBuffers[];
#define INPUT_PADS_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C2E8-0x8009C310 (#2602); its
 * layout differs from the North American block the shared source defines. */
u16 gInput_wPad1RepeatBackup;
u16 gInput_wPad1PressedBackup;
u32 gInput_dwPendingHeld;
volatile u16 gInput_awPadHeld[2];
u32 gInput_dwDeferredRepeat;
u32 gInput_dwDeferredPressed;
u8 gInput_bRepeatDelay;
u8 D_8009C2FD_sles_03947;
u16 gInput_wPad1HeldBackup;
u32 gInput_dwPreviousHeld;
u8 gInput_bRepeatInterval;
u8 D_8009C305_sles_03947;
u16 D_8009C306_sles_03947;
volatile u16 gInput_awPadRepeat[2];
volatile u16 gInput_awPadPressed[2];

#define VERSION_EUROPE

#define INPUT_RAW_PAD_BUFFER_SIZE 0x24
#define INPUT_REPEAT_THRESHOLD 0x14
#define INPUT_REPEAT_RELOAD_VALUE 0x11

#include "../input_pads.c"
