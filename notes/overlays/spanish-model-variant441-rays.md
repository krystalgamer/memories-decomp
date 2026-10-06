# Spanish MODEL441 retained rays

Helper `+0x23FC..+0x2A48` is independently reconstructed matching C: 1,612
bytes in all ten configured header-441/591 MODEL loads (MODEL166, 360, 487
and 590, stages 7/8; MODEL709, stages 9/10). That is 16,120 instruction bytes
from one unique routine. An independent second-slot compilation also matches.
Screening 6,207 accepted regional C entries compared 64 same-size bodies
(MODEL435 and MODEL442 ribbons, MODEL411 sheets); none has this normalized
shape, so no region has accepted C for this routine.

The accepted MODEL442 ribbons share this routine's shape (99% normalized
similarity), but they are a different routine with different layouts and
colors. They were used only as structural evidence. The same shape also occurs
at `+0x2480` in eight header-417/567 images. Those instances are left for a
separate change.

The modules already carry the accepted lines, tube, ribbon and framebuffer
rings. This change converts the `+0x23FC` helper from ASM to C; as recorded in the
existing inventory, no entry call-graph path to it has been recovered.
The MODEL441 binding list gains the SDK alias `RotTransPers` for `0x80087868`,
which is also bound as `func_spanish_80087868`, as in the MODEL442 and MODEL445
binding lists.

## Behavior

Eight 0x6C-byte ray records at context `+0x1174` each hold two stations: an
inner one with radius 40 and an outer one with radius 150. Each station also
has an offset point, 16 units along the axis angle.

- **Angles:** the axis angle is `ratan2(direction_b) + 3072`, negated when the
  mirror halfword at `+0x2730` is one. The tilt is `ratan2(direction) + 1024`.
  The other two `ratan2` results are discarded.
- **Stations:** angles start at `+0x2718` and advance by 512 per ray. The outer
  station's depth is 160, or 192 on odd frames.
- **Matrix:** one matrix places the rays at the origin advanced by the progress
  along the direction, scaled by the pulse size at `+0x10CC`.
- **Projection:** each station projects its segment and offset point, and
  stores its screen angle and its perpendicular width components.
- **Drawing:** each ray draws one `POLY_G3` arrowhead through `+0x252C`, gray
  at the base and magenta at the tip, when its depth is positive.
- **Rotation:** on the last timing slot, the base angle advances by
  `step * 32`.

## Source detail that reproduces the target

The constant width is assigned once, before the loops. The frame word is
cached before the loops, and the station depth is computed inline from it. The
radius defaults to 150 and is overridden for the inner station. This is the
construction used by the accepted MODEL442 ribbons.

Earlier forms rematerialized the width per station and ended up 16 bytes
short. Nine such experiments are preserved. There are no extra locals, forced
registers or inline assembly.

## Acceptance evidence

All ten complete 20KiB images match using `gcc_2_8_1_g0_split`. The
21-row ledger preserves all nine rejected experiments, the exact source and
second-slot compilations, and all ten terminal full-image results.

Regressions cover:

- the record and state layouts, including repeated header inclusion
- selected relocations and callees against every image
- C segment order, statuses, totals and bindings in the existing MODEL441
  suites

Terminal dependency fingerprints hash the body, the private header, the shared
model-variant header and `gpu_packets.h`, in that order.
