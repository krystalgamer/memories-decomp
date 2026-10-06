import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant117Tests(family435.FrenchModelVariant435Tests):
    family = 117
    slot_header_delta = 130
    module_count = 6
    distinct_images = 5
    binding_count = 22
    tail_start = 0x1144
    spans = ((4, 0x108C),)
    helpers = ((4, 4232, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (139, 146)), (9, (15,)))
    entry_anchors = {
        4: 0x27BDFEE8, 0xD4: 0xA6A00000,
        0xD8: 0xA640FFFE, 0xE0: 0xA6400000,
        0x4DC: 0x86E20706, 0xC98: 0x1040000A,
        0x1084: 0x03E00008, 0x1088: 0x27BD0118,
        0x108C: 4096, 0x1090: 4096, 0x1094: 4096, 0x1098: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant117_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013C08C D_8017C08C\n'
                         '#define D_8013C09C D_8017C09C\n'
                         '#define D_8013C144 D_8017C144\n'
                         '#include "variant117_entry.c"\n')
        source = (directory / "variant117_entry.c").read_text()
        header = (directory / "variant117_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__|register|volatile)\b")
        for declaration in (
            "Model117Config *G32 config;", "SVECTOR positions[40];",
            "SVECTOR targets[40];", "SVECTOR ends[40];",
            "u8 unknown_144[0xc0];", "u8 unknown_344[0xc0];",
            "u8 unknown_544[0xc0];", "u8 rows[40], unknown_62c[0x18];",
            "u8 frames[40], unknown_66c[0x18];", "s16 scales[40];",
            "s32 textures[6];", "extern GsIMAGE D_8013C09C[];",
            "extern Model117Config D_8013C144[];",
        ):
            self.assertIn(declaration, header)
        for expression in (
            "config = work->config = &D_8013C144[command];",
            "work->elapsed = -config->delay;",
            "(s16)(config->spread + 350)",
            "direction * (450 - config->radius)",
            "target = work->targets;", "end = work->ends;",
            "if (time <= 0)", "((s16 *)Model_GetViewMetricsBuffer())[1]",
            "if (time > config->spacing * config->count + config->fade_duration)",
            "work->frames[i] = (work->frames[i] + 1) % config->frames;",
        ):
            self.assertIn(expression, source)
        for expression, count in (
            ("Model_GetFrameStep()", 2), ("SetPolyFT4(&quad)", 1),
            ("SetSemiTrans(", 3), ("RotAverage4(", 3),
            ("GsSortPoly(", 2), ("GsGetLwUnit(", 2),
            ("ccos(", 1), ("csin(", 2), ("rand()", 7),
        ):
            self.assertEqual(source.count(expression), count)
        self.assertNotIn("MulMatrix2(", source)
        self.assertNotIn("point->pad", source)
        self.assertLess(source.index("target = work->targets;"), source.index("SetPolyFT4"))
        self.assertGreater(source.index("end = work->ends;"), source.index("SetPolyFT4"))

    def test_attempt_ledger_preserves_failed_and_exact_experiments(self):
        with (family435.ROOT / "notes/overlays/french-model-variant117-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        expected = {
            "native-two-phase-particles": (4232, 860),
            "native-bounded-offset-and-sdk-uv": (4232, 118),
            "native-target-before-end-cursor": (4232, 0),
        }
        self.assertEqual(len(rows), 8)
        self.assertEqual(len({(r["attempt"], r["slot"]) for r in rows}), 8)
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
            path = family435.ROOT / f"src/overlays/french_model_variant/variant117_entry{suffix}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("4232", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(
                f"D_{base + 0x109C:X} = 0x{base + 0x109C:X}; // type:u8 size:0xA8 defined:true",
                symbols)
            self.assertIn(f"[0x109C, data, overlays/{module['name']}/image_view]", layout.read_text())
            self.assertIn(
                f"D_{base + 0x108C:X} = 0x{base + 0x108C:X}; // type:u32 size:0x10 defined:true",
                symbols)

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (128,128,128,1,80,100,80,200,200,32,2,34,30,5,40,4,32,8,0),
            1: (128,128,128,2,100,50,80,200,120,20,2,34,30,10,30,4,32,8,0),
            3: (128,128,128,8,70,30,120,200,200,40,1,34,30,4,100,4,64,1,0),
        }
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                argument = {15: 0, 146: 1, 139: 3}[int(row["model"])]
                self.assertEqual(int(row["command_word"]), 48000 + argument)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 48000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                descriptor = struct.unpack_from("<4B11h4B", data, 0x1144 + 30 * argument)
                self.assertEqual(descriptor, descriptors[argument])
                self.assertTrue(0 < descriptor[9] <= 40)
                self.assertTrue(all(descriptor[index] > 0 for index in (6, 10, 11, 12, 17)))
                self.assertLess(descriptor[6] + 350, 32768)
                observed.add(argument)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(v, 16) for v in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<1058I", data[4:0x108C])
                calls = [0x80000000 | ((w & 0x3FFFFFF) << 2) for w in words if w >> 26 == 3]
                self.assertEqual(set(calls), addresses)
                self.assertEqual(len(addresses), 22)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x724 <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model117Config)": 30, "sizeof(Model117State)": 0x724,
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(POLY_FT4)": 40, "sizeof(GsIMAGE)": 28,
        }
        for typename, fields in (
            ("Model117Config", (
                ("size", 4), ("arc_height", 6), ("spread", 8), ("radius", 10),
                ("flash_size", 12), ("count", 14), ("rows", 16),
                ("travel_duration", 18), ("fade_duration", 20), ("spacing", 22),
                ("delay", 24), ("tile_size", 27), ("frames", 28),
            )),
            ("Model117State", (
                ("positions", 4), ("targets", 0x204), ("ends", 0x404),
                ("rows", 0x604), ("frames", 0x644), ("scales", 0x684),
                ("textures", 0x704), ("completed", 0x71C), ("elapsed", 0x720),
            )),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 29)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant117_entry.h")
