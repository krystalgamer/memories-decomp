import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant141Tests(family435.FrenchModelVariant435Tests):
    family = 141
    slot_header_delta = 130
    module_count = 6
    distinct_images = 6
    binding_count = 23
    tail_start = 0xFA8
    spans = ((4, 0xF60),)
    helpers = ((4, 3932, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (428, 445)), (9, (20,)))
    entry_anchors = {
        4: 0x27BDFE90, 0xD0: 0xA6C00000, 0xD4: 0xA6C00002,
        0xD8: 0xA6C00004, 0xE8: 0x1440FFF9, 0xEC: 0,
        0xD18: 0x92240006, 0xD68: 0x92240006, 0xDB8: 0x92240006,
        0xF18: 0x10400003, 0xF1C: 0x24020001, 0xF28: 0xA2E20410,
        0xF58: 0x03E00008, 0xF5C: 0x27BD0170,
        0xF60: 4096, 0xF64: 4096, 0xF68: 4096, 0xF6C: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant141_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013BF60 D_8017BF60\n'
                         '#define D_8013BF70 D_8017BF70\n'
                         '#define D_8013BFA8 D_8017BFA8\n'
                         '#include "variant141_entry.c"\n')
        source = (directory / "variant141_entry.c").read_text()
        header = (directory / "variant141_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__|register|volatile)\b")
        for declaration in (
            "Model141Config *G32 config;", "SVECTOR positions[28];",
            "u8 unknown_e4[0x120];", "SVECTOR offsets[32], velocities[32];",
            "s32 texture[2], elapsed;", "u8 completed, unknown_411[3];",
            "extern GsIMAGE D_8013BF70[];", "extern Model141Config D_8013BFA8[];",
        ):
            self.assertIn(declaration, header)
        for expression in (
            "const VECTOR D_8013BF60 = {4096, 4096, 4096, 0};",
            "work->config = &D_8013BFA8[command];",
            "for (i = 0; i < config->count; i++)",
            "(direction * rand()) % config->offset_spread",
            "SVECTOR vertices[9];", "box.w = 1;\n    box.h = 1;",
            "(-350 - point->vy) / (config->travel_duration - time)",
            "(-direction * config->travel_depth - point->vz) / (config->travel_duration - time)",
            "copyVector(&vertices[4], velocity);",
            "vertices[4].vx *= time;", "vertices[4].vy *= time;", "vertices[4].vz *= time;",
            "if (time >= config->sprite_duration)", "work->completed = 1;",
        ):
            self.assertIn(expression, source)
        for expression, count in (
            ("Model_GetFrameStep()", 1), ("SetPolyFT4(&quad)", 1),
            ("SetPolyF4(&flat)", 1), ("SetSemiTrans(&quad, 1)", 2),
            ("RotAverage4(", 5), ("RotTransPers(", 1),
            ("MulMatrix2(", 2), ("GsSortPoly(", 5),
            ("GsSortBoxFill(", 1), ("func_80059A50(", 1), ("rand()", 6),
            ("config->point_gray * (config->point_duration - time) / config->point_duration", 3),
            ("if (depth >= 0 && flag >= 0)", 6),
        ):
            self.assertEqual(source.count(expression), count)

    def test_attempt_ledger_preserves_failed_and_exact_experiments(self):
        with (family435.ROOT / "notes/overlays/french-model-variant141-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        expected = {
            "native-local-rand-header": (3936, 931),
            "native-descriptor-through-state": (3940, 23),
            "native-early-negative-terminal": (3932, 5),
            "native-completion-else": (3932, 5),
            "native-first-completion-test": (3928, 22),
            "native-nested-terminal-else": (3940, 21),
            "native-terminal-else-if-chain": (3932, 5),
            "native-first-completion-increment": (3932, 1),
            "native-first-completion-assignment": (3928, 22),
            "native-completion-window-ownership": (3932, 0),
        }
        self.assertEqual(len(rows), 22)
        self.assertEqual(len({(row["attempt"], row["slot"]) for row in rows}), 22)
        experiments = [row for row in rows if row["result"] in ("mismatch", "text_exact")]
        self.assertEqual({(row["attempt"], row["slot"]) for row in experiments},
                         {(name, slot) for name in expected for slot in ("0", "1")})
        for row in rows:
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
        for row in experiments:
            size, differences = expected[row["attempt"]]
            self.assertEqual((int(row["instruction_bytes"]), int(row["different_words"])),
                             (size, differences))
            self.assertEqual(row["result"], "text_exact" if differences == 0 else "mismatch")
        terminal = [row for row in rows if row["result"] == "matched"]
        self.assertEqual({row["slot"] for row in terminal}, {"0", "1"})
        for row in terminal:
            suffix = "_slot1" if row["slot"] == "1" else ""
            path = family435.ROOT / f"src/overlays/french_model_variant/variant141_entry{suffix}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("3932", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(
                f"D_{base + 0xF70:X} = 0x{base + 0xF70:X}; // type:u8 size:0x38 defined:true",
                symbols)
            self.assertIn(f"[0xF70, data, overlays/{module['name']}/image_view]", layout.read_text())
            self.assertIn(
                f"D_{base + 0xF60:X} = 0x{base + 0xF60:X}; // type:u32 size:0x10 defined:true",
                symbols)
            suffix = "_slot1" if module["name"].endswith("1") else ""
            self.assertIn(f"[0xF60, .rodata, overlays/french_model_variant/variant141_entry{suffix}]",
                          layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (128,128,128,128,128,128,128,128,128,24,200,150,100,300,15,20,12,30,4,30,12,70,150),
            1: (192,128,128,128,128,128,128,64,64,18,200,150,100,300,15,30,12,30,8,40,28,80,260),
            3: (128,192,192,192,192,128,128,64,64,6,200,100,100,300,15,30,12,30,6,8,20,52,150),
        }
        arguments = {428: 0, 445: 1, 20: 3}
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                argument = arguments[int(row["model"])]
                self.assertEqual(int(row["command_word"]), 72000 + argument)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 72000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                descriptor = struct.unpack_from("<10B13h", data, 0xFA8 + 36 * argument)
                self.assertEqual(descriptor, descriptors[argument])
                self.assertTrue(0 < descriptor[20] <= 28)
                self.assertTrue(all(descriptor[index] > 0 for index in (13, 14, 15, 16, 17)))
                observed.add(argument)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<983I", data[4:0xF60])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 52)
                self.assertEqual(set(calls), addresses)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x414 <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model141Config)": 36, "sizeof(Model141State)": 0x414,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(GsIMAGE)": 28, "sizeof(POLY_FT4)": 40, "sizeof(POLY_F4)": 24,
            "sizeof(GsBOXF)": 16,
        }
        for typename, fields in (
            ("Model141Config", (("part", 9), ("grid_half_size", 10), ("travel_depth", 12),
                                ("sprite_half_size", 14), ("offset_spread", 16), ("point_spread", 18),
                                ("travel_duration", 20), ("sprite_duration", 22), ("point_duration", 24),
                                ("spacing", 26), ("unknown_1c", 28), ("count", 30),
                                ("travel_delay", 32), ("burst_delay", 34))),
            ("Model141State", (("positions", 4), ("offsets", 0x204), ("velocities", 0x304),
                               ("texture", 0x404), ("elapsed", 0x40C), ("completed", 0x410))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 29)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant141_entry.h")
