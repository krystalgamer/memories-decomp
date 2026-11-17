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
    DisplayObject *object,
    const u8 *data
);
void DisplayObject_ResetVelocity(DisplayObject *object);
void DisplayObject_StepPositionX(DisplayObject *object);
void DisplayObject_StepPositionY(DisplayObject *object);
void DisplayObject_StepPositionZ(DisplayObject *object);
void DisplayObject_StepPositionXY(DisplayObject *object);
void DisplayObject_StepPositionXYZ(DisplayObject *object);
s32 DisplayObject_StepToward(s32 value, s32 target, s32 step);
s32 DisplayObject_StepTowardZero(s32 value, s32 step);

/* Draws an elliptical spotlight mask of eight gouraud quads around the
 * object's +0x30 centre. Nothing calls it directly: dialog_transition.c and
 * the overworld's set_location.c store its address in a display object's
 * field_4C, and the object's draw pass supplies both arguments. */
void DisplayObject_RenderSpotlightMask(DisplayObject *object, GsOT *ot);

/* Saves the live output position at xy into field_34/field_38's `ease`
 * pair before one of the interpolation helpers starts moving it. The
 * helpers ease from that pair toward a target and write xy. */
void DisplayObject_SavePosition(DisplayObject *object);
void DisplayObject_InterpolatePositionCosine(
    DisplayObject *object, s32 target_x, s32 target_y, s32 phase);
void Widget_SlideSine(
    DisplayObject *object,
    int target_x,
    int target_y,
    int phase);

s32 DisplayObject_MarkInitialized(DisplayObject *object);
/* An update callback: steps the colour's red/green/blue down together by
 * field_60 each frame and releases the object once it reaches black. */
void DisplayObject_FadeBrightnessAndRelease(DisplayObject *object);

/* The screen origin DisplayObject_SubmitPacket subtracts from every vertex before the
 * perspective transform, followed by the subdivision count for textured
 * quads. A zero count sorts the quad as it is; anything else sends it
 * through DivideFT4. The projection helpers below fill the same record:
 * the input screen origin narrowed for packet vertices and the quad
 * subdivision count (subdivisions for objects, zero for packed rotation).
 * These are not transformed vertices, depth or MAC0. */
typedef struct ProjectionOut {
    s16 x;
    s16 y;
    s32 divisions;
} DisplayObjectPacketOrigin;

/* The sprite renderers' clip work area in the scratchpad at 0x1F800378:
 * `out` is the projection DisplayObject_ProjectRotationFacing fills and
 * DisplayObject_SubmitPacket takes as its origin (0x1F800398), and
 * DisplayObject_RenderSpriteStrips keeps its GsROTOFF state in `flag`. */
typedef struct {
    u32 unk0;
    u32 flag;
    u8 pad8[0x18];
    struct ProjectionOut out;
} ClipState;

typedef char ClipState_out_offset_must_be_0x20[
    OFFSETOF(ClipState, out) == 0x20 ? 1 : -1
];

/* The scratchpad packets the display-object renderers build at 0x1F800344.
 * The Gouraud-quad lists copy each colour and vertex in as one word from the
 * display object (and each GT4 u/v pair as one halfword), and the sprite
 * renderers' clip-test quad takes the sprite's colour word, so each
 * primitive is paired with its word layout. The x/y words are `long`, the
 * type RotAverageNclip4 stores screen coordinates through; each FT4 uvN
 * word is the u/v pair with the clut or tpage halfword above it, as
 * DivideFT4 takes them. */
typedef union {
    POLY_FT4 prim;
    struct {
        u32 tag;
        u32 rgb0;
        long xy0;
        u32 uv0;
        long xy1;
        u32 uv1;
        long xy2;
        u32 uv2;
        long xy3;
        u32 uv3;
    } w;
} DisplayObjectFT4Packet;

typedef union {
    POLY_G4 prim;
    struct {
        u32 tag;
        u32 rgb0;
        long xy0;
        u32 rgb1;
        long xy1;
        u32 rgb2;
        long xy2;
        u32 rgb3;
        long xy3;
    } w;
} DisplayObjectG4Packet;

typedef union {
    POLY_GT4 prim;
    struct {
        u32 tag;
        u32 rgb0;
        long xy0;
        u16 uv0;
        u16 clut;
        u32 rgb1;
        long xy1;
        u16 uv1;
        u16 tpage;
        u32 rgb2;
        long xy2;
        u16 uv2;
        u16 pad2;
        u32 rgb3;
        long xy3;
        u16 uv3;
        u16 pad3;
    } w;
} DisplayObjectGT4Packet;

/* The packet dispatcher receives a sprite pointer in the sprite renderers,
 * while the Gouraud-quad renderers pass the display object's raw attribute
 * word. The packet is untyped because its primitive type depends on the
 * dispatch case (POLY_G4, POLY_GT4 or POLY_FT4). */
/* ot is the selected GsOT descriptor, not a depth, table index or texture.
 * Word-shaped wrapper APIs keep explicit pointer conversions at their calls. */
void DisplayObject_SubmitPacket(SpritePrim *sprite, void *packet, GsOT *ot, s32 mode,
                                DisplayObjectPacketOrigin *origin);

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

/* The four-byte header of a display object's sprite sheet, reached through
 * its +0x4C word and followed by `count` part records. */
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

/* The sheet itself: the header and its `count` parts. */
typedef struct SpriteSheet {
    SpriteSheetHeader header;
    SpriteSheetPart parts[1];
} SpriteSheet;

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
    DisplayObject *object, s32 script, s32 attribute, DisplayObject *owner
);

s32 DisplayObjectStream_Stop(
    DisplayObject *object,
    const u8 *data
);
s32 DisplayObjectStream_ResetOffset(
    DisplayObject *object,
    const u8 *data
);
s32 DisplayObjectStream_Noop(
    DisplayObject *object,
    const u8 *data
);
s32 DisplayObjectStream_JumpToOffset(
    DisplayObject *object,
    const u8 *data
);
s32 DisplayObjectStream_ToggleFlagAndJumpToOffset(
    DisplayObject *object,
    const u8 *data
);

/* Display-object stream opcode 0xFA, entry 5 of gDisplayObjectStream_apfnCommandHandlers
 * (display_object_stream_read_next_command.c). `data` holds a count byte followed by that many
 * little-endian halfword offsets. One is picked with rand(), and the stream
 * cursor at +0x50 is set to the stream base at +0x54 plus that offset, so the
 * opcode is a random jump. Also zeroes the halfword at +0x58. Returns 1. */
s32 DisplayObjectStream_JumpToRandomOffset(
    DisplayObject *object,
    const u8 *data
);

/* Display-object stream opcode 0xF9, entry 6 of gDisplayObjectStream_apfnCommandHandlers
 * (display_object_stream_read_next_command.c). It reads the rotation byte, a signed y pivot, and
 * a little-endian x pivot from four operand bytes. It also raises GsROTOFF in
 * the attribute word `attribute` and advances `current` by 4. Returns 1. */
s32 DisplayObjectStream_ConfigureRotation(
    DisplayObject *object,
    const u8 *data
);

void DisplayObject_UpdateCommandStream(DisplayObject *object);
#endif
