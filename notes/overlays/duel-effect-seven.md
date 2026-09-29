# French duel-bank effect seven

`func_801587D8`, at `0x801587D8..0x801593A8`, is now one complete
3,024-byte matching C object. Recovery used the French instructions, existing
local contracts and effect-six control flow; it does not register pending
matching batches or another region's unverified archive copies.

The first candidate matched every function byte with `gcc_2_8_1_g0_split`
(GCC 2.8.1 and MASPSX 2.81). Before promotion, an independent bank containing
the 63 accepted C entries plus this candidate reproduced all 90,112 bytes:
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Production extraction/build verification covers all seven configured French
images and all seven terrain copies of the bank.

## Layout and behavior

The target-compiled configuration has size 30; its five records occupy the
real data object `D_8015B5B8` (150 bytes). `D_80146228` is a separate 16-byte
initial scale vector. Neither owner is replaced by an absolute linker alias.
The work structure is `0x268C` bytes, including sixteen four-point trails,
sixteen velocities, three 32-point rings, sixteen 32-particle arrays and
their separate velocity arrays. Forty-six target-compiled size, offset and
array-count constants verify the declarations.

The five configurations select 16/6/8/12/16 trails and 8/12/16/24/32
particles per burst. All durations are 14, so the initialization divisor
is nonzero and the last-selected-trail index stays within the arrays.
The starting Y halfword encodes -60; number values are
-50/-100/-200/-500/-1000.

Each trail grows until it becomes a particle burst. Burst rendering copies
the saved matrix explicitly before scaling, uses three rings and the current
sprite size, and fades the entry color. Unlike effect six, the number has
its own position and velocity, gravity, five diminishing bounces and fade.
Completion requires the last selected entry, background and number colors
all to be zero. The cross fallback retains its 180-frame threshold.
The number renderer remains assembly, using its existing declaration from
`effect_6.h`; no renderer match is claimed.

## Ownership and scope

The production proof checks all 188 configured C symbols (81,820 bytes),
all 30 complete bank C objects, all 17 named overlay routines used by this
source, and both data owners in their input objects and final linked image.
The bank reaches 64/85 C functions and 25,968 matching bytes on this
independent branch.

Twenty-one bank boundaries remain assembly. Boot ownership, MODEL/SU loads
and the overworld tail still require investigation; configured-image
matching does not establish exhaustive runtime completion. Progress
snapshots under #443 remain separate.

## Accepted baseline integration

Integration through master `2ef4aaba` retains all 72 accepted French
matching entries, including effect 4, and adds only this unchanged
effect-7 source/header. The combined inventory is **73/85 bank C functions /
41,564 bytes**, 12 assembly boundaries, and **197 configured French C
instances / 97,416 bytes**. Both sides' bindings, owners, evidence and
tests are retained; no pending branch is stacked or counted.

The number-renderer prototype now resides in accepted `drawing_helpers.h`
and is still available through `effect_6.h`; the formal remains `s32` and
the renderer body remains assembly. The accepted authenticated CI input
repair is included without changing retail hashes or acceptance gates.
