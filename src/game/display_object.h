#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_H

#include "../types.h"
#include "display_object_layout.h"

typedef void (*DisplayObjectCallback)(u8 *);

/* One entry of the DISPLAY_OBJECT_POOL_CAPACITY-entry pool at D_800EFE48.
 *
 * previous and next are pool indices, not pointers: func_800402A0 unlinks an
 * entry by writing -1 to both and then repairs the neighbour's link through
 * D_800EFE48[v].previous, and func_80040814 walks the list through next.
 */
/* The union of the four private views this record had: display_object.h's
 * own (previous/next/flags/update), display_slot_lifecycle.c's DisplaySlot
 * (the richest), display_projection.c's ProjectionEntry (+0x28) and
 * func_80040588.c's local DisplayObject (+0x17, +0x30, +0x3C, +0x5C). Every
 * offset they share agrees; each named a different subset.
 *
 * That exception is now resolved. func_80040588.c and func_800408D0.c used
 * to keep private copies because they reach a u8 at +0x22 and a pair of u16
 * at +0x3C/+0x3E, which fall inside the words display_slot_lifecycle.c
 * stores with single word writes. Splitting those words outright would have
 * turned an sw into an sh; carrying each as a union of both widths does not,
 * so the word writes keep their sw and both renderers now take this record.
 *
 * The same device now covers 0x40 and 0x48, the two words
 * display_object_config.h's separate halfword view names that were still
 * word-only here. All four of the offsets that view exists for -- 0x30, 0x3C,
 * 0x40 and 0x48 -- are therefore reachable from this record at both widths.
 * What is left of that view is its function signature, not its layout. */
/* `attribute` is a GsSPRITE / GsBOXF attribute word, not a game flag word and
 * not a GPU packet tag. Both renderers copy it verbatim into the descriptor
 * they hand to GsSortSprite and friends, so every bit the game sets there is
 * read by libgs, and the names are libgs.h's:
 *
 *   0x01000000 / 0x02000000  colour mode; the texture-page step of 1, 2 or 4
 *                            a strip wrap applies is 4bpp, 8bpp and 16bpp
 *   0x04000000  GsPERS       perspective
 *   0x08000000  GsROTOFF     rotation off -- which is why the renderers only
 *                            compute `rotate` when it is clear
 *   0x10000000  GsAONE   \
 *   0x20000000  GsATWO    >  the two-bit semi-transparency rate at bit 28
 *   0x30000000  GsATHREE /
 *   0x40000000  GsALON       semi-transparency on
 *   0x80000000  GsDOFF       display off
 *
 * That is what the recurring composites mean: 0x50000000 is GsALON | GsAONE,
 * additive blending, which is what sparkles and afterimages want; 0x60000000
 * is GsALON | GsATWO, subtractive, which is what a fade-to-black overlay
 * wants. The `& 0x8FFFFFFF` masks clear the rate and GsALON together and keep
 * GsDOFF -- "turn semi-transparency off" -- and `& 0xF7FFFFFF` clears
 * GsROTOFF, "turn rotation on". */
