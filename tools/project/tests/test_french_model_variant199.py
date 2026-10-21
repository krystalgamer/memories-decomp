import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant199Tests(family435.FrenchModelVariant435Tests):
    family = 199
    slot_header_delta = 130
    module_count = 4
    distinct_images = 4
    binding_count = 24
    tail_start = 0x1630
    spans = ((4, 0x1594),)
    helpers = ((4, 5520, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((9, (243, 622)),)
    entry_anchors = {
        4: 0x27BDFED0,
        0x23C: 0x26F20554,
        0x250: 0x26FE0458,
        0xE20: 0x8682001A,
        0xE2C: 0x00041A00,
        0xE30: 0x00641823,
        0xE5C: 0x00001812,
        0xE70: 0xA3A30094,
        0xE74: 0xA3A30095,
        0xE7C: 0xA3A30096,
        0x14C0: 0x8683001A,
        0x14D0: 0x14400002,
        0x1510: 0x8EE305A8,
        0x154C: 0x10400003,
        0x155C: 0xA2E205AC,
        0x1560: 0x24020001,
        0x158C: 0x03E00008,
        0x1590: 0x27BD0130,
        0x1594: 4096,
        0x1598: 4096,
        0x159C: 4096,
        0x15A0: 0,
    }


    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{base + 0x15A4:X} = 0x{base + 0x15A4:X}; // type:u8 size:0x8C defined:true", symbols)
            self.assertIn(f"D_{base + 0x1594:X} = 0x{base + 0x1594:X}; // type:u32 size:0x10 defined:true", symbols)
            slot = int(module["name"][-1])
            source = "overlays/french_model_variant/variant199_entry" + ("_slot1" if slot else "")
            self.assertIn(f"[0x1594, .rodata, {source}]", layout.read_text())
            self.assertIn(f"[0x15A4, data, overlays/{module['name']}/image_view]", layout.read_text())

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
                self.assertEqual(command, 130000)
                descriptor = struct.unpack_from("<10B16h", data, 0x1630 + command % 1000 * 42)
                self.assertEqual(descriptor, (64, 64, 64, 160, 128, 128, 48, 48, 48, 6,
                                             200, 150, 300, 150, 200, 400, 300, 60,
                                             40, 40, 60, 60, 24, 16, 10, 300))
                self.assertEqual(0x15A4 + 5 * 28, 0x1630)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                self.assertIn("Model_GetSlotDimensionsWithMinimums = 0x8005AFA4;", bindings)
                self.assertIn("rand = 0x8008F708;", bindings)
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<1380I", data[4:0x1594])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 66)
                self.assertEqual(set(calls), addresses)
                slot = int(row["slot"])
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 10 * 2048)):
                    self.assertTrue(context + 0x5B0 <= start or start + size <= context)

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model199Config)": 42, "sizeof(Model199State)": 0x5B0,
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(POLY_FT4)": 40, "sizeof(POLY_F4)": 24,
            "sizeof(DVECTOR)": 4, "sizeof(GsIMAGE)": 28,
            "sizeof(ModelEffectAdjustment)": 8,
        }
        for typename, fields in (
            ("Model199State", (("rays", 4), ("anchor", 0x404), ("grid", 0x40C),
                               ("sphere", 0x454), ("quads", 0x554), ("texture", 0x594),
                               ("elapsed", 0x5A8), ("completed", 0x5AC))),
            ("Model199Config", (("ray_size", 0xA), ("ray_radius", 0xC), ("ray_height", 0xE),
                                ("grid_radius", 0x10), ("sphere_size", 0x12),
                                ("sphere_radius", 0x14), ("sphere_fall", 0x16),
                                ("ray_fade_in", 0x18), ("flash_duration", 0x1A),
                                ("grid_duration", 0x1C), ("ray_duration", 0x1E),
                                ("sphere_duration", 0x20), ("ray_spacing", 0x22),
                                ("ray_group_size", 0x24), ("ray_delay", 0x26),
                                ("burst_delay", 0x28))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 34)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant199_entry.h")
