# Spanish MODEL474 offset-sheet helper

Independently recover the previously unmatched 1,272-byte helper at image
`+0x2BD8..+0x30D0`, using named `gcc_2_8_1_g0_split` (GCC 2.8.1 / MASPSX 2.81).
This is not a port of an accepted regional implementation.

**Execution is not established.** The entry calls only `+0xE38` and `+0x1738`.
The selected helper has no incoming edge in the seven recovered local CFGs,
and no literal pointer to it was found in any of the four images. These facts
do not prove that external or computed invocation is impossible. Exact C
coverage here means recovered instruction ownership, not observed runtime
coverage. No caller-side timing guard or original-context call proof is claimed.

## Physical scope

An exhaustive scan of Spanish MODEL records and stages 7 through 10 finds four
physical images with headers 474/624:

| Model | Record | Stages | Sectors | Request | Descriptor | Growth | Fade |
|---|---|---|---|---|---|---|---|
| 121 | 121 | 7/8 | 33576/33586 | 640001 | `+0x39D0` | 120..140 | 500..520 |
| 431 | 381 | 9/10 | 105356/105366 | 640000 | `+0x399C` | 40..80 | 500..520 |

Load bases are `0x8013B000` and `0x8017B000`; selected symbols remain
`func_8013DBD8` and `func_8017DBD8`. The second source changes only the symbol.
Each complete 20,480-byte image matches its own retail hash. The manifest
retains every previously registered module and adds these four images.
The instance CSV records physical identity and hashes.

## Complete code inventory and retained raw owners

| Image range | Bytes | Owner / observed reachability |
|---|---|---|
| `+0x0004..+0x0E38` | 3,636 | Assembly entry |
| `+0x0E38..+0x1738` | 2,304 | Assembly, directly entry-called |
| `+0x1738..+0x1DC0` | 1,672 | Assembly, directly entry-called |
| `+0x1DC0..+0x2240` | 1,152 | Assembly, no entry-call edge |
| `+0x2240..+0x2BD8` | 2,456 | Assembly, no entry-call edge |
| `+0x2BD8..+0x30D0` | 1,272 | Selected exact C, no entry-call edge |
| `+0x30D0..+0x38BC` | 2,028 | Assembly, no entry-call edge |

All seven CFGs are closed and contiguous, with all words visited, one return
and no indirect calls. Four instances become C (5,088 instruction bytes);
24 other function instances remain generated assembly, including twelve
instances of the three other unreferenced helpers.

The four-byte header and 5,956-byte suffix `+0x38BC..+0x5000` remain raw owners
in each image. Descriptor evidence within the suffix does not classify every
remaining byte as data or establish that no additional code exists there.

## Local layout and bounded ownership evidence

The private header uses established local primitive/SDK declarations and G32
for the stored timing pointer. Target-compiled constants verify:

| Context field | Offset / extent |
|---|---|
| Two 152-byte sheet records | `+0x1198..+0x12C8` |
| Reused 52-byte GT4 packet | `+0x1AA8..+0x1ADC` |
| Three signed origin words | `+0x1B50..+0x1B5C` |
| Three signed direction words | `+0x1B64..+0x1B70` |
| Frame / time / step / timing pointer | `+0x1B90 / +0x1B94 / +0x1B9C / +0x1BA4` |
| Signed word displacement / phase | `+0x1BE0 / +0x1C1C` |

The `0x1C20` state view is a partial helper view, **not allocation capacity**.
Entry instructions independently establish the two-sheet base, 152-byte stride
and count. The helper preserves its own incoming `a0` in `s2`; its packet
pointer remains `a0+0x1AA8`. Register-write checks bound both sheet iterations,
four indexed corner arrays, twelve RGB byte stores, and stack-local projection
outputs. These are helper-internal lifetime and layout proofs, not proof that
entry supplies its context to this unreferenced function.

The origin and direction are three words each, not padded VECTORs. Entry also
uses an SVECTOR beginning immediately after the origin at `+0x1B5C`.
Descriptor addressing at entry `+0x94..+0xC0` proves base `+0x399C`, a 52-byte
stride, and the stored pointer offset. The final private descriptor includes
the unknown four-byte tail after `fade_end`; the initial scratch view ended
at 48 bytes. Both actual requests select nonzero growth/fade intervals.

Resident ELF/map checks establish all 35 C/assembly callee bindings, four
loader/dispatcher owners and eight stored slot-pointer values. The private
view is compatible with those addresses without overlapping loaded regions.
This compatibility does not establish actual invocation of the selected helper.

## Recovered behavior

Each of two sheets projects four Gouraud-textured quads. Sheet zero uses the
origin; sheet one adds three signed `direction * displacement / 1024`
components, computing all components before storing matrix translations.
Odd frames add signed `size / 8` uniformly to the scale. The first three
corners receive inner RGB and the fourth outer RGB. Sorting requires strictly
positive depth, narrows priority to 16 bits, and has no projection-flag check
or depth multiplier.

After rendering, the first sheet grows toward 4096 during phase zero and
advances to phase one when clamped. The second sets 4096 in phases one/two;
in phase three it grows by `step * 512` to 8192 and advances to phase four.
Each sheet independently checks timed fade, with amplitudes 4096 and 8192.
Fade starts at `time >= fade_begin`, and nonpositive results become zero.
The fade checks are not else branches of growth. Preserve the unsigned timing
arithmetic; no unobserved caller-side growth-start precondition is added.

## Recovery and regression evidence

The first independent candidate had exact length but twelve induction-register
differences. Reversing cursor/counter initialization increased this to fifteen;
changing declaration order retained twelve. Ordinary indexing of all four
corner arrays matched exactly; a separate second-slot compilation also matched.
No forced registers, artificial frame padding, instruction edits or ad hoc
compiler flags were used.

The attempt CSV retains those five comparisons and four complete-image terminal
records. Production dependency fingerprints hash body C followed by its private
header. Eight regression tests cover physical census/requests, registrations,
closed CFGs and the absent selected-call edge, internal lifetimes, 34 compiled
layout constants, complete-image C/assembly/raw owners and ten selected call
relocations per image, resident owners/layout compatibility, and fingerprints.
