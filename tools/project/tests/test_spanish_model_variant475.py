import csv
import hashlib
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant475Tests(family435.FrenchModelVariant435Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    resident_name = "SLES_039.51"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    family = 475
    source_family = 458
    source_directories = dict.fromkeys(("sheets", "quads", "strand"), "spanish_model_variant")
    module_count = 4
    distinct_images = 4
    binding_count = 44
    tail_start = 0x2D38
    spans = ((4, 0xE98), (0xE98, 0x1978), (0x1978, 0x1E34),
             (0x1E34, 0x21E0), (0x21E0, 0x2520), (0x2520, 0x2D38))
    helpers = ((0x1978, 1212, "sheets", "func_8013C968"),
               (0x1E34, 940, "quads", "func_8013CE20"),
               (0x21E0, 832, "strand", "func_8013D1D0"))
    reachable_helpers = {0x1978, 0x1E34}
    local_call_targets = {0xE98, 0x1978, 0x1E34}
    models_by_stage = ((7, (116, 576)),)
    entry_anchors = {
        0xC: 0x00809821, 0x14: 0x0260B821, 0x18: 0x26F81904,
        0x24: 0x26F62C84, 0x30: 0xAFB80088, 0x54: 0x26F82D98,
        0x80: 0x04A00239, 0x84: 0xAFB8009C,
        0x394: 0x26D60034, 0x398: 0x0C020BBA, 0x39C: 0x02C02021,
        0x4C4: 0x8FA4009C, 0x4C8: 0x0C020BAA,
        0x59C: 0xAE320154, 0x5A0: 0xAE200174, 0x5A8: 0xAE200194,
        0x5AC: 0x2652FF00, 0x5B0: 0x26310004, 0x5B4: 0x26100008,
        0x5F0: 0x2A620008, 0x614: 0x1BC0FFC2,
        0x88C: 0xAC80FFF0, 0x898: 0xAC800000, 0x89C: 0x2484009C,
        0x8A4: 0x2A620010, 0x914: 0xAEE02FFC,
        0x924: 0xA6E02FF6, 0x928: 0xA6E02FF8,
        0xD5C: 0x0C016FC9, 0xD70: 0x0C016FC9, 0xD78: 0xAEE22FC4,
        0x1978: 0x27BDFEE8, 0x198C: 0x265E1904, 0x19B8: 0x26512CB8,
        0x19C0: 0x2650198C, 0x1B70: 0x27A200D0, 0x1B78: 0x27A200D4,
        0x1C14: 0x04C00008, 0x1C1C: 0x8FA200D4, 0x1C24: 0x04400004,
        0x1C34: 0x30C6FFFF, 0x1DDC: 0x2610009C, 0x1DE0: 0x27DE009C,
        0x1E34: 0x27BDFED8, 0x1E50: 0x26912D98, 0x1EBC: 0x8C440154,
        0x1EC4: 0x28820601, 0x209C: 0x27A200E0, 0x20A4: 0x27A200E4,
        0x20C8: 0x04C0000E, 0x20D0: 0x8FA200E4, 0x20D8: 0x0440000A,
        0x20E4: 0x8C420174, 0x20EC: 0x14400006, 0x2100: 0x30C6FFFF,
        0x21E0: 0x27BDFEF8, 0x21F0: 0x269722C4, 0x2224: 0x26952DC0,
        0x2338: 0x00021242, 0x235C: 0x2A42000D, 0x2384: 0x2AC20006,
        0x238C: 0x26F70084, 0x23D4: 0xAEA20000, 0x2418: 0x1A00000A,
        0x2420: 0x2A020800, 0x2424: 0x10400007, 0x2440: 0x3206FFFF,
        0x2494: 0xA6822FF8, 0x2498: 0x00021400, 0x249C: 0x00021403,
        0x24D0: 0xA6822FF6, 0x24D4: 0x00021400, 0x24D8: 0x00021403,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/spanish_model_variant"
        for slot in (0, 1):
            for offset, _, role, original in self.helpers:
                name = f"variant475_{role}" + ("_slot1" if slot else "") + ".c"
                self.assertEqual((directory / name).read_text(),
                                 '#include "../../types.h"\n'
                                 f"#define {original} func_{0x8013B000 + slot * 0x40000 + offset:X}\n"
                                 f'#include "../model_variant/variant458_{role}.c"\n')
        with (family435.ROOT / "notes/overlays/spanish-model-variant475-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 32)
        self.assertEqual(sum(r["result"] == "text_exact" for r in rows), 12)
        self.assertEqual(sum(r["result"] == "mismatch" for r in rows), 8)
        terminal = [r for r in rows if r["result"] == "matched"]
        self.assertEqual(len(terminal), 12)
        self.assertEqual({(r["module"], int(r["function_offset"], 0)) for r in terminal},
                         {(m["name"], offset) for m in self.modules for offset, _, _, _ in self.helpers})
        for row in terminal:
            offset, slot = int(row["function_offset"], 0), int(row["slot"])
            _, size, role, _ = next(h for h in self.helpers if h[0] == offset)
            source = directory / (f"variant475_{role}" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["profile"], row["instruction_bytes"], row["different_words"]),
                             ("gcc_2_8_1_g0_split", str(size), "0"))
        for row in rows:
            if row["function_offset"] == "0x2520":
                self.assertEqual((row["instruction_bytes"], row["different_words"]), ("2004", ""))
                self.assertIn("not streamer identity", row["reason"])

    def test_descriptor_and_distinct_packet_views(self):
        path = family435.ROOT / f"game/{self.region}/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest(f"legal {self.region} MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<6I", data, 0x2E54), (20, 80, 160, 280, 460, 560))
        self.assertEqual(0x1904 + 16 * 156, 0x22C4)
        self.assertEqual(0x22C4 + 6 * 132, 0x25DC)
        self.assertEqual(0x2D98 + 40, 0x2DC0)
        self.assertLessEqual(0x2CB8 + 52, 0x2D98)

    def test_actual_frame_getter_is_two_reads_not_an_unconditional_clamp(self):
        path = family435.ROOT / f"game/{self.region}/{self.resident_name}"
        if not path.exists():
            self.skipTest(f"legal {self.region} resident input required")
        data = path.read_bytes()
        base = struct.unpack_from("<I", data, 0x18)[0]
        words = struct.unpack_from("<8I", data, 0x800 + 0x8005BF24 - base)
        self.assertEqual(words, (0x9382009B, 0x24030006, 0x0043102B, 0x10400002,
                                 0, 0x9383009B, 0x03E00008, 0x00601021))
        two_reads = lambda first, second: second if first < 6 else 6
        self.assertEqual(two_reads(0, 255), 255)
        self.assertTrue(all(two_reads(a, b) <= 2 for a in (0, 1, 2) for b in (0, 1, 2)))
        for address, length, code in ((0x80082EA8, 9, 0x2C), (0x80082EE8, 12, 0x3C)):
            words = struct.unpack_from("<5I", data, 0x800 + address - base)
            self.assertEqual(words, (0x24020000 | length, 0xA0820003, 0x24020000 | code,
                                     0x03E00008, 0xA0820007))
