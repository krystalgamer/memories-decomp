#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_SYSTEM_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_SYSTEM_H
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "display_asset_banks.h"
#include "../ygo_types.h"
#include "sprite_primitive.h"
#include "display_object_layout.h"

/* The two flat lights at 0x80090FCC, declared with the type GsSetFlatLight
 * takes rather than as byte arrays its caller has to cast: one along +y at
 * 0x60 grey and one along -y at 0x30 grey. The display-object runtime installs
 * them as GsSetFlatLight 0 and 1/2. */
extern GsF_LIGHT gDisplayObject_FlatLightDown;
extern GsF_LIGHT gDisplayObject_FlatLightUp;

extern volatile u16 gDisplayObject_awOrderingTableDepthBases[4];

/* The per-list display object renderers at 0x80090FB0.
 *
 * Sized, unlike the other tables carved alongside it. DISPLAY_OBJECT_LIST_COUNT
 * is seven, equals the entry count, and is the same constant that bounds the
 * list heads in display_object.h and DisplayObject_ResetPool's loop. */
extern void (*gDisplayObject_ListRenderers[DISPLAY_OBJECT_LIST_COUNT])(void);

/* Renders the untextured Gouraud-quad list in renderer-table slot 4. */
void DisplayObject_RenderGouraudQuadList(void);

/* Renders the textured Gouraud-quad list in renderer-table slot 5. */
void DisplayObject_RenderTexturedGouraudQuadList(void);

/* The canonical record, declared in display_object.h. Named rather than
 * included; every caller of the functions below already has the full record. */
struct DisplayObject;

/* Clears bit 0x10 of the flags halfword and stores `value` into the
 * resource_variant selector byte at 0x69. The parameter is s32 rather than
 * the u8 the definition used to spell: the body stores it with an sb, which
 * truncates anyway, so widening it costs nothing -- while a u8 parameter
 * costs an andi at every call site whose argument range the compiler cannot
 * prove. See notes/research/matching-evidence.md. */
void DisplayObject_SetResourceVariant(struct DisplayObject *object, s32 value);

/* Changes the third resource selector only when needed and clears the
 * resolution flag so the display-object runtime rebuilds its resource. */
void DisplayObject_UpdateResourceVariant(
    struct DisplayObject *object,
    s32 value
);
void DisplayObject_SetResourcePath(
    struct DisplayObject *object, u8 resource_root_index, u8 resource_branch_index, u8 resource_variant
);

/* Writes the three resource selectors plus packed texture-page/UV and
 * palette settings, and returns the object. Every caller discards the result,
 * which is how three of the four declared it as returning void.
 *
 * It takes the canonical record, whose offset members retain overlapping
 * meanings. The two callers that still hand it a byte
 * cursor keep their own view and name the record at the call: shop.c its
 * byte arithmetic, DuelScene_UpdateHandActions its hand-card view. */
void *DisplayObject_ConfigureSpriteResource(struct DisplayObject *object, s32 resource_root_index,
                    s32 resource_branch_index, s32 resource_variant, s32 texture_config, s32 palette_config);

struct DisplayObject *DisplayObject_ConfigureScreenSprite(
    struct DisplayObject *object,
    s32 x,
    s32 y,
    s32 width,
    s32 height,
    s32 texture_u,
    s32 texture_v,
    s32 texture_page,
    s32 clut_x,
    s32 clut_y);

struct DisplayObject;

extern u8 tail_data_start[];

s32 DisplayObject_FindFreeGeneralSlot(void);
s32 DisplayObject_FindFreeSlot(void);
void *DisplayObject_AcquireSlot(s32 index, s32 key);

void DisplayObject_Release(struct DisplayObject *slot);
void DisplayObject_MoveToListHead(struct DisplayObject *slot, s32 key);

void DisplayObject_ReleaseIfPresent(void *object);

