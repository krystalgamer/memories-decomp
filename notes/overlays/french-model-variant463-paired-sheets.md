# French header-463 paired sheets helper

`func_8013D318` / `func_8017D318` (image offset `0x2318`, 0x548 bytes)
draws and sizes the header-463 sheet set whose count depends on the timing
mode. `french_model_variant/variant463_paired_sheets_slot1.c` renames the
body for the slot-1 images.

It is built into the same eight images as the
[header-463 sheets helper](french-model-variant463-sheets.md). In the French
raw slot-1 images, a new C segment at `0x2318` is followed by an
`unclassified_mid` data segment that runs to the sheets helper at `0x32E8`.

- **Shape.** The control flow follows the Spanish header-461 sheets helper
  (`variant461_sheets.c`), with these differences:
  - the count is 1 in timing mode 1, and the timing record's first count
    otherwise;
  - each sheet scales uniformly, and the scale is written in both position
    branches;
  - sheets below the count grow by timed division from phase 0 to 1;
  - sheets fade from phase 5 and move to phase 6 when they vanish;
  - the remaining sheets follow the primary records.
- **Count selection.** It is written as two mode tests chained with the
  paired test. The first-count arm also resets the sheet index. The reset is
  later deleted, but before that it stops jump optimisation from hoisting
  the second constant count store above its branch.
- **View.** The view lives in `variant463_paired_sheets.h`. The bindings
  are the ones the sheets helper uses.

All eight images rebuild exactly. The
[attempt ledger](french-model-variant463-paired-sheets-attempts.csv) records
the probes.
