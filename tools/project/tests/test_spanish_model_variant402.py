import csv
import struct

from tools.project.tests import test_french_model_variant402 as reference
from tools.project.tests import test_spanish_model_variant338 as shared


class SpanishModelVariant402Tests(shared.SpanishModelVariant338Tests):
    family = 402
    module_count = 4
    tail_start = 0x2060
    models_by_stage = ((7, (6, 551)),)
    helpers = ((4, 2648, "entry"), (0xA5C, 1516, "strip"), (0x1048, 1620, "ribbons"),
               (0x169C, 1180, "rings"), (0x1B38, 1320, "bands"))
    spans = ((4, 0xA5C), (0xA5C, 0x1048), (0x1048, 0x169C),
             (0x169C, 0x1B38), (0x1B38, 0x2060))
    reachable_helpers = {0x1B38}
    entry_calls = {0x1B38}
    entry_anchors = reference.FrenchModelVariant402Tests.entry_anchors

    def test_terminal_records_identify_selected_wrappers(self):
        with (shared.ROOT / "notes/overlays/spanish-model-variant402-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 12)
        historical = [row for row in rows[:10] if row["function_offset"] == "0x1048"]
        self.assertEqual([(row["slot"], row["fingerprint"]) for row in historical], [
            ("0", "02590b45e5bac28f3f327621520bcc669386f21b66c529a022b2f7b2c906ae1b"),
            ("1", "a00b05fc264b896c765963f682e4d2fa7c4a3638341cb1843aea86d446f21b83"),
        ])
        for row in historical:
            self.assertEqual((row["result"], row["instruction_bytes"],
                              row["different_words"], row["profile"]),
                             ("matched", "1620", "0", "gcc_2_8_1_g0_split"))
        self.assertEqual([(row["function_offset"], row["slot"]) for row in rows[10:]],
                         [("0x1048", "0"), ("0x1048", "1")])
        self.check_terminal_records(
            [row for row in rows[:10] if row["function_offset"] != "0x1048"] + rows[10:])

    def check_descriptor(self, data, base, request):
        self.assertEqual(request, 568000)
        config = base + 0x215C
        self.assertEqual(struct.unpack_from("<II", data, 0x84),
                         (0x3C020000 | ((config+0x8000) >> 16 & 0xFFFF),
                          0x24420000 | (config & 0xFFFF)))
        offset = 0x215C + request % 1000 * 20
        self.assertTrue(self.tail_start <= offset and offset+20 <= len(data))
        self.assertEqual(struct.unpack_from("<H", data, offset+12)[0], 1)
        self.assertEqual(struct.unpack_from("<I", data, offset+16)[0], 92)
        self.assertEqual(0x58 + 2 * 144, 0x178)
        self.assertEqual(0x178 + 8 * 88, 0x438)
        self.assertEqual(0x438 + 2 * 280, 0x668)

    def test_retained_layers_and_entry_context_extents(self):
        bindings = (self.config / "overlays/model_variant402_linker_symbols.txt").read_text()
        self.assertIn("RotTransPers = 0x80087868;", bindings)
        self.assertIn("RotTransPers3 = 0x80087898;", bindings)
        path = shared.ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
        roots = ((30, 0x14, 0x8C4), (19, 0xA64, 0x8B0),
                 (30, 0x1050, 0x8C2), (16, 0x16A4, 0x8B0),
                 (20, 0x1B40, 0x8C2))
        anchors = {
            0x698: 0xAFC008B0, 0x6AC: 0xA7D808C2,
            0x1084: 0x27D30178, 0x10A4: 0x27C80058,
            0x10C0: 0x27D60668, 0x1194: 0xA6020020,
            0x11BC: 0x28420002, 0x11DC: 0x26730058,
            0x11F8: 0x28420008, 0x12B0: 0x8D020080,
            0x12C0: 0x27D30178, 0x12D0: 0x27D70188,
            0x14EC: 0x26F70058, 0x1508: 0x28420008,
            0x1510: 0x26730058, 0x1550: 0xA6C20008,
            0x1564: 0xA6C2000A, 0x1570: 0xA6C20010,
            0x157C: 0xA6C20012, 0x1590: 0xA6C20018,
            0x15CC: 0xA6C3001A, 0x1630: 0x26730058,
        }
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                for offset, word in anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                for (start, end), (register, capture, expected) in zip(self.spans, roots):
                    accesses, writes = [], []
                    for offset in range(start, end, 4):
                        word, = struct.unpack_from("<I", data, offset)
                        op, rs, rt, rd = word >> 26, word >> 21 & 31, word >> 16 & 31, word >> 11 & 31
                        if op in widths and rs == register and not word & 0x8000:
                            accesses.append((word & 0xFFFF) + widths[op])
                        destination = rd if op == 0 else rt if op in (
                            8, 9, 10, 11, 12, 13, 14, 15, 32, 33, 35, 36, 37) else None
                        if destination == register:
                            writes.append((offset, word))
                    self.assertEqual(len(writes), 2)
                    self.assertEqual(writes[0][0], capture)
                    self.assertEqual((writes[1][1] >> 26, writes[1][1] >> 21 & 31), (35, 29))
                    self.assertEqual(max(accesses), expected)
