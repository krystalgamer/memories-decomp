#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object_layout.h"

struct DisplayObject;
typedef void (*DisplayObjectCallback)(struct DisplayObject *);
/* The draw pass's second callback, at 0x4C: called with the object and the
   ordering table its ot_index selects. */
typedef void (*DisplayObjectDrawCallback)(struct DisplayObject *, GsOT *);

/* One entry of the DISPLAY_OBJECT_POOL_CAPACITY-entry pool at gDisplayObject_aPool.
 *
 * previous and next are pool indices, not pointers:
 * DisplayObject_MoveToListHead unlinks an entry by writing -1 to both and then
 * repairs the neighbour's link through gDisplayObject_aPool[v].previous, and
 * DisplayObject_RenderSpriteSheetList walks the list through next.
 */
/* The union of the four private views this record had: display_object.h's
 * own (previous/next/flags/update), display_object_core.c's DisplaySlot
 * (the richest), display_projection.c's ProjectionEntry (+0x28) and
 * display_object_core.c's renderer view (+0x17, +0x30, +0x3C, +0x5C). Every
 * offset they share agrees; each named a different subset.
 *
 * That exception is now resolved. display_object_core.c and
 * display_object_runtime.c used to keep private copies because they reach a
 * u8 at +0x22 and a pair of u16 at +0x3C/+0x3E, which fall inside the words
 * display_object_core.c stores with single word writes. Splitting those
 * words outright would have turned an sw into an sh; carrying each as a
 * union of both widths does not, so the word writes keep their sw and both
 * renderers now take this record.
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
 *   DISPLAY_OBJECT_ATTRIBUTE_8BPP / DISPLAY_OBJECT_ATTRIBUTE_16BPP
 *                            colour mode; the texture-page step of 1, 2 or 4
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
 * That is what the recurring composites mean: GsALON | GsAONE is additive
 * blending, which is what sparkles and afterimages want; GsALON | GsATWO is
 * subtractive, which is what a fade-to-black overlay wants. */
