import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant76Tests(family435.FrenchModelVariant435Tests):
    family = 76
    slot_header_delta = 130
    module_count = 8
    distinct_images = 8
    binding_count = 21
    tail_start = 0xB98
    spans = ((4, 0xB98),)
    helpers = ((4, 2964, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (57, 447, 562)), (9, (489,)))
    entry_anchors = {
        4: 0x27BDFF08, 0xB40: 0x14400009, 0xB44: 0x24020004,
        0xB50: 0x10400003, 0xB54: 0x24020001, 0xB5C: 0x24020002,
        0xB60: 0xA2221A28, 0xB64: 0x24020001,
        0xB90: 0x03E00008, 0xB94: 0x27BD00F8,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant76_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013BB98 D_8017BB98\n'
                         '#include "variant76_entry.c"\n')
        source = (directory / "variant76_entry.c").read_text()
        header = (directory / "variant76_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__)\b")
        for declaration in (
            "extern Model76Config D_8013BB98[];", "Model76Config *G32 config;",
            "SVECTOR positions[256];", "SVECTOR velocities[256];",
            "SVECTOR rotations[256];", "SVECTOR vertices[4];", "s16 life[256];",
            "Model76Face faces[4];", "SVECTOR *G32 a;", "SVECTOR *G32 b;",
            "SVECTOR *G32 c;",
        ):
            self.assertIn(declaration, header)
        for expression in (
            "u8 step;", "&D_8013BB98[command]",
            "rotation->vx = rand() % (config->angle_x_range * 2) - config->angle_x_range + j;",
            "rotation->vy = rand() % (config->angle_y_range * 2) - config->angle_y_range + j;",
            "rotation->vz = rand() % 4096;",
            "j = (side * 450 - origin.vz) / config->lifetime * 3 / 2;",
            "for (j = 0; j < step; j++)",
            "rotation->vz = (rotation->vz + 4096 / config->lifetime) % 4096;",
            "clip > 0 && depth >= 0 && flag >= 0",
            "func_8005B260((u32 *)&triangle, ot, (u16)depth, 1);",
            "if (work->elapsed >= config->delay + config->interval * config->count + config->lifetime)",
        ):
            self.assertIn(expression, source)
        self.assertEqual(source.count("rand()"), 3)
        self.assertEqual(source.count("work->elapsed += step;"), 2)
        self.assertEqual(source.count("work->completed = 1;"), 1)
        self.assertNotIn("const VECTOR", source)

    def test_attempt_ledger_preserves_failed_and_exact_experiments(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        with (family435.ROOT / "notes/overlays/french-model-variant76-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 38)
        self.assertEqual(len({(row["attempt"], row["slot"]) for row in rows}), 38)
        expected = {
            "complete-status-decision-chain": ("2964", "5"),
            "completion-state-switch": ("2964", "5"),
            "context-relative-face-offset": ("2968", "271"),
            "explicit-first-completion-edge": ("2964", "5"),
            "face-offset-from-live-work": ("2960", "653"),
            "first-completion-fallthrough-return": ("2964", "5"),
            "flat-triangle-pointer-table": ("3024", "730"),
            "initialization-pointer-boundary-and-terminal-else": ("2960", "22"),
            "measured-face-records": ("2956", "658"),
            "native-cursor-and-completion-order": ("2960", "44"),
            "native-first-candidate": ("2956", "658"),
            "native-pointer-preparation-order": ("2960", "36"),
            "return-new-completion-state": ("2964", "5"),
            "return-stored-completion-state": ("2964", "5"),
            "shared-terminal-status-return": ("2960", "22"),
            "terminal-existing-state-first": ("2964", "5"),
            "accepted-master-control": ("2964", "5"),
            "completion-inside-positive-final-threshold": ("2964", "0"),
        }
        experiments = [row for row in rows if row["result"] in ("mismatch", "text_exact")]
        self.assertEqual({(row["attempt"], row["slot"]) for row in experiments},
                         {(name, slot) for name in expected for slot in ("0", "1")})
        for row in rows:
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
        for row in experiments:
            self.assertEqual((row["instruction_bytes"], row["different_words"]),
                             expected[row["attempt"]])
            self.assertEqual(row["result"], "text_exact" if row["different_words"] == "0" else "mismatch")
            frame = 256 if row["attempt"] == "flat-triangle-pointer-table" else 248
            self.assertIn(f"Frame {frame}/248;", row["reason"])
        terminal = [row for row in rows if row["result"] == "matched"]
        self.assertEqual({row["slot"] for row in terminal}, {"0", "1"})
        for row in terminal:
            source = directory / ("variant76_entry" + ("_slot1" if row["slot"] == "1" else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("2964", "0"))

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (128, 64, 0, 15, 40, 12, 8, 2, 150, 114, 114, 32),
            1: (160, 160, 192, 50, 40, 12, 6, 0, 100, 114, 114, 40),
            2: (160, 160, 192, 45, 40, 12, 4, 7, 100, 114, 114, 140),
        }
        arguments = {(7, 57): 0, (7, 447): 2, (7, 562): 0, (9, 489): 1}
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot = int(row["slot"])
                argument = arguments[int(row["stage"]) - slot, int(row["model"])]
                self.assertEqual(int(row["command_word"]), 6000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                observed.add(argument)
                self.assertEqual(struct.unpack_from("<8B8x4h", data, 0xB98 + 24 * argument),
                                 descriptors[argument])
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                self.assertIn("SetPolyF3 = 0x80082E08;", bindings)
                self.assertIn("RotAverageNclip3 = 0x80087AB8;", bindings)
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<741I", data[4:0xB98])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 36)
                self.assertEqual(set(calls), addresses)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 10 * 2048)):
                    self.assertTrue(context + 0x1A5C <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model76Config)": 24, "sizeof(Model76State)": 0x1A5C,
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(POLY_F3)": 20, "sizeof(PSXLONG)": 4,
            "sizeof(((Model76State *)0)->faces[0].a)": 4, "sizeof(Model76Face)": 12,
        }
        for typename, fields in (
            ("Model76State", (("positions", 4), ("velocities", 0x804), ("rotations", 0x1004),
                              ("vertices", 0x1804), ("life", 0x1824), ("elapsed", 0x1A24),
                              ("completed", 0x1A28), ("faces", 0x1A2C))),
            ("Model76Config", (("count", 3), ("lifetime", 4), ("fade", 5), ("interval", 6),
                               ("part", 7), ("size", 16), ("angle_y_range", 18),
                               ("angle_x_range", 20), ("delay", 22))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 26)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant76_entry.h")