void DisplayObject_Reset(void);
void DisplayObject_ConfigureSpriteAtPosition(
    void *object, s32 x, s32 y, s32 resource_root_index, s32 resource_branch_index,
    s32 resource_variant, s32 texture_config, s32 palette_config
);

typedef struct {
    u8 pad_00[0x54];
    u8 *base;
} DisplayObjectStream;

/* The motion view of a display object: a position triple at 0x30 with its
 * three 8.8 fraction bytes at 0x62, and a per-frame delta triple at 0x36.
 *
 * 0x36/0x38 is read two ways across the tree, and this is the second of them.
 * DisplayObject_SavePosition.h's DisplayObjectSnapshot and display_object_interpolation.h's
 * DisplayObjectPosition both treat the pair as a *saved* or *source* position
 * that eases into the live pair at 0x30/0x32. Here the same halfwords are
 * added to the position every frame, which is only meaningful as a velocity.
 *
 * Neither view is wrong and neither generalises: the memory belongs to
 * whichever motion path owns the object, exactly as 0x44 and 0x4C in
 * display_object.h are a colour to one renderer and a scale or a callback to
 * another. name_entry_runtime.c settles this reading for the objects
 * it drives -- it accelerates 0x38 toward 0x800 by 0x40 a frame through
 * DisplayObject_StepToward, decays 0x36 toward zero by 8 through
 * DisplayObject_StepTowardZero, then calls DisplayObject_StepPositionXY.
 * That is gravity, friction and integration, in that order. */
typedef struct {
    u8 pad_00[0x30];
    s16 x;
    s16 y;
    s16 z;
    s16 velocity_x;
    s16 velocity_y;
    s16 velocity_z;
    u8 pad_3C[0x26];
    u8 fraction_x;
    u8 fraction_y;
    u8 fraction_z;
} DisplayObjectVelocity;

/* Lives in .sdata. Declared volatile because display_object_helpers.c
   reads it through a volatile pointer; the qualifier is load-bearing
   there, not decorative.

   Indexed by a display object's ot_index: DisplayObject_SelectOrderingTable1 sets 1 and reads
   [1], DisplayObject_SelectOrderingTable3 sets 3 and reads [3], DisplayObject_AcquireSlot reads
   [ot_index]. [1] and [3] used to be spelled D_8009AF76 and D_8009AF7A
   privately. Seven byte readers (dialog_transition.c, build_deck.c,
   Dialog_UpdateMessageTransition, text_apply_fade_command.c,
   src/game/duel_effect_states.c and
   DuelEffect_CreateChannel) take the low byte of [1] through %hi/%lo,
   outside small data; dialog_transition.c, build_deck.c
   and src/game/duel_effect_states.c define the arm below for that. */

/* Returns the first allocated display object carrying the requested tag. */
void *DisplayObject_FindAllocatedByTag(s32 value);

void DisplayObject_InitializeGouraudQuad(
    DisplayObject *object,
    s32 has_secondary_quad
);
void DisplayObject_InitializeTexturedGouraudQuad(
    DisplayObject *object,
    s32 has_secondary_quad
);
void DisplayObject_ConfigureSpriteWithResource(
    DisplayObject *object,
    s32 resource_root_index,
    s32 resource_branch_index,
    s32 resource_variant,
    s32 texture_config,
    s32 palette_config,
    void *resource
);
/* resource is a pointer, not an integer. Every caller passes one -- the
   overlays pass gDisplay_abResourceBank0 / gDisplay_abResourceBank1, the resident callers pass a spec or
   table address -- and the function stores it straight into the object's
   0x54 word, which DisplayObject already declares void *.
   DisplayObject_ConfigureSpriteWithResource just above does the same job
   through the canonical record. */