typedef struct DisplayObject {
    s16 previous;                  /* 0x00 */
    s16 next;                      /* 0x02 */
    u32 attribute;                 /* 0x04 */
    u16 flags;                     /* 0x08 */
    /* The entry's own index in gDisplayObject_aPool. DisplayObject_AcquireSlot
       stores it; DisplayObject_MoveToListHead links the entry back in through
       it; Duel_GetOrCreateActionReadyMarker copies another entry's value into
       the byte DisplayObject_ProjectLinkedGroundPosition indexes the pool by. */
    u8 pool_index;                 /* 0x0A */
    u8 field_0B;                   /* 0x0B */
    /* GsSPRITE-style r/g/b and its padding byte; the renderers copy the
       word into SpritePrim.rgb. */
    union {
        u32 word;
        struct {
            u8 r;
            u8 g;
            u8 b;
            u8 pad;
        } c;
    } rgb;                         /* 0x0C */
    /* The back-face control DisplayObject_ProjectRotationFacing reads. Zero
       returns a negative facing area unchanged; any nonzero value mirrors the
       X and Z matrix columns and reprojects instead, and a negative one -- a
       code address -- is first called as void (*)(DisplayObject *, s32) with
       the area. The preview constructor in card_preview.c
       stores CardPreview_UpdateVariant here (as the word), and the duel card
       objects store DisplayObject_SetResourceVariantFromSign as the callback,
       so a card turned away selects its back-face variant.
       Library_CreateFadingCardOutline stores 1, and AcquireSlot and both
       Initialize*GouraudQuad helpers clear it. */
    union {
        u32 word;
        void (*callback)(struct DisplayObject *, u32);
    } back_face_handler;           /* 0x10 */
    /* Cached ordering-table depth. SubmitPacket reads the signed low half
       of its mode word; the upper half selects a handler. The effect-request
       buffer at +0x14 belongs to a separate record, not this field. */
    u16 ot_depth;                  /* 0x14 */
    s8 depth_offset;                   /* 0x16 */
    /* An ordering-table index, not a texture index. Duel_DrawDecimalDigits uses it
       to pick gGraphics_apOrderingTables[ot_index], a GsOT * element, and hands it
       to GsSortFastSprite as the ordering table; display_object_core.c indexes the
       same array -- its local table is assigned gGraphics_apOrderingTables -- and passes the
       element to DisplayObject_SubmitPacket. ordering_tables.h shares the four-pointer
       array with the overlays. display_object_helpers.h's gDisplayObject_awOrderingTableDepthBases[4] is a
       parallel per-layer table indexed by the same byte, which agrees.

       It was tex_index until DuelStatusPosition was retired into this record;
       that view already called it ot_index and was right. */
    u8 ot_index;                   /* 0x17 */
    /* Two per-kind views of one halfword pair.

       center: the offset from xy to the object's centre. Every renderer
       adds it to the draw position before the clip-test projection
       (DisplayObject_ProjectRotationFacing and its packed sibling), and
       DisplayObject_ConfigureScreenSprite and TextBox_RefreshDisplayObjects
       set it to half the sprite's size.

       target: a position the object moves toward. The build-deck transfer
       icon and the password digit cursor steer toward it and finally copy
       the pair into xy as one word; neither sets the clip-test flag, so no
       renderer reads it as a centre for them. */
    union {
        struct {
            u16 x;
            u16 y;
        } center;
        struct {
            u16 x;
            u16 y;
        } target;
    } field_18;                    /* 0x18 */
    u16 field_1C;                  /* 0x1C */
    /* The display list the entry was acquired on (the `key` argument of
       DisplayObject_AcquireSlot, an index into gDisplayObject_awListHeads and
       gDisplayObject_ListRenderers). DisplayObject_Release unlinks a list head
       through it, and TextBox_SetPos tests it against 4, the Gouraud-quad
       list, to pick a quad layout over a sprite position. */
    s16 list_key;                  /* 0x1E */
    /* 0x20 is reached both as a word and as the byte at +0x22. Both are
       retail's: display_object_core.c clears the whole word with one sw,
       and the two sprite renderers read only the byte. Neither view is a
       superset, so the record carries both rather than choosing. */
    /* The packed bytes are Euler X/Y/Z angles: projection expands each
       byte by 16 for RotMatrixZYX_gte. The sprite renderers expand the same
       Z byte by 5760 for GsSPRITE.rotate. Preserve the word/halfword aliases
       and the opaque fourth byte rather than splitting the word stores. */
    union {
        u32 word;
        struct {
            u16 field_20;
            u8 rotation_z;
            u8 field_23;
        } h;
        struct {
            u8 rotation_x;
            u8 rotation_y;
            u8 rotation_z;
            u8 field_23;
        } b;
    } field_20;                    /* 0x20 */
    DisplayObjectCallback update;  /* 0x24 */
    /* 0x28 and 0x30 are each read both ways: display_projection.c and the two
       sprite emitters take whole words, while duel_scene.c derives a
       parent-relative offset from the halves. A union records both without
       forcing either side to spell the other's access. */
    union {
        struct {
            u16 field_28;
            u16 field_2A;
        } h;
        /* The same halves as signed screen coordinates (the duel hand's
           slide target). */
        struct {
            s16 x;
            s16 y;
        } s;
        s32 word;
    } position;                    /* 0x28 */
    /* The first of the six words at stride 8 that the 0x4C comment below
       describes, and read both ways like its neighbours at 0x30, 0x3C, 0x40
       and 0x48.

       Whole: display_object_helpers.c zeroes it and writes an initial value,
       both renderers copy it into a primitive's colour word,
       text_box_build_step.c clears it, and text_control_commands.c and
       Dialog_UpdateChoice write colour constants.

       Halves: duel_scene.c compares 0x2C against the
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
        /* A gouraud quad's vertex colour (DisplayObject_InitializeGouraudQuad
           clears it, DebugMenu_Init sets its green). */
        CVECTOR colour;
    } field_2C;                    /* 0x2C */
    /* The 2D x/y pair the renderers draw at. DisplayObject_RenderSpriteList
       and DisplayObject_RenderSpriteStrips copy the word into SpritePrim.xy
       (GsSPRITE x/y) and subtract the viewport origin unless the object is
       screen-space; DisplayObject_ProjectGroundPosition stores the projected
       screen x/y here with gte_stsxy; DisplayObject_ConfigureScreenSprite and
       DisplayObject_ConfigureSpriteAtPosition take it as their x/y arguments.
       The Gouraud-quad list reads the word as its second vertex's x/y. The one
       non-coordinate reader is DisplayObject_RenderTexturedGouraudQuadList,
       which takes the low halfword as the first vertex's u/v; no C caller
       acquires an object on that list (key 5). */
    union {
        struct {
            u16 x;
            u16 y;
        } h;
        /* The same coordinates read signed, as duel card objects use them. */
        struct {
            s16 x;
            s16 y;
        } s;
        s32 word;
    } xy;                          /* 0x30 */
    /* The second of those six words, and read both ways like its
       neighbours.

       Whole: display_object_helpers.c zeroes it, both renderers copy it into
       a primitive's colour word, and text_control_commands.c and Dialog_UpdateChoice
       write colour constants into it.

       Halves: the value-setup screen's widget tween in the main_menu overlay
       saves the live position at 0x30/0x32 into 0x36/0x38 and eases back
       out of it -- the reading the `ease` arm names. The `motion` arm gives
       0x36 a different meaning (an x velocity), so the word and the plain
       halves keep their offset names.

       z: the low half is the depth coordinate that completes xy wherever a
       halfword reads it. Duel_CreateFieldCardObject stores its third
       argument here beside x and a fixed
       -24 y; Duel_RenderFieldCard3D loads it as the translation's t[2] and
       Duel_DrawProjectedFieldCardFrame as the vz it hands to gte_rtps; the
       equip-bonus and trap code copy it into an effect request's field_04
       beside the request's x/y; duel effect 15 takes it as the card's
       translation vz, and effects 17, 22 and the row attacks integrate it
       with their per-card velocity vz. DisplayObject_StepPositionZ steps
       the same halfword. */
    union {
        u32 word;
        struct {
            s16 z;
            s16 field_36;
        } h;
        /* DisplayObject_StepPosition*'s reading: 0x36 is the 8.8 x velocity
           added to xy every frame. */
        struct {
            s16 z;
            s16 velocity_x;
        } motion;
        /* DisplayObject_SavePosition's and the cosine/sine easers' reading:
           0x36 is the x the object eases away from. */
        struct {
            s16 z;
            s16 start_x;
        } ease;
        /* A gouraud quad's vertex colour (DisplayObject_InitializeGouraudQuad
           clears it, DebugMenu_Init sets its green). */
        CVECTOR colour;
    } field_34;                    /* 0x34 */
    /* Read both ways, and by the same device as its neighbours.

       Whole: display_object_helpers.c writes 0x00808080 here as the third of
       six words at stride 0xC -- 0x2C, 0x38, 0x44, 0x50, 0x5C, 0x68 -- and
       DisplayObject_RenderGouraudQuadList and
       DisplayObject_RenderTexturedGouraudQuadList copy it into a primitive's
       colour word.

       Halves: DuelEffect_UpdateObjectLayout writes 0x38 and 0x3A as an x/y
       pair. That function writes six such pairs at stride 8 -- 0x28, 0x30,
       0x38, 0x40, 0x48, 0x50 -- of which the record already had a halfword
       view for four.

       That is one witness under one object kind, and this halfword has more
       readings than almost any other on the record, so it does not
       generalise. The stride-0xC run above and the stride-8 colour run at
       0x2C both cross this range and disagree with it. So, separately, do
       the two motion-path readings: DisplayObject_StepPosition* treat 0x36,
       0x38 and 0x3A as velocity_x, velocity_y and velocity_z and add them to
       0x30/0x32 every frame, while DisplayObject_SavePosition and the
       cosine/sine easers read the same 0x36/0x38 pair as a saved position
       that eases into the live one.

       Several readings, no winner, so the plain halves are named for their
       offsets -- the same grounds 0x44 and 0x4C keep theirs on -- and each
       motion path takes the arm that names what it means: `motion` for the
       velocity integrators, `ease` for the save-and-ease helpers. */
    union {
        u32 word;
        struct {
            s16 field_38;
            s16 field_3A;
        } h;
        /* The y and z velocities beside field_34.motion.velocity_x. */
        struct {
            s16 velocity_y;
            s16 velocity_z;
        } motion;
        /* The y beside field_34.ease.start_x. */
        struct {
            s16 start_y;
            s16 field_3A;
        } ease;
    } field_38;                    /* 0x38 */
    /* 0x3C likewise: DisplayObject_RenderSpriteList copies the whole word
       into the sprite primitive's extent, and the quad renderers copy it as
       a colour or u/v word.

       size: the sprite's GsSPRITE w and h. DisplayObject_RenderSpriteStrips
       reads w as the strip width left to draw and h as each strip's height;
       DisplayObject_ConfigureScreenSprite stores its width and height
       arguments here and half of each into center and pivot; and the text
       box, Library outline and duel layout code take xy + w as the right
       edge and xy + h as the bottom. Every halfword user reads it this way;
       the quad readers only take the word. */
    union {
        s32 word;
        struct {
            u16 w;
            u16 h;
        } size;
        /* A gouraud quad's vertex colour (DisplayObject_InitializeGouraudQuad
           clears it, DebugMenu_Init sets its green). */
        CVECTOR colour;
    } field_3C;                    /* 0x3C */
    /* 0x40 and 0x48 are the last two words display_object_config.h's separate
       halfword view covers. display_object_core.c clears each with one sw
       and the sprite emitters copy each as a word; the halves have one
       reading per object kind.

       clut: a sprite's GsSPRITE cx/cy. Both sprite renderers copy the word
       into SpritePrim.cxcy; DisplayObject_ConfigureScreenSprite stores its
       clut_x/clut_y arguments here and DisplayObject_ConfigureSpriteResource
       derives them from its palette argument; the text-box, card and
       portrait code pack them into a primitive's clut or step them a palette
       at a time (x by 16, y by one row).

       vertex: a Gouraud quad's vertex x/y. DisplayObject_RenderGouraudQuadList
       copies the word into the fourth vertex (the textured renderer into the
       third), and the text-box outline, debug editor and duel effect layout
       code write the halves as a corner. */
    union {
        u32 word;
        struct {
            s16 x;
            s16 y;
        } clut;
        struct {
            s16 x;
            s16 y;
        } vertex;
    } field_40;                    /* 0x40 */
    /* Read at least two different ways, so the halves are named for their
       offsets and nothing more.

       0x44 is the fourth of six words at stride 8 -- 0x2C, 0x34, 0x3C, 0x44,
       0x4C, 0x54 -- which display_object_helpers.c zeroes in one run. For a
       gouraud-rendered object those six are per-vertex colours:
       DisplayObject_RenderTexturedGouraudQuadList, the twelve-word code-0x3C
       renderer, copies 0x44 into its third vertex's colour word, and
       Dialog_UpdateChoice writes all six with colour constants (0x2000 four
       times, 0xC000 twice) before installing Widget_UpdatePulseColour as the
       update callback.

       The sprite emitters read the same word as a scale instead:
       DisplayObject_RenderSpriteList and DisplayObject_RenderSpriteStrips assign it to sprite_primitive.h's u32
       `scale`, and main_boot_frontend.c animates the two halves from a
       0x1000 base, with that file and display_object_core.c resetting the
       pair to 0x10001000 -- 1.0 in each half of 12-bit fixed point.

       An earlier revision of this comment, from #2985, gave only the second
       reading and called 0x44 "the scale". That generalised one witness, which
       is the mistake display_object_config.h avoids for 0x6A. Both readings
       are recorded here because the record is shared across object kinds and
       neither one governs.

       The union itself is unaffected: it asserts that some users take the word
       and others the halves, which is true under either reading, and it is the
       same device as 0x40 and 0x48 on either side. */
    /* The halves have two readings, one per object kind, and get a view
       each.

       scale: GsSPRITE scalex/scaley in 4.12 fixed point. The sprite
       renderers copy the word into SpritePrim.scale, the spotlight mask,
       projected card and clip checks read the halves as x/y scale, and the
       pulse, zoom and fade code animates them around 0x1000. GlyphSprite in
       ygo_types.h names the same halves scale_x/scale_y.

       target: the password name-entry glyphs instead keep the position
       they fly toward here, dividing the distance from xy by the frame
       count and finally copying the word into xy. */
    union {
        u32 word;
        struct {
            s16 x;
            s16 y;
        } scale;
        struct {
            s16 x;
            s16 y;
        } target;
        /* A gouraud quad's vertex colour (DisplayObject_InitializeGouraudQuad
           clears it, DebugMenu_Init sets its green). */
        CVECTOR colour;
    } field_44;                    /* 0x44 */
    /* Three readings, one per object kind.

       pivot: GsSPRITE mx/my, the rotation and scaling centre relative to
       xy. DisplayObject_RenderSpriteList copies the word into SpritePrim.mxmy
       and adds it to the draw position; DisplayObject_ConfigureScreenSprite
       and the text-box, card-overlay and boot-screen code set it to half the
       sprite's size. display_object_config.h's view calls the pair
       half_width_2/half_height_2.

       spotlight: the inner and outer radii DisplayObject_RenderSpotlightMask
       draws its ring of quads between; the dialog transition and the
       overworld location reveal shrink or set them.

       vertex: the first vertex of a Gouraud quad's second submission, which
       DisplayObject_RenderGouraudQuadList copies from the word; the
       text-box outline and duel effect layout write it as a corner. */
    union {
        u32 word;
        struct {
            s16 x;
            s16 y;
        } pivot;
        struct {
            s16 inner;
            s16 outer;
        } spotlight;
        struct {
            s16 x;
            s16 y;
        } vertex;
    } field_48;                    /* 0x48 */
    /* Read three ways, one per object kind. 0x4C is also the fifth of the
       six words at stride 8 -- 0x2C, 0x34, 0x3C, 0x44, 0x4C, 0x54 -- that
       display_object_helpers.c zeroes in one run.

       color: a Gouraud-rendered object's vertex colour.
       DisplayObject_RenderGouraudQuadList and
       DisplayObject_RenderTexturedGouraudQuadList copy it into a primitive's
       colour word; Dialog_UpdateChoice writes 0x2000 and
       TextBox_RefreshDisplayObjects writes 0xA0A0A0.

       draw: for other objects, a second callback. The draw pass calls it
       after `update`, with the object and its ordering table.

       frame_data: a timed sprite stream's sheet (SpriteSheet in
       display_object_system.h), whose header and parts the sheet renderer
       consumes; the stream's ReadNextCommand selects it.

       word: the raw value, for code that copies or clears it whatever the
       kind. */
    union {
        s32 word;
        u32 color;
        DisplayObjectDrawCallback draw;
        struct SpriteSheet *frame_data;
    } field_4C;                    /* 0x4C */
    /* The last unnamed word of the tail, and read as incompatibly as 0x4C
       just above it, so the offset is again the name.

       src/game/duel_scene.c stores a pointer to another display
       object here and Duel_DrawLifePointsAndDeckCounts loads it back.
       DisplayObject_InitializeTexturedGouraudQuad in display_object_helpers.c writes the colour 0x00808080,
       as the fourth of six words at stride 0xC -- 0x2C, 0x38, 0x44, 0x50,
       0x5C, 0x68. That is
       a different run from the stride-8 one described at 0x4C, and the two
       agree only at 0x2C and 0x44, which is why neither run's reading can be
       pushed onto the whole tail. DisplayObjectStream_ConfigureRotation
       advances it by 4; DisplayObjectStream_ReadNextCommand adds the halfword at 0x58 to form a
       byte pointer, the `current` arm;
       and the two DisplayObject_Render*GouraudQuadList functions copy it into
       a primitive word.

       s32 serves all of them: a word load and store do not distinguish
       signedness. */
    /* Reached as halves as well, by the same witness 0x38 describes and
       under the same limit: DuelEffect_UpdateObjectLayout writes 0x50 and
       0x52 as the last of its six x/y pairs. The word view stays for every
       user the comment above lists; the duel scene's object link, stored by
       src/game/duel_scene.c and loaded back by
       Duel_DrawLifePointsAndDeckCounts, is the `linked` arm. */
    union {
        s32 word;
        struct {
            s16 field_50;
            s16 field_52;
        } h;
        /* A timed sprite stream's script cursor; the stream handlers in
           display_object_system.c set it and ReadNextCommand reads through it. */
        u8 *current;
        /* The panel Duel_DrawLifePointsAndDeckCounts draws its digits beside;
           Duel_InitScene links the life-points panel here. */
        struct DisplayObject *linked;
    } field_50;                    /* 0x50 */
    /* resource: the serialized resource the stream resolver and the stream
       jumps offset into (AcquireSlot's tail_data_start, or the bank the
       ConfigureSprite*WithResource helpers pass). link: card_preview.c's
       secondary object keeps its primary here. word: a Gouraud quad's
       colour or vertex word, which duel_effect_text.c and script_scene.c
       write and the quad renderers copy; uv: the textured quad's fourth
       u/v halfword. */
    union {
        u8 *resource;
        struct DisplayObject *link;
        s32 word;
        u16 uv;
    } field_54;                    /* 0x54 */
    /* The command stream reads this pair as command_offset/frame_delay.
       ReadNextCommand fetches current + (u16)command_offset and advances by
       three; resets and jumps clear the offset. UpdateCommandStream opens
       the script and counts the delay down by gGraphics_nBaseFrameStep.
       These are stream-only names: other object kinds reuse these words
       as packet coordinates/colours, so the generic offsets remain. */
    s16 field_58;                  /* 0x58 */
    s16 field_5A;                  /* 0x5A */
    /* The texture u/v (low byte u, high byte v) a sprite or card quad draws
       from. DisplayObject_RenderSpriteList and DisplayObject_RenderSpriteStrips
       copy the halfword into SpritePrim.uv (GsSPRITE u/v), as do the card
       preview panel and the PAL options grid; DisplayObject_ConfigureScreenSprite
       stores its texture_u/texture_v arguments here; Duel_RenderFieldCard3D
       takes byte 0 as u0/u2 and byte 1 as v0/v1 of its POLY_GT4; the
       display-effect and name-entry callbacks pass the two bytes on as a
       spawned sprite's u/v. DisplayObject_RenderTexturedGouraudQuadList
       instead copies the word as a colour (an alternate view). The reviewed
       current named C acquisition calls do not select its list (key 5), and
       no direct named C call to DisplayObject_InitializeTexturedGouraudQuad
       was found; this does not establish whole-program unreachability. */
    union {
        u16 word;
        struct {
            u8 u;
            u8 v;
        } b;
    } texture_uv;                  /* 0x5C */
    /* The texture u/v (low byte u, high byte v) of a resource-configured
       sprite. DisplayObject_ConfigureSpriteResource fills both bytes from
       its texture_config argument, DisplayObject_RenderSpriteSheet starts
       its parts from them, CardPreview_DrawDetailPanel copies the halfword
       into a primitive's uv, and the duel effect, game-over, Library and
       free-duel code write u or v directly. Screen sprites keep their u/v
       at 0x5C instead (texture_uv). */
    union {
        u16 word;
        struct {
            u8 u;
            u8 v;
        } b;
    } resource_uv;                 /* 0x5E */
    /* An easing amount, agreed on in shape and not in name. dialog_transition.c
       sets it to -0x400 or +0x400 and sweeps it toward zero;
       its Dialog_UpdateMessageTransition sets -0x400 and adds 0x20; mem_card_dialog.c
       steps it by 0x40 and passes it to Widget_SlideSine, which takes it as
       `phase` for an rsin ease; duel_scene.c calls
       it `speed` in one function and `step` in the other. Four callers, four
       words, one range -- so it keeps the offset for a name, on the same
       grounds #3004 set out for 0x44. */
    s16 field_60;                  /* 0x60 */
    /* The 8.8 fraction bytes DisplayObject_StepPosition* carry below xy;
       the z fraction is fraction_z at 0x64. */
    u8 fraction_x;                 /* 0x62 */
    u8 fraction_y;                 /* 0x63 */
    /* The 8.8 fraction byte DisplayObject_StepPositionZ carries below
       field_34.motion.z, reset to 0x80 with fraction_x/fraction_y.
       DisplayObject_RenderTexturedGouraudQuadList instead reads the s32 at
       0x64 as the second vertex's x/y pair of its second POLY_GT4
       submission, the same slot 0x34 fills in its first; the other three
       bytes of that word are named below for their own byte users. */
    u8 fraction_z;                 /* 0x64 */
    /* DisplayObject_ProjectRotationFacing copies it into
       ProjectionOut.divisions, which the packet backend hands to DivideFT4
       as DIVPOLYGON4.ndiv when nonzero. AcquireSlot clears it; the card
       preview objects set 3. */
    u8 subdivisions;               /* 0x65 */
    /* The GsSPRITE tpage. Both sprite list renderers copy it into
       SpritePrim.tpage, the sheet renderer adds each part's page to it,
       DisplayObject_ConfigureScreenSprite stores its texture_page argument
       here and DisplayObject_ConfigureSpriteResource the low byte of
       texture_config. */
    u8 texture_page;               /* 0x66 */
    /* The three resource selectors. DisplayObject_ConfigureSpriteResource and
       DisplayObject_SetResourcePath store them, DisplayObject_SetResourceVariant
       and DisplayObject_UpdateResourceVariant rewrite the last one and clear
       flags bit 0x10, and the resolver in display_object_system.c walks the
       serialized offset tables with them in this order (root, branch, final
       entry) to find the object's resource. Root/branch name the first two
       table indices, not package-specific contents.

       Object kinds without a resource reuse the bytes: duel_effect_text.c
       keeps a text channel index in the root byte, and duel cards keep their
       card type (CARD_TYPE_* in card_constants.h) in the branch byte, which
       Duel_DrawCardFrame switches on to pick the frame each spell class
       gets. */
    u8 resource_root_index;        /* 0x67 */
    u8 resource_branch_index;      /* 0x68 */
    u8 resource_variant;           /* 0x69 */
    /* Two bytes each object kind uses for its own index and argument.

       kind_index (0x6A):
       - card_record_index: a duel card's gDuel_aCardRecords index. The card
         object constructors store it, and the card display, flag and
         marker code reads its record through it.
       - row_index: a card-list deck box's row; CardList_RenderDeckBoxStats
         indexes the staged CardListRowSet table by it.
       - front_variant: a card preview's resource variant while it faces the
         camera; CardPreview_UpdateVariant selects variant 2 instead when it
         is turned away.
       - linked_pool_index: the gDisplayObject_aPool entry whose ground
         position DisplayObject_ProjectLinkedGroundPosition follows, copied
         from that entry's pool_index.
       - language: a PAL options language flag; PalOptions_DrawWaveSprite
         draws the wave effect only on the flag whose language matches
         gPalOptions_bLanguage.

       kind_arg (0x6B):
       - deck_index: a duel card's deck entry, the second argument the card
         replay hands to Duel_SetupFieldCardFromDeck beside its record.
       - choice_index: a two-player value-setup widget's slot;
         MainMenu_StartValueWidgetTween stores it and the tween callback
         settles the widget's value into gTwoPlayerDuel_abSetupChoices[it].
       - keep_after_pulse: in the European name entry, 1 for the persistent
         keyboard sprites, which survive their pulse, and 0 for spawned glyph
         sprites, which the pulse releases. */
    union {
        u8 card_record_index;
        u8 row_index;
        u8 front_variant;
        u8 linked_pool_index;
        u8 language;
    } kind_index;                  /* 0x6A */
    union {
        u8 deck_index;
        u8 choice_index;
        u8 keep_after_pulse;
    } kind_arg;                    /* 0x6B */
    /* The byte each update callback keeps its own state in, cleared on
       acquire. Its low nibble is the tag DisplayObject_FindAllocatedByTag
       matches: callers set a tag (0xF for the duel card rotations) and wait
       for the callback to clear it. The high bits are callback flags:
       DisplayObject_MarkInitialized sets 0x80 on the first call, and
       callbacks use 0x40/0x20 as phase bits. Some callbacks keep a counter
       or value here instead (the main menu's 0x3C blink countdown, the
       value-setup widget's choice). */
    u8 update_state;               /* 0x6C */
    u8 pad_6D[DISPLAY_OBJECT_RECORD_SIZE - 0x6D];
} DisplayObject;

