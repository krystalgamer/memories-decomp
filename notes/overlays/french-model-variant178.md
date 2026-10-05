# French MODEL178 entry

Model 141 stages 9/10 load two distinct 20,480-byte runtime images with
headers 178/308 and command 109000. Each entry is 3,384 instruction bytes
at image offset `0x4`, followed by a 16-byte source-owned unit-scale vector.
The slot wrapper renames only the entry and three image-local symbols.

The entries were recovered independently from both complete retail images.
All 846 reachable instructions form one closed entry, with 41 direct calls
to 21 existing resident destinations and no local or indirect calls.
Both complete images reproduce exactly with sized linked C entry definitions,
exclusive C literal contributions and disjoint raw owners. The 21 actual
resident callee bodies were also checked against the exact French resident
ELF. Existing local declarations and GCC 2.8.1/MASPSX 2.81 profiles remain
authoritative; no new SDK signature calibration is claimed.

## Measured state and behavior

The 30-byte descriptor contains primary RGB at `0..2`, burst RGB at `3..5`,
the active model part at `6`, and an unidentified byte at `7`. Its halfword
fields are quad half-size `8`, target spread `0xA`, burst half-size `0xC`,
initial upward velocity magnitude `0xE`, horizontal velocity spread `0x10`,
count `0x12`, spacing `0x14`, travel `0x16`, fade `0x18`, burst duration
`0x1A` and delay `0x1C`. Selection uses `command % 100`.

The minimum context view is `0x450` bytes: descriptor pointer at `0`,
64 position SVECTORs at `4`, 64 endpoints at `0x204`, four velocity
SVECTORs at `0x404`, two packed texture words at `0x424`, completion byte
at `0x448`, and elapsed time at `0x44C`. The intervening `0x42C..0x448`
bytes remain unidentified. These are measured accessed spans, not a claim
that the entire allocator extent has been recovered.

Initialization clears all four position halfwords, copies and randomizes
opponent endpoints, creates four velocities, and uploads texture 1 before
texture 0 with different upload modes. The frame step is read once per
entry invocation, including initialization.

The first update phase anchors delayed particles to an active model part,
then renders expanding and fading textured quads. Position interpolation
divides each remaining displacement by remaining travel time before
multiplying by the saved frame step. Texture frames use `(time / 2 + i) % 8`.
The second phase emits four velocity-based quads per endpoint. Its position
arithmetic deliberately narrows each multiplication to a signed halfword
before dividing, then adds the endpoint. Projection is followed by sorting
without an added depth/flag guard.

After elapsed time advances, the native return sequence is 0 before travel,
4 while the spaced sequence remains active, 1 on the first completion,
0 on subsequent completion calls, and 2 after the final burst duration.
The initialized but otherwise unused F4 primitive is retained.

## Matching evidence and remaining coverage

The first paired candidate had the correct 3,384-byte size, 304-byte frame
and literal, but differed in 14 words per slot. Nine were the first-phase
matrix/endpoint register allocation; five were terminal branch layout.
Sharing the real endpoint/velocity walker across disjoint phases and using
a byte completion result recovered every instruction in both slots.
The six-row attempt ledger preserves both paired experiments and two
terminal canonical matches. No forced registers, artificial dependencies,
inline assembly or masked comparisons are used.

Each image retains a four-byte raw header, two contiguous 28-byte GsIMAGE
views at `0xD4C..0xD84`, and a 17,020-byte unclassified suffix at
`0xD84..0x5000`. The descriptor begins inside that suffix; it is read
through a local typed view without promoting its storage to C.
The 34,040 suffix bytes across both images are not claimed as data-only
or excluded game code. This change adds two C instances, 6,768 instruction
bytes and 32 literal bytes; it does not establish exhaustive French
runtime completion.
