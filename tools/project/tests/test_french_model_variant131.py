import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant131Tests(family435.FrenchModelVariant435Tests):
    family = 131
    slot_header_delta = 130
    module_count = 12
    distinct_images = 12
    binding_count = 24
    tail_start = 0x1604
    spans = ((4, 0x15BC),)
    helpers = ((4, 5560, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (183, 273, 486, 490, 540, 637)),)
    entry_anchors = {
        4: 0x27BDFD88, 0x15B4: 0x03E00008, 0x15B8: 0x27BD0278,
        0x15BC: 4096, 0x15C0: 4096, 0x15C4: 4096, 0x15C8: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant131_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013C5BC D_8017C5BC\n'
                         '#define D_8013C5CC D_8017C5CC\n'
                         '#define D_8013C604 D_8017C604\n'
                         '#include "variant131_entry.c"\n')
        source = (directory / "variant131_entry.c").read_text()
        header = (directory / "variant131_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__|register|volatile)\b")
        for declaration in (
            "Model131Config *G32 config;", "SVECTOR ring[14];",
            "SVECTOR origin;", "SVECTOR particles[32];", "s32 texture[2];",
            "s32 elapsed;", "u8 command_group, completed, unknown_18A[2];",
            "extern GsIMAGE D_8013C5CC[];", "extern Model131Config D_8013C604[];",
            '#include "../../psyq/sdk_internal.h"',
        ):
            self.assertIn(declaration, header)
        for expression in (
            "const VECTOR D_8013C5BC = {4096, 4096, 4096, 0};",
            "work->config = &D_8013C604[command % 100];",
            "ring_angle = (i + 1) * 4096 / 7;",
            "ring_angle = i * 4096 / 7 - 4096 / 14;",
            "point->pad = i % 16;", "work->command_group = command / 100;",
            "config->radius * (csin(4096 / 14) << 1) / 4096;",
            "while (direction * position.vz <= direction * value)",
            "position.vz += direction * strip_length;",
            "if (time >= 0 && time < config->duration - j * 2)",
            "if (time < config->duration - j * 2 - 16)",
            "j = rand() % config->radius;", "point->pad = -1;",
            "if (time < j)", "else if (time >= config->duration)",
            "else if (time < config->duration - j)", "if (work->completed)",
        ):
            self.assertIn(expression, source)
        for expression, count in (
            ("Model_GetFrameStep()", 3), ("SetPolyFT4(&quad)", 1),
            ("RotAverage4(", 1), ("AverageZ4(", 2), ("RotTransPersN(", 3),
            ("MulMatrix2(", 3), ("GsSortPoly(", 3), ("func_80059A50(", 1),
            ("which ^= 1;", 1),
        ):
            self.assertEqual(source.count(expression), count)
        self.assertIn("                which ^= 1;\n            }\n", source)
        particle = source[source.index("if (time >= 0 && time < config->duration - j * 2)"):]
        self.assertNotIn("if (depth", particle)
        self.assertNotIn("if (flag", particle)

    def test_attempt_ledger_preserves_failed_and_exact_experiments(self):
        with (family435.ROOT / "notes/overlays/french-model-variant131-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        expected = {
            "native-seven-pair-beam-and-respawn": (5560, 895),
            "native-spilled-scalar-order": (5560, 880),
            "native-scale-before-rotation-and-sine-shift": (5556, 685),
            "native-page-initialization-and-terminal-precedence": (5568, 47),
            "native-terminal-inactive-else": (5552, 61),
            "native-nested-terminal-zero-return": (5568, 47),
            "native-ordered-terminal-guards": (5568, 47),
            "native-shared-zero-exit": (5552, 61),
            "native-distinct-ring-angle": (5552, 39),
            "native-terminal-no-cse-follow-jumps": (5556, 319),
            "native-completed-positive-terminal-branch": (5560, 0),
        }
        self.assertEqual(len(rows), 24)
        self.assertEqual(len({(r["attempt"], r["slot"]) for r in rows}), 24)
        experiments = [r for r in rows if r["result"] in ("mismatch", "text_exact")]
        self.assertEqual({(r["attempt"], r["slot"]) for r in experiments},
                         {(name, slot) for name in expected for slot in ("0", "1")})
        for row in rows:
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
            profile = ("gcc_2_8_1_g0_split_no_cse_follow_jumps"
                       if row["attempt"] == "native-terminal-no-cse-follow-jumps"
                       else "gcc_2_8_1_g0_split")
            self.assertEqual(row["profile"], profile)
        for row in experiments:
            size, differences = expected[row["attempt"]]
            self.assertEqual((int(row["instruction_bytes"]), int(row["different_words"])),
                             (size, differences))
            self.assertEqual(row["result"], "text_exact" if differences == 0 else "mismatch")
        terminal = [r for r in rows if r["result"] == "matched"]
        self.assertEqual({r["slot"] for r in terminal}, {"0", "1"})
        for row in terminal:
            suffix = "_slot1" if row["slot"] == "1" else ""
            path = family435.ROOT / f"src/overlays/french_model_variant/variant131_entry{suffix}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("5560", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(
                f"D_{base + 0x15CC:X} = 0x{base + 0x15CC:X}; // type:u8 size:0x38 defined:true",
                symbols)
            self.assertIn(f"[0x15CC, data, overlays/{module['name']}/image_view]", layout.read_text())
            self.assertIn(
                f"D_{base + 0x15BC:X} = 0x{base + 0x15BC:X}; // type:u32 size:0x10 defined:true",
                symbols)
            suffix = "_slot1" if module["name"].endswith("1") else ""
            self.assertIn(f"[0x15BC, .rodata, overlays/french_model_variant/variant131_entry{suffix}]",
                          layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (128, 128, 128, 128, 128, 128, 15, 0, 200, 100, 60, 76, 240, 80),
            1: (128, 128, 128, 128, 128, 128, 1, 0, 300, 150, 60, 90, 400, 120),
            2: (128, 128, 128, 128, 128, 128, 13, 0, 150, 150, -60, 80, 240, 40),
            3: (32, 128, 32, 64, 128, 64, 28, 0, 150, 150, 50, 90, 320, 60),
            5: (16, 128, 8, 96, 128, 32, 8, 0, 100, 70, 30, 60, 250, 70),
        }
        arguments = {183: 2, 273: 5, 486: 0, 490: 3, 540: 1, 637: 5}
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                argument = arguments[int(row["model"])]
                self.assertEqual(int(row["command_word"]), 62000 + argument)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 62000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<8B6h", data, 0x1604 + 20 * argument),
                                 descriptors[argument])
                observed.add(argument)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<1390I", data[4:0x15BC])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 71)
                self.assertEqual(set(calls), addresses)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x18C <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model131Config)": 20, "sizeof(Model131State)": 0x18C,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(GsIMAGE)": 28, "sizeof(POLY_FT4)": 40, "sizeof(DVECTOR)": 4,
        }
        for typename, fields in (
            ("Model131Config", (("part", 6), ("radius", 8), ("particle_half_size", 10),
                                ("spin", 12), ("fade", 14), ("duration", 16), ("delay", 18))),
            ("Model131State", (("ring", 4), ("origin", 0x74), ("particles", 0x7C),
                               ("texture", 0x17C), ("elapsed", 0x184),
                               ("command_group", 0x188), ("completed", 0x189))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 22)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant131_entry.h")
