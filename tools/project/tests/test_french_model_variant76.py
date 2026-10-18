import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant76Tests(family435.FrenchModelVariant435Tests):
    family = 76
    slot_header_delta = 130
    module_count = 8
    distinct_images = 8
    binding_count = 21
    tail_start = 0xB98
    spans = ((4, 0xB98),)
    helpers = ((4, 2964, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (57, 447, 562)), (9, (489,)))
    entry_anchors = {
        4: 0x27BDFF08, 0xB40: 0x14400009, 0xB44: 0x24020004,
        0xB50: 0x10400003, 0xB54: 0x24020001, 0xB5C: 0x24020002,
        0xB60: 0xA2221A28, 0xB64: 0x24020001,
        0xB90: 0x03E00008, 0xB94: 0x27BD00F8,
    }



    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (128, 64, 0, 15, 40, 12, 8, 2, 150, 114, 114, 32),
            1: (160, 160, 192, 50, 40, 12, 6, 0, 100, 114, 114, 40),
            2: (160, 160, 192, 45, 40, 12, 4, 7, 100, 114, 114, 140),
        }
        arguments = {(7, 57): 0, (7, 447): 2, (7, 562): 0, (9, 489): 1}
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot = int(row["slot"])
                argument = arguments[int(row["stage"]) - slot, int(row["model"])]
                self.assertEqual(int(row["command_word"]), 6000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                observed.add(argument)
                self.assertEqual(struct.unpack_from("<8B8x4h", data, 0xB98 + 24 * argument),
                                 descriptors[argument])
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                self.assertIn("SetPolyF3 = 0x80082E08;", bindings)
                self.assertIn("RotAverageNclip3 = 0x80087AB8;", bindings)
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<741I", data[4:0xB98])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 36)
                self.assertEqual(set(calls), addresses)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 10 * 2048)):
                    self.assertTrue(context + 0x1A5C <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model76Config)": 24, "sizeof(Model76State)": 0x1A5C,
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(POLY_F3)": 20, "sizeof(PSXLONG)": 4,
            "sizeof(((Model76State *)0)->faces[0].a)": 4, "sizeof(Model76Face)": 12,
        }
        for typename, fields in (
            ("Model76State", (("positions", 4), ("velocities", 0x804), ("rotations", 0x1004),
                              ("vertices", 0x1804), ("life", 0x1824), ("elapsed", 0x1A24),
                              ("completed", 0x1A28), ("faces", 0x1A2C))),
            ("Model76Config", (("count", 3), ("lifetime", 4), ("fade", 5), ("interval", 6),
                               ("part", 7), ("size", 16), ("angle_y_range", 18),
                               ("angle_x_range", 20), ("delay", 22))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 26)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant76_entry.h")
