import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant337 as french337
from tools.project.tests import test_spanish_model_variant460 as lifetimes460


class SpanishModelVariant337EntryTests(french337.FrenchModelVariant337Tests):
    region = "spanish"
    config_path = "config/sles_03951"
    archive_path = "game/spain/DATA/MODEL.MRG"
    register_writes = staticmethod(lifetimes460.SpanishModelVariant460Tests.register_writes)
    direct_stores = staticmethod(lifetimes460.SpanishModelVariant460Tests.direct_stores)

    def legal_images(self):
        path = french337.spanish337.ROOT / self.archive_path
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        with path.open("rb") as archive:
            for module, *_ in self.selected():
                archive.seek(module["sector_offset"] * 2048)
                yield module, archive.read(20480)

    def test_french_attempt_fingerprints_and_bindings(self):
        root = french337.spanish337.ROOT
        with (root / "notes/overlays/spanish-model-variant337-attempts.csv").open() as handle:
            rows = [row for row in csv.DictReader(handle) if int(row["function_offset"], 0) == 4]
        self.assertEqual(len(rows), 2)
        for slot, row in enumerate(rows):
            source = root / ("src/overlays/french_model_variant/variant337_entry" +
                             ("_slot1" if slot else "") + ".c")
            self.assertTrue(row["reason"].startswith(f"Slot{slot}:"))
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["profile"], row["result"], row["instruction_bytes"], row["different_words"]),
                             ("gcc_2_8_1_g0_split", "matched", "2464", "0"))
        aliases = (("GetTPage", 0x80082CE8), ("GetClut", 0x80082D28),
                   ("SetSemiTrans", 0x80082DA8), ("SetShadeTex", 0x80082DD8),
                   ("SetPolyG3", 0x80082E48), ("SetPolyFT4", 0x80082EA8),
                   ("SetPolyG4", 0x80082EC8), ("SetPolyGT4", 0x80082EE8),
                   ("SquareRoot0", 0x80086DD8), ("Square0", 0x80089BC8),
                   ("GsGetLwUnit", 0x8008A428))
        for module, data in self.legal_images():
            bindings = (root / module["linker_symbols"]).read_text()
            addresses = {int(value, 0) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
            self.assertEqual(len(addresses), 33)
            for name, address in aliases:
                self.assertIn(f"{name} = 0x{address:X};", bindings)
            base = int(module["load_address"], 0)
            calls = [(pc, 0x80000000 | ((struct.unpack_from("<I", data, pc)[0] & 0x3FFFFFF) << 2))
                     for pc in range(4, 0x9A4, 4) if struct.unpack_from("<I", data, pc)[0] >> 26 == 3]
            self.assertEqual(len(calls), 48)
            for _, target in calls:
                self.assertIn(target, addresses | {base + off for off in (0x9A4, 0x1278, 0x1738)})
            self.assertEqual([pc for pc, target in calls if target == 0x8005BF24], [0x868, 0x87C])
            self.assertEqual([pc for pc, target in calls if target == 0x8005CB58],
                             [0x17C, 0x198, 0x1A8, 0x1B8, 0x1C8])

    def test_original_context_reaches_all_three_helpers(self):
        calls = {0x818: 0x9A4, 0x820: 0x1278, 0x83C: 0x1738}
        for module, data in self.legal_images():
            base = int(module["load_address"], 0)
            definitions = set(self.register_writes(data, 4, 0x9A4, 18))
            for pc, offset in calls.items():
                self.assertEqual(struct.unpack_from("<2I", data, pc),
                                 (0x0C000000 | ((base + offset) >> 2 & 0x3FFFFFF), 0x02402021))
            self.assertEqual(struct.unpack_from("<I", data, 0x690)[0],
                             0x08000000 | ((base + 0x88C) >> 2 & 0x3FFFFFF))
            pending, visited, reaching = [(4, None)], set(), {}
            while pending:
                pc, definition = pending.pop()
                self.assertTrue(4 <= pc < 0x9A4 and pc % 4 == 0)
                if (pc, definition) in visited:
                    continue
                visited.add((pc, definition))
                word, = struct.unpack_from("<I", data, pc)
                op = word >> 26
                if pc in definitions:
                    definition = pc
                if op in (1, 2, 3, 4, 5, 6, 7) or word == 0x03E00008:
                    if pc + 4 in definitions:
                        definition = pc + 4
                    if pc in calls:
                        reaching.setdefault(pc, set()).add(definition)
                    if word == 0x03E00008:
                        continue
                    if op == 2:
                        targets = [(((base + pc + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)) - base]
                    elif op == 3:
                        targets = [pc + 8]
                    else:
                        displacement = (word & 65535) - (65536 if word & 32768 else 0)
                        targets = [pc + 8, pc + 4 + displacement * 4]
                    pending.extend((target, definition) for target in targets)
                else:
                    pending.append((pc + 4, definition))
            self.assertEqual(reaching, {pc: {0xC} for pc in calls})

    def test_register_lifetimes_and_incoming_argument_home(self):
        lifetimes = ((22, [0x14, 0x980]), (18, [0xC, 0x18C, 0x990]),
                     (19, [0x1C, 0x5C4, 0x98C]), (21, [0x30, 0x304, 0x984]),
                     (23, [0xE0, 0x418, 0x97C]), (30, [0x28, 0x254, 0x978]),
                     (29, [4, 0x9A0]), (20, [0xDC, 0x3F4, 0x5E4, 0x614, 0x988]))
        for _, data in self.legal_images():
            for register, expected in lifetimes:
                self.assertEqual(self.register_writes(data, 4, 0x9A4, register), expected)
            stores = self.direct_stores(data, 4, 0x9A4, 29)
            self.assertEqual([(pc, off, size) for pc, off, size in stores if off + size > 192],
                             [(0x58, 196, 4)])
            self.assertTrue(all(0 <= off and (off + size <= 192 or (pc, off, size) == (0x58, 196, 4))
                                for pc, off, size in stores))
            self.assertFalse(any(off < 128 and 112 < off + size for _, off, size in stores))
            self.assertEqual([pc for pc, off, size in stores if off < 136 and 132 < off + size], [0x64])
            self.assertEqual([pc for pc, off, size in stores if off < 132 and 128 < off + size], [0x44, 0x62C])

    def test_packet_fields_projection_and_phase_anchors(self):
        anchors = {
            0x4: 0x27BDFF40, 0x9A0: 0x27BD00C0, 0x58: 0xAFA500C4,
            0x7C: 0x8FB800C4, 0x68C: 0x97B800C4, 0x18C: 0x00409021,
            0x28: 0x26DE0C04, 0x254: 0x27DE0034, 0x304: 0x26B50028,
            0x778: 0x27A40050, 0x77C: 0x27A50070, 0x780: 0x27B10074,
            0x794: 0x27B00078, 0x7A4: 0x0C021E1A, 0x7AC: 0x02203021,
            0x7B0: 0x02003821, 0x7B8: 0x97B00070, 0x7BC: 0x87B10072,
            0x7C8: 0x27A40058, 0x7D4: 0x27A5007C, 0x7D8: 0x0C021E1A,
            0x7EC: 0xA6C20D04, 0x7FC: 0xA6C20D06, 0x830: 0x28420003,
            0x854: 0xAEC20D20, 0x880: 0xAEC30D24, 0x884: 0xAEC20D2C,
            0x888: 0xAED00D28, 0x8A8: 0x28420006, 0x8BC: 0x18400009,
            0x8C0: 0x2442FF80, 0x8C8: 0xAEC20D78, 0x8D0: 0xAEC00D78,
            0x8DC: 0x00021140, 0x8E0: 0xAEC20D78, 0x8EC: 0x2462FFFE,
            0x8F0: 0x2C420003, 0x8F8: 0x24180004, 0x908: 0x24180001,
            0x914: 0xAEC20D6C, 0x928: 0x28620040, 0x940: 0xAEC20D30,
            0x944: 0x28420040, 0x94C: 0x24020040, 0x950: 0xAEC20D30,
            0x954: 0x24020007, 0x95C: 0xAEC20D6C, 0x968: 0x24180002,
        }
        for _, data in self.legal_images():
            for pc, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], word)
            for register, regions, offsets, halves, size in (
                    (30, ((0x1FC, 0x254), (0x254, 0x2AC)),
                     (12, 13, 24, 25, 36, 37, 48, 49), (14, 26), 52),
                    (21, ((0x2AC, 0x304), (0x304, 0x35C)),
                     (12, 13, 20, 21, 28, 29, 36, 37), (14, 22), 40)):
                for begin, end in regions:
                    fields = {(off, width) for _, off, width in self.direct_stores(data, begin, end, register)}
                    self.assertEqual(fields, {(off, 1) for off in offsets} | {(off, 2) for off in halves})
                    self.assertLessEqual(max(off + width for off, width in fields), size)
