#include "../types.h"
#include "model_transfer_flags.h"

/* The model transfer family's .sdata block, 0x8009B074 to 0x8009B07F.
 *
 * model_transfer_flags.h already settled everything this file needs. It
 * records what each byte is, and it worked out that D_8009B074 is a pointer
 * rather than the s32 two units had spelled it: every retail read uses the
 * word as a base for a load at a fixed offset or a null test, and both writes
 * store either the address of D_800F5788 or zero. This file adds no claim of
 * its own; it only gives the block a definition, and includes that header so
 * the two cannot drift apart.
 *
 * The header also answers in advance the question a carve would otherwise
 * have to ask, noting that every retail access to the run is gp-relative.
 * That is what .sdata provides, so no consumer needs a declaration arm.
 *
 * This is the last of the three owners in the 40-byte range at 0x8009B058
 * that the notes call the scattered tail. The middle block belongs to the
 * movie pipeline and the first eight bytes to main_menu_load_package_stage.c,
 * which stays extracted.
 *
 * D_8009B07A is initialised to -1. The header describes it as a signed
 * counter armed at -1 and the retail byte is 0xFF, so the semantics recorded
 * there and the image agree; that agreement is also what makes s8 rather
 * than u8 the right spelling for the definition.
 *
 * The block ends with three bytes no symbol names, so splat's label for
 * D_8009B07C spans four bytes although the object is one -- the same way the
 * label at D_8009AF18 ran long. They are written out as padding because an
 * object stopping at D_8009B07C would leave the window three bytes short.
 *
 * The padding is three separate bytes rather than a u8[3], which is not a
 * style choice. An array of three bytes is given four-byte alignment, so it
 * lands at offset 0x0C instead of 0x09 and the compiler fills 0x09 to 0x0B
 * itself; the section then measures fifteen bytes and the link fails with
 * .initialized_data overlapping the bss image. Separate bytes align to one
 * and pack where they belong. model_graphics_state.c spells its own
 * continuation bytes this way for the same reason.
 *
 * Every value is zero apart from that -1, so the section attributes are load
 * bearing rather than decorative: without them these objects would be placed
 * in .sbss and leave .sdata entirely. */

u8 *D_8009B074 __attribute__((section(".sdata"))) = 0;
u8 D_8009B078 __attribute__((section(".sdata"))) = 0;
u8 D_8009B079 __attribute__((section(".sdata"))) = 0;
s8 D_8009B07A __attribute__((section(".sdata"))) = -1;
u8 D_8009B07B __attribute__((section(".sdata"))) = 0;
u8 D_8009B07C __attribute__((section(".sdata"))) = 0;
static u8 sModelTransferFlags_Pad7D __attribute__((section(".sdata"))) = 0;
static u8 sModelTransferFlags_Pad7E __attribute__((section(".sdata"))) = 0;
static u8 sModelTransferFlags_Pad7F __attribute__((section(".sdata"))) = 0;
