import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant121Tests(family435.FrenchModelVariant435Tests):
    family = 121
    slot_header_delta = 130
    module_count = 24
    distinct_images = 24
    binding_count = 25
    tail_start = 0x10F0
    spans = ((4, 0x10A8),)
    helpers = ((4, 4260, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (281, 502, 509, 514, 641)),
                       (9, (39, 128, 227, 477, 514, 557, 708)))
    entry_anchors = {
        4: 0x27BDFC18,
        0x10A0: 0x03E00008,
        0x10A4: 0x27BD03E8,
        0x10A8: 4096, 0x10AC: 4096, 0x10B0: 4096, 0x10B4: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant121_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013C0A8 D_8017C0A8\n'
                         '#define D_8013C0B8 D_8017C0B8\n'
                         '#define D_8013C0F0 D_8017C0F0\n'
                         '#include "variant121_entry.c"\n')
        source = (directory / "variant121_entry.c").read_text()
        header = (directory / "variant121_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__)\b")
        self.assertIn("static const VECTOR D_8013C0A8 = {4096, 4096, 4096, 0};", source)
        self.assertIn("extern GsIMAGE D_8013C0B8[];", header)
        self.assertIn("extern Model121Config D_8013C0F0[];", header)
        self.assertIn("CVECTOR colors[26];", header)
        self.assertIn("CVECTOR initialized_colors[24];", header)
        self.assertIn("u32 texture[2];", header)
        self.assertIn("Model121Config *G32 config;", header)
        self.assertLess(source.index("s32 direction;"), source.index("GsOT *ot;"))
        for expression in (
            "&D_8013C0F0[command % 100]", "if (command < 100)",
            "(i - 8) * 3 * 255 / 24", "(i - 16) * 3 * 255 / 24",
            "rand() % config->spread * 2 - config->spread - 350",
            "for (i = 0; i < 24; point++, i++)",
            "work->palette.colors[i + 1].r",
            "AverageZ3(depths[0], depths[i], depths[i + 1])",
            "(flags[0] | flags[i] | flags[i + 1]) & 0x20",
            "screen[i + 2].vx", "&& time >= config->fade_duration",
            "+ (s16)(config->travel_duration / 3)",
        ):
            self.assertIn(expression, source)
        self.assertLess(source.index("blue = work->palette.colors[(time / 2) % 24].b"),
                        source.index("setRGB0(&quad, red, green, blue);"))
        self.assertEqual(source.count("depth = RotAverage4("), 4)
        self.assertEqual(source.count("work->elapsed += Model_GetFrameStep();"), 2)

    def test_attempt_ledger_preserves_failed_and_exact_experiments(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        with (family435.ROOT / "notes/overlays/french-model-variant121-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 31)
        self.assertEqual(len({(row["attempt"], row["slot"]) for row in rows}), 31)
        expected = {
            "native-candidate-verified-gt3": ("4304", "880"),
            "sprite-rgb-before-packet-stores": ("4244", "867"),
            "rgb-control-no-cse-follow-jumps": ("4252", "868"),
            "function-scope-rgb-locals": ("4244", "867"),
            "shared-palette-and-sprite-rgb": ("4236", "856"),
            "ring-duration-through-scale-scalar": ("4244", "867"),
            "retained-duration-no-cse-control": ("4244", "867"),
            "shared-rainbow-phase-scalar": ("4244", "866"),
            "conditional-expression-ring-scale": ("4244", "866"),
            "unsplit-address-control": ("4244", "867"),
            "short-circuit-full-scale-plateau": ("4256", "868"),
            "segment-local-rainbow-indices": ("4260", "12"),
            "native-spill-order-and-texture-handles": ("4260", "0"),
            "canonical-addressed-scale-literal": ("4260", "0"),
        }
        experiments = [row for row in rows if row["result"] in ("mismatch", "text_exact")]
        self.assertEqual(len(experiments), 28)
        self.assertEqual({(row["attempt"], row["slot"]) for row in experiments},
                         {(name, slot) for name in expected for slot in ("0", "1")})
        for row in rows:
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
            profile = ("gcc_2_8_1_g0_split_no_cse_follow_jumps"
                       if row["attempt"] in ("rgb-control-no-cse-follow-jumps",
                                             "retained-duration-no-cse-control")
                       else "gcc_2_8_1_g0_split")
            if row["attempt"] == "unsplit-address-control":
                profile = "gcc_2_8_1_g0"
            self.assertEqual(row["profile"], profile)
        for row in experiments:
            self.assertEqual((row["instruction_bytes"], row["different_words"]),
                             expected[row["attempt"]])
            self.assertEqual(row["result"], "text_exact" if row["different_words"] == "0" else "mismatch")
        failure, = [row for row in rows if row["result"] == "blocked_before_link"]
        self.assertEqual((failure["attempt"], failure["slot"]), ("native-first-candidate", "0"))
        self.assertEqual((failure["instruction_bytes"], failure["different_words"]), ("", ""))
        self.assertIn("SetPolyGT3", failure["reason"])
        self.assertIn("No function/literal comparison completed", failure["reason"])
        terminal = [row for row in rows if row["result"] == "matched"]
        self.assertEqual({row["slot"] for row in terminal}, {"0", "1"})
        for row in terminal:
            source = directory / ("variant121_entry" + ("_slot1" if row["slot"] == "1" else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("4260", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{base + 0x10B8:X} = 0x{base + 0x10B8:X}; // type:u8 size:0x38 defined:true", symbols)
            self.assertIn(f"D_{base + 0x10A8:X} = 0x{base + 0x10A8:X}; // type:u32 size:0x10 defined:true", symbols)
            slot = int(module["name"][-1])
            source = "overlays/french_model_variant/variant121_entry" + ("_slot1" if slot else "")
            self.assertIn(f"[0x10A8, .rodata, {source}]", layout.read_text())
            self.assertIn(f"[0x10B8, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (128, 128, 128, 17, 18, 0, 200, 100, 150, 250, 60, 30, 8, 60, 140),
            1: (128, 128, 128, 24, 52, 0, 200, 100, 150, 250, 60, 30, 8, 60, 420),
            2: (128, 128, 128, 8, 64, 0, 200, 100, 150, 250, 60, 30, 8, 70, 560),
            3: (160, 48, 160, 33, 40, 0, 200, 150, 150, 250, 60, 30, 8, 50, 360),
            4: (48, 160, 128, 27, 30, 0, 200, 200, 150, 250, 60, 30, 6, 30, 200),
            5: (48, 160, 128, 7, 20, 0, 200, 400, 150, 250, 90, 30, 12, 120, 200),
            6: (32, 128, 128, 4, 40, 0, 200, 200, 150, 250, 90, 30, 4, 50, 200),
            7: (32, 128, 128, 21, 15, 0, 200, 150, 150, 250, 200, 30, 20, 110, 400),
            8: (128, 32, 128, 36, 15, 0, 200, 250, 150, 250, 60, 30, 10, 40, 400),
        }
        arguments = {(7, 281): 1, (7, 502): 7, (7, 509): 4, (7, 514): 105, (7, 641): 1,
                     (9, 39): 0, (9, 128): 108, (9, 227): 102, (9, 477): 6,
                     (9, 514): 5, (9, 557): 0, (9, 708): 103}
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot = int(row["slot"])
                argument = arguments[int(row["stage"]) - slot, int(row["model"])]
                self.assertEqual(int(row["command_word"]), 52000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                index = argument % 100
                observed.add(index)
                self.assertEqual(struct.unpack_from("<6B9h", data, 0x10F0 + 24 * index),
                                 descriptors[index])
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                self.assertIn("SetPolyGT3 = 0x80082E68;", bindings)
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<1065I", data[4:0x10A8])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 57)
                self.assertEqual(set(calls), addresses)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 10 * 2048)):
                    self.assertTrue(context + 0x544 <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_sdk_gt3_binding_matches_native_packet_initialization(self):
        path = family435.ROOT / "game/france/SLES_039.48"
        if not path.exists():
            self.skipTest("legal French executable required")
        data = path.read_bytes()
        load_address, = struct.unpack_from("<I", data, 0x18)
        self.assertEqual(struct.unpack_from("<5I", data, 0x800 + 0x80082E68 - load_address),
                         (0x24020009, 0xA0820003, 0x24020034, 0x03E00008, 0xA0820007))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model121Config)": 24, "sizeof(Model121State)": 0x544,
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(POLY_FT4)": 40, "sizeof(POLY_GT3)": 40, "sizeof(GsIMAGE)": 28,
            "sizeof(CVECTOR)": 4, "sizeof(Model121Palette)": 104,
            "sizeof(DVECTOR)": 4, "sizeof(PSXLONG)": 4,
        }
        for typename, fields in (
            ("Model121State", (("positions", 4), ("destinations", 0x204), ("ring", 0x404),
                               ("palette", 0x4D4), ("palette.storage.texture", 0x534),
                               ("completed", 0x53C), ("elapsed", 0x540),
                               ("palette.colors[24]", 0x534), ("palette.colors[25]", 0x538))),
            ("Model121Config", (("part", 3), ("count", 4), ("sprite_size", 6), ("spread", 8),
                                ("ring_radius", 0xA), ("ring_depth", 0xC), ("travel_duration", 0xE),
                                ("fade_duration", 0x10), ("particle_stagger", 0x12),
                                ("initial_delay", 0x14), ("ring_duration", 0x16))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 32)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant121_entry.h")
