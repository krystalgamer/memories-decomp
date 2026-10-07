# Spanish MODEL444 funnels

Helper `+0x240C..+0x291C` is matching C. It is 1,296 bytes in the two MODEL444
loads (MODEL175, stages 9/10), giving 2,592 instruction bytes from one unique
routine. A separate second-slot compilation also matches. No region has
accepted C for this routine. The loader entry calls it.

## Behaviour

This is the MODEL411 funnels routine (#7180) with smaller rings:

- The inner ring has radius 64.
- The outer ring has radius 128 plus the flicker and is set back by 64 plus
  the flicker.
- The flicker is a 128th of the first sheet's size on odd frames.

The record offsets match MODEL411 exactly, so the source includes the accepted
`variant411_funnels.h` through `variant444_funnels.h`. Like MODEL411, it reads
the fade start from the overlay data that follows the code, here
`D_8013D91C + 0x114`.

## Integration

Both modules convert the `+0x240C` helper from ASM to C. The slot-1 wrapper
renames both the function and the data label. The MODEL444 binding list gains
the SDK aliases `rcos` and `rsin`, whose addresses were already bound.

## Acceptance evidence

All complete 20 KiB images match using `gcc_2_8_1_g0_split`. The ledger keeps:

- the exact source with records local to the scratch file
- the source on the shared records
- the second-slot compilation
- both terminal full-image results

The regressions cover the shared record offsets, every relocation (including
the HI16/LO16 data pair) against both images, and the MODEL444 suite's C
segments, statuses, totals and bindings. Terminal dependency fingerprints hash
the body, its header, the MODEL411 funnel records and the shared model-variant
header.