/* Two functions reach past this record, and it is not a mistake in
   either of them, so do not "fix" the size to accommodate them
   without settling what follows.

   The 0x70 stride is not merely asserted, it is fixed by the layout:
   gDisplayObject_aPool holds DISPLAY_OBJECT_POOL_CAPACITY (96) entries, and
   0x800EFE48 + 96 * 0x70 is 0x800F2848, which is exactly where
   gViewState_Current begins. There is no room for a larger record.

   Yet DisplayObject_RenderTexturedGouraudQuadList walks this pool with that
   stride and then tests object[0x72] as a flag, reading a second vertex set from
   0x58, 0x64, 0x68 and 0x6C when it is set; and
   DisplayObject_InitializeTexturedGouraudQuad in display_object_helpers.c writes object[0x72]. Both
   land two bytes past the record, which is `next` of the following
   entry.

   What that means is not established here. The sibling renderer
   DisplayObject_RenderGouraudQuadList gates its second submission on
   object[0x5A] instead, so the two list heads (gDisplayObject_awListHeads[4] and [5]) may simply
   carry differently shaped payloads. Recorded as an open question rather
   than guessed at.

   Neither reach stops the file converting. The renderer reads object[0x72]
   as (u8)object[1].next, the low byte of the following entry's `next`, which
   compiles to the same lbu; DisplayObject_InitializeTexturedGouraudQuad keeps
   the byte store ((u8 *)object)[0x72], and the second vertex set is reached from the members
   its words begin at: *(s32 *)&object->field_58, &object->fraction_z and so on,
   since those words span members no union here pairs.

   The scratchpad in both renderers is typed as its libgpu primitive paired
   with the primitive's word layout (DisplayObjectG4Packet and
   DisplayObjectGT4Packet in display_object_system.h). With the packet
   stores struct references too, GCC 2.8.1 keeps every member read from the
   object after the store before it, so the reads are ordinary member reads
   (field_2C.word and so on). */

