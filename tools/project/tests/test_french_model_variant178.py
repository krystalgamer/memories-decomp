import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant178Tests(family435.FrenchModelVariant435Tests):
    family = 178
    slot_header_delta = 130
    module_count = 2
    distinct_images = 2
    binding_count = 21
    tail_start = 0xD84
    spans = ((4, 0xD3C),)
    helpers = ((4, 3384, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((9, (141,)),)
    entry_anchors = {
        4: 0x27BDFED0,
        0x100: 0xA6400000,
        0xB64: 0x0C021556,
        0xD34: 0x03E00008,
        0xD38: 0x27BD0130,
        0xD3C: 4096,
        0xD40: 4096,
        0xD44: 4096,
        0xD48: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant178_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013BD3C D_8017BD3C\n'
                         '#define D_8013BD4C D_8017BD4C\n'
                         '#define D_8013BD84 D_8017BD84\n'
                         '#include "variant178_entry.c"\n')
        source = (directory / "variant178_entry.c").read_text()
        header = (directory / "variant178_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__)\b")
        self.assertIn("const VECTOR D_8013BD3C = {4096, 4096, 4096, 0};", source)
        self.assertIn("extern GsIMAGE D_8013BD4C[];", header)
        self.assertIn("extern Model178Config D_8013BD84[];", header)
        self.assertIn("&D_8013BD84[command % 100]", source)
        self.assertIn("endpoint = work->endpoints;", source)
        self.assertIn("endpoint = work->velocities;", source)
        self.assertIn("position.vx *= time;", source)
        self.assertIn("position.vx /= config->burst_duration;", source)
        with (family435.ROOT / "notes/overlays/french-model-variant178-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 6)
        self.assertEqual(len({(row["attempt"], row["slot"]) for row in rows}), 6)
        self.assertEqual(sum(row["result"] == "mismatch" for row in rows), 2)
        self.assertEqual(sum(row["result"] == "text_exact" for row in rows), 2)
        terminal = [row for row in rows if row["result"] == "matched"]
        self.assertEqual({row["slot"] for row in terminal}, {"0", "1"})
        for row in rows:
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
        for row in terminal:
            path = directory / ("variant178_entry" + ("_slot1" if row["slot"] == "1" else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("3384", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{base + 0xD4C:X} = 0x{base + 0xD4C:X}; // type:u8 size:0x38 defined:true", symbols)
            self.assertIn(f"D_{base + 0xD3C:X} = 0x{base + 0xD3C:X}; // type:u32 size:0x10 defined:true", symbols)
            slot = int(module["name"][-1])
            source = "overlays/french_model_variant/variant178_entry" + ("_slot1" if slot else "")
            self.assertIn(f"[0xD3C, .rodata, {source}]", layout.read_text())
            self.assertIn(f"[0xD4C, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptor_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                command = int(row["command_word"])
                self.assertEqual(command, 109000)
                config = struct.unpack_from("<8B11h", data, 0xD84 + command % 100 * 30)
                self.assertEqual(config, (128, 128, 128, 96, 96, 128, 15, 0,
                                          100, 300, 120, 300, 200, 36, 2, 60, 60, 60, 60))
                self.assertLessEqual(config[13], 64)
                for index in (9, 12, 15, 16, 17):
                    self.assertGreater(config[index], 0)
                self.assertEqual(0xD4C + 2 * 28, 0xD84)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<846I", data[4:0xD3C])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 41)
                self.assertEqual(set(calls), addresses)
                slot = int(row["slot"])
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 10 * 2048)):
                    self.assertTrue(context + 0x450 <= start or start + size <= context)

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model178Config)": 30, "sizeof(Model178State)": 0x450,
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(POLY_FT4)": 40, "sizeof(POLY_F4)": 24, "sizeof(GsIMAGE)": 28,
        }
        for typename, fields in (
            ("Model178State", (("positions", 4), ("endpoints", 0x204), ("velocities", 0x404),
                               ("texture", 0x424), ("completed", 0x448), ("elapsed", 0x44C))),
            ("Model178Config", (("half_size", 8), ("spread", 10), ("burst_half_size", 12),
                                ("rise", 14), ("velocity_spread", 16), ("count", 18),
                                ("spacing", 20), ("travel", 22), ("fade", 24),
                                ("burst_duration", 26), ("delay", 28))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 25)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant178_entry.h")
