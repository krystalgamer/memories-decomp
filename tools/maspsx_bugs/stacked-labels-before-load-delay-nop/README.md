# Stacked labels in front of a load-delay `nop`

Status: awaiting verification against ASPSX 2.81.

`func_8013C864` (North American MODEL variant header 404, `model_variant_114`
and eleven more images; Spanish header 421 builds the same C, see
`src/overlays/spanish_model_variant/variant421_spiral.c`) matches retail in every
word except one: the `j` at function offset `0xE4`. Retail jumps to `+0x104`;
the build jumps to `+0x108`.

## What gcc hands maspsx

The signed `x / 8` expansion (`bgez` / `addu 7` / `sra 3`) has three paths into
one join, and gcc 2.7.2 labels it twice in a row:

```
	lw	$8,312($sp)
$L78:
$L79:
	addu	$20,$8,1248
```

`bgez ...,$L78` is the non-negative path, `j $L79` the biased one, and the
fall-through is the third. `lw $8` followed by `addu ... $8` needs a load-delay
`nop`.

## What maspsx does

`_handle_nop_before_next_instruction` moves the label that follows the load in
front of the `nop`, so a branch to it still executes the `nop`. It looks at one
label only:

```
lw	$8,312($sp)
$L78:
nop # DEBUG: Reuse of '$8'. ...
$L79:
addu	$20,$8,1248
```

`$L79` stays behind the `nop`, so the `j` lands one instruction past where the
`bgez` does.

## Retail

`game/SLUS_014.11` model 114 stage 9 slot 0 image, function at `0x8013C864`:

```
0xD8   0441000a   bgez  $v0, +0x104
0xDC   0002f0c3   sra   $fp, $v0, 3
0xE0   24420007   addiu $v0, $v0, 7
0xE4   0804f25a   j     0x8013C968        (= function + 0x104)
0xE8   0002f0c3   sra   $fp, $v0, 3
0xEC   8fa80138   lw    $t0, 0x138($sp)
0xF0   00000000   nop
0xF4   85031936   lh    $v1, 0x1936($t0)
0xF8   00000000   nop
0xFC   0043f023   subu  $fp, $v0, $v1
0x100  8fa80138   lw    $t0, 0x138($sp)
0x104  00000000   nop                      <- both branches land here
0x108  251404e0   addiu $s4, $t0, 0x4E0
```

The ASPSX output puts both labels in front of the `nop`.

## Reproduce

```
sh tools/maspsx_bugs/stacked-labels-before-load-delay-nop/build.sh
MASPSX=<patched maspsx.py> sh tools/maspsx_bugs/stacked-labels-before-load-delay-nop/build.sh
```

The first prints `j 108`, the second `j 104`. `func_8013C864.c` is the
preprocessed Spanish source (no project headers), and its gcc output is
identical to the one the project's compile produces.

## What was ruled out

- The C. The labels come from gcc's expansion of the signed divide by 8, not
  from a statement; no spelling of the source changes their number.
- The other compiler. gcc 2.8.1 (`gcc_2_8_1_g0_split`'s compiler) sends both
  branches to one label (`bgez $2,$L8` / `j $L8`) for this source, so the Spanish
  build never meets the stacked pair; gcc 2.7.2, the North American compiler, is
  the one that emits it.
- Anything else in the function: every other word is identical outside
  relocations.

## Suggested fix

`maspsx.patch` makes `_handle_nop_before_next_instruction` carry every
consecutive label in front of the `nop`, not only the first. With it the
function's `j` matches retail. Its blast radius, measured with the patch against
the unpatched checkout:

- the 288 C sources in `src/overlays/model_variant` compiled with the North
  American profile: **0** change, so no current match moves;
- the 2519 compiler outputs of a census of the open North American instances:
  **5** change, all Spanish `*_spiral` sources (headers 417, 421, 438, 441, 474)
  compiled with the North American compiler -- the same family as this function.

`tools/vendor/maspsx` is pinned and the build rejects a dirty one, so nothing is
patched here; the 12 images of this body (models 84, 88, 114, 162, 184, 369)
stay `unmatched_asm` until the pinned checkout is moved.
