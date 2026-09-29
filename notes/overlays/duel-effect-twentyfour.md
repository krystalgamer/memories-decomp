# French effect twenty-four: Exodia burst

`func_8014D3E8`, `8014D3E8..8014E35C`, is a complete **3,956-byte**
matching C lifecycle. It is independently recovered from the French bank
with the existing `gcc_2_8_1_g0_split` profile (GCC 2.8.1 / MASPSX 2.81).
No source, header, compiler profile or registration from another pending
French branch is required.

## Caller and storage contracts

The matching dispatcher calls this function for effect 24. The matching
resident `DuelScene_UpdateExodiaResult` creates request `0x18` after its
five-card presentation and preceding centre sparkle. Before that request,
it distributes the five display objects into the canonical work slots using
the pose table, then waits through the preceding animation phases.
`DisplayObject_CopyWorkSlots`, French `8002CD24..8002CD54`, copies exactly
five pointer words and a terminating zero. This is not the compact
field-row collector and there is no empty-row lifecycle here.

The output is the real 21-word bank object `D_8015B7A0`, not a new allocation
or a private five-word array. The lifecycle reads the first five objects'
signed coordinates through the canonical 112-byte `DisplayObject` layout.
The pose table maps its five records to distinct slots 3, 2, 4, 1, 0.
At their target positions, the effect centre is `(158,126)` and its unsigned
half extents are `(91,87)`. Its centre adjustments deliberately use 24/28,
not the caller's 26/30 biases. No guessed null fallback or geometry correction
has been added.

The work buffer is **2,120 bytes / `0x848`**, with measured target offsets:

| Offset | Storage |
| --- | --- |
| `000`, `028`, `050` | Five targets, moving points, point steps |
| `078` | Centre vector |
| `080`, `280`, `480` | Three separate 64-vector head, tail, velocity pools |
| `680` | 32 ray rotations |
| `780`, `782`, `784` | Half extents and two strip-width halfwords |
| `788` | 32 unused bytes; no invented meaning or initialization |
| `7A8`, `7AA`, `7AC` | Beam width, arrival counter, connection tick |
| `7AE`, `7B0`, `7B2` | Active lines, active rays, lifecycle stage |
| `7B4`, `7B8` | Centre scale and 32 independent ray scales |
| `838`, `83C`, `840`, `844` | Tick, centre colour, flash colour, line colour |

## Preserved lifecycle

Initialization accepts every nonnegative phase, captures five object
positions, initializes 64 paired line endpoints and 32 ray rotations/scales,
and establishes the three colours. Negative phases advance rendering.
The original frame-step query is called and its return discarded; the
override is forced to one. Matrix push/pop and local initial scale remain
the original sequence.

Five points move for eight updates. Connections then appear in the order
`0-2`, `2-4`, `1-4`, `1-3`, `0-3`, at successive four-tick thresholds.
The two connections incident to slot zero use twice the beam width.
The connection counter caps at 24, beam width at eight and arrival at eight.
The line colour alternates between stored pink and white; only the
connection pass halves its stored-colour RGB channels.

Once the connections widen, the burst activates up to 64 projected
gradient lines and 32 independently scaled rays. Each line starts with
head `component*96/4096`, tail `component*128/4096` and velocity
`-tail/8`, with signed truncation and components in `[-4095,4095]`.
The line advances while any head component lies outside `[-32,32]`;
otherwise all endpoints and velocities are regenerated. Projection ignores
the return depths and tests the final GTE **flag**, then uses the real
ordering-table pointer object and depth halfword.

The centre scale rises by 128 to `0x6000`; ray scales rise by `0x400`,
reset to 4096 at the threshold, and regenerate their rotations. Line
activation grows each update; ray activation grows on even ticks.
Stage two requires both activation counts at their caps, the centre at
full scale, and all three centre colour bytes white. The screen flash
rises to 192 by fours, then resets white and writes the canonical request
byte `field_1D`. Stage three draws the flash and signals `D_8009B261`.
Completion does not wait for individual ray scales to reach their caps.

## Experiments and exact proof

Four complete source candidates were retained locally:

1. 3,960 bytes: centre-coordinate subtraction reassociated across the
   adjustment, with an extra instruction; scale-loop setup also differed.
2. 3,956 bytes: explicit signed-halfword centre adjustments fixed every
   coordinate instruction. Only the constant/counter load order differed.
3. 3,956 bytes: a predecrement countdown left those same two words different.
4. **Exact 3,956 bytes**: a forward scale-initialization loop gives GCC's
   reversed loop with precisely the target setup order. Replacing an
   unnecessary effect-six header dependency with the actual shared
   contracts preserved exactness.

The independent complete-bank link passed before source promotion. The
90,112-byte image retains SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Production checks cover all seven configured French images, all seven
terrain copies, all **188 C owners / 82,752 bytes**, all **30 complete bank
C objects**, and 15 named overlay routine owners. Existing 63 bank C
registrations are preserved; the branch has **64/85 C / 26,900 bytes**.

Six real input-object and linked-ELF data owners were checked byte-for-byte:
initial scale `80146148/16`, texture words `8015B748/84`, copied slots
`8015B7A0/84`, ordering-table pointer `8015B7F4/4`, offset `8015B7F8/8`
and depth `8015B800/2`. None is an absolute data alias. The only added
resident alias is the already matching 48-byte slot-copy function.
The target compiler also verifies 51 layout constants, including the
20-byte gradient line, 40-byte textured quad and canonical 32-byte request.

A target-derived deterministic control/bounds model verifies the five-slot
permutation and connection indices, capped arrays, all 8,191 possible
particle component values, and stages reached at updates 40, 293 and 340.

## Accepted baseline integration

Integration through accepted master `3ed6b0f15` preserves all 77 accepted
French matching entries and inventory rows verbatim, including effect 23.
Only the reviewed effect-24 registration and unchanged source/header are
added: **78/85 bank C functions / 56,984 bytes**,
seven assembly boundaries, and **202 configured French C instances /
112,836 bytes**. No pending branch is stacked or counted.

Both sides' bindings, real owners, evidence and tests remain present, with
the shared 21-word card buffer declared once. All seven complete French images
and terrain copies match. The combined proof checks 202 linked C owners,
all 44 complete bank C objects, 15 routine owners, six real input/final data
owners and 51 target layout constants. Duel and progress regressions cover
both the accepted effect-23 and reviewed effect-24 registrations.

Accepted shared sources, North American and Spanish registrations, and the
authenticated CI input repair remain unchanged. Fresh hosted checks must pass
on this head; historical input-staging failures are not bypassed.
This model is not execution of the C lifecycle; full-image byte identity
is the acceptance gate. Focused bank/progress regressions and repository
source/metadata policies accompany it.

The remaining seven assembly boundaries, boot ownership, MODEL/SU loads and
overworld fragment remain unresolved. This match does not establish
exhaustive French runtime completion.
