# French Exodia/SU helper recovery

The special model ID `0x309` uses an SU package starting at sector 1492
with 275 sectors, rather than a normal 276-sector MODEL record. Its load
phases have counts `96,96,2,10,10,1,8,1,50,1`. The two ten-sector handler
images start at sectors 1686 and 1696 and load at `0x8013B000` and
`0x8017B000`. Each entry is load address plus four.

The dedicated controller `func_8004FE2C` in
`src/game/model_intro_controller.c` calls both entries directly with command
zero for initialization and minus one for updates. Disabled general MODEL
command words do not make these images dead. Both entries select active slot
zero, while their contexts are at `0x80136000` and `0x80176000`.

## Exact inventoried registration

| Image | Function | Bytes | Ownership |
|---|---|---:|---|
| slot 0 | `func_8013B004` | 2140 | C entry/configuration handler |
| slot 0 | `func_8013B860` | 940 | C ring helper |
| slot 0 | `func_8013BC0C` | 2976 | C spoke renderer |
| slot 1 | `func_8017B004` | 2476 | C entry/configuration handler |
| slot 1 | `func_8017B9B0` | 1088 | C ring helper |
| slot 1 | `func_8017BDF0` | 2416 | C petals renderer |
| slot 1 | `func_8017C760` | 1500 | C beam helper |

Both complete 20,480-byte images match independently extracted French SU
targets after production linking. Their SHA-256 values are:

- slot 0: `4d00af8618d3d6c6b7073af686dfda103bcf314127e14f139072d060de753812`
- slot 1: `972c95a8134b3e932fac57c92059df0c19b0392c220e0f840765b40a7b2d29af`

The source uses the named `gcc_2_8_1_g0_split` profile with GCC 2.8.1 and
MASPSX 2.81. Actual selected input objects and final section-defined function
symbols own all seven matching extents, totaling 13,536 instruction bytes.
Slot 0's three inventoried functions are now 6,056 bytes of compiled C.
Slot 1's entry, ring, petals and beam total 7,480 bytes of compiled C.
The four-byte headers and remaining tails have real generated storage
owners and exact bytes, rather than absolute aliases.

Slot 0's 14,420-byte tail and slot 1's 12,996-byte tail remain **unclassified**.
Their placement in data sections preserves bytes; it is not evidence that they
contain no executable code. Matching every inventoried function does not resolve
these tails or establish exhaustive runtime coverage.

## Layout and compiler evidence

The slot-0 entry uses a 176-byte frame and initializes a 56-byte configuration
view, sixteen 28-byte color records, one 152-byte ring, and G3/G4/GT4 packet
views. Its independently compiled state view reaches `0x4F4`; that is not an
allocation-bound claim. Configuration command zero selects part bytes
`3,0,0` and timing words `540,570,576,716,1994`. The 56-byte descriptor stride
does not establish additional legal commands or classify opaque members.
Two unused local matrices retain the stack gap without an asserted runtime role.

Nineteen recorded entry attempts preserve the original fourteen experiments,
an accepted-master recalibration, and four new source experiments. Canonical
`DVECTOR` projections retain the observed halfword accesses. An explicit
halfword X delta and full-width Y delta recover the final load widths,
register allocation and interpolation scheduling; making both deltas full-width
adds two X sign-extension instructions. The complete image is matched with
the accepted ring and spoke compiled objects, not copies of their retail code.
All 65 entry layout constants and seven resident game callees are checked
independently. The existing controller still calls both entries with commands
zero and minus one; its matching source is unchanged.

The slot-1 entry also has a 176-byte frame, but a distinct 68-byte configuration
view and a `0xCBC`-byte state access view. Its one beam, two rings and twelve
132-byte petal records begin at `0`, `0x308` and `0x438`; packet views begin at
`0xA68`, `0xA80`, `0xA9C` and `0xAC0`. These offsets do not establish context
allocation capacity. Command zero's part bytes are `34,0,0`; thirteen timing
words are `60,280,350,400,460,480,540,560,620,640,644,720,760`.

