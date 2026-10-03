import csv
from collections import deque
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
    source_directories = {
        **dict.fromkeys(("ribbons", "sheets", "quads", "strand"), "spanish_model_variant"),
        "entry": "french_model_variant",
    }
    module_count = 4
    distinct_images = 4
    binding_count = 44
    tail_start = 0x2D38
    spans = ((4, 0xE98), (0xE98, 0x1978), (0x1978, 0x1E34),
             (0x1E34, 0x21E0), (0x21E0, 0x2520), (0x2520, 0x2D38))
    helpers = ((4, 3732, "entry", "func_8013B004"),
               (0xE98, 2784, "ribbons", "func_8013BE98"),
               (0x1978, 1212, "sheets", "func_8013C968"),
               (0x1E34, 940, "quads", "func_8013CE20"),
               (0x21E0, 832, "strand", "func_8013D1D0"))
    reachable_helpers = {4, 0xE98, 0x1978, 0x1E34}
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
        0x40: 0xAFB80084, 0x63C: 0x27300240, 0x644: 0xA202FF65,
        0x650: 0xAE15FF68, 0x65C: 0xA213FF60, 0x678: 0xAE02FF80,
        0x680: 0xAE00FF88, 0x688: 0xAE16FF8C, 0x6FC: 0xAE02FFF8,
        0x710: 0xAE02FFFC, 0x718: 0x2BC20008, 0x720: 0x261002E4,
        0xE98: 0x27BDFEC8, 0xEA4: 0x249601E4, 0x12B0: 0x26CA0030,
        0x18C4: 0x27DE02E4, 0x18C8: 0x26D602E4, 0x1974: 0x27BD0138,
        0x3E8: 0x0C020BAA, 0x3F8: 0x240200AF, 0x42C: 0x24050001,
        0x440: 0x24050001, 0x444: 0x26940028, 0x45C: 0x240200DF,
        0x49C: 0x24050001, 0x11C8: 0x27A40028, 0x11CC: 0x27B00040,
        0x1240: 0x27A50030, 0x1244: 0x27A40080, 0x1248: 0x27B00060,
        0x1210: 0x27A800D0, 0x1320: 0x27A700D4,
        0xA4: 0x001910C0, 0xA8: 0x00591023, 0xAC: 0x000210C0,
        0xB4: 0xAEE22FCC, 0xCD8: 0x02602021, 0xD14: 0x02602021, 0xD34: 0x02602021,
    }

    def test_wrappers_only_rename_verified_functions(self):
        for slot in (0, 1):
            for offset, _, role, original in self.helpers:
                directory = family435.ROOT / "src/overlays" / self.source_directories[role]
                name = f"variant475_{role}" + ("_slot1" if slot else "") + ".c"
                if role == "entry":
                    text = (directory / name).read_text()
                    if slot == 0:
                        self.assertIn('#include "variant475_entry.h"', text)
                        self.assertIn("s32 func_8013B004(u8 *context, s32 command)", text)
                    else:
                        self.assertEqual(text, '#include "../../types.h"\n'
                                         '#define func_8013B004 func_8017B004\n'
                                         '#define func_8013BE98 func_8017BE98\n'
                                         '#define func_8013C978 func_8017C978\n'
                                         '#define func_8013CE34 func_8017CE34\n'
                                         '#define D_8013DD38 D_8017DD38\n'
                                         '#include "variant475_entry.c"\n')
                    continue
                if role == "ribbons":
                    text = (directory / name).read_text()
                    if slot == 0:
                        self.assertIn('#include "../../types.h"\n', text)
                        self.assertIn("void func_8013BE98(u8 *ctx)", text)
                        endpoint = text.split("if (k == 12) {", 1)[1].split("} else {", 1)[0]
                        self.assertIn("ribbon->otz[k]", endpoint)
                        self.assertIn("&ribbon->a[k - 1]", endpoint)
                        self.assertNotIn("[12]", endpoint)
                        self.assertNotIn("[11]", endpoint)
                    else:
                        self.assertEqual(text, '#include "../../types.h"\n'
                                         '#define func_8013BE98 func_8017BE98\n'
                                         '#include "variant475_ribbons.c"\n')
                    continue
                self.assertEqual((directory / name).read_text(),
                                 '#include "../../types.h"\n'
                                 f"#define {original} func_{0x8013B000 + slot * 0x40000 + offset:X}\n"
                                 f'#include "../model_variant/variant458_{role}.c"\n')
        with (family435.ROOT / "notes/overlays/spanish-model-variant475-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 48)
        self.assertEqual(sum(r["result"] == "text_exact" for r in rows), 20)
        self.assertEqual(sum(r["result"] == "mismatch" for r in rows), 8)
        terminal = [r for r in rows if r["result"] == "matched"]
        self.assertEqual(len(terminal), 20)
        self.assertEqual({(r["module"], int(r["function_offset"], 0)) for r in terminal},
                         {(m["name"], offset) for m in self.modules for offset, _, _, _ in self.helpers})
        for row in terminal:
            offset, slot = int(row["function_offset"], 0), int(row["slot"])
            _, size, role, _ = next(h for h in self.helpers if h[0] == offset)
            directory = family435.ROOT / "src/overlays" / self.source_directories[role]
            source = directory / (f"variant475_{role}" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["profile"], row["instruction_bytes"], row["different_words"]),
                             ("gcc_2_8_1_g0_split", str(size), "0"))
        for row in rows:
            if row["function_offset"] == "0x2520":
                self.assertEqual((row["instruction_bytes"], row["different_words"]), ("2004", ""))
                self.assertIn("not streamer identity", row["reason"])

    def test_entry_descriptor_domain_and_packed_projection(self):
        path = family435.ROOT / f"game/{self.region}/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest(f"legal {self.region} MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110)
                request, = struct.unpack("<i", archive.read(4))
                self.assertEqual(request, 641000)
                command = request % 1000
                self.assertEqual(command, 0)
                archive.seek(module["sector_offset"] * 2048)
                payload = archive.read(20480)
                self.assertEqual(hashlib.sha256(payload).hexdigest(), module["sha256"])
                descriptor = 0x2E34 + 56 * command
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 56, len(payload))
                self.assertEqual(struct.unpack_from("<6I", payload, descriptor + 0x20),
                                 (20, 80, 160, 280, 460, 560))
        source = (family435.ROOT / "src/overlays/french_model_variant/variant475_entry.c").read_text()
        self.assertIn("Model_CopySlotU16Values(1, (u16 *)&work->target)", source)
        self.assertIn("Model_CopySlotU16Values(0, (u16 *)&work->target)", source)
        self.assertIn("origin_y = projection.origin >> 16;", source)
        self.assertIn("work->screen_delta.vy = (projection.target >> 16) - origin_y;", source)
        self.assertEqual(source.count("Model_GetFrameStep()"), 2)

    def test_descriptor_and_distinct_packet_views(self):
        path = family435.ROOT / f"game/{self.region}/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest(f"legal {self.region} MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<I", data, 0x2E48)[0], 8)
                self.assertEqual(struct.unpack_from("<6I", data, 0x2E54), (20, 80, 160, 280, 460, 560))
        self.assertEqual(0x1E4 + 8 * 740, 0x1904)
        self.assertEqual(0x2D20 + 2 * 40, 0x2D70)
        self.assertEqual(0x1904 + 16 * 156, 0x22C4)
        self.assertEqual(0x22C4 + 6 * 132, 0x25DC)
        self.assertEqual(0x2D98 + 40, 0x2DC0)
        self.assertLessEqual(0x2CB8 + 52, 0x2D98)

    def test_ribbon_record_bounds_cover_every_getter_byte(self):
        initial = {(-index * 16, 1024, 0) for index in range(8)}
        states, pending = set(initial), deque(initial)
        while pending:
            count, length, state = pending.popleft()
            for step in range(256):
                for retire in (False, True):
                    next_count, next_length, next_state = count, length, state
                    if state == 0 and count < 12:
                        next_count = count + step * 2
                        if next_count >= 12:
                            next_count, next_state = 12, 1
                    elif state == 1 and length > 0:
                        next_length = length - step * 128
                        if next_length <= 0:
                            next_count, next_length, next_state = (0, 0, 2) if retire else (-64, 1024, 0)
                    result = next_count, next_length, next_state
                    if result not in states:
                        states.add(result)
                        pending.append(result)
        self.assertEqual(states, {(count, 1024, 0) for count in range(-112, 12, 2)} |
                         {(12, length, 1) for length in range(128, 1025, 128)} | {(0, 0, 2)})
        self.assertEqual(len(states), 71)

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