typedef char DisplayObject_size_must_match_record_size[
    sizeof(DisplayObject) == DISPLAY_OBJECT_RECORD_SIZE ? 1 : -1
];
typedef char DisplayObject_rotation_z_must_be_at_0x22[
    OFFSETOF(DisplayObject, field_20.h.rotation_z) == 0x22 ? 1 : -1
];
typedef char DisplayObject_update_must_be_at_0x24[
    OFFSETOF(DisplayObject, update) == 0x24 ? 1 : -1
];
typedef char DisplayObject_position_must_be_at_0x28[
    OFFSETOF(DisplayObject, position) == 0x28 ? 1 : -1
];
typedef char DisplayObject_size_h_must_be_at_0x3E[
    OFFSETOF(DisplayObject, field_3C.size.h) == 0x3E ? 1 : -1
];
typedef char DisplayObject_clut_y_must_be_at_0x42[
    OFFSETOF(DisplayObject, field_40.clut.y) == 0x42 ? 1 : -1
];
typedef char DisplayObject_pivot_y_must_be_at_0x4A[
    OFFSETOF(DisplayObject, field_48.pivot.y) == 0x4A ? 1 : -1
];
typedef char DisplayObject_field_3A_must_be_at_0x3A[
    OFFSETOF(DisplayObject, field_38.h.field_3A) == 0x3A ? 1 : -1
];
typedef char DisplayObject_field_52_must_be_at_0x52[
    OFFSETOF(DisplayObject, field_50.h.field_52) == 0x52 ? 1 : -1
];
typedef char DisplayObject_field_58_must_be_at_0x58[
    OFFSETOF(DisplayObject, field_58) == 0x58 ? 1 : -1
];
typedef char DisplayObject_fraction_z_must_be_at_0x64[
    OFFSETOF(DisplayObject, fraction_z) == 0x64 ? 1 : -1
];
typedef char DisplayObject_subdivisions_must_be_at_0x65[
    OFFSETOF(DisplayObject, subdivisions) == 0x65 ? 1 : -1
];
typedef char DisplayObject_rgb_must_be_at_0x0C[
    OFFSETOF(DisplayObject, rgb) == 0x0C && sizeof(((DisplayObject *)0)->rgb) == 4 ? 1 : -1
];
typedef char DisplayObject_back_face_handler_must_be_at_0x10[
    OFFSETOF(DisplayObject, back_face_handler) == 0x10 ? 1 : -1
];
typedef char DisplayObject_resource_uv_must_be_at_0x5E[
    OFFSETOF(DisplayObject, resource_uv) == 0x5E ? 1 : -1
];
typedef char DisplayObject_texture_page_must_be_at_0x66[
    OFFSETOF(DisplayObject, texture_page) == 0x66 ? 1 : -1
];

