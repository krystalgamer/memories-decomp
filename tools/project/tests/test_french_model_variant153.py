import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant153Tests(family435.FrenchModelVariant435Tests):
    family = 153
    slot_header_delta = 130
    module_count = 10
    distinct_images = 10
    binding_count = 21
    tail_start = 0xF18
    spans = ((4, 0xE0C),)
    helpers = ((4, 3592, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (38, 271, 635)), (9, (167, 356)))
    entry_anchors = {
        4: 0x27BDFED0, 0xE04: 0x03E00008, 0xE08: 0x27BD0130,
        0xE0C: 4096, 0xE10: 4096, 0xE14: 4096, 0xE18: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant153_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013BE0C D_8017BE0C\n'
                         '#define D_8013BE1C D_8017BE1C\n'
                         '#define D_8013BF18 D_8017BF18\n'
                         '#include "variant153_entry.c"\n')
        source = (directory / "variant153_entry.c").read_text()
        header = (directory / "variant153_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__|register|volatile)\b")
        for declaration in (
            "extern GsIMAGE D_8013BE1C[];", "extern Model153Config D_8013BF18[];",
            "Model153Config *G32 config;", "SVECTOR positions[64];",
            "SVECTOR targets[64];", "SVECTOR burst_offsets[4];",
            "u32 texture[9];", "u8 completed;", "s32 elapsed;",
        ):
            self.assertIn(declaration, header)
        for expression in (
            "const VECTOR D_8013BE0C = {4096, 4096, 4096, 0};",
            "&D_8013BF18[command % 100]",
            "point->vx += rand() % config->spread * 2 - config->spread;",
            "point->vy += rand() % config->spread;",
            "point->vz += direction * (rand() % config->spread);",
            "point->vy = -config->rise_height;",
            "func_80059A50(Model_GetActiveSlotIndex(), 2, &D_8013BE1C[0])",
            "func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013BE1C[i])",
            "GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);",
            "value = time * 3584 / config->travel_duration + 512;",
            "value = (time / 2 + i) % 8;", "value / 4 * 87 + 86",
            "point->vy -= config->rise_height / config->fade_duration * step;",
            "SetPolyF4(&flash);", "work->elapsed += step;",
            "if (time < config->spacing * config->count)", "if (work->completed == 0)",
        ):
            self.assertIn(expression, source)
        self.assertEqual(source.count("point->pad = 0;"), 2)
        for axis in ("x", "y", "z"):
            self.assertIn(
                f"point->v{axis} += (target->v{axis} - point->v{axis}) / (config->travel_duration - time);",
                source)
            multiply = source.index(f"position.v{axis} *= time;")
            divide = source.index(f"position.v{axis} /= config->burst_duration;")
            add = source.index(f"position.v{axis} += point->v{axis};")
            self.assertLess(multiply, divide)
            self.assertLess(divide, add)
        self.assertEqual(source.count("depth = RotAverage4("), 2)
        self.assertEqual(source.count("if (depth >= 0 && flag >= 0)"), 2)
        self.assertLess(source.index("step = Model_GetFrameStep();"), source.index("if (command >= 0)"))

    def test_attempt_ledger_preserves_failed_and_exact_experiments(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        with (family435.ROOT / "notes/overlays/french-model-variant153-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 10)
        self.assertEqual(len({(row["attempt"], row["slot"]) for row in rows}), 10)
        expected = {
            "native-travel-rise-and-burst": "8",
            "direct-terminal-spacing-products": "5",
            "nested-one-shot-completion": "22",
            "flat-positive-completion-transition": "0",
        }
        experiments = [row for row in rows if row["result"] in ("mismatch", "text_exact")]
        self.assertEqual({(row["attempt"], row["slot"]) for row in experiments},
                         {(name, slot) for name in expected for slot in ("0", "1")})
        for row in rows:
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
        for row in experiments:
            differences = expected[row["attempt"]]
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("3592", differences))
            self.assertEqual(row["result"], "text_exact" if differences == "0" else "mismatch")
            self.assertIn("Frame 304/304;", row["reason"])
        terminal = [row for row in rows if row["result"] == "matched"]
        self.assertEqual({row["slot"] for row in terminal}, {"0", "1"})
        self.assertEqual(len(terminal), 2)
        for row in terminal:
            source = directory / ("variant153_entry" + ("_slot1" if row["slot"] == "1" else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("3592", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{base + 0xE1C:X} = 0x{base + 0xE1C:X}; // type:u8 size:0xFC defined:true", symbols)
            self.assertIn(f"D_{base + 0xE0C:X} = 0x{base + 0xE0C:X}; // type:u32 size:0x10 defined:true", symbols)
            slot = int(module["name"][-1])
            source = "overlays/french_model_variant/variant153_entry" + ("_slot1" if slot else "")
            self.assertIn(f"[0xE0C, .rodata, {source}]", layout.read_text())
            self.assertIn(f"[0xE1C, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (128, 128, 128, 96, 96, 128, 8, 0, 150, 200, 120, 300, 200, 48, 3, 120, 30, 30, 150),
            2: (128, 128, 128, 96, 96, 128, 6, 0, 70, 300, 120, 300, 150, 64, 4, 60, 60, 60, 80),
            3: (128, 128, 128, 96, 96, 128, 26, 0, 200, 300, 200, 300, 150, 56, 4, 60, 60, 60, 70),
            5: (128, 128, 128, 96, 96, 128, 19, 0, 100, 100, 200, 300, 150, 54, 4, 60, 60, 60, 90),
        }
        arguments = {(7, 38): 0, (7, 271): 2, (7, 635): 2, (9, 167): 5, (9, 356): 3}
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot = int(row["slot"])
                argument = arguments[int(row["stage"]) - slot, int(row["model"])]
                self.assertEqual(int(row["command_word"]), 84000 + argument)
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110
                             + (int(row["stage"]) - 7) // 2 * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 84000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                observed.add(argument)
                descriptor = struct.unpack_from("<8B11h", data, 0xF18 + 30 * (argument % 100))
                self.assertEqual(descriptor, descriptors[argument])
                self.assertTrue(0 < descriptor[13] <= 64)
                self.assertTrue(all(descriptor[index] > 0 for index in (9, 12, 15, 16, 17)))
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                self.assertIn("GsGetLwUnit = 0x8008A428;", bindings)
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<898I", data[4:0xE0C])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 41)
                self.assertEqual(set(calls), addresses)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 10 * 2048)):
                    self.assertTrue(context + 0x450 <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model153Config)": 30, "sizeof(Model153State)": 0x450,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(GsIMAGE)": 28, "sizeof(POLY_FT4)": 40, "sizeof(POLY_F4)": 24,
        }
        for typename, fields in (
            ("Model153Config", (("part", 6), ("half_size", 8), ("spread", 10),
                                ("burst_half_size", 12), ("rise_height", 14),
                                ("burst_spread", 16), ("count", 18), ("spacing", 20),
                                ("travel_duration", 22), ("fade_duration", 24),
                                ("burst_duration", 26), ("delay", 28))),
            ("Model153State", (("positions", 4), ("targets", 0x204),
                               ("burst_offsets", 0x404), ("texture", 0x424),
                               ("completed", 0x448), ("elapsed", 0x44C))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 26)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant153_entry.h")
