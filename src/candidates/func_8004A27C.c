/* Reclassified from matching_c (#3859). This was
 * src/game/sound_voice_volume.c, byte-exact only under
 * gcc_2_8_1_cc_g8_as_g0_split, whose compiler and assembler disagree about
 * small data (GCC -G8, MASPSX -G0), and with 1 variable pinned to a hard
 * register. This source carries NO register pin. Under
 * gcc_2_8_1_cc_g8_as_g0_split it is 31 of 31 instructions and its .text is
 * byte-identical to the previous cast-based spelling; 4 of the 31 words
 * differ from retail, all in the second product's registers. */
#include "../types.h"
#include "../psyq/libspu.h"
#include "../game/sound.h"

/* Scales one secondary object's stereo level by the two channel masters
 * (field_0514/field_0516) and submits it as a volume-only SpuSetVoiceAttr
 * request through the shared voice_attr block of the secondary state. The
 * mask is VOLL|VOLR|VOLMODEL|VOLMODER; volmode is cleared so both channels
 * stay in direct mode, and the 7.7 products are shifted back down by 7.
 *
 * This spelling reaches the block through the shared SDSecondaryState layout,
 * as every matched sibling does (sd_init_voice_state.c, sound_voice_setup.c),
 * instead of u8 * casts at +0x4C0..+0x4CE and +0x514/+0x516. The masters are
 * declared u16 because this, their only reader, loads them lhu; their two
 * writers store 127 and a parameter, which is sh either way. Measured on the
 * installed body: the u8 * spelling, the typed spelling through a local, the
 * typed spelling inline, the masters read through an lvalue cast, a value
 * cast, and with no cast at all are six byte-identical objects.
 *
 * THE RESIDUE, READ FROM RAW WORDS RATHER THAN THE ALIGNER: four words differ
 * from retail, 11 (mult a1,a2 for mult a2,a1), 23 (mflo a1 for mflo t1),
 * 24 (sra a3,a1,7 for sra v0,t1,7) and 26 (sh a3 for sh v0).
 * align_functions.py counts word 11 as aligned because its literal regex
 * also erases the a0-a3 register names, so its 28 of 31 overstates this
 * function by one row. The mult operand order is the commutative-operand
 * rule: with r as both operand and destination, gcc emits mult r,right and
 * no spelling reaches mult right,r; a fresh product name does.
 *
 * WHY THE PIN WAS THERE, from cc1's own -dl/-dg/-dS dumps (gcc 2.8.1): the
 * second master is a local-alloc quantity with two references and a life of
 * four instructions, and MIPS has no REG_ALLOC_ORDER, so it takes the first
 * free register numerically. Retail has it in a1, which needs something alive
 * in v0 across those four instructions. sched1 in gcc 2.8 schedules
 * BACKWARDS from the call: the second mult lands about eleven slots before its
 * shift on latency alone, and the D_80011434 address chain is placed just
 * before the load it feeds, which is placed just before its store. With the
 * volmode stores written before the table read, the store->load dependence
 * keeps the second store where retail has it (word 12) and the chain lands
 * AFTER the second mult, so nothing occupies v0 and the master takes it
 * (target allocation of the products then rotates to a1/t0). With the table
 * read written between the two stores, the chain is born before the mult,
 * v0 is busy, and the master, both products and both shifts land exactly in
 * retail's registers (a1, a3, t1, v0) -- but the anti-dependence now pins
 * the second store after the load, word 17 instead of 12, which is two
 * structural blocks and ranks below this source on the project's key. The
 * byte-exact ancestor is this exact statement order plus
 * register s32 master_right asm("$5"); that pin stood in for one fact, and
 * no spelling produces it without breaking the schedule.
 *
 * MEASURED AND DEAD on the fresh-product base under this profile, every one
 * byte-identical to one of the two states above: ten positions of the two
 * volmode stores relative to the loads and products; the typed spelling at
 * five positions of the table read (MEM_IN_STRUCT_P does not change the
 * dependence here); the table read as a byte address and as an integer sum;
 * a u16 and an s16 local for the master; a u16 pointer indexed [0]/[1]; the
 * master read before the first multiply; both masters read first; the
 * products inline at the stores; the dead parameter `left` reassigned to hold
 * the master (gcc gives the second value a fresh pseudo with no copy
 * preference for a1, so it is not a parameter borrow); `right` as the
 * product's destination; and do { } while (0) around either store or the
 * table read, which adds an instruction. The nine shapes an earlier version
 * of this comment called dead were measured under gcc_2_8_1_g0_split, which
 * does not discriminate between them; they are re-measured above.
 *
 * The one lever that moves the aligner's count from 26 to 28 is the last two
 * lines: the second shift is assigned to l, dead after the volume.left store.
 * Retail computes the second product into t1 and shifts it into v0; this
 * source uses a1 and a3. decomp-permuter saturates from this base without
 * producing an output. */
void SD_SetVoiceVolume(s32 voice, s32 left, s32 right) {
    s32 l;
    s32 r;
    s32 v;

    l = left * D_8009B458->field_0514;
    r = D_8009B458->field_0516;
    r = right * r;
    D_8009B458->voice_attr.volmode.left = SPU_VOICE_DIRECT;
    D_8009B458->voice_attr.volmode.right = SPU_VOICE_DIRECT;
    v = D_80011434[voice];
    D_8009B458->voice_attr.mask =
        SPU_VOICE_VOLL | SPU_VOICE_VOLR | SPU_VOICE_VOLMODEL | SPU_VOICE_VOLMODER;
    D_8009B458->voice_attr.voice = v;
    D_8009B458->voice_attr.volume.left = l >> 7;
    l = r >> 7;
    D_8009B458->voice_attr.volume.right = l;
    SpuSetVoiceAttr(&D_8009B458->voice_attr);
}