/* The DISPLAY_OBJECT_LIST_COUNT list heads, immediately below the pool.
 *
 * Each entry is the index of the first object on one list, or -1 for an empty
 * list; an object's own `next` continues the chain.  DisplayObject_ResetPool
 * writes -1 through all DISPLAY_OBJECT_LIST_COUNT of them with a single s16
 * cursor started at this address, which is what says the seven halfwords are
 * one array rather than seven objects that happen to be adjacent.
 */
/* The current object's render-retry request. CardPreview_UpdateVariant sets
 * it to 1 after changing an object's config. DisplayObject_RenderSpriteList clears it
 * before that projection and, unless the projection failed, `continue`s its
 * inner while (1) to re-run the SAME object (the next index was already
 * saved, so the list is not rewound). DisplayObject_RenderSpriteSheet's
 * `goto retry` likewise rebuilds only the current object's setup, and
 * DisplayObject_RenderSpriteStrips clears it without testing it. It is
 * declared beside the list state these renderers walk rather than beside
 * either renderer. */
extern s32 gDisplayObject_fRenderRetry;

extern s16 gDisplayObject_awListHeads[DISPLAY_OBJECT_LIST_COUNT];

/* The per-list key array, walked in lockstep with the list heads above.
 *
 * DisplayObject_ResetPool advances one pointer into each and writes -1 through
 * both for DISPLAY_OBJECT_LIST_COUNT iterations, which is what fixes this
 * length, and display_object_core.c stores into it by list key. The element
 * type is s16 by the same evidence: that store is gDisplayObject_awListKeys[key] = index, and
 * the reset walk uses an s16 *.
 *
 * DisplayObject_MoveToListHead in display_object_core.c indexes this typed
 * array and gDisplayObject_awListHeads directly by list key. */