void DisplayObject_ConfigureSpriteAtPositionWithResource(
    DisplayObject *object,
    s32 x,
    s32 y,
    s32 resource_root_index,
    s32 resource_branch_index,
    s32 resource_variant,
    s32 texture_config,
    s32 palette_config,
    void *resource
);
/* Stores the object's depth order and returns its resulting ordering-table
 * depth. It takes the canonical record: it reads ot_index and writes
 * depth_offset and ot_depth, and two thirds of its call sites were already
 * holding a DisplayObject and casting it down. The sites that do keep a byte
 * cursor -- five candidates and three resident units that walk the same
 * object by offset either side of the call -- name the record at the call
 * instead. */
s32 DisplayObject_SetDepthOffset(DisplayObject *object, s8 value);
void DisplayObject_SelectOrderingTable1(DisplayObject *object);
void DisplayObject_SelectOrderingTable3(DisplayObject *object);
int DisplayObject_RunUpdateAndCheckRenderable(DisplayObject *object);
u32 DisplayObjectStream_ReadU16LE(const u8 *data);
u8 *DisplayObjectStream_ResolveOffset(
    DisplayObjectStream *object,
    const u8 *data
);
void DisplayObject_ResetVelocity(DisplayObjectVelocity *object);
void DisplayObject_StepPositionX(DisplayObjectVelocity *object);
void DisplayObject_StepPositionY(DisplayObjectVelocity *object);
void DisplayObject_StepPositionZ(DisplayObjectVelocity *object);
void DisplayObject_StepPositionXY(DisplayObjectVelocity *object);
void DisplayObject_StepPositionXYZ(DisplayObjectVelocity *object);
s32 DisplayObject_StepToward(s32 value, s32 target, s32 step);
s32 DisplayObject_StepTowardZero(s32 value, s32 step);

/* Draws an elliptical spotlight mask of eight gouraud quads around the
 * object's +0x30 centre. Nothing calls it directly: dialog_transition.c and
 * the overworld's set_location.c store its address in a display object's
 * field_4C, and the object's draw pass supplies both arguments. */
void DisplayObject_RenderSpotlightMask(DisplayObject *object, GsOT *ot);

/* Saves the live output position before one of the interpolation helpers
 * starts moving it. The names follow DisplayObjectPosition below: the
 * helpers read the saved pair as their start point and write the output. */
typedef struct {
    u8 pad_00[0x30];
    u16 out_x;
    u16 out_y;
    u8 pad_34[2];
    u16 x;
    u16 y;
} DisplayObjectSnapshot;

/* Fields the interpolation helpers read and write while easing an object
 * between its current pair at 0x36/0x38 and a target, leaving the result in
 * the pair at 0x30/0x32. Other translation units describe the same memory
 * through their own wider views.
 *
 * The halfword at 0x34 is padding here in the same sense as everywhere else:
 * these two functions do not touch it. That is not evidence that the memory
 * is unused, and it must not be treated as free space. The canonical record
 * in display_object.h spans it with pad_34[8], and DisplayObjectSnapshot
 * above spans it with pad_34[2]. */
typedef struct {
    u8 pad_00[0x30];
    s16 out_x;
    s16 out_y;
    u8 pad_34[0x02];
    s16 x;
    s16 y;
} DisplayObjectPosition;

void DisplayObject_SavePosition(DisplayObjectSnapshot *object);
void DisplayObject_InterpolatePositionCosine(
    DisplayObjectPosition *object, s32 target_x, s32 target_y, s32 phase);
void Widget_SlideSine(
    DisplayObjectPosition *object,
    int target_x,
    int target_y,
    int phase);

/* Narrow lifecycle view for the initialization marker and uniform RGB fade. */
typedef struct {
    u8 pad_00[0x0C];
    u8 red;
    u8 green;
    u8 blue;
    u8 pad_0F[0x51];
    s16 fade_step;
    u8 pad_62[0x0A];
    u8 flags;
} DisplayObjectLifecycle;

#define DISPLAY_OBJECT_LIFECYCLE_OFFSET(member) \
    ((u32)&((DisplayObjectLifecycle *)0)->member)