typedef struct DisplayObject {
    s16 previous;                  /* 0x00 */
    s16 next;                      /* 0x02 */
    u32 attribute;                 /* 0x04 */
    u16 flags;                     /* 0x08 */
    u8 field_0A;                   /* 0x0A */
    u8 field_0B;                   /* 0x0B */
    u32 field_0C;                  /* 0x0C */
    u32 field_10;                  /* 0x10 */
    /* DuelStatusPosition called this priority before it was retired into
       this record, and for its own consumer that is right: func_80016D2C.c
       passes it to GsSortFastSprite as the ordering-table depth, and
       display_slot_lifecycle.c seeds it from D_8009AF74[ot_index].

       The name is not taken, because four other consumers treat it as a bit
       field rather than a depth: func_80025D30.c adds D_8009B1D0 << 14,
       duel_field_effect_steps.c adds step * 0x3000, and func_80040588.c or's
       it with 0x10000, 0xF0000 and 0x30000 into a mode word. Taking one
       consumer's reading for the shared record is the mistake 0x6A avoids. */
    u16 field_14;                  /* 0x14 */
    s8 field_16;                   /* 0x16 */
    /* An ordering-table index, not a texture index. func_80016D2C.c uses it
       to pick D_800E9D90[ot_index], casts that element to GsOT * and hands it
       to GsSortFastSprite as the ordering table; func_80040588.c indexes the
       same array -- its local tb is assigned D_800E9D90 -- and passes the
       element to func_80042188; and three overlay files declare the array as
       GsOT *D_800E9D90[]. display_object_helpers.h's D_8009AF74[4] is a
       parallel per-layer table indexed by the same byte, which agrees.

       It was tex_index until DuelStatusPosition was retired into this record;
       that view already called it ot_index and was right. */
    u8 ot_index;                   /* 0x17 */
    u16 field_18;                  /* 0x18 */
    u16 field_1A;                  /* 0x1A */
    u16 field_1C;                  /* 0x1C */
    s16 field_1E;                  /* 0x1E */
    /* 0x20 is reached both as a word and as the byte at +0x22. Both are
       retail's: display_slot_lifecycle.c clears the whole word with one sw,
       and the two sprite renderers read only the byte. Neither view is a
       superset, so the record carries both rather than choosing. */
    /* The byte at 0x21 is inside the halfword at 0x20, so it gets a third
       view rather than a new field: display_object_property_transitions.c
       reads and writes it alone, and DuelCardDisplayObject in
       duel_card_display_state.h already names the same byte field_21 on this
       record. */
    union {
        u32 word;
        struct {
            u16 field_20;
            u8 field_22;
            u8 field_23;
        } h;
        struct {
            u8 field_20;
            u8 field_21;
            u8 field_22;
            u8 field_23;
        } b;
    } field_20;                    /* 0x20 */
    DisplayObjectCallback update;  /* 0x24 */
    /* 0x28 and 0x30 are each read both ways: display_projection.c and the two
       sprite emitters take whole words, while display_parent_links.c derives a
       parent-relative offset from the halves. A union records both without
       forcing either side to spell the other's access. */
    union {
        struct {
            u16 field_28;
            u16 field_2A;
        } h;
        s32 word;
    } position;                    /* 0x28 */
    /* The first of the six words at stride 8 that the 0x4C comment below
       describes, and read both ways like its neighbours at 0x30, 0x3C, 0x40
       and 0x48.

       Whole: display_object_helpers.c zeroes it and writes an initial value,
       both renderers copy it into a primitive's colour word,
       text_box_build_step.c clears it, and func_800391E4.c and
       Dialog_UpdateChoice write colour constants.

       Halves: display_object_property_transitions.c compares 0x2C against the
       byte at 0x21, and divides 0x80 and 0x800 by 0x2E to derive its
       per-frame increments -- so 0x2E is a nonzero divisor rather than
       padding. That file calls them `target` and `step`; the names here stay
       with the offsets. */
    union {
        u32 word;
        struct {
            u16 field_2C;
            s16 field_2E;
        } h;
    } field_2C;                    /* 0x2C */
    union {
        struct {
            u16 field_30;
            u16 field_32;
        } h;
        s32 word;
    } field_30;                    /* 0x30 */
    /* The second of those six words. No union: every user takes it whole.
       display_object_helpers.c zeroes it, both renderers copy it into a
       primitive's colour word, and func_800391E4.c and Dialog_UpdateChoice
       write colour constants into it. */
    u32 field_34;                  /* 0x34 */
    /* Read both ways, and by the same device as its neighbours.

       Whole: display_object_helpers.c writes 0x00808080 here as the third of
       six words at stride 0xC -- 0x2C, 0x38, 0x44, 0x50, 0x5C, 0x68 -- and
       display_object_list_renderers.c copies it into a primitive's colour
       word.

       Halves: DuelEffect_UpdateObjectLayout writes 0x38 and 0x3A as an x/y
       pair. That function writes six such pairs at stride 8 -- 0x28, 0x30,
       0x38, 0x40, 0x48, 0x50 -- of which the record already had a halfword
       view for four.

       That is one witness under one object kind, and this halfword has more
       readings than almost any other on the record, so it does not
       generalise. The stride-0xC run above and the stride-8 colour run at
       0x2C both cross this range and disagree with it. So, separately, do
       the three views that already name these halves outside this header:
       DisplayObjectVelocity in display_object_helpers.c calls 0x36, 0x38 and
       0x3A velocity_x, velocity_y and velocity_z and adds them to 0x30/0x32
       every frame, while DisplayObjectSnapshot in func_80043178.h and
       DisplayObjectPosition in display_object_interpolation.h read the same
       0x36/0x38 pair as a saved position that eases into the live one.
       display_object_helpers.h sets out at length why neither of those
       generalises either.

       Five readings, no winner, so the halves are named for their offsets
       and nothing more -- the same grounds 0x44 and 0x4C keep theirs on. A
       consumer that knows which motion path owns its object should take the
       view that names what it means, as name_entry_glyph_effects.c does with
       the velocity one. */
    union {
        u32 word;
        struct {
            s16 field_38;
            s16 field_3A;
        } h;
    } field_38;                    /* 0x38 */
    /* 0x3C likewise: func_80040588 copies the whole word into the sprite
       primitive, while func_800408D0 reads the two halves separately. */
    union {
        s32 word;
        struct {
            u16 field_3C;
            u16 field_3E;
        } h;
    } field_3C;                    /* 0x3C */
    /* 0x40 and 0x48 are the last two words display_object_config.h's separate
       halfword view covers, and they are read both ways for the same reason
       0x3C is: display_slot_lifecycle.c clears each with one sw and the two
       sprite emitters copy each as a word, while func_80040510 and the dialog
       and duel layout code write the halves. The view calls 0x48/0x4A
       half_height_2/half_width_2; the halves are left field_-named here, as
       0x3C's are, until a caller pins the meaning. */
    union {
        u32 word;
        struct {
            s16 field_40;
            s16 field_42;
        } h;
    } field_40;                    /* 0x40 */
    /* Read at least two different ways, so the halves are named for their
       offsets and nothing more.

       0x44 is the fourth of six words at stride 8 -- 0x2C, 0x34, 0x3C, 0x44,
       0x4C, 0x54 -- which display_object_helpers.c zeroes in one run. For a
       gouraud-rendered object those six are per-vertex colours: func_80041068,
       the twelve-word code-0x3C renderer, copies 0x44 into its third vertex's
       colour word, and Dialog_UpdateChoice writes all six with colour
       constants (0x2000 four times, 0xC000 twice) before installing
       Widget_UpdatePulseColour as the update callback.

       The sprite emitters read the same word as a scale instead:
       func_80040588 and func_800408D0 assign it to sprite_primitive.h's u32
       `scale`, and display_object_transition.c animates the two halves from a
       0x1000 base, with that file and display_slot_lifecycle.c resetting the
       pair to 0x10001000 -- 1.0 in each half of 12-bit fixed point.

       An earlier revision of this comment, from #2985, gave only the second
       reading and called 0x44 "the scale". That generalised one witness, which
       is the mistake display_object_config.h avoids for 0x6A. Both readings
       are recorded here because the record is shared across object kinds and
       neither one governs.

       The union itself is unaffected: it asserts that some users take the word
       and others the halves, which is true under either reading, and it is the
       same device as 0x40 and 0x48 on either side. */
    union {
        u32 word;
        struct {
            s16 field_44;
            s16 field_46;
        } h;
    } field_44;                    /* 0x44 */
    union {
        u32 word;
        struct {
            s16 field_48;
            s16 field_4A;
        } h;
    } field_48;                    /* 0x48 */
    /* Read two ways, like 0x44, and for the same reason: 0x4C is the fifth of
       the six words at stride 8 -- 0x2C, 0x34, 0x3C, 0x44, 0x4C, 0x54 -- that
       display_object_helpers.c zeroes in one run.

       For a gouraud-rendered object it is a vertex colour:
       display_object_list_renderers.c copies it into a primitive's colour
       word, Dialog_UpdateChoice writes 0x2000 and func_800391E4 writes
       0xA0A0A0.

       For others it holds a second callback: display_object_updates.c calls
       through it as void (*)(u8 *, s32), and dialog_transition.c,
       func_800179F4.c and func_8002ABB4.c each store a function's address
       here.

       Neither reading governs, so the offset is the name. s32 is the spelling
       that serves both: the callback writers in this file already cast the
       function to it, and a colour word is not a pointer. DuelCardDisplayObject
       in duel_card_display_state.h names the same word field_4C, as a void *,
       because on that view only the callback reading occurs. */
    s32 field_4C;                  /* 0x4C */
    /* The last unnamed word of the tail, and read as incompatibly as 0x4C
       just above it, so the offset is again the name.

       func_800179F4.c stores a pointer to another display object here and
       Duel_DrawLifePointsAndDeckCounts loads it back. func_80042824 in
       display_object_helpers.c writes the colour 0x00808080, as the fourth of
       six words at stride 0xC -- 0x2C, 0x38, 0x44, 0x50, 0x5C, 0x68. That is
       a different run from the stride-8 one described at 0x4C, and the two
       agree only at 0x2C and 0x44, which is why neither run's reading can be
       pushed onto the whole tail. func_80041534.c advances it by 4;
       func_80041C8C.c adds the halfword at 0x58 to form a byte pointer, the
       reading DisplayObjectStreamState spells as `current`; and
       display_object_list_renderers.c copies it into a primitive word.

       s32 serves all of them: a word load and store do not distinguish
       signedness, and the pointer writers cast, as they already do at 0x4C. */
    /* Reached as halves as well, by the same witness 0x38 describes and
       under the same limit: DuelEffect_UpdateObjectLayout writes 0x50 and
       0x52 as the last of its six x/y pairs. The word view stays for every
       user the comment above lists, and the two callers that reach the word
       by name spell it .word: func_800179F4.c, which stores another object's
       address here, and Duel_DrawLifePointsAndDeckCounts, which loads that
       address back. Those two are why the word view is s32 rather than
       unsigned. */
    union {
        s32 word;
        struct {
            s16 field_50;
            s16 field_52;
        } h;
    } field_50;                    /* 0x50 */
    void *field_54;                /* 0x54 */
    /* Named field_58 and field_5A by DisplayObjectStreamState in
       ygo_types.h, on this same record and at this same pair of offsets, on
       the grounds the 0x50 comment above already gives: that view is
       func_80041C8C's reading of the script buffer. func_80041D60 resets
       both when it opens a script and then counts 0x5A down by D_8009B0D8
       until it reaches zero, which is what says the pair is a status field
       and a countdown rather than four padding bytes. */
    s16 field_58;                  /* 0x58 */
    s16 field_5A;                  /* 0x5A */
    u16 field_5C;                  /* 0x5C */
    u16 field_5E;                  /* 0x5E */
    /* An easing amount, agreed on in shape and not in name. dialog_transition.c
       sets it to -0x400 or +0x400 and sweeps it toward zero;
       func_8003DA40.c sets -0x400 and adds 0x20; mem_card_dialog_runtime.c
       steps it by 0x40 and passes it to func_80043230, which takes it as
       `phase` for an rsin ease; display_object_property_transitions.c calls
       it `speed` in one function and `step` in the other. Four callers, four
       words, one range -- so it keeps the offset for a name, on the same
       grounds #3004 set out for 0x44. */
    s16 field_60;                  /* 0x60 */
    u8 pad_62[3];                  /* 0x62 */
    u8 field_65;                   /* 0x65 */
    u8 field_66;                   /* 0x66 */
    /* Named field_67 by DisplayObjectConfig in display_object_config.h, on
       this same record, and read by func_800391E4.c. 0x68 is named just
       below, now that func_80016784 reaches it through this record. */
    u8 field_67;                   /* 0x67 */
    /* The card type of the duel card this object draws. func_80016784
       switches on it over card_constants.h's CARD_TYPE_EQUIP, CARD_TYPE_MAGIC,
       CARD_TYPE_TRAP and CARD_TYPE_RITUAL, picking the frame each spell class
       gets and falling through to the monster arm for everything else.
       DuelCardDisplayObject in duel_card_display_state.h already names the
       same byte field_68; that record's field_67 and field_69 are this
       record's too, and 0x6A is the D_801A7AD8 index both agree on. It keeps
       the offset for a name because only duel cards evidence it, and the pool
       is shared across object kinds. */
    u8 field_68;                   /* 0x68 */
    /* Named on this same record by DisplayObjectConfig in
       display_object_config.h. display_object_transition.c reads it and hands
       it to func_800428A8. */
    u8 field_69;                   /* 0x69 */
    /* func_8001D518.c copies a byte into this offset when it builds the
       projection slot's object, taking it from 0x0A on the record it is given.
       That is the only evidence for it, so it takes the offset for a name and
       0x6B stays padding. */
    u8 field_6A;                   /* 0x6A */
    u8 pad_6B[1];                  /* 0x6B */
    u8 field_6C;                   /* 0x6C */
    u8 pad_6D[DISPLAY_OBJECT_RECORD_SIZE - 0x6D];
} DisplayObject;

