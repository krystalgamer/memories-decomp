import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant184Tests(family435.FrenchModelVariant435Tests):
    family = 184
    slot_header_delta = 130
    module_count = 6
    distinct_images = 6
    binding_count = 25
    tail_start = 0xF58
    spans = ((4, 0xEF4),)
    helpers = ((4, 3824, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((9, (169, 524, 591)),)
    entry_anchors = {
        4: 0x27BDFD58, 0xEEC: 0x03E00008, 0xEF0: 0x27BD02A8,
        0xEF4: 4096, 0xEF8: 4096, 0xEFC: 4096, 0xF00: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant184_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013BEF4 D_8017BEF4\n'
                         '#define D_8013BF04 D_8017BF04\n'
                         '#define D_8013BF58 D_8017BF58\n'
                         '#include "variant184_entry.c"\n')
        source = (directory / "variant184_entry.c").read_text()
        header = (directory / "variant184_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__|register|volatile)\b")
        for declaration in (
            "Model184Config *G32 config;", "SVECTOR positions[80];",
            "u8 unknown_284[0x80];", "SVECTOR targets[80];",
            "u8 unknown_584[0x80];", "SVECTOR origin;", "SVECTOR rings[34];",
            "u32 texture[3];", "u8 completed, unknown_729[3];",
            "s32 elapsed, updates;", "u8 command_group, unknown_735[3];",
            "extern GsIMAGE D_8013BF04[];", "extern Model184Config D_8013BF58[];",
        ):
            self.assertIn(declaration, header)
        for expression in (
            "const VECTOR D_8013BEF4 = {4096, 4096, 4096, 0};",
            "work->config = &D_8013BF58[command % 100];",
            "work->origin.vy = -350;", "work->command_group = command / 100;",
            "radius * csin(j) / 4096 * csin(k) / 4096",
            "radius * ccos(j) / 4096 * csin(k) / 4096",
            "for (i = 0; i < 17; point++, i++)",
            "s32 travel_red, travel_green, travel_blue;",
            "limitRange(j, 0, 4096);",
            "(target->vx - point->vx) / (config->travel_duration - time) * step",
            "(target->vy - point->vy) / (config->travel_duration - time) * step",
            "(target->vz - point->vz) / (config->travel_duration - time) * step",
            "screen1 = screen + 17;", "depth1 = depths + 17;",
            "first_flags = flags0[0];",
            "flag = (flags0[1] | first_flags) & 0x20;",
            "if (time < 0)", "if (time > config->burst_duration)",
            "if (work->completed)", "work->completed++;", "work->updates++;",
        ):
            self.assertIn(expression, source)
        for expression, count in (
            ("Model_GetFrameStep()", 1), ("SetPolyFT4(&quad)", 1),
            ("SetPolyF4(&flat)", 1), ("RotAverage4(", 2),
            ("AverageZ4(", 1), ("RotTransPersN(", 1),
            ("MulMatrix2(", 0), ("GsSortPoly(", 3), ("func_80059A50(", 1),
            ("rand()", 3), ("if (depth >= 0 && flag >= 0)", 2),
        ):
            self.assertEqual(source.count(expression), count)
        self.assertNotIn("flags1", source)

    def test_attempt_ledger_preserves_failed_and_exact_experiments(self):
        with (family435.ROOT / "notes/overlays/french-model-variant184-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        expected = {
            "native-travel-burst-rings": (3752, 863),
            "native-descriptor-state-store-order": (3756, 904),
            "native-distinct-burst-index": (3764, 886),
            "native-fixed-ring-bank-bases": (3700, 886),
            "native-corresponding-ring-index": (3772, 890),
            "native-ring-bank-cursors": (3828, 379),
            "native-cursors-before-loop-indices": (3816, 34),
            "native-unsigned-texture-and-direct-travel-colors": (3824, 499),
            "native-distinct-travel-color-lifetimes": (3816, 25),
            "native-positive-terminal-thresholds": (3816, 29),
            "native-negative-terminal-gate": (3824, 2),
            "native-ring-flag-operand-order": (3824, 2),
            "native-first-ring-flag-capture": (3824, 0),
        }
        self.assertEqual(len(rows), 28)
        self.assertEqual(len({(r["attempt"], r["slot"]) for r in rows}), 28)
        experiments = [r for r in rows if r["result"] in ("mismatch", "text_exact")]
        self.assertEqual({(r["attempt"], r["slot"]) for r in experiments},
                         {(name, slot) for name in expected for slot in ("0", "1")})
        for row in rows:
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
        for row in experiments:
            size, differences = expected[row["attempt"]]
            self.assertEqual((int(row["instruction_bytes"]), int(row["different_words"])),
                             (size, differences))
            self.assertEqual(row["result"], "text_exact" if differences == 0 else "mismatch")
        terminal = [r for r in rows if r["result"] == "matched"]
        self.assertEqual({r["slot"] for r in terminal}, {"0", "1"})
        for row in terminal:
            suffix = "_slot1" if row["slot"] == "1" else ""
            path = family435.ROOT / f"src/overlays/french_model_variant/variant184_entry{suffix}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("3824", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(
                f"D_{base + 0xF04:X} = 0x{base + 0xF04:X}; // type:u8 size:0x54 defined:true",
                symbols)
            self.assertIn(f"[0xF04, data, overlays/{module['name']}/image_view]", layout.read_text())
            self.assertIn(
                f"D_{base + 0xEF4:X} = 0x{base + 0xEF4:X}; // type:u32 size:0x10 defined:true",
                symbols)
            suffix = "_slot1" if module["name"].endswith("1") else ""
            self.assertIn(f"[0xEF4, .rodata, overlays/french_model_variant/variant184_entry{suffix}]",
                          layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (128, 128, 128, 16, 16, 8, 2, 80, 100, 400, 50, 200, 500, 120, 60, 20, 2, 110),
            1: (128, 128, 128, 32, 32, 8, 7, 24, 70, 500, 50, 200, 100, 120, 60, 20, 1, 40),
        }
        arguments = {169: 1, 524: 0, 591: 1}
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                argument = arguments[int(row["model"])]
                self.assertEqual(int(row["command_word"]), 115000 + argument)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 115000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<8B10h", data, 0xF58 + 28 * argument),
                                 descriptors[argument])
                observed.add(argument)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<956I", data[4:0xEF4])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 51)
                self.assertEqual(set(calls), addresses)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x738 <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model184Config)": 28, "sizeof(Model184State)": 0x738,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(GsIMAGE)": 28, "sizeof(POLY_FT4)": 40, "sizeof(POLY_F4)": 24,
            "sizeof(DVECTOR)": 4,
        }
        for typename, fields in (
            ("Model184Config", (("part", 6), ("count", 7), ("moving_half_size", 8),
                                ("spread", 10), ("inner_radius", 12), ("outer_radius", 14),
                                ("lift", 16), ("burst_half_size", 18), ("travel_duration", 20),
                                ("burst_duration", 22), ("interval", 24), ("delay", 26))),
            ("Model184State", (("positions", 4), ("unknown_284", 0x284), ("targets", 0x304),
                               ("unknown_584", 0x584), ("origin", 0x604), ("rings", 0x60C),
                               ("texture", 0x71C), ("completed", 0x728), ("elapsed", 0x72C),
                               ("updates", 0x730), ("command_group", 0x734))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 32)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant184_entry.h")
