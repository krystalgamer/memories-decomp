# Spanish MODEL411 beams

Helper `+0x157C..+0x1E58` is matching C. It is 2,268 bytes in the four
MODEL411 loads (MODEL364 and MODEL452, stages 9/10), giving 9,072 instruction
bytes from one unique routine. A separate second-slot compilation also
matches.

No region has accepted C for this routine. A screen of 8,038 accepted regional
C entries compared five same-size bodies and found no match. The loader entry
does not call it; it is a closed retained helper.

## Behaviour and method

From phase 4 on, the routine builds sixteen two-point beams at `+0xDC4` in
four tilt groups (0x100, 0x300, 0x500, 0x700) around the spin. The last beam of
each of the first three groups is offset by a further 400. Each point:

- **Growth.** Grows by 80 up to 0x400.
- **Length.** Its copy along the view is 16 or 64 long.
- **Drawing.** Both halves are drawn as `POLY_GT4` fading to black. The first
  half needs a non-negative depth below 0x800; the second half needs a
  positive depth below 0x800.

The 0x64-byte `Beam411` records are declared in `e577_beams_pal.h` and type
the entry's `beams[16]` in both header 411 (`EntryState411`) and header 444
(`Variant444EntryState`, whose `Variant444EntryColorRecord` named only the
colours and age of the same records). The helper was first matched through
a private `Beam411State` view; it now takes the entry state of its build,
with `BEAMS_FRAME`/`BEAMS_STEP`/`BEAMS_QUAD`/`BEAMS_PHASE` selecting the
members whose paths differ between the two. The sheet is `sheets[0]`, the
position and direction are `aim.target` and `aim.view_delta`, and the spin
at `+0x1674` is `beam_spin` in both entries.

The earlier campaign reached the retail layout with:

- duplicated projection tails
- nested depth tests
- the shifted height product

It then stopped on a callee-saved permutation. The fix is the beam length as an
`s16` local set to 16 or 64: that gives it the global-allocation priority that
retail gives it over the packet pointer. The ledger keeps the conditional
expression (88 words) and arithmetic (505 words) forms.

## Acceptance evidence

All four complete 20 KiB images match using `gcc_2_8_1_g0_split`. The MODEL411
binding list gains the SDK alias `RotTransPers`; its address was already
bound. Regression tests cover:

- record offsets
- every relocation against all four images
- C segment order, statuses and owners in the MODEL411 suite

The terminal dependency fingerprints hash the body, the beam header, the
shared model-variant header and `gpu_packets.h`.
