# French MODEL423 loader entries

The MODEL385 stage-7/slot-zero and stage-8/slot-one images now use exact C
for their 3,604-byte entries at `0x8013B004` / `0x8017B004`.
Both complete 20,480-byte images reproduce retail with the existing
`gcc_2_8_1_g0_split` profile (GCC 2.8.1 and MASPSX 2.81).
This adds two C instances and 7,208 bytes. All five existing C helper
registrations and their original-sized owners are unchanged. The separate
2,224-byte ribbon helper at image `+0x1CB4` remains generated assembly.

## Measured layout and initialization

All 901 native entry words, including zero instructions, were inspected.
The component header uses the accepted local mesh, sheet, funnel and streamer
types, with opaque entry-only views for the ribbon and spiral initialization.
The similarly numbered North American MODEL423 entry has a different effect
and layout; its declarations and body are not substituted.

| Work offset | Observed storage |
|---|---|
| `0x0000` | Nine rows of seventeen mesh points and nine color rows |
| `0x04EC` | One `0x360`-byte ribbon record |
| `0x084C` | Twelve `0x7C`-byte spiral records |
| `0x0E1C` | Two `0x98`-byte sheets |
| `0x0F4C` | Two `0x334`-byte streamers |
| `0x15B4` | Five initialized `0x118`-byte funnel records |
| `0x1B2C` | Mesh GT4; funnel GT4 at `0x1B60` |
| `0x1BC8` | Additional initialized GT4 |
| `0x1C30` / `0x1C4C` | G3 / G4 |
| `0x1C70` / `0x1CD8` | Two GT4s / two FT4s |
| `0x1D38` | Matrix, including origin at `0x1D4C` |
| `0x1D58` / `0x1D60` | Target / direction |
| `0x1D70` / `0x1D74` | Screen delta / view direction |
| `0x1DA0` / `0x1DA8` | Configuration / selected part pointers |
| `0x1DF0` / `0x1DF4` | Inner / outer colors |
| `0x1E0C` / `0x1E20` | Phase / ambient intensity |
| `0x1E24` / `0x1E26` | Signed slot / command |

These observations establish a minimum context extent of `0x1E28`, not
allocation capacity. Untouched packet intervals, vector pads and state gaps
remain opaque.

The signed command selects a `0x2C`-byte descriptor at suffix `+0xFC`.
Its part index is byte `+0x10`, inner RGB is `+4`, outer RGB is `+0`,
and unsigned sheet start is `+0x18`. The slot determines the initial target's
z sign. Three mode-one image uploads use suffix `+0xC4`, `+0x8C` and `+0x1C`;
the two explicit page/CLUT queries retain their distinct results.

Mesh latitude advances by the measured signed numerator sequence
`(base << 2) / 8`, with `base += 325` in the loop body. Radius is
`rsin(latitude) * 384 >> 12`; height combines the similarly scaled cosine
with `rsin(276)`. Seventeen longitudes advance by 256. Red and green
advance by 20 per row; blue is 192.

The entry initializes **five** funnel records through `0x1B2C`, although the
accepted [funnel helper](french-model-variant423-funnel.md) renders four.
Records zero through three receive `-(index * 8192 / 3)`; record four
explicitly receives zero. The entry view therefore has five records without
changing the helper's separately measured four-record draw bound.

## Runtime and reachability

The entry derives camera-facing angles, copies the opposite slot's target
halfwords, clears target y, refreshes the selected-part matrix, and projects
origin and target. Projection x is unsigned low-halfword; y is signed.
The height reset stays paired with each slot-copy branch, preserving the
native merged-call scheduling.

Sheets run when their unsigned start time is reached. Phase at least three
enables funnels followed by mesh; phase at least two enables the ribbon;
positive phase enables spirals. Both streamer records are initialized, but
there is no direct entry call to the retained
[streamer helper](french-model-variant423-streamers.md). This does not establish
global unreachability.

Animation changes advance elapsed time and capture the signed frame step.
Common submission uses ambient slot two. Ambient decreases by 128 and clamps
at zero below phase seven; later it uses `fade << 5`. Phases three/four
return four, phase six returns one and enters seven, phase seven grows fade
to 64 and enters eight, and phase eight returns two.

## Experiments and acceptance

The [attempt ledger](french-model-variant423-entry-attempts.csv) records eight
immutable native compiler experiments and two production-image terminals.
The first recovery was 3,572 bytes. Sharing the actual angle variable across
mesh latitude, ribbon bearing and funnel longitude, while retaining the
separate mesh longitude counter, recovered native register ownership and
stack spills. Assigning each FT4 page before its UVs recovered two native
load schedules. Pairing target-height reset with each slot-copy branch
resolved the final three words without fabricated dependencies or accesses.

Direct authoritative SDK/game imports replace an unrelated entry-header
dependency; the final source has complete declarations. The source retains
the native 248-byte frame and makes no claims about the types of its unused
stack regions. No compiler profile, existing helper body, raw suffix,
archive slice or resident implementation changes.

| Complete image | SHA-256 |
|---|---|
| MODEL385 stage 7, slot 0 | `428977d728a5a7b009bbdc48a718b095dd3dcb3c8fc643bef02c0f2388eeb245` |
| MODEL385 stage 8, slot 1 | `ca1e3b27754f08d325d1fada607435a4fe25b0910e06149155e7a52157bea79c` |

Both production ELFs retain all six C functions at their original addresses,
sections and sizes. The unmatched ribbon remains the assembly fallback.
