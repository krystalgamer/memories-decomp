# Model geometry-effect configuration data

The subdivided, disc and burst handlers read four independently stored
initialized objects, each defined as the record type its handler reads.
Naming does not infer meanings for unread bytes.

`gModel_SubdividedEffectConfig` is the former `D_800915E8`, a 28-byte
`ModelSubdividedEffectConfig`: three six-byte colour channels (one byte per
base vertex), signed height/radius minima (300, 300) and growth/fade
durations (10, 90), then an unread zero halfword. Its bytes, alignment,
duration arithmetic and regional behavior are unchanged from the former seven
`u32` words.

`gModel_DiscEffectConfig` is the former `D_80091604`, still the established
12-byte `ModelDiscEffectConfig` instance. The handler reads its rim colour,
minimum radius, initial/full level and target scale. The last zero halfword
remains unread and unclassified.

`gModel_BurstImages` is the former `D_80091610`: seven 28-byte
`ModelBurstImage` records (196 bytes, the former 98 halfwords), each a pixel
mode and the frame-buffer rectangles of an image and its colour table, with
zero image/CLUT address words. Model_UpdateBurstEffect reads only the first;
the other six are unread. Every record's CLUT x is the regional
`MODEL_BURST_CLUT_X` (0x200, 0x280 in the European build).

`gModel_BurstRingPalette` is the former `D_800916D4`, a twelve-byte
`ModelBurstPalette`: one byte per ring for each channel (red, green and blue
rings at 0xFF over 0x40), then three unread zero bytes.

Native aliases preserve the original binding values and region-specific
providers. No regional table contents are assumed equal to NA.

## Independent initial scale vectors

`gModel_SubdividedEffectInitialScale`, `gModel_DiscEffectInitialScale`,
`gModel_BurstEffectInitialScale` and `gModel_SparkEffectInitialScale` are the
former `D_8001185C`, `D_8001187C`, `D_8001188C` and `D_8001189C`. Their
respective handlers copy each complete `VECTOR` into a local scale before the
existing effect work.

The words at each label are `{ONE, ONE, ONE, 0}` in every region. They
nevertheless remain four independent sixteen-byte constants and relocation
identities, not deduplicated. `model_effect_handlers.c` defines them as
`const VECTOR` read-only data (#2602), each ahead of its handler: in NA and
Japan the subdivided handler's `"%d (0x%x) Byte\n"` format lies between the
first and the other three; PAL builds have no such trace and keep the four
back to back at `0x80011820`.

The European subdivided-effect scale bridge retires with its explicit
canonical provider binding. The similarly spelled French compatibility
labels are distinct identities and remain untouched. Existing regression
controls change identifiers only; all reader signatures, profiles, source
groups, overlay bodies, historical attempts and numeric Lua traces remain
unchanged.