typedef char DisplayObjectLifecycle_rgb_offsets_must_match[
    DISPLAY_OBJECT_LIFECYCLE_OFFSET(red) == 0x0C &&
    DISPLAY_OBJECT_LIFECYCLE_OFFSET(green) == 0x0D &&
    DISPLAY_OBJECT_LIFECYCLE_OFFSET(blue) == 0x0E ? 1 : -1
];
typedef char DisplayObjectLifecycle_fade_step_offset_must_be_0x60[
    DISPLAY_OBJECT_LIFECYCLE_OFFSET(fade_step) == 0x60 ? 1 : -1
];
typedef char DisplayObjectLifecycle_flags_offset_must_be_0x6C[
    DISPLAY_OBJECT_LIFECYCLE_OFFSET(flags) == 0x6C ? 1 : -1
];
#undef DISPLAY_OBJECT_LIFECYCLE_OFFSET

s32 DisplayObject_MarkInitialized(DisplayObject *object);
void DisplayObject_FadeBrightnessAndRelease(DisplayObjectLifecycle *object);

/* The screen origin DisplayObject_SubmitPacket subtracts from every vertex before the
 * perspective transform, followed by the subdivision count for textured
 * quads. A zero count sorts the quad as it is; anything else sends it
 * through DivideFT4. */
typedef struct {
    s16 x;
    s16 y;
    s32 divisions;
} DisplayObjectPacketOrigin;

/* The packet dispatcher receives a sprite pointer in the sprite renderers,
 * while the Gouraud-quad renderers pass the display object's raw attribute
 * word. The definition in display_object_system.c reads the packet and the
 * extra record as bytes because their type depends on the dispatch case, so
 * every caller passes byte pointers. For the card-list renderer the extra is
 * the DisplayObjectPacketOrigin record the dispatcher consumes, and its
 * packet is the complete SDK textured quad. */
/* ot is the selected GsOT descriptor, not a depth, table index or texture.
 * Word-shaped wrapper APIs keep explicit pointer conversions at their calls. */
void DisplayObject_SubmitPacket(SpritePrim *sprite, u8 *packet, GsOT *ot, s32 mode, u8 *extra);

/* Input screen origin narrowed for packet vertices, followed by the quad
   subdivision count (subdivisions for objects, zero for packed rotation).
   These are not transformed vertices, depth or MAC0. The separate packet
   origin view has the same byte roles; no API/type unification is implied. */
struct ProjectionOut {
    s16 x;
    s16 y;
    s32 divisions;
};

/* The record DisplayObject_ProjectRotationFacing projects is a display
   object. Only the tag is named here rather than including display_object.h:
   a pointer parameter needs no more, and an include on a header this widely
   consumed can reach an overlay source that carries a deliberate local
   declaration of its own. */
struct DisplayObject;

/* The facing test the Gouraud-quad list renderers run before submitting an
 * object with DISPLAY_OBJECT_FLAG_CLIP_TEST. It stores the input origin's
 * low halfwords in `out`, but sets the full input as the GTE geometry offset
 * with the default projection
 * distance, and builds a ZYX rotation from the three bytes of the packed
 * field_20 word (each scaled by 16), with translation (0, 0, projection), as
 * the local-screen matrix. It then projects a reference triangle at the
 * origin, +X and +Y through RotAverageNclip3 and returns the signed NCLIP
 * area from MAC0, not OTZ. A non-positive result means the rotated plane
 * faces away, and the caller skips the object.
 *
 * packed_rotation carries the three rotation bytes from field_20.word,
 * not the display object's attribute flags; its high byte is unused.
 * The first parameter remains u32. The matching Gouraud-quad list renderers
 * formerly declared it s32 and the fourth parameter u8 *, holding the
 * scratchpad address at 0x1F800398 as a byte pointer; both call sites now
 * state that conversion instead. */
