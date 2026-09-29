# French duel-bank effect twenty-three

`func_80152048`, at `0x80152048..0x80152EC4`, is one complete 3,708-byte
matching C object. The dispatcher routes effect 23 here; the resident
Harpie's Feather Duster request uses this effect. Recovery uses the existing
`Duel_CollectFieldRowCardObjects`, `DisplayObject` and sound contracts,
without private card-object layouts or pending matching registrations.

## Experiments and exact-image evidence

Four source experiments used the existing `gcc_2_8_1_g0_split` profile
(GCC 2.8.1 and MASPSX 2.81):

| Candidate | Result |
|---|---|
| 01 | 3,780 bytes: conditional comparisons duplicate card-coordinate loads; slot-sign branches and direct vector copy differ |
| 02 | 3,712 bytes: comparing the coordinate against a conditional constant and using `copyVector` leaves only slot-sign arithmetic |
| 03 | Applying sign before scaling the slot index still gives 3,712 bytes |
| 04 | Scaling the index before multiplying by the conditional sign reproduces all 3,708 bytes |

Before promotion, the accepted bank plus candidate reproduced the complete
90,112-byte retail bank:
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Production verification covers seven French configured images and all
seven terrain bank copies. It checks 188 configured C owners (82,504 bytes),
all 30 complete bank C objects and nine named overlay routine owners.

Four data objects retain real input-object and linked-image ownership:
`D_801461A8` (16-byte scale vector), `D_8015B7A0` (84-byte card-output array),
`D_8015B748` (84-byte texture table) and `D_8015B7F8` (8-byte offset).
No overlay-owned data is replaced by an absolute alias.

The additional resident bindings are the accepted
`Duel_CollectFieldRowCardObjects` at `0x8002CB0C` (124 bytes),
`Model_GetLightSourceMatrix` at `0x8005C328` (12 bytes), and
`SD_SEPlayFull` at `0x80040204` (40 bytes).

## Layout and preserved behavior

Thirty-nine target-compiled layout constants verify the work structure,
canonical display-object members and array extents. The work structure is
**414 bytes (`0x19E`)**, aligned to two bytes: unlike pointer-bearing
effects it does not need the initially presumed four-byte tail rounding.
The probe corrected that expectation without changing the matching source.

The collector selects at most five occupied back-row cards and writes a
null terminator; its existing 21-word output buffer has ample capacity.
Five states track occupied columns independently of the compact card list.
The two sweep directions have distinct trigger thresholds. Activation
changes each selected column once, keeping the active prefix within the
collected count. Each column owns three particle positions/velocities and
a 24-tick animation. Bounds were exercised for all 32 occupancy masks on
both sides; empty rows keep their own completion path and never index -1.

The sprite rises, sweeps sideways while oscillating and plays sound 36
at each swing reversal. Selected cards receive the original attribute bits,
position increments, RGB fade and flag update. All three rotation bytes
intentionally use the first rotation-vector component, as the target does.
Completion checks the sweep color and, for nonempty rows, the last
collected card's RGB word. It does not wait for every particle animation.
The unused `Model_GetFrameStep` call remains present; there is no cross
fallback for nonnegative phases.

This independent branch preserves all 63 accepted bank entries and reaches
64/85 C functions, 26,652 bytes. Twenty-one bank boundaries remain assembly;
boot ownership, MODEL/SU loads and the overworld tail still require coverage
work. Configured-image identity is not exhaustive runtime completion.
Progress snapshots under #443 remain separate.

## Accepted baseline integration

Integration through accepted master `b73bc0b5` preserves all 75 accepted
French matching entries and inventory rows verbatim. Only the unchanged
reviewed effect-23 source/header is added: **76/85 bank C functions /
50,460 bytes**, nine assembly boundaries, and **200 configured French C
instances / 106,312 bytes**. No pending branch is stacked or counted.

Both sides' bindings, real data owners, evidence and tests are retained;
the shared matrix getter, card collector and card-pointer buffer are
deduplicated without changing their ownership. All seven complete French
images and bank copies match. The proof checks 200 linked C owners, all
42 complete bank C objects, nine routine owners, four real input/final
data owners and 39 target layout constants. All 64 focused regressions pass.

The accepted authenticated CI input repair, shared sources and Spanish
registrations remain unchanged. Old staging failures are not bypassed:
fresh hosted checks must pass on this published head.

Integration through accepted master `ab446402` additionally retains French
effect 13 and all 76 accepted matching entries and inventory rows verbatim.
The unchanged effect-23 addition gives **77/85 bank C functions / 53,028
bytes**, eight assembly boundaries, and **201 configured French C instances /
108,880 bytes**. The accepted North American and Spanish additions are preserved.

All seven French images and terrain copies, 201 exact linked C owners and
43 complete bank objects pass, together with the same nine routine owners,
four data owners and 39 target layout constants. All 65 focused regressions
pass. No reviewed source/header, other regional registration or CI gate changes.