/* Two functions reach past this record, and it is not a mistake in
   either of them, so do not "fix" the size to accommodate them
   without settling what follows.

   The 0x70 stride is not merely asserted, it is fixed by the layout:
   D_800EFE48 holds DISPLAY_OBJECT_POOL_CAPACITY (96) entries, and
   0x800EFE48 + 96 * 0x70 is 0x800F2848, which is exactly where
   D_800F2848 begins. There is no room for a larger record.

   Yet func_80041068 in display_object_list_renderers.c walks this pool
   with that stride and then tests e[0x72] as a flag, reading a second
   vertex set from 0x58, 0x64, 0x68 and 0x6C when it is set; and
   func_80042824 in display_object_helpers.c writes object[0x72]. Both
   land two bytes past the record, which is `next` of the following
   entry.

   What that means is not established here. The sibling renderer
   func_80040DD8 gates its second submission on e[0x5A] instead, so the
   two list heads (D_800EFE38[4] and [5]) may simply carry differently
   shaped payloads. Recorded as an open question rather than guessed at.

   The practical consequence, restated after measuring it. The earlier
   wording said display_object_list_renderers.c could not be converted
   to member access because the record cannot express its spans. That
   reason is right for func_80041068, wrong for func_80040DD8, and not
   the whole reason for either.

   func_80041068 genuinely cannot convert: it reads 0x64 as an s32
   beginning inside pad_62, and tests e[0x72], which is outside the
   record. Neither is expressible however the members are spelled. Its
   0x58, 0x68 and 0x6C spans are a lesser matter -- *(s32 *)&object->
   field_58 and its like express those, as this record's own users
   already do for its unions.

   func_80040DD8 reaches nothing past 0x5A and every offset it uses
   lands on a named member, so the span argument never applied to it.
   Converting all of it still fails, for the reason func_80016784.c
   records about its own 0x0C read: a member read is a struct reference,
   the scratchpad stores it sits between are not, and GCC 2.8.1 floats
   the load across them. Measured here as five instructions gone and
   twenty bytes off the executable, retail's v0/v1/a0/a1 with their
   load-delay nops becoming a2/a3/t0/t1 with none.

   So the reads that do not sit between scratchpad stores are converted
   -- next, attribute, flags, field_14, ot_index, update and the 0x5A
   test -- and the vertex block between them keeps its casts, re-based
   on (u8 *) so retyping the cursor cannot rescale them. */