s32 DisplayObject_ProjectPackedRotationFacing(u32 packed_rotation, s32 x, s32 y,
                                              struct ProjectionOut *out);

/* The same facing test for one display object: the rotation comes from its
   field_20/21/22 bytes, subdivisions goes into `out->divisions` (the packed
   variant stores 0), and the matrix is scaled by field_44.scale unless
   GsROTOFF is set.
   A nonzero back_face_handler gates the back-face handling. A negative (code-address)
   back_face_handler is first called with the result, whatever its sign. Then, for any
   nonzero back_face_handler, a negative result mirrors the X and Z matrix columns and
   reprojects, which appears to draw the back face mirrored. With back_face_handler
   zero a negative result is returned unchanged. It returns the signed MAC0
   result. The return type matters: one caller used to declare this `void`,
   which is invisible on this ABI because the result comes back in $v0 and
   nothing forces the caller to read it. */
s32 DisplayObject_ProjectRotationFacing(struct DisplayObject *obj, s32 x,
                                        s32 y, struct ProjectionOut *out);

void DisplayObject_RenderFrame(void);

/* The gDisplayObject_ListRenderers entry for the list rooted at gDisplayObject_awListHeads[1].
 * Runs each object's callback, then for every renderable object fills the
 * sprite primitive in the scratchpad at 0x1F800320, offsets it by the viewport
 * origin unless the object's bit 3 is set, and submits it through
 * DisplayObject_SubmitPacket. With bit 2 the position goes through
 * DisplayObject_ProjectRotationFacing's clip test first, which may ask through
 * gDisplayObject_fRenderRetry for the object to be run again,
 * and the packet built is the nine-word semi-transparent quad at 0x1F800344
 * instead. */
void DisplayObject_RenderSpriteList(void);

/* The gDisplayObject_ListRenderers entry for the list rooted at
 * gDisplayObject_awListHeads[2]. Runs each object's callback, updates its command stream, and
 * renders the eligible sprite sheets. */
void DisplayObject_RenderSpriteSheetList(void);

/* Submits one display object as a sprite in vertical strips of up to 64
 * pixels: `ordering_table` is the selected GsOT descriptor, not a texture.
 * `mode_arg` carries the halfword at
 * +0x14. DisplayObject_RenderSpriteStripList below is its only caller, and
 * reaches it for every renderable object in the update list.
 *
 * The third parameter is s32 and the definition narrows it to a u16 local.
 * The caller loads the halfword signed (lh), and a u16 parameter turns that
 * into lhu. The callee masks with andi 0xFFFF either way, so the narrowing
 * belongs on its side of the call. */
void DisplayObject_RenderSpriteStrips(DisplayObject *object, GsOT *ordering_table, s32 mode_arg);

/* Three of the seven entries in gDisplayObject_ListRenderers. Each walks one
 * display object list from its head in gDisplayObject_awListHeads, runs every object's update
 * callback, and differs only in which list it takes and what it does afterwards:
 *
 *   DisplayObject_RunUpdateCallbackList  list 0, callback-only
 *   DisplayObject_RenderSpriteStripList  list 3
 *   DisplayObject_RunSecondaryCallbackList  list 6
 *
 * The list-3 walker submits sprite strips, and the list-6 walker invokes the
 * object's second callback slot at +0x4C. All three are reached only as
 * entries of that table, so the table file is their only consumer and
 * declared all three itself before this header. */
void DisplayObject_RenderSpriteStripList(void);
void DisplayObject_RunUpdateCallbackList(void);
void DisplayObject_RunSecondaryCallbackList(void);

/* A display object's sprite sheet, reached through its +0x4C word: a
 * four-byte header followed by `count` part records. */
typedef struct {
    u8 count;
    u8 flags;      /* 0x10: 10-bit offsets; 0xE0: parts carry their own page */
    u8 tpage;      /* added to the object's +0x66 page, doubled when wide */
    u8 clut;       /* low nibble: clut x step (x16); high nibble: clut y step */
} SpriteSheetHeader;

