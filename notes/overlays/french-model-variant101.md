# French MODEL headers 101 and 231

Model 130's stages 7/8 have exact 2,784-byte entry functions using the named
`gcc_2_8_1_g0_split` profile, GCC 2.8.1 and MASPSX 2.81. One implementation
and a symbol-only slot wrapper provide both entries and their 16-byte
scale constants. No shared SDK headers, compiler profiles or other
regional implementations change.

## Loader and physical ownership

The [instance ledger](french-model-variant101-instances.csv) records both
complete 20,480-byte image hashes. Compact record 130 selects sectors
36060/36070, loaded at `0x8013B000`/`0x8017B000`. The controller's command
32004 supplies initialization index 4; updates use the negative-command path.
Both closed entry CFGs cover all 696 instructions, with 43 direct calls
to 23 resident destinations and no local or indirect calls.

| Image offsets | Bytes | Owner |
|---|---:|---|
| `0..4` | 4 | raw module header |
| `4..0xAE4` | 2784 | sized entry C definition |
| `0xAE4..0xAF4` | 16 | C scale constant |
| `0xAF4..0xB10` | 28 | raw image descriptor view |
| `0xB10..0x5000` | 17648 | unclassified raw suffix |

Independent scratch links reproduce both complete unmasked images.
The linked function definitions have the exact address and size; no
absolute alias substitutes for C ownership. GCC emits the scale symbol
without ELF object size metadata, so its extent is established by its
exclusive 16-byte C object rodata section and exact linked contribution.
All three raw ranges have separate, nonoverlapping, sized definitions.
Production links also reproduce both complete hashes.

The image descriptor and configuration are typed views of distinct raw
byte symbols, not invented C backing objects. The image view is 28 bytes;
the configuration starts immediately afterward. Combining their addresses
under one suffix symbol enables nonnative common-address elimination and
shortens the entry by four bytes. Keeping the observed disjoint views
preserves exact code without claiming the suffix is entirely descriptors.

## Measured context and behavior

Twenty-two target-compiled layout constants establish the 16-byte descriptor,
existing SDK types and context offsets. The context view spans `0xA60` bytes:
positions at `4`, velocities at `0x304`, 66 ring vertices at `0x604`,
projected coordinates at `0x814`, depths at `0x994`, texture at `0xA54`,
completion byte at `0xA58` and elapsed time at `0xA5C`.
This is a minimum accessed view, not a new allocation-capacity claim.

The selected descriptor has particle RGB `(255, 0, 64)`, ring RGB
`(204, 204, 204)`, speed 60, radius 180, count 30, delay 76 and duration 30.
Its accesses fit the measured 96-particle spans, and its divisors are
positive. The minimum context extent is separate from the selected primary,
secondary and variant image loads in both slots.

Initialization creates randomized particle velocities and two 33-vertex
ring halves. Updates draw particle boxes, integrate their positions, apply
vertical acceleration and project a growing, fading 32-quad ring.
The first box pass reads the transient stack flags before current
projection, as the native code does; this is preserved, not presented as
initialized or repaired behavior.

## Matching evidence and remaining scope

The [attempt ledger](french-model-variant101-attempts.csv) preserves fourteen
paired experiments and two terminal canonical matches. A separate gravity
velocity walker recovers the native induction addresses without extending
the initialization walker's lifetime. Placing projection walker setup in
the first drawing loop recovers the last scheduling difference.

The unchanged compiler RTL explains that difference: scheduling initially
used a projection-pointer calculation to fill a color load delay, then
delayed-branch optimization moved it into an earlier call's delay slot
without rescheduling the resulting hole. The loop initializer avoids this
interaction naturally; no forced register, artificial dependency, assembly
patch or compiler change is used.

All 23 actual resident callee bodies were compared with the exact resident
ELF. Five additional SDK identities were independently calibrated against
known North American bodies; `memset` required its one proven internal
jump relocation, not masking.

This independent branch starts at accepted
`849166a96a716b32f15807121acd58e9b5386905`, preserves all 323 prior French
module registrations, and adds two C function instances (5,568 instruction
bytes) plus 32 source-owned literal bytes. The two suffixes retain 35,296
unclassified bytes. Neither configured coverage nor these entry matches
establish exhaustive runtime completion; progress snapshots remain separate.

Final acceptance reproduces the clean French resident and all 325 production
overlays. Production ELFs independently verify the new entry, literal and raw
owners, all 23 resident callees and three resident controller owners.
Seven focused regressions and 57 source/extraction/toolchain checks pass,
along with metadata, basic-type, attempt-ledger, G32 and source-contract
checks. Expected configured totals after acceptance are 1,757/1,977 C
instances and 2,467,644 C instruction bytes, not a claim that the remaining
runtime scope is complete.
