# French MODEL headers 424 and 574

Thirty physical instances contain 29 distinct complete images. The
[instance ledger](french-model-variant424-instances.csv) records each legal
archive slice, command and hash independently, including identical images
selected by different commands: model 96 stage 7 uses 590000, while model
242 stage 9 uses 590002.

## Shared C and measured regional differences

Two four-line wrappers reuse `variant407_petals.c`, defining
`VERSION_FRENCH` and renaming the helper to `0x8013C2D8`/`0x8017C2D8`.
French uses the named GCC 2.8.1/MASPSX 2.81 profile
`gcc_2_8_1_g0_split`. Existing US profile metadata is not changed or
used as a French compiler-selection rule.

The unchanged body produced the correct 1,500-byte size and 336-byte
frame but eight unequal instructions: three initialization stores and
five commuted address additions. Moving the matrix-pointer initialization
after the counters removes the first three. Equivalent negative-offset
address expressions must be used consistently for scale, done and reset
arrays; otherwise common-subexpression elimination leaves an extra
addition in the reset path. Other addition/indexing forms retain the five
operand-order differences.

The [experiment ledger](french-model-variant424-experiments.csv) preserves
all eight rejected experiments and the exact source result, including
fingerprints, sizes and mismatch counts. The
[attempt ledger](french-model-variant424-attempts.csv) records both terminal
canonical-wrapper matches and the shared-body fingerprint. The regional
guard leaves the original non-French expressions and initialization order
intact. All 30 affected existing US images were independently clean-built
with unchanged configured profiles, matched in full, and checked for actual
C ownership inside their existing whole-module ELF sections.

## Complete image ownership

Each image occupies ten 2,048-byte sectors in a 276-sector compact MODEL
record, loaded at `0x8013B000` or `0x8017B000`, with entry at `+4`.
Stages 7/8 use record offsets 180/190; stages 9/10 use 200/210.

| Offset range | Bytes | Owner |
|---|---:|---|
| `0..4` | 4 | raw header |
| `4..0x12D8` | 4820 | generated assembly entry |
| `0x12D8..0x18B4` | 1500 | petals C |
| `0x18B4..0x5000` | 14156 | unclassified raw suffix |

Both function spans have complete instruction/delay-slot walks and one
terminal return, without unresolved indirect transfers. The helper is
direct-entry reachable. The suffix has real storage ownership but remains
unclassified, not proven non-code.

The physical registrations add 60 inventoried function instances:
30 C / 45,000 C bytes and 30 assembly / 144,600 assembly bytes, plus
60 raw owners / 424,800 raw bytes. Unique-image C bytes are 43,500;
physical-instance and distinct-image counts are not interchangeable.

## Independently recovered accessed views

Entry captures `a0 -> s2 -> s6` and saves context at stack `0x80`.
It initializes **five** arrays of 48 eight-byte `SVECTOR`s at context
`0/0x180/0x300/0x480/0x600`; the helper draws using the **first four**.
The fifth initialized array is not silently omitted from the layout.
Shared RGB bytes are at `0x780..0x782`.

Four 48-element, four-byte arrays begin at `0x794` (scale),
`0x854` (done), `0x914` (unresolved role), and `0x9D4` (reset flag).
Entry initializes each array with a four-byte advance and 48-element
bound. Its update loop walks 48 sixteen-byte position/delta records at
`0x23A8`/`0x26A8`; the helper uses XYZ from the latter.
The helper writes the 36-byte `POLY_G4` at context `0x2320`, projecting
to screen-pair offsets `8/16/24/32`. Sorting accepts nonnegative depth
and flags when the corresponding done value is zero.

Commands `590000`, `590001`, `590002`, and `590004..590013` select
**68-byte** descriptors at module `0x19B0 + (command % 1000) * 68`.
Entry's shift-by-four/add/shift-by-two sequence independently establishes
the stride; context `0x29D4` stores the pointer. The helper reads radius
as `u16` at descriptor `0x1E`, step as `u16` at `0x20`, and duration
as `u32` at `0x28`.

Sixty-five retail instruction anchors, 31 target-compiled C/SDK constants,
the complete archive hash, all physical slices and actual commands support
these views. All 33 resident callee addresses have actual resident ELF
owners. Loader/controller source and pointer evidence establish separation
of the accessed context from measured model, auxiliary and overlay loads.
The direct-access minimum `0x2A10` is not whole-allocation capacity or a
global-isolation proof.

## Original acceptance scope

The original independent batch started from accepted `706c922f2`, preserving all
198 prior French registrations and excluding the then-pending Family415/439
batch. All 228 configured French images and the clean French resident
matched complete unmasked retail bytes. Actual production owners, all
33 resident bindings, three loader/controller owners and 31 freshly
compiled layout constants passed independent checks.
Configured totals are 872/1,437 C instances and 837,204 C instruction bytes.
All 123 French MODEL variant regressions, 16 progress regressions,
five US toolchain regressions and repository policy gates passed.

Shared declarations and compiler profiles remain unchanged. Eight family
regressions cover the physical census, sources, terminal fingerprints,
rejected experiments, complete spans, reachability, storage, descriptors,
commands and context separation. Untranslated entry functions, suffixes
and other runtime areas remain; these counts are not an exhaustive French
coverage claim. General report snapshots remain separate.

## Accepted-record reconciliation

The branch was reconciled with fixed accepted master `46cee06c5`, preserving
all 222 accepted French module records byte-for-byte as structured records,
including all 24 Family415/439 registrations accepted after the original
cutoff. Exactly the same 30 petal registrations are appended. No unrelated
pending French branch is stacked, and the original petal source, wrappers,
retail hashes, experiment records and terminal fingerprints are unchanged.

The combined inventory is 252 physical registrations, 920/1,581 matching
C instances and 895,164 C bytes. The denominator is 1,521 accepted functions
plus 60 new functions: each petal image retains an assembly entry as well
as its C helper. Counting only the new C helpers would incorrectly omit
30 preserved assembly inventory rows.

Fresh complete gates pass for all 252 French and 263 US images and both
clean resident executables. Production checks verify the 30 petal C owners,
30 assembly entries, 60 raw owners, 33 resident binding addresses, three
resident caller/loader owners and all previously accepted regional records.
All 30 US petal C owners remain exact, and 31 freshly compiled layout
constants agree. The 137 French, 57 Spanish, 16 progress and five US
toolchain regressions pass without skips, as do the metadata, basic-types,
external-attempts and G32 gates. These configured inventory totals still
do not establish exhaustive runtime coverage.

The later fixed accepted cutoff `2fb35390d` includes twelve newly accepted
Family445 webs C owners. A further reconciliation preserves those owners
and all 222 accepted registrations, appending exactly the same thirty
petal records. Combined totals are now 252 images, 932/1,581 C instances
and 911,724 C bytes. Petal source, wrappers, image hashes and attempt
fingerprints remain unchanged. Fresh gates reproduce all 252 French and
263 US images and both clean residents. Actual production selections and
bytes verify all twelve accepted Family445 webs C owners, all thirty
French petal C owners and all thirty existing US petal C owners. The 31
compiled layouts, 33 bindings, three caller/loader owners, 139 French
variant, 71 Spanish variant, 16 progress and five US toolchain regressions
pass again, together with the repository policy gates.
