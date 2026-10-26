# Model texture wrapper interfaces

`ModelTexture_UploadTimFile` is the former `func_800599FC`.
It supplies the original eight-word local scratch to `ModelTexture_LoadTim`,
forwards the file path and six integers, and discards both the descriptor
and packed return value. The callee reads the host file, decodes TIM
information and performs the existing pixel/CLUT uploads.

The wrapper's `void` return, `char *` path, six `int` arguments, scratch cast
and `s32[8]` allocation remain exact. A retail `GsIMAGE` is 28 bytes;
the 32-byte scratch has one extra word, not the four stated by the old
comment. No smaller typed local, new status return, pointer qualifier or
different failure/upload behavior is introduced. Its original profile
remains `gcc_2_8_1_g0` and its native extent remains `0x54`.

`ModelTexture_PackPageClutCopy` is the former `func_80059A50`.
It copies the caller's complete `GsIMAGE` into a local, calls
`ModelTexture_PackPageClut` with that local and returns the packed word.
The provider changes `px`, `cx` and `cy` while applying side/region biases;
the copy therefore isolates those changes from repeated queries of an
overlay's shared image table.

The original `s32` arguments, non-const image pointer, whole-struct copy,
`u32` return, `gcc_2_8_1_g8` profile and `0x58` extent are unchanged.
The provider's biases, masks, mutation order and packed page/CLUT layout
are not rewritten.

Both standalone sources have semantic paths, selected by every native
manifest and applicable static split. The existing texture owner header
publishes their exact interfaces. One old-spelling bridge there preserves
all 54 historical matching overlay C bodies and their fingerprints; those
bodies already reach the owning header through their existing include
chains. No overlay source is renamed or edited.

Resident and per-image import/linker/Splat maps publish canonical names
at unchanged addresses while preserving every other binding and attribute.
Existing controls migrate identifiers/include paths only. Historical
attempts, reuse records, compiler claims, profiles and the full Lua tree
remain unchanged; no new source-string or synthetic harness is added.

## PAL provider inventory alignment

The two underlying providers already have canonical C interfaces and
resident linker bindings. Their five PAL inventories now use those same
names rather than the older native spellings:

| Release | `ModelTexture_LoadTim` | `ModelTexture_PackPageClut` |
|---|---|---|
| UK SLES-03947 | `0x8005CFD0`, `0x2B4` bytes | `0x8005EE50`, `0xC4` bytes |
| French SLES-03948 | `0x8005BC14`, `0x2B4` bytes | `0x8005DA94`, `0xC4` bytes |
| German SLES-03949 | `0x8005BC14`, `0x2B4` bytes | `0x8005DA94`, `0xC4` bytes |
| Italian SLES-03950 | `0x8005BC14`, `0x2B4` bytes | `0x8005DA94`, `0xC4` bytes |
| Spanish SLES-03951 | `0x8005BC14`, `0x2B4` bytes | `0x8005DA94`, `0xC4` bytes |

Actual compiled PAL definitions remain native `func_8005CFD0` and
`func_8005EE50`, selected by the existing wrappers. UK keeps its symbolic
linker aliases; the other four releases keep their existing numeric
canonical bindings. Neither is newly introduced or changed, and an alias
is not substituted for the real compiled owner. The inventory change is
only two name fields per release, not new code, a new ABI or a linker
override.

### Inline loader bias is not the standalone side contract

The PAL loader's wrapper selects an `extern inline` packing helper with
`MODEL_TEXTURE_PACK_SIDE_BIAS` equal to three. The shared loader passes its
original mode to that inline helper for modes at least three, so the
effective side is `(mode - 3) & 1`. The separate public PAL packer instead
receives a side directly and retains `side & 1`. Moving the subtraction
into that exported helper would change callers such as the copy wrapper.
No inline/out-of-line definition or subtraction is relocated.

