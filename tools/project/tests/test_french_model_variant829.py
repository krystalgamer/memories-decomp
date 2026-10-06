import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant829Tests(family435.FrenchModelVariant435Tests):
    family = 829
    slot_header_delta = 97
    module_count = 2
    distinct_images = 2
    binding_count = 20
    tail_start = 0x1008
    spans = ((4, 0xF88),)
    helpers = ((4, 3972, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (485,)),)
    entry_anchors = {
        4: 0x27BDFED8, 0x100: 0x02028023, 0x104: 0x02001821,
        0x110: 0x9444000A, 0x120: 0x00031B03, 0x124: 0x00031B00,
        0x128: 0x02031823, 0x12C: 0x00031040, 0x130: 0x00431021,
        0x134: 0x00820018, 0x138: 0x00001012,
        0xF88: 4096, 0xF8C: 4096, 0xF90: 4096, 0xF94: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant829_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013BF88 D_8017BF88\n'
                         '#define D_8013BF98 D_8017BF98\n'
                         '#define D_8013C008 D_8017C008\n'
                         '#include "variant829_entry.c"\n')
        source = (directory / "variant829_entry.c").read_text()
        header = (directory / "variant829_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__|register|volatile)\b")
        for declaration in (
            "Model829Config *G32 config;", "u16 phase, states[4], active;",
            "u32 first_pair;", "Model829Status status;", "ModelEffectAdjustment adjustment;",
            "SVECTOR positions[4], velocities[4], cloud_offsets[4][32];",
            "u16 cloud_counts[4];", "u8 red[4], green[4], blue[4], cloud_gray[4][32];",
            "extern GsIMAGE D_8013BF98[];", "extern Model829Config D_8013C008[];",
        ):
            self.assertIn(declaration, header)
        for expression in (
            "work->config = &D_8013C008[command];",
            "sample = rand();", "sample -= rand();",
            "work->config->wave_amplitude * ((sample % 4096) * 2 + sample % 4096) / 4096",
            "-350, -direction * 354", "func_80057E20(slot ^ 1, &position.adjustment);",
            "Model_SetFrameStepOverride(1);", "work->status.first_pair",
        ):
            self.assertIn(expression, source)
        for expression, count in (
            ("j = rand() % 9;", 2), ("setPolyFT4(&quad)", 2),
            ("RotAverage4(", 2), ("if (depth >= 0)", 2), ("addVector(", 2),
            ("MulMatrix2(", 0), ("Model_GetFrameStep()", 1),
        ):
            self.assertEqual(source.count(expression), count)
        self.assertLess(source.index("s32 step;"), source.index("GsOT *ot;"))
        self.assertLess(source.index("work->elapsed < work->config->startup_delay"),
                        source.index("PushMatrix();"))

    def test_attempt_ledger_preserves_failed_and_exact_experiments(self):
        with (family435.ROOT / "notes/overlays/french-model-variant829-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        expected = {
            "native-four-sprites-and-clouds": (3984, 898),
            "native-staged-random-displacement": (3980, 885),
            "native-linear-sprite-case-chain": (3940, 889),
            "native-sdk-vector-addition": (3964, 886),
            "native-left-associated-random-scale": (3968, 930),
            "native-amplitude-remainder-product-chain": (3972, 46),
            "native-step-before-ordering-table": (3972, 37),
            "native-main-pass-tile-index-lifetime": (3972, 7),
            "native-captured-amplitude-and-displacement": (3968, 932),
            "native-wrapping-displacement-scale": (3968, 931),
            "native-bounded-signed-displacement": (3980, 876),
            "native-sampled-double-plus-remainder": (3972, 10),
            "native-in-place-random-difference": (3996, 658),
            "native-constructor-local-random-sample": (3972, 0),
        }
        self.assertEqual(len(rows), 30)
        self.assertEqual(len({(row["attempt"], row["slot"]) for row in rows}), 30)
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
            path = family435.ROOT / f"src/overlays/french_model_variant/variant829_entry{suffix}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("3972", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(
                f"D_{base + 0xF98:X} = 0x{base + 0xF98:X}; // type:u8 size:0x70 defined:true",
                symbols)
            self.assertIn(f"[0xF98, data, overlays/{module['name']}/image_view]", layout.read_text())
            self.assertIn(
                f"D_{base + 0xF88:X} = 0x{base + 0xF88:X}; // type:u32 size:0x10 defined:true",
                symbols)
            suffix = "_slot1" if module["name"].endswith("1") else ""
            self.assertIn(f"[0xF88, .rodata, overlays/french_model_variant/variant829_entry{suffix}]",
                          layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                self.assertEqual(int(row["command_word"]), 361000)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 361000)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<11H2BH", data, 0x1008),
                                 (80, 100, 80, 40, 80, 48, 64, 128, 64, 96, 15, 8, 0, 100))
                self.assertEqual(struct.unpack_from("<I", data)[0], 829 + slot * 97)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<993I", data[4:0xF88])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 41)
                self.assertEqual(set(calls), addresses)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x504 <= start or start + size <= context)

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model829Config)": 26, "sizeof(Model829State)": 0x504,
            "sizeof(Model829Status)": 12, "sizeof(Model829Position)": 8,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(GsIMAGE)": 28, "sizeof(POLY_FT4)": 40, "sizeof(ModelEffectAdjustment)": 8,
        }
        for typename, fields in (
            ("Model829Config", (("square_half_size", 0), ("rectangle_half_width", 2),
                               ("rectangle_half_height", 4), ("animated_half_width", 6),
                               ("animated_half_height", 8), ("wave_amplitude", 10),
                               ("travel_divisor", 12), ("cloud_spread", 14),
                               ("cloud_half_width", 16), ("cloud_half_height", 18),
                               ("unknown_14", 20), ("fade_step", 22), ("startup_delay", 24))),
            ("Model829State", (("config", 0), ("positions", 4), ("velocities", 0x24),
                              ("cloud_offsets", 0x44), ("elapsed", 0x444), ("updates", 0x448),
                              ("phase_frames", 0x44C), ("status", 0x450),
                              ("status.fields.states", 0x452), ("status.fields.active", 0x45A),
                              ("cloud_counts", 0x45C), ("brightness", 0x464), ("texture", 0x468),
                              ("red", 0x478), ("green", 0x47C), ("blue", 0x480), ("cloud_gray", 0x484))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 40)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant829_entry.h")
