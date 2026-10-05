import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant101Tests(family435.FrenchModelVariant435Tests):
    family = 101
    slot_header_delta = 130
    module_count = 2
    distinct_images = 2
    binding_count = 23
    tail_start = 0xB10
    spans = ((4, 0xAE4),)
    helpers = ((4, 2784, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (130,)),)
    entry_anchors = {
        4: 0x27BDFBF0,
        0x498: 0x27A40058,
        0x6D8: 0x26E40306,
        0xADC: 0x03E00008,
        0xAE0: 0x27BD0410,
        0xAE4: 4096,
        0xAE8: 4096,
        0xAEC: 4096,
        0xAF0: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant101_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013BAE4 D_8017BAE4\n'
                         '#define D_8013BAF4 D_8017BAF4\n'
                         '#define D_8013BB10 D_8017BB10\n'
                         '#include "variant101_entry.c"\n')
        source = (directory / "variant101_entry.c").read_text()
        header = (directory / "variant101_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__)\b")
        self.assertIn("const VECTOR D_8013BAE4 = {4096, 4096, 4096, 0};", source)
        self.assertIn("extern u8 D_8013BAF4[];", header)
        self.assertIn("extern u8 D_8013BB10[];", header)
        self.assertIn("&((Model101Config *)D_8013BB10)[command]", source)
        self.assertIn("(GsIMAGE *)D_8013BAF4", source)
        self.assertIn("for (point = work->positions, screen = work->projected, i = 0;", source)
        self.assertIn("SVECTOR *gravity_velocity = work->velocities;", source)
        with (family435.ROOT / "notes/overlays/french-model-variant101-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 30)
        self.assertEqual(len({(row["attempt"], row["slot"]) for row in rows}), 30)
        self.assertEqual(sum(row["result"] == "mismatch" for row in rows), 24)
        self.assertEqual(sum(row["result"] == "text_exact" for row in rows), 4)
        terminal = [row for row in rows if row["result"] == "matched"]
        self.assertEqual({row["slot"] for row in terminal}, {"0", "1"})
        for row in rows:
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
        for row in terminal:
            path = directory / ("variant101_entry" + ("_slot1" if row["slot"] == "1" else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("2784", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{base + 0xAF4:X} = 0x{base + 0xAF4:X}; // type:u8 size:0x1C defined:true", symbols)
            self.assertIn(f"D_{base + 0xAE4:X} = 0x{base + 0xAE4:X}; // type:u32 size:0x10 defined:true", symbols)
            slot = int(module["name"][-1])
            source = "overlays/french_model_variant/variant101_entry" + ("_slot1" if slot else "")
            self.assertIn(f"[0xAE4, .rodata, {source}]", layout.read_text())
            self.assertIn(f"[0xAF4, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptor_and_resident_calls(self):
        path = family435.ROOT / f"game/{self.region}/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest(f"legal {self.region} MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                command = int(row["command_word"])
                self.assertEqual(command, 32004)
                config = struct.unpack_from("<6B5h", data, 0xB10 + command % 1000 * 16)
                self.assertEqual(config, (255, 0, 64, 204, 204, 204, 60, 180, 30, 76, 30))
                self.assertLessEqual(config[8], 96)
                self.assertGreater(config[6], 0)
                self.assertGreater(config[10], 0)
                self.assertEqual(0xAF4 + 28, 0xB10)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<696I", data[4:0xAE4])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 43)
                self.assertEqual(set(calls), addresses)
                slot = int(row["slot"])
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 10 * 2048)):
                    self.assertTrue(context + 0xA60 <= start or start + size <= context)

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model101Config)": 16, "sizeof(Model101State)": 0xA60,
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(POLY_FT4)": 40, "sizeof(GsBOXF)": 16,
            "sizeof(GsIMAGE)": 28, "sizeof(DVECTOR)": 4,
        }
        for typename, fields in (
            ("Model101State", (("positions", 4), ("velocities", 0x304), ("ring", 0x604),
                               ("projected", 0x814), ("depths", 0x994), ("texture", 0xA54),
                               ("completed", 0xA58), ("elapsed", 0xA5C))),
            ("Model101Config", (("speed", 6), ("radius", 8), ("count", 10),
                                ("delay", 12), ("duration", 14))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 22)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant101_entry.h")
