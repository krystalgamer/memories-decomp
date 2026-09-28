# Italian language wrappers

These three wrappers reuse the existing European/shared function bodies and
headers, selecting the independently measured Italian language index **3**.
All other resident units reuse their existing shared or Spanish sources.
There are no copied function bodies, inline assembly or source-local externs.

| Wrapper | Functions | Bytes | Named profile |
|---|---:|---:|---|
| `main_init.c` | 1 | 404 | `gcc_2_8_1_g8_split_no_sched1` |
| `debug_menu_sound_entry.c` | 3 | 1284 | `gcc_2_8_1_g8_split` |
| `main_run_boot_sequence.c` | 3 | 776 | `gcc_2_8_1_g8` |

Only the language immediate differs inside each complete source/profile group;
the four neighboring functions remain byte-identical. Instruction evidence,
full-image acceptance and the preserved C-data ownership boundary are recorded
in the [Italian target configuration](../../../config/sles_03950/README.md).
