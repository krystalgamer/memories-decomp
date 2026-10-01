import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant435Tests(family435.FrenchModelVariant435Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    reachable_helpers = {0x1AD4, 0x1E7C}
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    helpers = ((0x1AD4, 936, "sheet", "func_8013CAA4"),
               (0x1E7C, 988, "webs", "func_8013CE50"),
               (0x28A4, 1804, "bands", "func_8013D86C"),
               (0x2FB0, 776, "spokes", "func_8013DF78"),
               (0x32B8, 892, "rings", "func_8013E284"),
               (0x3634, 868, "quad", "func_8013E604"))

    def test_bindings_cover_fallback_functions_as_well_as_c(self):
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            bindings = (family435.ROOT / module["linker_symbols"]).read_text()
            for name, address in re.findall(r"(\w+) = (0x[0-9A-F]+); // type:func absolute:true", symbols):
                self.assertIn(f"{name} = {address};", bindings)

    def test_actual_descriptors_and_minimum_context_view(self):
        path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        resident = family435.ROOT / "game/spain/SLES_039.51"
        if not path.exists() or not resident.exists():
            self.skipTest("legal Spanish MODEL and resident inputs required")
        pointers = struct.unpack_from("<14I", resident.read_bytes(), 0x800)
        self.assertEqual((pointers[9], pointers[10]), (0x80136000, 0x80176000))
        widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
        requests = set()
        with path.open("rb") as archive:
            for module in self.modules:
                instance = self.instances[module["name"]]
                base, slot = int(module["load_address"], 0), int(instance["slot"])
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                table = base + 0x3A94
                anchors = {
                    0xA0: 0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF),
                    0xA4: 0x24420000 | (table & 0xFFFF),
                    0xB0: 0x00181900, 0xB4: 0x00781821, 0xB8: 0x00031880,
                    0xBC: 0x00621821, 0xC0: 0xAEC31B74,
                    0xF14: 0x0C000000 | (((base + 0x1E7C) >> 2) & 0x3FFFFFF),
                }
                for offset, word in anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                archive.seek((int(instance["record"]) * 276 + 275) * 2048 + 0x110 +
                             (int(instance["stage"]) - 7) // 2 * 4)
                request, = struct.unpack("<i", archive.read(4))
                self.assertEqual(request, int(instance["command_word"]))
                self.assertGreaterEqual(request, 0)
                requests.add(request)
                descriptor = 0x3A94 + request % 1000 * 68
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 68, len(data))
                accesses = [(w & 0xFFFF) + widths[w >> 26]
                            for w in struct.unpack("<1056I", data[4:0x1084])
                            if w >> 26 in widths and w >> 21 & 31 == 22 and w & 0x8000 == 0]
                self.assertEqual(max(accesses), 0x1BB4)
                context = pointers[9 + slot]
                for start, size in ((pointers[slot], 96 * 2048),
                                    (pointers[3 + slot], 2 * 2048), (base, 10 * 2048)):
                    self.assertTrue(context + max(accesses) <= start or start + size <= context)
        self.assertEqual(requests, {601000, 601001, 601002, 601003, 601005,
                                   601006, 601007, 601009, 601010, 601011})
