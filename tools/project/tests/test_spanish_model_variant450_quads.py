import csv
import hashlib
import struct
import unittest

from tools.project.tests import test_spanish_model_variant450 as lines

ROOT = lines.ROOT
SOURCE = ROOT / "src/overlays/spanish_model_variant/variant450_quads.c"


class SpanishModelVariant450QuadTests(unittest.TestCase):
    def test_attempts_and_final_source_fingerprints(self):
        with (ROOT / "notes/overlays/spanish-model-variant450-quads-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 6)
        for slot, row in enumerate(rows[-2:]):
            source = SOURCE.with_name("variant450_quads"+("_slot1" if slot else "")+".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())


    def test_retail_extent_calls_and_post_line_caller(self):
        archive = ROOT / "game/spain/DATA/MODEL.MRG"
        if not archive.exists():
            self.skipTest("legal Spanish MODEL input required")
        expected_calls = [0x8005C018, 0x80087CB8, 0x80086258, 0x80085558, 0x800872A8,
                          0x80087CB8, 0x800875F8, 0x80087738, 0x80087958, 0x800842A8]
        with archive.open("rb") as handle:
            for stage, slot, sector, digest in lines.IMAGES:
                base = 0x8013B000+slot*0x40000
                handle.seek(sector*2048)
                image = handle.read(20480)
                self.assertEqual(hashlib.sha256(image).hexdigest(), digest)
                self.assertEqual(struct.unpack_from("<4I", image, 0xC34),
                                 (0x0C000000 | ((base+0x2D88) >> 2 & 0x3FFFFFF), 0x02602021,
                                  0x0C000000 | ((base+0x27C0) >> 2 & 0x3FFFFFF), 0x02602021))
                words = struct.unpack("<370I", image[0x27C0:0x2D88])
                self.assertEqual(words[0], 0x27BDFEF8)
                self.assertEqual([0x80000000 | ((w & 0x3FFFFFF) << 2) for w in words if w >> 26 == 3],
                                 expected_calls)

    def test_target_compiled_quad_view(self):
        checks = {"sizeof(Variant450QuadGroup)": 160, "sizeof(Variant450QuadConfig)": 32,
                  "sizeof(Variant450QuadView)": 0x42FC, "sizeof(POLY_GT4)": 52,
                  "sizeof(Variant450Primary)": 536}
        for typename, fields in (
            ("Variant450QuadGroup", (("points", 0), ("end_color", 128), ("color", 132),
                                     ("scale", 136), ("fading", 152), ("brightness", 156))),
            ("Variant450QuadConfig", (("grow_start", 12), ("grow_end", 16),
                                      ("fade_start", 24), ("fade_end", 28))),
            ("Variant450QuadView", (("primary", 0x440), ("groups", 0x3598), ("quad", 0x415C),
                                    ("translation", 0x4268), ("flags", 0x42A8), ("elapsed", 0x42AC),
                                    ("step", 0x42B4), ("config", 0x42BC), ("phase", 0x42F8))),
            ("POLY_GT4", (("x0", 8), ("x1", 20), ("x2", 32), ("x3", 44),
                          ("r0", 4), ("r1", 16), ("r2", 28), ("r3", 40))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 32)
        lines.layouts.FrenchModelVariant373Tests.assert_target_layout(
            self, checks, "../spanish_model_variant/variant450_quads.h")
