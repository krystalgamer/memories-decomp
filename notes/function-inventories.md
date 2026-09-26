# Function inventories for another game version

Start with [`config/templates/functions.csv`](../config/templates/functions.csv)
and [`config/templates/function_regions.json`](../config/templates/function_regions.json).
Copy them to a new version's configuration directory. The empty regions array
is a placeholder, not a valid classification: fill it with contiguous
`start`/`end` ranges covering the resident text in that version's
`image_map.json`. Each range needs a `name` and `module` (`game`, `psyq/crt`,
or `psyq/sdk`); SDK/CRT ranges also need `status: "sdk_asm"`. Split game
ranges around embedded SDK routines. Record the ownership evidence in each
region's `evidence` field rather than assuming the same addresses across
versions.

After building and byte-verifying that version's image, populate the CSV from
its own matching-C manifest, generated assembly, and **linked ELF symbols**
using `tools/project/regional_inventory.py`:

```sh
python3 tools/project/regional_inventory.py \
  --assembly-root tmp/splat/VERSION/asm \
  --manifest config/VERSION/matching_c.json \
  --elf tmp/project-build/VERSION.elf \
  --regions config/VERSION/function_regions.json \
  --output config/VERSION/functions.csv
```

For each runtime overlay, pass its assembly directory, matching manifest and
linked ELF in the same way, but use `--module overlay/NAME` instead of
`--regions`; write to a function-inventory CSV under that version's overlay
configuration directory. A fully matched overlay may have no unmatched
assembly functions.
Data labels and mixed code/data tails without independently established
function boundaries are **not** separate functions; report percentages only
against identified functions and bytes. Do not copy byte totals from another
release.

The CSV records `address,size,name,status,module,notes`, in image order.
Statuses are `matching_c`, `unmatched_asm`, `handwritten_asm`, and `sdk_asm`.
The generator retains documented classifications and notes on refresh, checks
function ranges for overlap, and requires a unique linked symbol with exactly
the matched range's size. Assign `handwritten_asm` only after independent
source/instruction evidence. When such evidence has already been reviewed
against another version, `--handwritten-reference path/to/functions.csv`
and `--reference-assembly-root tmp/splat/REFERENCE/asm` can seed same-named,
same-size handwritten entries only when their instruction opcodes and
registers match after masking address/branch immediates. Review provenance
separately: structural similarity alone cannot establish that code was
handwritten. The Japanese inventory uses the independently reviewed North
American counterparts for its initial seed.

For the current Japanese release, `MAKEFLAGS=-j4 make japanese-inventory`
sequentially verifies the resident image and all configured overlays, then
refreshes their inventories. `make progress` regenerates the root README from
these tracked inventories, their manifests, and the generated resident split.