#define DISPLAY_OBJECT_OFFSET(member) ((u32)&(((DisplayObject *)0)->member))

typedef char DisplayObject_size_must_match_record_size[
    sizeof(DisplayObject) == DISPLAY_OBJECT_RECORD_SIZE ? 1 : -1
];
typedef char DisplayObject_field_22_must_be_at_0x22[
    DISPLAY_OBJECT_OFFSET(field_20.h.field_22) == 0x22 ? 1 : -1
];
typedef char DisplayObject_update_must_be_at_0x24[
    DISPLAY_OBJECT_OFFSET(update) == 0x24 ? 1 : -1
];
typedef char DisplayObject_position_must_be_at_0x28[
    DISPLAY_OBJECT_OFFSET(position) == 0x28 ? 1 : -1
];
typedef char DisplayObject_field_3E_must_be_at_0x3E[
    DISPLAY_OBJECT_OFFSET(field_3C.h.field_3E) == 0x3E ? 1 : -1
];
typedef char DisplayObject_field_42_must_be_at_0x42[
    DISPLAY_OBJECT_OFFSET(field_40.h.field_42) == 0x42 ? 1 : -1
];
typedef char DisplayObject_field_4A_must_be_at_0x4A[
    DISPLAY_OBJECT_OFFSET(field_48.h.field_4A) == 0x4A ? 1 : -1
];
typedef char DisplayObject_field_3A_must_be_at_0x3A[
    DISPLAY_OBJECT_OFFSET(field_38.h.field_3A) == 0x3A ? 1 : -1
];
typedef char DisplayObject_field_52_must_be_at_0x52[
    DISPLAY_OBJECT_OFFSET(field_50.h.field_52) == 0x52 ? 1 : -1
];
typedef char DisplayObject_field_58_must_be_at_0x58[
    DISPLAY_OBJECT_OFFSET(field_58) == 0x58 ? 1 : -1
];
typedef char DisplayObject_field_65_must_be_at_0x65[
    DISPLAY_OBJECT_OFFSET(field_65) == 0x65 ? 1 : -1
];

