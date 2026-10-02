import csv
import re
import struct

from tools.project.tests import test_french_model_variant476 as family476
from tools.project.tests import test_spanish_model_variant460 as lifetimes460
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant476Tests(family476.FrenchModelVariant476Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    resident_name = "SLES_039.51"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    helpers = ((0x135C, 944, "sheet", "func_8013C360"),
               (0x20BC, 960, "webs", "func_8013D064"),
               (0x247C, 972, "curtains", "func_8013D430"),
               (0x2848, 1420, "globe", "func_8013D848"),
               (0x2DD4, 1440, "screen_grid", "func_8013DDD4"))
    standalone_helpers = frozenset({"globe", "screen_grid"})
    reachable_helpers = {0x135C, 0x247C, 0x2848, 0x2DD4}
    register_writes = staticmethod(lifetimes460.SpanishModelVariant460Tests.register_writes)
    direct_stores = staticmethod(lifetimes460.SpanishModelVariant460Tests.direct_stores)
    entry_anchors = {
        **family476.FrenchModelVariant476Tests.entry_anchors,
        0x4: 0x27BDFEF0, 0x1358: 0x27BD0110, 0x1708: 0x27BD0100,
        0x20BC: 0x27BDFED8, 0x20C4: 0x00809821, 0x20F0: 0xAFB300D8,
        0x2108: 0x2671237C, 0x2148: 0x26720254, 0x22E0: 0x27A200D0,
        0x22E8: 0x27A200D4, 0x2424: 0x26520260, 0x2448: 0x26730260,
        0x2478: 0x27BD0128, 0x2844: 0x27BD0130, 0x2DD0: 0x27BD0118,
        0x2DDC: 0x00808821, 0x3370: 0x27BD0130, 0x10DC: 0x02602021,
        0x2850: 0x00808821, 0x3160: 0x1040001B, 0x3184: 0x02002021,
        0x31D4: 0x24040002, 0x31D8: 0x24050001, 0x31E0: 0x0C020B3A,
        0x31EC: 0x0C020BBA, 0x31F0: 0x02002021, 0x3210: 0x2442FF80,
        0x3224: 0x2442FF80, 0x323C: 0x2442FF80, 0x3258: 0x26020008,
        0x3260: 0x26020014, 0x3268: 0x26020020, 0x3270: 0x2602002C,
        0x3278: 0x27A200D0, 0x3280: 0x27A200D4, 0x329C: 0x02002021,
        0x32A8: 0x00408821, 0x32AC: 0x02002021, 0x32C0: 0x8FA200D4,
    }

    def legal_images(self):
        path = family476.family435.ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                yield module, archive.read(20480)

    def test_original_context_reaches_all_five_entry_calls(self):
        calls = {0x10A0: 0x135C, 0x10D8: 0x170C, 0x1138: 0x2DD4,
                 0x1178: 0x247C, 0x1194: 0x2848}
        for module, data in self.legal_images():
            definitions = set(self.register_writes(data, 4, 0x135C, 19))
            base = int(module["load_address"], 0)
            for pc, target in calls.items():
                self.assertEqual(struct.unpack_from("<2I", data, pc),
                                 (0x0C000000 | ((base + target) >> 2 & 0x3FFFFFF), 0x02602021))
            self.assertEqual(struct.unpack_from("<I", data, 0x31C8)[0],
                             0x08000000 | ((base + 0x3248) >> 2 & 0x3FFFFFF))
            pending, visited, reaching = [(4, None)], set(), {}
            while pending:
                pc, definition = pending.pop()
                self.assertTrue(4 <= pc < 0x135C and pc % 4 == 0)
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

    def test_context_packet_and_record_register_lifetimes(self):
        lifetimes = (
            (4, 0x135C, 30, [0x14, 0x1330]), (4, 0x135C, 29, [4, 0x1358]),
            (0x135C, 0x170C, 18, [0x1364, 0x16F8]),
            (0x135C, 0x170C, 21, [0x136C, 0x16D8, 0x16EC]),
            (0x135C, 0x170C, 17, [0x1398, 0x16FC]),
            (0x135C, 0x170C, 29, [0x135C, 0x1708]),
            (0x20BC, 0x247C, 19, [0x20C4, 0x2448, 0x2464]),
            (0x20BC, 0x247C, 18, [0x2148, 0x2424, 0x2468]),
            (0x20BC, 0x247C, 17, [0x2108, 0x246C]),
            (0x20BC, 0x247C, 29, [0x20BC, 0x2478]),
            (0x247C, 0x2848, 23, [0x2484, 0x2820]),
            (0x247C, 0x2848, 30, [0x248C, 0x2804, 0x281C]),
            (0x247C, 0x2848, 18, [0x24DC, 0x27FC, 0x2834]),
            (0x247C, 0x2848, 17, [0x24C4, 0x2838]),
            (0x247C, 0x2848, 29, [0x247C, 0x2844]),
            (0x2848, 0x2DD4, 17, [0x2850, 0x2DC4]),
            (0x2848, 0x2DD4, 18, [0x2898, 0x2DC0]),
            (0x2848, 0x2DD4, 29, [0x2848, 0x2DD0]),
            (0x2DD4, 0x3374, 17, [0x2DDC, 0x317C, 0x31E8, 0x32A8, 0x3364]),
            (0x2DD4, 0x3374, 16, [0x2E30, 0x2E74, 0x2EE8, 0x3368]),
            (0x2DD4, 0x3374, 29, [0x2DD4, 0x3370]),
        )
        for _, data in self.legal_images():
            for start, end, register, expected in lifetimes:
                self.assertEqual(self.register_writes(data, start, end, register), expected)
            stores = self.direct_stores(data, 0x20BC, 0x247C, 29)
            self.assertEqual([pc for pc, off, width in stores if off < 220 and 216 < off + width],
                             [0x20F0])

    def test_packet_fields_projection_slots_and_record_bounds(self):
        packets = ((0x135C, 0x170C, 17, 52), (0x20BC, 0x247C, 17, 20),
                   (0x247C, 0x2848, 17, 52), (0x2848, 0x2DD4, 18, 52),
                   (0x2EE8, 0x3374, 16, 52))
        for _, data in self.legal_images():
            for start, end, register, size in packets:
                fields = {(off, width) for _, off, width in self.direct_stores(data, start, end, register)}
                expected = {(off + channel, 1) for off in (4, 16, 28, 40) for channel in range(3)}
                if size == 20:
                    expected = {(0, 4)} | {(off, 1) for off in range(12, 18)}
                elif start == 0x2EE8:
                    expected |= {(off, 1) for off in (12, 13, 24, 25, 36, 37, 48, 49)} | {(26, 2)}
                self.assertEqual(fields, expected)
                self.assertLessEqual(max(off + width for off, width in fields), size)
            for start, end, frame, projection in (
                    (0x135C, 0x170C, 256, 208), (0x20BC, 0x247C, 296, 208),
                    (0x247C, 0x2848, 304, 248), (0x2848, 0x2DD4, 280, 208),
                    (0x2DD4, 0x3374, 304, 208)):
                stores = self.direct_stores(data, start, end, 29)
                self.assertTrue(all(0 <= off and off + width <= frame for _, off, width in stores))
                self.assertFalse(any(off < projection + 8 and projection < off + width
                                     for _, off, width in stores))
        self.assertEqual(3 * 608, 0x720)
        self.assertEqual(0xF60 + 152, 0xFF8)
        self.assertEqual(0xFF8 + 1260, 0x14E4)
        self.assertEqual(0x14E4 + 1332, 0x1A18)
        self.assertEqual(0x1A18 + 5 * 428, 0x2274)
        self.assertEqual(0x26D0 + 52, 0x2704)

    def test_active_buffer_alias_has_initialization_draw_and_swap_context(self):
        root = family476.family435.ROOT
        path = root / "game/spain/SLES_039.51"
        if not path.exists():
            self.skipTest("legal Spanish resident input required")
        data = path.read_bytes()
        with (root / "config/sles_03951/functions.csv").open() as handle:
            inventory = {int(row["address"], 0): row for row in csv.DictReader(handle)}
        for address, size in ((0x80084D58, 116), (0x800852A8, 16),
                              (0x800852B8, 264), (0x80085488, 164)):
            self.assertEqual(inventory[address]["status"], "sdk_asm")
            self.assertEqual(int(inventory[address]["size"], 0), size)
        anchors = {
            0x80084D98: 0x3C018010, 0x80084D9C: 0xA420F454,
            0x80084DA8: 0x0C0214F2, 0x80084DB0: 0x0C0214AE,
            0x800852A8: 0x3C028010, 0x800852AC: 0x8442F454,
            0x800852B0: 0x03E00008, 0x800852B4: 0,
            0x800852DC: 0x3C038010, 0x800852E0: 0x8463F454,
            0x800852E8: 0x00031840, 0x800852F4: 0x9484F3B8,
            0x80085324: 0x9423F3BC, 0x80085394: 0x0C021E0E,
            0x80085488: 0x3C028010, 0x8008548C: 0x8442F454,
            0x80085498: 0x00021040, 0x800854A4: 0x9463F3B8,
            0x800854BC: 0x9422F3BC, 0x800854C0: 0x0C020125,
            0x800854F4: 0x3C028010, 0x800854F8: 0x8442F454,
            0x80085504: 0x2C420001, 0x80085508: 0x3C018010,
            0x8008550C: 0x0C0214F2, 0x80085510: 0xA422F454,
            0x80085514: 0x0C0214AE,
        }
        for address, expected in anchors.items():
            self.assertEqual(struct.unpack_from("<I", data, address - 0x8000F800)[0], expected)

    def test_all_fallback_and_c_calls_have_resident_bindings(self):
        for module, data in self.legal_images():
            bindings = (family476.family435.ROOT / module["linker_symbols"]).read_text()
            addresses = {int(address, 0) for address in re.findall(r"= (0x[0-9A-F]+);", bindings)}
            base = int(module["load_address"], 0)
            local = {base + start for start, _ in self.spans}
            for word in struct.unpack("<3292I", data[4:0x3374]):
                if word >> 26 == 3:
                    target = 0x80000000 | ((word & 0x3FFFFFF) << 2)
                    self.assertIn(target, addresses | local)