extern s16 gDisplayObject_awListKeys[DISPLAY_OBJECT_LIST_COUNT];

/* Entries from DISPLAY_OBJECT_RESERVED_CAPACITY on are the allocatable tail
 * that DisplayObject_FindFreeGeneralSlot scans. */
extern DisplayObject gDisplayObject_aPool[DISPLAY_OBJECT_POOL_CAPACITY];

/* Two counters the pool keeps beside the tables above. DisplayObject_AcquireSlot bumps
 * gDisplayObject_wAcquireCount once it has picked a slot, after reading the slot's
 * ALLOCATED flag and before testing it, so it counts every allocation
 * request that reached a slot, initialised or not; DisplayObject_Release bumps
 * gDisplayObject_wUnlinkCount after reading the slot's previous and next links
 * (display_object_core.h calls it the unlink counter). DisplayObject_ResetPool
 * stores 0 into both beside its -1 fill of gDisplayObject_awListHeads and gDisplayObject_awListKeys.
 * Nothing else in C or in the resident listings touches either; both
 * reads are lhu, so u16. Initial value not read.
 *
 * Every retail access is gp-relative (lhu/addiu/sh in DisplayObject_Release.s:4-8
 * and DisplayObject_AcquireSlot.s:16-20, sh $zero in DisplayObject_ResetPool.s:10-11), so the
 * plain halfword serves both the per-slot and whole-pool paths in
 * display_object_core.c. */
extern u16 gDisplayObject_wUnlinkCount;
extern u16 gDisplayObject_wAcquireCount;

/* One of the per-frame update callbacks installed into DisplayObject::update.
 * It orbits an object around the base position it keeps at field_2C/field_2E,
 * stepping the angle at field_2A by 0x30 a frame and taking the radius from
 * field_28, which doubles as a countdown and decays by 2 each frame. When that
 * reaches zero it clears update_state and update and snaps xy back to the
 * base in one word. While bit 0 of gGraphics_dwSyncedFrameCount is set it also spawns a
 * companion object at the current position each frame.
 *
 * Declared here because this header owns both halves of its contract: the
 * DisplayObject it takes, and the DisplayObjectCallback typedef
 * DuelScene_UpdateResultOutro installs it as. DuelScene_UpdateResultOutro's old file held
 * the only declaration and did not call the function itself, so installing it
 * was the whole use -- the declaration has to match for the address to be taken.
 *
 * This names one callback and claims nothing about the others that share the
 * slot; DisplayObject_FadeBrightnessAndRelease in display_object_system.h is
 * another. */

#endif
