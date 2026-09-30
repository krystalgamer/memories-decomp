import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant442 as family442
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant442Tests(family442.FrenchModelVariant442Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    source_directories = {"ribbons": "spanish_model_variant"}
    standalone_helpers = frozenset({"ribbons"})
    helpers = tuple(sorted((*family442.FrenchModelVariant442Tests.helpers,
                            (0x2CE8, 1612, "ribbons", "func_8013DCE8"))))

    def test_ribbon_indexed_first_edge_and_packet_boundaries(self):
        source = (family435.ROOT / "src/overlays/spanish_model_variant/variant442_ribbons.c").read_text()
        self.assertIn("dx = (s16)ribbon->sa[k + 1] - (s16)ribbon->sa[0];", source)
        self.assertIn("dy = (ribbon->sa[k + 1] >> 16) - (ribbon->sa[0] >> 16);", source)
        self.assertIn("#define RotTransPers func_french_80087868", source)
        self.assertEqual(0x1940 + 8 * 108, 0x1CA0)
        self.assertEqual(0x2530 + 28, 0x254C)

    def test_bindings_cover_fallback_functions_as_well_as_c(self):
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            bindings = (family435.ROOT / module["linker_symbols"]).read_text()
            for name, address in re.findall(r"(\w+) = (0x[0-9A-F]+); // type:func absolute:true", symbols):
                self.assertIn(f"{name} = {address};", bindings)

    def test_actual_descriptor_arithmetic_and_minimum_context(self):
        archive_path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        resident_path = family435.ROOT / "game/spain/SLES_039.51"
        if not archive_path.exists() or not resident_path.exists():
            self.skipTest("legal Spanish MODEL and resident inputs required")
        pointers = struct.unpack_from("<14I", resident_path.read_bytes(), 0x800)
        self.assertEqual(pointers[9:11], (0x80136000, 0x80176000))
        widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
        with archive_path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, base = int(row["slot"]), int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                self.assertEqual(struct.unpack_from("<5I", data, 0xB8),
                                 (0x001818C0, 0x00781823, 0x000318C0, 0x00621821, 0xAEC32714))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110)
                request, = struct.unpack("<i", archive.read(4))
                self.assertEqual(request, 608000)
                self.assertEqual(request, int(row["command_word"]))
                accesses = [(word & 0xFFFF) + widths[word >> 26]
                            for word in struct.unpack("<1116I", data[4:0x1174])
                            if word >> 26 in widths and word >> 21 & 31 == 22
                            and word & 0x8000 == 0]
                self.assertEqual(max(accesses), 0x2760)
                context = pointers[9 + slot]
                for start, size in ((pointers[slot], 96 * 2048),
                                    (pointers[3 + slot], 2 * 2048), (base, 10 * 2048)):
                    self.assertTrue(context + max(accesses) <= start or start + size <= context)
