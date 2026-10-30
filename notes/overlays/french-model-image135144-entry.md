# French MODEL135144 projectile entry

Tracked for #6460. The MODEL.MRG images at sectors 135144/135154 contain
the same game-owned entry at `0x8013B004`/`0x8017B004`. Each original
`0x2188`-byte body has a 448-byte frame and 93 static calls to 24 resident
targets. Both complete 20,480-byte images reproduce their retail bytes and
SHA-256 using the named `gcc_2_8_1_g0_split` profile, GCC 2.8.1 and MASPSX 2.81.
The original module headers and all bytes from image offset `0x218C` onward
remain raw and unchanged, including the scale vector, three texture
descriptors and configuration data.

## Recovered component

The 42-byte command row contains three RGB groups, three model-part
selectors, projectile dimensions/speed/count, impact glow size, puff
speed/count/size, ray width/spread/count, a byte fade step and two time gates.
Unused bytes retain unknown names. Native offsets establish an 8,604-byte
work area with eight projectile positions, steps, angles and saved impact
origins; four eight-by-32 vector arrays for puffs, puff velocities, rays and
trails; elapsed time, phase, arrival flags, trail heads/lengths, ambient light,
three packed textures and per-projectile brightness/colours.

Before the first time gate the textured projectile aims between the source
and charge model points. Until launch it aims toward the aim point and
refreshes the initial projectile/trail positions. It then moves with a
Gouraud ribbon and saves its last flight origin. Arrival draws an impact
glow, two differently coloured/scaled fans of static random rays and
animated moving puffs, while the ambient level dims and recovers.
The first projectile's arrival and recovery states control notification and
completion returns.

The shared regional `variant805_entry.c` and `variant801_entry.c` supplied
related packet, matrix and ribbon structure, not an identical whole body.
This entry's layouts, model-part selection, camera phases, constructor return
and complete call sequence were independently recovered. Raw header values
820/917 do not establish identity with same-numbered regional C. No external
reference types, headers, compiler claims or flags were imported.

## Native details retained

- The constructor returns zero rather than falling through to an update.
- Puff velocity XYZ and ray XY each use two random calls; ray Z uses only
  one and includes the negative slot direction.
- SDK `addVector` comma expressions preserve the native component-store
  ordering and scalar/pointer lifetimes. Separate component statements
  produce a different register graph.
- The ribbon bound `head - (tail + 1)` shares `head - 1` with its initial
  index. Its previous-node vertices deliberately retain the current node's
  Y coordinate and use the native halfword-truncated width quotient.
- Projectile transparency uses distinct opaque and blended paths.
- The glow saves its matrix before scaling; the second ray fan restores it
  and uses half scale. Impact packet submissions have no added depth guards.
- All nine colour arrays are initialized and faded. The third RGB group is
  not read for drawing in this entry; no use was invented.
- The SDK ordering-table getter keeps its canonical shared declaration
  `func_80058F10`, bound to the measured French address `0x8005C018`.

## Evidence and integration

Both original native images were reproduced before candidate work.
The 23 differences between native bodies are exactly 20 internal direct
jumps and three immediately consumed image-local address producers, each
relocated by `0x40000`. Their complete resident call sequences agree.
Fifty-one target-compiler layout assertions cover configuration/work offsets
and SDK object sizes. Original headers, project tools and target metadata
were frozen and checked throughout the candidate experiments.

`french-model-image135144-entry-attempts.csv` records all eight compiled
experiments for both slots, including genuine undefined-symbol and overlap
rejections. The seventh experiment matches both full images; the eighth
only improves readability and independently reproduces those hashes.
Production rows record the normal overlay builder's separate full-image
proofs and original-sized C ownership.

Integration follows the existing regional overlay manifests, inventories,
layout and provider conventions. The resident-only
`integrate_verified_match.py` hardcodes the North American resident ledgers
and is not applied to these overlays.

## North American images

The US MODEL.MRG images at the same sectors (headers 803/900, modules
`model_variant_539_pos0_slot0` and `model_variant_539_stage8_slot1`) carry a
byte-identical entry and tail; only the 93 resident call targets differ, which
`config/slus_01411/overlays/variant803us_linker_symbols.txt` supplies. The
wrappers `src/overlays/model_variant/variant803us_entry.c` and
`variant803us_entry_slot1.c` include this French source unchanged, so no US
selector is needed. As in the French layout, the scale vector at `0x218C`
stays in the raw tail (`D_8013D18C`/`D_8017D18C`). Both US images reproduce
their retail SHA-256.