/* The DISPLAY_OBJECT_LIST_COUNT list heads, immediately below the pool.
 *
 * Each entry is the index of the first object on one list, or -1 for an empty
 * list; an object's own `next` continues the chain.  DisplayObject_ResetPool
 * writes -1 through all DISPLAY_OBJECT_LIST_COUNT of them with a single s16
 * cursor started at this address, which is what says the seven halfwords are
 * one array rather than seven objects that happen to be adjacent.
 */
/* The list walk's rerun request. func_80029108 sets it to 1 after changing an
 * object's config, and the two list renderers clear it before their pass and
 * test it at the end -- func_80040588's inventory row calls that its
 * "while(1)/continue rerun loop". So it is state of the walk over the lists
 * declared just below, which is why it is declared here rather than beside
 * either renderer. */
extern s32 D_8009B424;

extern s16 D_800EFE38[DISPLAY_OBJECT_LIST_COUNT];

/* The per-list key array, walked in lockstep with the list heads above.
 *
 * DisplayObject_ResetPool advances one pointer into each and writes -1 through
 * both for DISPLAY_OBJECT_LIST_COUNT iterations, which is what fixes this
 * length, and display_slot_lifecycle.c stores into it by list key. The element
 * type is s16 by the same evidence: that store is D_800F2878[key] = index, and
 * the reset walk uses an s16 *.
 *
 * func_800402A0.c reached it through a u8 * and scaled by two by hand. It now
 * takes this declaration and casts at the use site, which is the form it
 * already uses one line earlier for D_800EFE38. */
