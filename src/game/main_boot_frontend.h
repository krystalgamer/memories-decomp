#ifndef MEMORIES_DECOMP_MAIN_BOOT_FRONTEND_H
#define MEMORIES_DECOMP_MAIN_BOOT_FRONTEND_H
#include "../types.h"
#include "display_object.h"
#include "file_transfer.h"

/* Main_RunBootSequence supplies these callbacks to the boot package and boot
 * image requests. */
void Main_LoadBootPackageStage(
    FileTransferDescriptor *descriptor, s32 stage
);
void Main_LoadBootImageStage(FileTransferDescriptor *descriptor, s32 mode);

/* Crossfades one display object into another and releases what it replaces:
 * over 17 frames b and a blended copy brighten from 0 to 0x80 while a and its
 * copy dim, and b zooms from 4.0 to 1.0 scale when a is present. It then
 * settles b's field_44 and clears its blend bits, and frees a and both
 * copies. The first argument may be null, which Main_RunBootSequence relies
 * on for the first transition of the boot sequence. */
void DisplayObject_CrossfadeReplace(DisplayObject *a, DisplayObject *b);

/* Holds a boot screen: spins the frame loop for count frames, or until START
 * or the confirm button is pressed once the file-transfer request words have
 * gone quiet. It never returns while a transfer is still in flight, even after
 * count runs out. Main_RunBootSequence passes 4 after the first boot image and
 * 0xB4 while the main-menu package loads behind the second. */
void Main_HoldBootScreen(s32 count);

void Main_RunBootSequence(s32 mode);

/* PAL resident helpers shared by boot and the language-options overlay. */
void Main_InitDebugFont(void);
void Main_RequestLanguageBootPackage(s32 language, s32 wait);

s32 Main_RunFrontendLoop(void);

/* The two entries of the boot module (module ID 0x16, WA_MRG 0xB61800,
 * loaded at 0x80168000) that the boot sequence calls: the first reads the
 * BIOS region byte and initializes the module, the second steps its 20-state
 * controller until it returns zero. The module is not decompiled; see
 * notes/boot-frontend-sequence.md. */
void SafetyInit(void);
s32 SafetySync(s32);

#endif
