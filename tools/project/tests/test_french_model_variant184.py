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