/* One part of the sheet, six bytes. `cell` packs the texture position (bits
 * 0-4 u and 5-9 v, in units of 8), the part's page offset (bits 10-12), the
 * mirror bit 0x2000 and bits 8-9 of dx (bits 14-15). `size` packs the clut
 * step of paged parts (low bits), the width (bits 5-8) and height (bits
 * 9-12) as (n + 1) * 8, and bits 8-9 of dy (bits 14-15). */
typedef struct {
    s8 dx;
    s8 dy;
    u16 cell;
    u16 size;
} SpriteSheetPart;

/* Per-object working state DisplayObject_RenderSpriteSheet keeps in the
 * scratchpad at 0x1F800378. `proj` is the projection
 * DisplayObject_ProjectRotationFacing fills and DisplayObject_SubmitPacket
 * takes as its origin record. */
typedef struct {
    u32 pad_00;
    u32 attribute;
    u16 cell;
    u16 size;
    u16 cx;
    u16 cy;
    u16 u;
    u16 v;
    s16 mx;
    s16 my;
    u16 tpage;
    u16 clip;
    s32 wide;
    struct ProjectionOut proj;
    s32 mode;
} SpriteSheetWork;

/* The rest of that state, at 0x1F8003A4. */
typedef struct {
    s32 pad_00;
    s32 mirror;
    s32 flip;
    s32 screen;
    s32 flags;
} SpriteSheetState;

/* Draws a display object whose image is a sprite sheet: every part becomes
 * one GsSPRITE-shaped primitive in the scratchpad at 0x1F800320, positioned
 * relative to the object and submitted through DisplayObject_SubmitPacket. */
void DisplayObject_RenderSpriteSheet(DisplayObject *object, GsOT *ordering_table, s32 depth);

/* Steps the object's command stream. Only object is read; its caller,
 * DisplayObject_UpdateCommandStream, also passes the script and attribute
 * it resolved and the object again. */
void DisplayObjectStream_ReadNextCommand(
    DisplayObjectStreamState *object, s32 script, s32 attribute, DisplayObject *owner
);

s32 DisplayObjectStream_Stop(
    DisplayObjectStreamState *object,
    const u8 *data
);
s32 DisplayObjectStream_ResetOffset(
    DisplayObjectStreamState *object,
    const u8 *data
);
s32 DisplayObjectStream_Noop(
    DisplayObjectStreamState *object,
    const u8 *data
);
s32 DisplayObjectStream_JumpToOffset(
    DisplayObjectStreamState *object,
    const u8 *data
);
s32 DisplayObjectStream_ToggleFlagAndJumpToOffset(
    DisplayObjectStreamState *object,
    const u8 *data
);

/* Display-object stream opcode 0xFA, entry 5 of gDisplayObjectStream_apfnCommandHandlers
 * (display_object_stream_read_next_command.c). `data` holds a count byte followed by that many
 * little-endian halfword offsets. One is picked with rand(), and the stream
 * cursor at +0x50 is set to the stream base at +0x54 plus that offset, so the
 * opcode is a random jump. Also zeroes the halfword at +0x58. Returns 1. */
s32 DisplayObjectStream_JumpToRandomOffset(
    DisplayObjectStreamState *object,
    const u8 *data
);

/* Display-object stream opcode 0xF9, entry 6 of gDisplayObjectStream_apfnCommandHandlers
 * (display_object_stream_read_next_command.c). It reads the rotation byte, a signed y pivot, and
 * a little-endian x pivot from four operand bytes. It also raises GsROTOFF in
 * the attribute word `attribute` and advances `current` by 4. Returns 1. */
s32 DisplayObjectStream_ConfigureRotation(
    DisplayObjectStreamState *object,
    const u8 *data
);

void DisplayObject_UpdateCommandStream(DisplayObject *object);
#endif
