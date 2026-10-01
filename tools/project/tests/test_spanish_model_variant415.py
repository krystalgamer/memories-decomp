import re
import struct

from tools.project.tests import test_french_model_variant415 as family415
from tools.project.tests import test_french_model_variant435 as family435
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant415Tests(family415.FrenchModelVariant415Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    resident_name = "SLES_039.51"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    helpers = (
        (0x18A4, 1920, "bands", "func_8013C8A4"),
        (0x2024, 1252, "sheets", "func_8013D02C"),
        (0x2508, 1324, "webs", "func_8013D514"),
        (0x2A34, 1412, "curtains", "func_8013DA44"),
    )
    reachable_helpers = {0x18A4, 0x2024, 0x2A34}
    source_directories = {"bands": "spanish_model_variant"}
    standalone_helpers = {"bands"}
    entry_anchors = {
        **family415.FrenchModelVariant415Tests.entry_anchors,
        0x18A4: 0x27BDFED8, 0x18AC: 0x00809821, 0x18E8: 0x26711854,
        0x18EC: 0x8E6319EC, 0x1900: 0x86621A18, 0x1948: 0x267404E0,
        0x195C: 0x27A800C8, 0x1960: 0xAFA800FC,
        0x1980: 0x001280C0, 0x19B4: 0x26020048, 0x19E0: 0x26100090,
        0x1BB4: 0x24C50048, 0x1BBC: 0x24C60090,
        0x1BC4: 0x00108080, 0x1BC8: 0x260200FC, 0x1BD4: 0x26020120,
        0x1BE0: 0x27A200F0, 0x1BF0: 0x000310C0,
        0x1BF4: 0x00431021, 0x1BF8: 0x00021080, 0x1BFC: 0x260700D8,
        0x1C00: 0x8FA800FC, 0x1C14: 0xAFA2001C, 0x1C2C: 0x28630009,
        0x1C34: 0xAE0201A4, 0x1C48: 0x269401C8,
        0x1D84: 0x27A300C8, 0x1D8C: 0xAEA00000,
        0x1DCC: 0x96020120, 0x1DD8: 0x86020122,
        0x1DFC: 0x960200FC, 0x1E08: 0x860200FE,
    }

    def test_band_indexed_loads_and_stack_owned_flags(self):
        source = (family435.ROOT / "src/overlays/spanish_model_variant/variant415_bands.c").read_text()
        for expression in ("poly->x1 = band->sc[j + 1];", "poly->y1 = band->sc[j + 1] >> 16;",
                           "poly->x3 = band->sb[j + 1];", "poly->y3 = band->sb[j + 1] >> 16;",
                           "PSXLONG flag[1][9];", "&flag[i][j]"):
            self.assertIn(expression, source)
        for expression in ("col->sc[1]", "col->sb[1]", "band->flag"):
            self.assertNotIn(expression, source)

    def test_bindings_cover_fallback_functions_as_well_as_c(self):
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            bindings = (family435.ROOT / module["linker_symbols"]).read_text()
            for name, address in re.findall(r"(\w+) = (0x[0-9A-F]+); // type:func absolute:true", symbols):
                self.assertIn(f"{name} = {address};", bindings)

    def test_curtain_packet_context_and_stack_ownership(self):
        archive_path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        if not archive_path.exists():
            self.skipTest("legal Spanish MODEL input required")
        anchors = {
            0x2A34: 0x27BDFED0, 0x2A3C: 0x0080A021, 0x2AB0: 0x269318F0,
            0x2C24: 0x27A40080, 0x2C70: 0xAFA000C8, 0x2C78: 0xAFA00080,
            0x2DA4: 0x26620008, 0x2DAC: 0x26620014, 0x2DB4: 0x26620020,
            0x2DBC: 0x2662002C, 0x2DC4: 0x27A200D0, 0x2DC8: 0xAFA20020,
            0x2DCC: 0x27A200D4, 0x2DD0: 0xAFA20024, 0x2DEC: 0x0C021E56,
            0x2E44: 0x8FA200D4, 0x2E50: 0x02602021, 0x2E68: 0x0C0210AA,
            0x2F9C: 0x8FB40118, 0x2FA0: 0x8FB30114,
            0x2FAC: 0x8FB00108, 0x2FB4: 0x27BD0130,
        }
        self.assertEqual(0x18F0 + 52, 0x1924)
        self.assertEqual(128 + 80, 208)
        self.assertEqual(208 + 4, 212)
        self.assertLessEqual(212 + 4, 264)
        self.assertLess(264, 304)
        with archive_path.open("rb") as archive:
            for module in self.modules:
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                for offset, word in anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                self.assertEqual(struct.unpack_from("<II", data, 0x1134),
                                 (0x0C000000 | ((base + 0x2A34) >> 2 & 0x3FFFFFF),
                                  0x02602021))
                writes = {19: [], 20: []}
                widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
                extents = []
                for offset in range(0x2A34, 0x2FB8, 4):
                    word, = struct.unpack_from("<I", data, offset)
                    op = word >> 26
                    destination = word >> 11 & 31 if op == 0 else word >> 16 & 31 if op in (
                        8, 9, 10, 11, 12, 13, 14, 15, 32, 33, 35, 36, 37) else None
                    if destination in writes:
                        writes[destination].append(offset)
                    if op in widths and word >> 21 & 31 == 20 and not word & 0x8000:
                        extents.append((word & 0xFFFF) + widths[op])
                self.assertEqual(writes[19], [0x2AB0, 0x2FA0])
                self.assertEqual(writes[20], [0x2A3C, 0x2F9C])
                self.assertEqual(max(extents), 0x1A34)
