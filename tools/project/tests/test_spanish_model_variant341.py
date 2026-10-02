import hashlib
import json
import re
import struct

from tools.project.tests import test_french_model_variant341 as family341
from tools.project.tests import test_french_model_variant435 as family435
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant341Tests(family341.FrenchModelVariant341Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    helpers = ((4, 3892, "entry", "func_8013B004"),
               (0xF38, 1052, "webs", "func_8013BF3C"),
               (0x1354, 1132, "fan", "func_8013C364"),
               (0x22F8, 972, "draw", "func_8013D2F8"),
               (0x26C4, 788, "spokes", "func_8013D6C8"),
               (0x29D8, 896, "rings", "func_8013D9E0"))
    reachable_helpers = {4, 0xF38, 0x1354, 0x22F8}

    def test_bindings_cover_fallback_functions_as_well_as_c(self):
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            bindings = (family435.ROOT / module["linker_symbols"]).read_text()
            for name, address in re.findall(r"(\w+) = (0x[0-9A-F]+); // type:func absolute:true", symbols):
                self.assertIn(f"{name} = {address};", bindings)

    def test_entry_minimum_context_extent_and_draw_argument(self):
        path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<I", data, 0xAE0)[0], 0xA6D80F62)
                self.assertEqual(struct.unpack_from("<II", data, 0xD84),
                                 (0x0C000000 | ((base + 0x22F8) >> 2 & 0x3FFFFFF), 0x02402021))

    def _web_images(self):
        path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                yield module, archive.read(20480)

    def test_web_entry_gate_packet_frame_and_actual_callees(self):
        calls = [0x8005C018, *([0x80089928] * 4), 0x80087CB8, 0x800875F8,
                 0x80086258, 0x80085558, 0x80087958, 0x800840B8]
        for module, data in self._web_images():
            base = int(module["load_address"], 0)
            self.assertEqual(struct.unpack_from("<II", data, 0xDD0),
                             (0x0C000000 | ((base + 0xF38) >> 2 & 0x3FFFFFF), 0x02402021))
            for offset, word in {
                0x84: 0x04A00297, 0xDBC: 0x8EC20F50, 0xDC4: 0x28420002,
                0xDC8: 0x14400003, 0xF38: 0x27BDFED8, 0xF7C: 0x26B10EB0,
                0xF90: 0x26A80EB4, 0xFA4: 0x26A90EB8, 0x1190: 0x3C025000,
                0x1194: 0xAE220000, 0x11A0: 0x27A200D0, 0x11A8: 0x27A200D4,
                0x1350: 0x27BD0128,
            }.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word,
                                 (module["name"], hex(offset)))
            words = struct.unpack("<263I", data[0xF38:0x1354])
            self.assertEqual([0x80000000 | ((word & 0x3FFFFFF) << 2)
                              for word in words if word >> 26 == 3], calls)
            bindings = (family435.ROOT / module["linker_symbols"]).read_text()
            self.assertIn("ratan2 = 0x80089928;", bindings)
            self.assertNotIn("func_80089928 =", bindings)

    def test_web_signed_fade_visibility_and_third_record_completion(self):
        anchors = {
            0xFC8: 0x18400003, 0xFD8: 0x00002021, 0xFE0: 0x28821801,
            0x1004: 0x246307FF, 0x1024: 0x248407FF, 0x1040: 0x244207FF,
            0x1054: 0xA3A800E8, 0x1074: 0x28420002,
            0x1080: 0x8E820ED8, 0x108C: 0x8E820EDC, 0x1098: 0x8E820EE0,
            0x10A4: 0x86820EE4, 0x10B0: 0x86820EE6, 0x10BC: 0x86820EE8,
            0x121C: 0x18C0000A, 0x1224: 0x8FA200D4, 0x122C: 0x18400006,
            0x1240: 0x30C6FFFF, 0x1290: 0x8E820F24, 0x1298: 0x00021200,
            0x12A0: 0x28822000, 0x12B0: 0x24020005, 0x12B4: 0x14620010,
            0x12B8: 0x2482E000, 0x12BC: 0x24022000, 0x12C0: 0x24040001,
            0x12C4: 0xAE420000, 0x12C8: 0xAE440004, 0x12D0: 0x24030003,
            0x12E0: 0x14430006, 0x12E8: 0x14840004, 0x12EC: 0x24020006,
            0x12F4: 0xAE820F50, 0x12F8: 0xAE420000, 0x12FC: 0x265201A0,
        }
        for module, data in self._web_images():
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word,
                                 (module["name"], hex(offset)))


class SpanishModelVariant341FanTests(family341.FrenchModelVariant341FanTests):
    @classmethod
    def setUpClass(cls):
        manifest = family435.ROOT / "config/sles_03951/overlays.json"
        cls.modules = [m for m in json.loads(manifest.read_text())["modules"]
                       if m["linker_symbols"].endswith("/model_variant341_linker_symbols.txt")]

    def _images(self):
        path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        self.assertEqual(len(self.modules), 4)
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                yield module, data

    def test_actual_fan_context_frame_and_static_callees(self):
        calls = [0x8005C018, 0x80087CB8, 0x80086258, 0x80085558, 0x800872A8,
                 0x80087CB8, 0x800875F8, 0x80087738, 0x80087958, 0x8004D5B8,
                 0x80087958, 0x8004D5B8]
        for module, data in self._images():
            base = int(module["load_address"], 0)
            self.assertEqual(struct.unpack_from("<I", data, 0xD08)[0],
                             0x08000000 | ((base + 0xD24) >> 2 & 0x3FFFFFF))
            for offset, word in {
                0x84: 0x04A00297, 0x224: 0x8FA40098, 0x228: 0x0C020BB2,
                0xD14: 0x02402021, 0xD24: 0x02402021, 0xD58: 0xAEC30EF4,
                0x135C: 0x0080A021, 0x1364: 0x26930D04, 0x136C: 0x26900D90,
                0x15A8: 0x24070001, 0x15E8: 0x27A200D0, 0x15F0: 0x27A200D4,
                0x1688: 0x04C00009, 0x1698: 0x04400005,
                0x16A4: 0x30C6FFFF, 0x16AC: 0x24070001, 0x1714: 0x0007000D,
                0x1774: 0x1AE0FF0A, 0x178C: 0xAE830F4C, 0x17BC: 0x27BD0100,
            }.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word,
                                 (module["name"], hex(offset)))
            words = struct.unpack("<283I", data[0x1354:0x17C0])
            self.assertEqual([0x80000000 | ((word & 0x3FFFFFF) << 2)
                              for word in words if word >> 26 == 3], calls)
