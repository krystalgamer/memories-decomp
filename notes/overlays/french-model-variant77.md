# French MODEL headers 77 and 207

Models 53, 84, 386 and 547 provide eight loader-paired images with exact
3,092-byte entry functions. One implementation and a symbol-only slot wrapper
use `gcc_2_8_1_g0_split`, GCC 2.8.1 and MASPSX 2.81. Existing SDK headers,
compiler profiles and other regional implementations are unchanged.

## Loader and physical ownership

The [instance ledger](french-model-variant77-instances.csv) records all eight
complete 20,480-byte hashes and their physical sectors. Model 386 uses compact
record 336 and stages 7/8; the other three models use stages 9/10. Commands
7000, 7001, 7003 and 7004 select configuration indices 0, 1, 3 and 4.
Updates use the negative-command path.

Both entry CFGs cover all 773 instructions, with 24 distinct resident callees
and no local or indirect calls.

| Image offsets | Bytes | Owner |
|---|---:|---|
| `0..4` | 4 | raw module header |
| `4..0xC18` | 3092 | sized entry C definition |
| `0xC18..0xC28` | 16 | C scale constant |
| `0xC28..0xC44` | 28 | raw image descriptor view |
| `0xC44..0x5000` | 17340 | unclassified raw suffix |

Independent scratch links reproduce every complete unmasked image. Sized
linked function definitions establish actual C ownership, not merely an
absolute symbol or a candidate's location. The compiler gives the scale
symbol no ELF object size; its exclusive 16-byte C rodata section and exact
linked contribution establish its extent. Raw ranges have disjoint sized
definitions. Descriptor declarations are typed views of these raw symbols,
not invented C backing storage.

## Measured context and behavior

Twenty-two target-compiled layout constants establish the 22-byte configuration,
existing SDK types and minimum `0x118`-byte context view. The context contains
the configuration pointer at `0`, position at `4`, 32 velocities at `0xC`,
texture at `0x10C`, completion byte at `0x110` and elapsed time at `0x114`.
Its accessed extent is separate from the primary, secondary and variant
image loads in both slots; this is not an allocation-capacity claim.

Initialization creates randomized velocities and uploads one image.
Updates follow the selected model bone, move the stored position toward its
destination, draw a sinusoidal triangle-strip beam, then draw timed particle
quads. The four selected configurations have positive travel and duration
divisors. Initialization does not clear the stored position; the delayed
update path copies the bone origin into it, as the native code does.

Inactive particles skip the velocity-pointer increment. An active particle
advances that pointer even when its depth or projection flag suppresses
drawing. This distinction is explicit in the native time-rejection branches:
they target the loop-counter update after the pointer increment.

## Matching evidence and remaining scope

The [attempt ledger](french-model-variant77-attempts.csv) preserves fourteen
paired source/profile experiments and two terminal canonical matches.
Compound short-vector negation preserves the native narrowing operations.
The beam start and end are assigned in their actual temporal/orientation
branches rather than initialized prematurely.

An unchanged compiler RTL capture identified the left-endpoint/depth
allocation mismatch. Branch-local start assignment recovered the native
retained endpoint and spilled depth,
leaving only five terminal words. A byte-sized terminal status, whose only
values are 1 and 2, recovered that final branch layout while preserving
the function's `s32` return ABI and the real completion-byte increment.
No forced registers, artificial dependencies, padding, assembly patches
or compiler changes were used.

All 24 actual resident callee bodies were checked against the exact resident
ELF. The SetPolyF3 identity was separately checked against the established
North American SDK calibration.

This independent branch starts at accepted
`df474958b442cfbf86ca0370ec17a1908b983a1f`, preserves all 325 prior French
module registrations and adds eight C function instances: 24,736 instruction
bytes and 128 source-owned literal bytes. The suffixes retain 138,720
unclassified bytes. Entry coverage does not establish exhaustive runtime
completion; project-wide progress snapshots remain separate.

Final acceptance reproduces the clean French resident and all 333 production
overlays. Production ELFs verify the eight new entry/literal/raw owner sets,
24 resident callees and three resident controller owners. The 73 focused,
progress, source-wiring and extraction regressions pass, along with metadata,
basic-type, attempt-ledger, G32 and matching-source contract checks.
Configured totals are 1,765/1,985 C function instances and 2,492,380 C
instruction bytes; the French runtime campaign remains incomplete.