extern s16 D_800F2878[DISPLAY_OBJECT_LIST_COUNT];

extern DisplayObject D_800EFE48[DISPLAY_OBJECT_POOL_CAPACITY];
/* &D_800EFE48[DISPLAY_OBJECT_RESERVED_CAPACITY]: the allocatable tail of the
 * same pool, which func_8004002C scans. */
extern DisplayObject D_800F0548[
    DISPLAY_OBJECT_POOL_CAPACITY - DISPLAY_OBJECT_RESERVED_CAPACITY
];

/* Two counters the pool keeps beside the tables above. func_800400AC bumps
 * D_8009B412 once it has picked a slot, after reading the slot's
 * ALLOCATED flag and before testing it, so it counts every allocation
 * request that reached a slot, initialised or not; func_8004020C bumps
 * D_8009B410 after reading the slot's previous and next links
 * (display_object_api.h calls it the unlink counter). DisplayObject_ResetPool
 * stores 0 into both beside its -1 fill of D_800EFE38 and D_800F2878.
 * Nothing else in C or in the resident listings touches either; both
 * reads are lhu, so u16. Initial value not read.
 *
 * Every retail access is gp-relative (lhu/addiu/sh in func_8004020C.s:4-8
 * and func_800400AC.s:16-20, sh $zero in func_80040390.s:10-11), so the
 * plain halfword serves display_slot_lifecycle.c and
 * display_object_reset_pool.c alike. */
extern u16 D_8009B410;
extern u16 D_8009B412;

/* One of the per-frame update callbacks installed into DisplayObject::update.
 * It orbits an object around the base position it keeps at field_2C/field_2E,
 * stepping the angle at field_2A by 0x30 a frame and taking the radius from
 * field_28, which doubles as a countdown and decays by 2 each frame. When that
 * reaches zero it clears field_6C and update and snaps field_30 back to the
 * base in one word. While bit 0 of D_8009B0CC is set it also spawns a
 * companion object at the current position each frame.
 *
 * Declared here because this header owns both halves of its contract: the
 * DisplayObject it takes, and the DisplayObjectCallback typedef that
 * func_80020F4C.c casts it to when installing it. That file held the only
 * declaration and does not call the function itself, so the cast is the whole
 * use -- the declaration has to match for the address to be taken.
 *
 * This names one callback and claims nothing about the others that share the
 * slot; func_80042BC0 in display_object_lifecycle.h is a sibling by role but
 * takes DisplayObjectLifecycle *, not this type. */
void func_80020D4C(DisplayObject *object);

#endif
