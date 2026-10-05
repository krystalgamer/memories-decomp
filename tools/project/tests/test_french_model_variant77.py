import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant77Tests(family435.FrenchModelVariant435Tests):
    family = 77
    slot_header_delta = 130
    module_count = 8
    distinct_images = 8
    binding_count = 24
    tail_start = 0xC44
    spans = ((4, 0xC18),)
    helpers = ((4, 3092, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (386,)), (9, (53, 84, 547)))
    entry_anchors = {
        4: 0x27BDFED0,
        0x808: 0x060000D7,
        0x81C: 0x104000D2,
        0xB64: 0x26520008,
        0xB68: 0x26730001,
        0xBD0: 0x10400003,
        0xBD4: 0x24420001,
        0xBE0: 0xA2C20110,
        0xC10: 0x03E00008,
        0xC14: 0x27BD0130,
        0xC18: 4096,
        0xC1C: 4096,
        0xC20: 4096,
        0xC24: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant77_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013BC18 D_8017BC18\n'
                         '#define D_8013BC28 D_8017BC28\n'
                         '#define D_8013BC44 D_8017BC44\n'
                         '#include "variant77_entry.c"\n')
        source = (directory / "variant77_entry.c").read_text()
        header = (directory / "variant77_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__)\b")
        self.assertIn("const VECTOR D_8013BC18 = {4096, 4096, 4096, 0};", source)
        self.assertIn("extern GsIMAGE D_8013BC28[];", header)
        self.assertIn("extern Model77Config D_8013BC44[];", header)
        self.assertIn("u8 result;", source)
        self.assertIn("            velocity++;\n        }\n", source)
        with (family435.ROOT / "notes/overlays/french-model-variant77-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 30)
        self.assertEqual(len({(row["attempt"], row["slot"]) for row in rows}), 30)
        self.assertEqual(sum(row["result"] == "mismatch" for row in rows), 26)
        self.assertEqual(sum(row["result"] == "text_exact" for row in rows), 2)
        terminal = [row for row in rows if row["result"] == "matched"]
        self.assertEqual({row["slot"] for row in terminal}, {"0", "1"})
        for row in rows:
            expected = ("gcc_2_8_1_g0_split_no_cse_follow_jumps"
                        if row["attempt"] == "endpoint-alias-no-cse-follow"
                        else "gcc_2_8_1_g0_split")
            self.assertEqual(row["profile"], expected)
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
        for row in terminal:
            path = directory / ("variant77_entry" + ("_slot1" if row["slot"] == "1" else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("3092", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{base + 0xC28:X} = 0x{base + 0xC28:X}; // type:u8 size:0x1C defined:true", symbols)
            self.assertIn(f"D_{base + 0xC18:X} = 0x{base + 0xC18:X}; // type:u32 size:0x10 defined:true", symbols)
            slot = int(module["name"][-1])
            source = "overlays/french_model_variant/variant77_entry" + ("_slot1" if slot else "")
            self.assertIn(f"[0xC18, .rodata, {source}]", layout.read_text())
            self.assertIn(f"[0xC28, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        expected = {
            53: (7000, (96, 128, 16, 64, 96, 8, 1, 0, 90, 60, 60, 20, 300, 400, 20)),
            84: (7001, (128, 16, 128, 64, 8, 96, 16, 0, 200, 40, 40, 10, 300, 400, 100)),
            386: (7003, (160, 64, 160, 96, 32, 128, 2, 0, 180, 40, 50, 20, 300, 500, 50)),
            547: (7004, (96, 32, 96, 96, 32, 128, 25, 0, 360, 40, 50, 20, 300, 500, 60)),
        }
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                command = int(row["command_word"])
                config = struct.unpack_from("<8B7h", data, 0xC44 + command % 1000 * 22)
                self.assertEqual((command, config), expected[int(row["model"])])
                self.assertGreater(config[8], 0)
                self.assertGreater(config[9], 0)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<773I", data[4:0xC18])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 37)
                self.assertEqual(set(calls), addresses)
                slot = int(row["slot"])
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 10 * 2048)):
                    self.assertTrue(context + 0x118 <= start or start + size <= context)

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model77Config)": 22, "sizeof(Model77State)": 0x118,
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(POLY_F3)": 20, "sizeof(POLY_FT4)": 40,
            "sizeof(GsIMAGE)": 28, "sizeof(DVECTOR)": 4,
        }
        for typename, fields in (
            ("Model77State", (("position", 4), ("velocities", 0xC), ("texture", 0x10C),
                              ("completed", 0x110), ("elapsed", 0x114))),
            ("Model77Config", (("part", 6), ("travel", 8), ("duration", 10),
                               ("width", 12), ("amplitude", 14), ("size", 16),
                               ("speed", 18), ("delay", 20))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 22)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant77_entry.h")
