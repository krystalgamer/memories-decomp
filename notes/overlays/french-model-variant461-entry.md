# French MODEL461 loader entry

The 5,380-byte loader entry is matching C in both physical MODEL705 images:
stage 9 at `0x8013B004` and stage 10 at `0x8017B004`. Each complete 20,480-byte
image matches its retail bytes. Both retain the named `gcc_2_8_1_g0_split`
profile, and all three existing helper sources, profiles, sizes and inventory
rows are unchanged.

| Image | SHA-256 |
| --- | --- |
| MODEL705 stage 9, slot 0 | `bf8d666909a25113c2ad7c8f46fb04dd78671202299fd439ceb457461100a45e` |
| MODEL705 stage 10, slot 1 | `97c3028074feb9bbdc2ef20460436ad642b958abd5eea951efe3ac7f75db31e7` |

## Recovered ownership

`src/overlays/french_model_variant/variant461_entry.c` implements the measured
constructors and runtime path, ring capture, projection and state gates.
Its local header keeps the uncertain layout local: configuration records have
stride `0x40`, and the observed context extends through `0x33C0`. SDK and
resident declarations come from the existing project headers.

The final allocation correction combines two real scalar lifetimes:

- `base` holds web latitude and the later small-ring phase, as well as the
  existing runtime path-origin angle.
- `j` holds the web column index and the later small-ring point bearing.
  The shared ribbon, fan, web and runtime bearing remains `angle`.

Separating the small-ring bearing into its own variable, as in MODEL470/477,
does not match this family. Sharing it with the column scalar first recovers
all nonweb bytes; sharing latitude with the actual phase then recovers the
native latitude/far-radius `s3` and reset-anchor `s5` allocation. Expressions,
accesses and control flow are unchanged. There are no fixed registers, inline
assembly, volatile accesses, fabricated dependencies or compiler changes.

The entry retains the native 256-byte frame. Its three prior helpers remain
the 2,636-byte French ribbons body at offset `0x1508`, the 1,348-byte Spanish
sheets body at `0x1F54`, and the 1,084-byte Spanish rings body at `0x2498`.
Both production ELFs contain all four C functions at their original addresses
and sizes.

## Attempts

`notes/overlays/french-model-variant461-entry-attempts.csv` records all 22
source/header/profile experiments and both final complete-image terminals.
Earlier independent-latitude and shared-phase attempts changed register
allocation outside the web; direct latitude expressions changed strength
reduction. Trial 21 recovered every nonweb byte but retained 20 web-only
register differences. Trial 22 is the exact combined ownership above.

The slot-1 wrapper remaps only the entry, three local helper names and local
configuration-data symbol. Data ownership, original image layout and helper
implementations remain unchanged.