The first slot-1 recovery already had the correct 2,476-byte size and frame;
ten words differed only in the beam initializer. Writing halfword `0x278`,
then word `0x27C`, then halfword `0x27A` restores the retail induction pointer
and all instruction bytes. Uncertain initialization fields retain address-based
names, and the unused GetTPage/GetClut calls remain. All 89 additional layout
constants, 25 literal retail anchors, 26 entry call targets and 34 resident/caller
owners are independently checked. At that checkpoint the complete image retained actual compiled
ring/beam objects and the generated petals assembly object, rather than raw
copies of any of those functions. The later petals integration below replaces
that assembly object while preserving all six previously compiled C objects.

The two ring views use 152-byte records: sixteen SVECTOR points at offset zero,
inner/outer colors at 128/132 and scale at 136. The first view reaches ring
`0x1C0`, packet `0x2CC`, origin `0x3F0`, frame count/frame `0x450/0x454`,
step `0x45C`, timing pointer `0x464` and grown state `0x4DC`.

The second ring view reaches two records at `0x308`, packet `0xAF4`,
origin/direction `0xC2C/0xC34`, frame count/frame `0xC60/0xC64`,
timing `0xC74`, signed distance `0xC98` and phase `0xCA4`.
Its unused 16-byte VECTOR local retains the observed stack gap and 288-byte
frame; no runtime meaning is asserted for that local.

The beam has a 776-byte record at context offset zero. Three arrays of
seventeen SVECTORs start at `0`, `0x88` and `0x110`; packed projected
coordinates start at `0x198`, `0x1DC` and `0x220`; flags/depths start at
`0x280/0x2C4`. Its packet is at `0xAC0`, origin/direction at
`0xC2C/0xC34`, angle input at `0xC44`, frame count at `0xC60`,
step at `0xC6C`, signed distance/width at `0xC98/0xC9A`, and phase at `0xCA4`.

The spoke renderer has sixteen pairs of stack-local points and projected
coordinates, a 2,016-byte frame, and two mirrored textured quads per spoke.
Its context view reaches packet `0x298`, origin `0x3F0`, direction `0x438`,
frame count/frame `0x450/0x454`, timing `0x464`, scale `0x488`, width `0x490`
and angle `0x494`. Its unused 256-byte SVECTOR array preserves the observed
stack gap; no runtime role is assigned to that local.

Six recorded experiments recover all 2,976 renderer bytes. Branch-local
angles, index-first point expressions and fixed-point shift ordering retain
the observed geometry sequence. Six byte-color locals preserve register
pressure. Packed signed 32-bit projections, the loop-indexed final endpoint,
and explicit X-before-Y delta locals recover the final nine differing words.
Canonical DVECTOR arrays were tested and rejected because old GCC emitted
different extensions and loads; canonical SDK types were not altered.

All 256 layout constants are independently compiled with the target compiler
against the promoted headers. These are access views, not proof of allocation
bounds. Opaque incoming context pointers, timing-first comparisons,
index-first beam point expressions, and multiplication by 128 preserve
observed register allocation, scheduling and load widths.
The [attempt ledger](exodia-helpers-attempts.csv) records entry and helper experiments,
including compilation and binding failures before exact results.

Canonical SDK declarations are reused. Missing SDK identities were independently
resolved in hash-verified US and French residents through the pinned Psy-Q 4.6
catalogue, digest
`81c20bb6e67db52fe39a1b26bbaf7700a4aad1f6849433642f53df9641b22b81`.
Unique object signatures include GS_134 (GsGetLs), MTX_009 (ReadRotMatrix),
CMB_00 (RotTransPers4), SMP_03 (RotTransPers3), and MTX_09 (SetRotMatrix).
The spoke proof independently checks RotTransPers, RotMatrix, ScaleMatrix,
GsSetLsMatrix and GsSortPoly as well. The complete 16-byte address-named
`func_80058F10` getter matches between hash-verified French and Spanish
residents; its withdrawn GsGetWorkBase identification is not revived.
Catalogue masks establish SDK identity only; game C and full-image acceptance
compare every byte without masks. Actual call relocations use independently
verified French resident bindings, not overlay aliases. The entry adds
seventeen resident bindings to the fourteen previously accepted bindings.
Its SDK proof also checks GetTPage, GetClut, SetShadeTex, SetPolyG3 and Square0.
In particular, the constructor at `0x80082E48` is SetPolyG3, not SetPolyF4.
The second entry's additional constructor at `0x80082E88` is independently
identified as SetPolyF4 through `P16.OBJ`. Its packet-copy and vector-query
calls use the existing `func_8005B260` and `func_80059B90` implementations
at French addresses `0x8004D5B8` and `0x8005CC98`. The shared binding table now
contains 34 resident bindings, retaining every previously accepted binding.

