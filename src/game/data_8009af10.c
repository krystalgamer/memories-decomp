#include "../types.h"
#include "file_transfer.h"

/* The .sdata window at 0x8009AF10, between main_frame and
 * duel_side_view_angles. Sixteen bytes: three objects and a trailing word
 * the image never names.
 *
 * As with data_8009af6c.c the contents share no subject -- two boot-time
 * sizing constants and a file-transfer pointer -- so the file is named for
 * its address rather than for a subject it does not have.
 *
 * Splat shows three labels here, and the last of them spans eight bytes.
 * That span is not an object: D_8009AF18 is a four-byte pointer, loaded and
 * immediately dereferenced by its readers at +0x08, +0x10 and +0x46. The
 * four bytes at 0x8009AF1C are a separate zero word that carries no name and
 * no reference anywhere in the image, so the label simply runs to the end of
 * the range. It is represented below as explicit padding, the way
 * model_graphics_state.c represents its own unnamed continuation bytes.
 *
 * The padding needs the section attribute more than anything else here: it
 * is a zero-valued object, and without the attribute it would be placed in
 * .sbss and the window would come up four bytes short. */

/* Read together by the boot path at 0x800129FC, which subtracts them from
 * bss_end to size the region below the stack. 0x00200000 is the console's
 * main RAM size and 0x00002000 the reserve held back from it; both are
 * loaded through %hi/%lo as independent words rather than as one array. */
u32 D_8009AF10 __attribute__((section(".sdata"))) = 0x00200000;
u32 D_8009AF14 __attribute__((section(".sdata"))) = 0x00002000;

/* Initialized to the primary transfer descriptor and read as the current
 * one. The pointer target is what gives this its type: the readers only
 * index raw offsets, but the image initializes it with the address of a
 * FileTransferDescriptor, which is evidence the offsets alone do not carry.
 * model_graphics_state.c shows a .sdata pointer relocation of this kind
 * surviving the link. */
FileTransferDescriptor *D_8009AF18
    __attribute__((section(".sdata"))) = &gFile_PrimaryTransferDescriptor;

static u32 sData8009AF10_Pad1C __attribute__((section(".sdata"))) = 0;
