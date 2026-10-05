import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant129Tests(family435.FrenchModelVariant435Tests):
    family = 129
    slot_header_delta = 130
    module_count = 10
    distinct_images = 10
    binding_count = 21
    tail_start = 0x1030
    spans = ((4, 0xFE8),)
    helpers = ((4, 4068, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (219,)), (9, (30, 355, 435, 490)))
    entry_anchors = {
        4: 0x27BDFE90, 0xFE0: 0x03E00008, 0xFE4: 0x27BD0170,
        0xFE8: 4096, 0xFEC: 4096, 0xFF0: 4096, 0xFF4: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant129_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013BFE8 D_8017BFE8\n'
                         '#define D_8013BFF8 D_8017BFF8\n'
                         '#define D_8013C030 D_8017C030\n'
                         '#include "variant129_entry.c"\n')
        source = (directory / "variant129_entry.c").read_text()
        header = (directory / "variant129_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__|register|volatile)\b")
        for declaration in (
            "Model129Config *G32 config;", "SVECTOR positions[64];",
            "SVECTOR velocities[64];", "s32 texture[2];", "s32 elapsed;",
            "u8 completed, effect_started, unknown_412[2];",
            "extern GsIMAGE D_8013BFF8[];", "extern Model129Config D_8013C030[];",
        ):
            self.assertIn(declaration, header)
        for expression in (
            "const VECTOR D_8013BFE8 = {4096, 4096, 4096, 0};",
            "s16 red, green, blue;", "&D_8013C030[command]",
            "for (i = 0; i < config->count; i++)",
            "work->elapsed = -config->delay;",
            "work->effect_started++;",
            "func_8005F7B0(30, config->count * config->spacing / 2);",
            "s32 duration = config->travel_duration;",
            "value = time * 4096 / duration;",
            "growth = burst_time * 1365 / config->travel_duration;",
            "value = growth * 3 + 4096;",
            "work->elapsed += Model_GetFrameStep();",
            "if (burst_time >= (s16)(config->travel_duration / 3))",
            "work->completed = 1;",
        ):
            self.assertIn(expression, source)
        self.assertEqual(source.count("Model_GetFrameStep()"), 3)
        self.assertEqual(source.count("DRAW_QUAD("), 6)
        self.assertEqual(source.count("func_80059A50("), 1)
        self.assertLess(source.index("s32 duration = config->travel_duration;"),
                        source.index("velocity->vx = -point->vx"))
        self.assertLess(source.index("setVector(&position, point->vx, point->vy, point->vz);"),
                        source.index("point->vx += velocity->vx;"))

    def test_attempt_ledger_preserves_failed_and_exact_experiments(self):
        with (family435.ROOT / "notes/overlays/french-model-variant129-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        expected = {
            "burst-byte-color-carriers": (4080, 228),
            "burst-doubled-plus-base-colors": (4068, 4),
            "burst-final-byte-output-carriers": (4068, 4),
            "burst-final-word-output-carriers": (4068, 4),
            "burst-rgb-scalars-before-packet": (4072, 381),
            "captured-travel-scale-duration": (4068, 9),
            "complete-burst-intensity-expressions": (4068, 4),
            "completion-inside-positive-burst-gate": (4068, 4),
            "distinct-burst-growth-lifetime": (4068, 17),
            "interleaved-burst-rgb-scaling": (4072, 388),
            "native-byte-state-transitions": (4064, 87),
            "native-start-increment-only": (4068, 69),
            "phase-timing-before-position-copy": (4068, 70),
            "positive-completion-common-return": (4064, 27),
            "sdk-sized-gradient-layout": (4072, 545),
            "shared-complete-byte-colors": (4068, 4),
            "shared-complete-phase-color-scalars": (4072, 410),
            "shared-final-halfword-colors": (4068, 0),
            "shared-rgb-and-scene-lifetimes": (4072, 610),
        }
        self.assertEqual(len(rows), 41)
        self.assertEqual(len({(r["attempt"], r["slot"]) for r in rows}), 41)
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
        failure, = [r for r in rows if r["result"] == "blocked_layout_check"]
        self.assertEqual((failure["attempt"], failure["slot"]),
                         ("native-flash-travel-four-quadrants", ""))
        self.assertEqual((failure["instruction_bytes"], failure["different_words"]), ("", ""))
        self.assertIn("Neither entry compiled, linked or compared.", failure["reason"])
        terminal = [r for r in rows if r["result"] == "matched"]
        self.assertEqual({r["slot"] for r in terminal}, {"0", "1"})
        for row in terminal:
            suffix = "_slot1" if row["slot"] == "1" else ""
            path = family435.ROOT / f"src/overlays/french_model_variant/variant129_entry{suffix}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("4068", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{base + 0xFF8:X} = 0x{base + 0xFF8:X}; // type:u8 size:0x38 defined:true",
                          symbols)
            self.assertIn(f"[0xFF8, data, overlays/{module['name']}/image_view]", layout.read_text())
            self.assertIn(f"D_{base + 0xFE8:X} = 0x{base + 0xFE8:X}; // type:u32 size:0x10 defined:true",
                          symbols)
            suffix = "_slot1" if module["name"].endswith("1") else ""
            self.assertIn(f"[0xFE8, .rodata, overlays/french_model_variant/variant129_entry{suffix}]",
                          layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (64, 32, 0, 128, 128, 128, 8, 0, 200, 20, 200, 400, 512, 300, 60, 240, 8, 30, 28, 100),
            1: (128, 128, 96, 160, 160, 128, 5, 0, 200, 20, 200, 400, 512, 300, 60, 90, 3, 30, 24, 120),
            2: (96, 128, 96, 128, 160, 128, 28, 0, 200, 20, 200, 400, 512, 300, 120, 280, 4, 30, 64, 140),
            3: (96, 96, 128, 128, 160, 128, 9, 0, 200, 20, 200, 400, 512, 300, 120, 320, 6, 30, 48, 90),
            4: (128, 96, 96, 128, 160, 128, 13, 0, 200, 20, 200, 400, 512, 300, 120, 280, 4, 30, 52, 120),
        }
        arguments = {219: 0, 355: 1, 490: 2, 30: 3, 435: 4}
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                argument = arguments[int(row["model"])]
                self.assertEqual(int(row["command_word"]), 60000 + argument)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 60000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<8B12h", data, 0x1030 + 32 * argument),
                                 descriptors[argument])
                observed.add(argument)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<1017I", data[4:0xFE8])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 48)
                self.assertEqual(set(calls), addresses)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x414 <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model129Config)": 32, "sizeof(Model129State)": 0x414,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(GsIMAGE)": 28, "sizeof(POLY_FT4)": 40, "sizeof(POLY_G4)": 36,
        }
        for typename, fields in (
            ("Model129Config", (("part", 6), ("half_size", 8), ("unknown_0A", 10),
                                ("travel_duration", 20), ("flash_duration", 22), ("spacing", 24),
                                ("unknown_1A", 26), ("count", 28), ("delay", 30))),
            ("Model129State", (("positions", 4), ("velocities", 0x204), ("texture", 0x404),
                               ("elapsed", 0x40C), ("completed", 0x410), ("effect_started", 0x411))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 23)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant129_entry.h")
