# Unchiga_Symbols — named-symbol research for SLUS_014.11

Symbol research from Unchiga's matching decompilation of Yu-Gi-Oh! Forbidden
Memories (SLUS-01411, USA), refreshed 2026-09-03. Every name here is 1:1
byte-verified: after each rename batch the whole executable was re-split and
rebuilt, and the rebuild is byte-identical to the original (MD5
`dab1b3c9a6b8a56558b5ca8f807339c3`, SHA1
`84747e64f6da8e764206ec203e489acf8c9dcf7d`). Names marked live-traced were
also verified against the running game (function-entry tracing, RAM diffing,
injected input on a recompiled build).

Naming style: every project-assigned name is lowerCamelCase (`dropTable`,
`textBoxCreate`, `storyFlags`); PsyQ SDK names and the `SD_*` sound-driver
names (DotR vocabulary) keep their original form. Map onto your
`Subsystem_VerbObject` scheme as you see fit — the addresses are the claim,
the spelling is ours.

## Files

| file | why it exists |
|---|---|
| `symbols_guide.md` | The headline document: 141 confirmed symbols organized by where in the game they run, in plain English. |
| `known_functions.md` | The complete roster of 1,111 named functions with addresses, each tagged with its evidence source. |
| `known_variables.md` | The complete roster of 108 named variables and data symbols, same treatment. |
| `descriptions.md` | The naming gate: 180 verified one-paragraph descriptions, because a name only counts as done once its description matches the matched code or a live observation. |
| `findings.md` | The live-trace evidence ledger (F1–F120) recording what was proven, how, and the status of each claim. |
| `suspects.md` | The not-yet-100% identifications, kept apart from the confirmed set with the evidence gathered and what would graduate each. |
| `modules.md` | What the disc-loaded screen modules are and how their shared address range was traced. |
| `modules/` | Per-module symbol files (42 symbols) for free duel, name entry, password, main menu and overworld, kept separate because those screens load into the same addresses. |
| `NAMING.md` | Where each kind of name comes from, how much to trust each evidence tier, and the lowerCamelCase house convention. |

Source repository: Unchiga/ygofm-decomp (private); contact Unchiga for
access or questions. Corrections and counter-evidence welcome.
