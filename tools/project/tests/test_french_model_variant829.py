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
    binding_count = 21
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