PAL also deliberately leaves low-page CLUT X unchanged, unlike the
NA/Japanese subtraction of `0x80`. The special CLUT-Y remap from eight,
high-page CLUT placement, pixel placement, masks, packed word and mutation
order remain the existing regional code. The packer changes its input
descriptor; the copy wrapper's whole-structure copy remains important.

The loader retains its original signed mode/override tests, paired default
coordinate tests, decoded-coordinate behavior in mode two, host-file
failure return, GPU polling/retry loops and pixel-then-CLUT uploads.
Neither the descriptor nor the packed signed/unsigned return contracts
is retyped. No CLUT upload is skipped, failure behavior improved or
coordinate range validated.

### Conservation evidence

This alignment changes only the five regional inventories, ten scoped
registry rows and this note. Every C/header byte, compiler profile,
manifest, wrapper, symbol/import map, existing test/control and historical
fingerprint remains unchanged, including all frozen overlay callers.

The actual native routes comprise three unique source/profile units:
NA/Japan share the complete two-function `model_texture_upload.c` unit
(836 bytes), while five PAL manifests select the existing loader and
standalone packer units (888 bytes together). All retain the named
`gcc_2_8_1_g0` profile. Complete untouched/candidate compiler output,
MASPSX assembly, symbol tables and disassembly/relocations remain
identical, including the loader's inlined helper and standalone export.

The clean complete NA resident retains retail SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.
That is not local full PAL-image execution; ordinary applicable exact-head
regional PR image/ownership checks remain mandatory. No new tests,
synthetic/generated-C/source-string harness, local cloud/regional/witness
suite or manual CI rerun is introduced.

## Placement, blend and coordinate arguments

The four complete texture entry definitions now expose the same argument
roles without changing their values, types or forwarding:

| Entry | Existing names | Argument roles |
| --- | --- | --- |
| ModelTexture_LoadTim | mode / arg3 / x / y / z / w | destination_mode / blend_mode / pixel_x / pixel_y / clut_x / clut_y |
| ModelTexture_UploadTimFile | b / c / d / e / f / g | The same six arguments, in the same order |
| ModelTexture_PackPageClut | mode | blend_mode |
| ModelTexture_PackPageClutCopy | a0 / a1 | side / blend_mode |

blend_mode is the two-bit ABR selector at texture-page bits 5..6.
The local Psy-Q libgpu.h getTPage macro independently establishes that
encoding; it is not the GsIMAGE pixel mode, a continuous opacity or a
new blend formula. Both pack paths preserve the signed mask and original
high-page/low-CLUT construction.

destination_mode is deliberately NOT named slot_index. The loader's
signed below-two path applies paired default-coordinate tests and
nonnegative pixel/CLUT overrides. Negative modes still enter that path:
the original mode-and-one pixel offset and palette-Y addition remain.
Mode two keeps the decoded TIM coordinates; modes at least three use
the existing regional packing helper. Override arguments stay ignored
outside the original below-two path.

PAL's loader still passes destination_mode to its extern-inline helper
with side bias three; NA/JP still pass destination_mode minus three.
The standalone packer and copy wrapper instead accept side directly.
No subtraction, inline/export definition, low-page CLUT-X omission,
special CLUT-Y case, mask, coordinate narrowing or mutation is relocated.

The upload wrapper keeps its 32-byte scratch, void return and six int
arguments; the loader keeps s32 arguments/return, paired defaults,
host-file failure return and every GPU poll/retry. Pixels are uploaded
BEFORE the CLUT. The copy wrapper keeps its full GsIMAGE copy, s32
arguments, non-const source pointer and u32 return. Its neighboring
shadow setter is unchanged.

Only bounded parameter identifiers in two complete C units, their owning
header, two existing header comments and four existing registry NOTES
cells migrate. RECT/GsIMAGE members, all expressions, callers, frozen
overlay spellings, aliases, source groups, compiler profiles, initialized
data, ABI and existing fixtures/stubs/oracles/scenarios/mutations/counts/
numeric controls stay unchanged. No new harness or local cloud-fixture
execution is introduced.
