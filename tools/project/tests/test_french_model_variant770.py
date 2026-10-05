import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant770Tests(family435.FrenchModelVariant435Tests):
    family = 770
    slot_header_delta = 97
    module_count = 18
    distinct_images = 18
    binding_count = 26
    tail_start = 0x1714
    spans = ((4, 0x16E8),)
    helpers = ((4, 5860, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (23, 85, 103, 239, 480, 499, 555)), (9, (187, 596)))
    entry_anchors = {
        4: 0x27BDFDB0, 0xC38: 0xAFAB021C,
        0x10FC: 0x00131400, 0x1104: 0x00021023,
        0x1108: 0x00409821, 0x110C: 0x00021400,
        0x121C: 0x00131400, 0x1224: 0x00021023,
        0x1228: 0x00409821, 0x122C: 0x00021400,
        0x16E0: 0x03E00008, 0x16E4: 0x27BD0250,
        0x16E8: 4096, 0x16EC: 4096, 0x16F0: 4096, 0x16F4: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant770_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013C6E8 D_8017C6E8\n'
                         '#define D_8013C6F8 D_8017C6F8\n'
                         '#define D_8013C714 D_8017C714\n'
                         '#include "variant770_entry.c"\n')
        source = (directory / "variant770_entry.c").read_text()
        header = (directory / "variant770_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__)\b")
        for declaration in (
            "extern GsIMAGE D_8013C6F8[];", "extern Model770Config D_8013C714[];",
            "Model770Config *G32 config;", "SVECTOR trail_first[32];",
            "SVECTOR trail_second[32];", "SVECTOR sprite_offsets[32];",
            "u8 field_424[0x100];", "SVECTOR positions[64];",
            "SVECTOR velocities[64];", "u16 sprite_frames[32];",
            "u16 depths[64];", "u32 scale;",
        ):
            self.assertIn(declaration, header)
        for expression in (
            "static const VECTOR D_8013C6E8 = {4096, 4096, 4096, 0};",
            "&D_8013C714[command]", "other = work->sprite_offsets;",
            "position.vx += other[i].vx;", "copyVector(&position, &work->sprite_origins[i]);",
            "u16 side;", "s32 next_side = -(s16)side;", "side = next_side;",
            "s32 first_x = -(s16)next_side * 64;",
            "s32 first_x = -(s16)next_side * 32;",
            "if (work->scale < 0x4000)", "flag = 1;",
            "velocity->vy += ((350 - velocity->vy) / work->config->dot_radius) / 2;",
        ):
            self.assertIn(expression, source)
        self.assertEqual(source.count("GsSortBoxFill(&box, ot, work->depths[i] / 4);"), 2)
        velocity = "(work->config->dot_radius * csin(k) / 4096) * ccos(j) / 4096;"
        self.assertIn("other->vy = " + velocity, source)
        self.assertIn("other->vz = " + velocity, source)
        self.assertLess(source.index("step = Model_GetFrameStep();"),
                        source.index("Model_SetFrameStepOverride(1);"))
        self.assertEqual(source.count("depth = RotAverage4("), 5)

    def test_attempt_ledger_preserves_failed_and_exact_experiments(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        with (family435.ROOT / "notes/overlays/french-model-variant770-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 152)
        self.assertEqual(len({(row["attempt"], row["slot"]) for row in rows}), 152)
        experiments = [row for row in rows if row["result"] in ("mismatch", "text_exact")]
        self.assertEqual(len(experiments), 148)
        groups = {}
        for row in rows:
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
        for row in experiments:
            groups.setdefault(row["attempt"], []).append(row)
            self.assertEqual(row["result"], "text_exact" if row["different_words"] == "0" else "mismatch")
            self.assertRegex(row["reason"], r"Frame \d+/592;")
        self.assertEqual(len(groups), 74)
        for pair in groups.values():
            self.assertEqual({row["slot"] for row in pair}, {"0", "1"})
            self.assertEqual(len({(row["instruction_bytes"], row["different_words"], row["profile"])
                                  for row in pair}), 1)
        milestones = {
            "particle-priority-division-expression": ("5852", "525"),
            "indexed-local-sprite-offset-base": ("5848", "679"),
            "indexed-shared-sprite-offset-base": ("5852", "380"),
            "quarter-orientation-after-first-coordinate": ("5860", "16"),
            "quarter-coordinate-before-orientation-store": ("5860", "17"),
            "shared-quarter-orientation-corners": ("5860", "8"),
            "halfword-persistent-orientation-with-word-result": ("5860", "0"),
            "isolated-no-cse-skip-blocks": ("5884", "1073"),
        }
        for name, expected in milestones.items():
            for row in groups[name]:
                self.assertEqual((row["instruction_bytes"], row["different_words"]), expected)
        failures = {row["attempt"]: row for row in rows if row["result"] == "blocked_compile"}
        self.assertEqual(set(failures), {"native-packet-pointer-lifetimes",
                                        "named-skip-block-profile-calibration"})
        for row in failures.values():
            self.assertEqual((row["slot"], row["instruction_bytes"], row["different_words"]), ("", "", ""))
        self.assertIn("All50 layout checks passed", failures["native-packet-pointer-lifetimes"]["reason"])
        self.assertIn("layout probe has none", failures["named-skip-block-profile-calibration"]["reason"])
        terminal = [row for row in rows if row["result"] == "matched"]
        self.assertEqual({row["slot"] for row in terminal}, {"0", "1"})
        for row in terminal:
            source = directory / ("variant770_entry" + ("_slot1" if row["slot"] == "1" else "") + ".c")
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("5860", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{base + 0x16F8:X} = 0x{base + 0x16F8:X}; // type:u8 size:0x1C defined:true", symbols)
            self.assertIn(f"D_{base + 0x16E8:X} = 0x{base + 0x16E8:X}; // type:u32 size:0x10 defined:true", symbols)
            slot = int(module["name"][-1])
            source = "overlays/french_model_variant/variant770_entry" + ("_slot1" if slot else "")
            self.assertIn(f"[0x16E8, .rodata, {source}]", layout.read_text())
            self.assertIn(f"[0x16F8, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (159, 159, 159, 255, 0, 0, 17, 9, 3, 32, 64, 16, 128, 64, 64, 48, 62, 52, 50),
            1: (159, 159, 159, 255, 0, 0, 15, 14, 4, 32, 64, 16, 128, 64, 64, 86, 98, 90, 86),
            2: (159, 159, 64, 255, 0, 0, 21, 20, 3, 32, 64, 16, 128, 64, 64, 166, 190, 172, 170),
            3: (255, 255, 144, 255, 0, 0, 19, 18, 4, 32, 64, 24, 128, 64, 64, 96, 110, 102, 98),
            4: (255, 255, 144, 255, 0, 0, 21, 20, 3, 32, 64, 24, 128, 64, 64, 50, 66, 58, 56),
            5: (255, 128, 144, 255, 0, 0, 13, 12, 3, 32, 64, 24, 128, 64, 64, 26, 36, 30, 28),
            6: (255, 96, 96, 255, 0, 0, 7, 6, 3, 32, 64, 24, 128, 64, 64, 92, 104, 100, 98),
        }
        arguments = {(7, 23): 0, (7, 85): 4, (7, 103): 2, (7, 239): 3,
                     (7, 480): 6, (7, 499): 1, (7, 555): 0,
                     (9, 187): 5, (9, 596): 5}
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot = int(row["slot"])
                argument = arguments[int(row["stage"]) - slot, int(row["model"])]
                self.assertEqual(int(row["command_word"]), 302000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                observed.add(argument)
                self.assertEqual(struct.unpack_from("<8B11h", data, 0x1714 + 30 * argument),
                                 descriptors[argument])
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<1465I", data[4:0x16E8])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 73)
                self.assertEqual(set(calls), addresses)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 10 * 2048)):
                    self.assertTrue(context + 0xC08 <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model770Config)": 30, "sizeof(Model770State)": 0xC08,
            "sizeof(Model770Color)": 4, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(MATRIX)": 32, "sizeof(POLY_G4)": 36, "sizeof(POLY_FT4)": 40,
            "sizeof(GsBOXF)": 16, "sizeof(GsIMAGE)": 28,
            "sizeof(DVECTOR)": 4, "sizeof(PSXLONG)": 4,
        }
        for typename, fields in (
            ("Model770State", (
                ("trail_first", 4), ("trail_second", 0x104), ("quad", 0x204),
                ("sprite_offsets", 0x224), ("sprite_origins", 0x324), ("field_424", 0x424),
                ("positions", 0x524), ("velocities", 0x724), ("inner_ring", 0x924),
                ("outer_ring", 0x9A4), ("screen", 0xA24), ("trail_count", 0xB24),
                ("level", 0xB28), ("scale", 0xB2C), ("sprite_frames", 0xB30),
                ("depths", 0xB70), ("active_sprites", 0xBF0), ("previous_sprites", 0xBF2),
                ("elapsed", 0xBF4), ("phase", 0xBF6), ("burst_tpage", 0xBF8),
                ("burst_clut", 0xBFA), ("texture", 0xBFC), ("trail_color", 0xC00),
                ("burst_color", 0xC04),
            )),
            ("Model770Config", (
                ("first_part", 6), ("second_part", 7), ("trail_layers", 8),
                ("sprite_count", 0xA), ("dot_count", 0xC), ("trail_layer_step", 0xE),
                ("sprite_spread", 0x10), ("dot_radius", 0x12), ("sprite_size", 0x14),
                ("initial_delay", 0x16), ("trail_duration", 0x18),
                ("burst_delay", 0x1A), ("sprite_delay", 0x1C),
            )),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 50)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant770_entry.h")