## Exact petals template recovery

The remaining slot-1 helper at `0x8017BDF0` now owns all 2,416 instruction
bytes through `src/overlays/model_exodia/petals.c`, with the observed
304-byte frame. Its dedicated header reuses the existing
`Variant418SpiralArm` access view and canonical SDK declarations; the entry
imports that header instead of retaining a separate helper prototype.
No shared layout, compiler profile or resident binding changes.

Ten ledger rows preserve the old scalar experiments and the new recovery.
Unused products, signed fixed-point quotients, halfword temporaries and
reused distance/spread locals all lose the target's two products and emit
2,368 bytes. The named no-cse-follow-jumps probe does not change that result.
Assigning the products to subsequently overwritten scale-vector fields emits
unwanted stack stores and 2,424 bytes; that candidate is rejected.
Distances now include the longer of actual and target bodies, so the old
541-common-word differences are recorded as 553, including the missing
48 bytes.

The accepted header-428 spiral supplies a distinct, measured template:
two negative-product tests assigning zero to the length local before the
loops overwrite it. GCC 2.8.1 retains the two multiply/result pairs but removes
the dead assignments, just as in this target. The first such Exodia candidate
is still nonexact at 2,420 bytes. Moving arm and packet pointer setup before
the products, as observed at image offsets `0xE44` and `0xE4C`, restores every
instruction. This is not a forced register, artificial store, or invented
dependency. The authoritative profile remains `gcc_2_8_1_g0_split`.

Forty-one independently target-compiled constants establish the reused
132-byte arm view and its agreement with the entry's color record. Twelve
arms occupy context `[0x438,0xA68)`, ending exactly at the flat packet.
Each arm has two original and displaced points, packed projections, angles,
widths, two color rows, flags at `0x64`, depths at `0x6C`, and halfword screen
offsets at `0x74/0x78`. The 52-byte textured packet occupies
`[0xAC0,0xAF4)` and is intentionally reused by the sequential beam helper.
The coordinate object occupies stack `[0x80,0xD0)`, followed by four-byte
perspective and flag outputs at `0xD0/0xD4`, before the context spill at `0xD8`.

Forty-one literal helper anchors check the products, pointer setup, SDK
outputs, arm stride, loop bounds, packed coordinates and stores. Unlike the
header-428 spiral, this helper clears the current point's depth and flag
before each of its two mirrored sorts. The actual stores at `0x1540/0x1550`
and `0x1674/0x1684`, subsequent depth checks and low-halfword sort arguments
are preserved; these are not extra stores introduced to influence codegen.
The phase halfword at `0xC8C` advances by step times 16; size at `0xC90`
grows by step times 64 and clamps to `0x400`.

The compiled entry calls the helper at image offset `0x6CC`, passing its
original `s2` context in the delay slot. The resident pointer table gives
`0x80176000`; the helper's accessed end `0xC94` lies within the already
observed entry extent `0xCBC`, separate from the model, auxiliary and overlay
loads. These are bounded access views and immediate load separation, not
allocation-capacity or whole-game exclusive-lifetime claims.

Both complete images retain exact retail bytes with seven actual C function
owners and four actual header/tail storage owners. All six previous C input
objects remain byte-identical. Eleven resident dependencies have independently
checked selected input definitions, map placement, final executable symbols
and complete retail function bodies. Production ownership is checked against
an identical mapped relink of the actual production ELF, not candidate
addresses or absolute aliases.

## Scope

All 253 configured French images preserve their complete retail bytes.
The petals integration changes one function from assembly to C without adding
an image or changing its storage boundaries. Configured coverage becomes
**1,350/1,595 matching C instances and 1,811,308 C instruction bytes** after
reconciliation with accepted master `32075aa5`, including the separately
accepted header-445 spiral. The Exodia change itself adds one instance and
2,416 bytes; it does not claim the other proposal's work.
The two Exodia images have no remaining inventoried assembly functions but
retain 27,416 unclassified tail bytes, excluded from C coverage.
Other special handlers, MODEL variants, auxiliary/boot/overworld loads and
unclassified storage remain separate campaign work. This registration does
not establish exhaustive French runtime completion.
