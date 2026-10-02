import csv
import hashlib
import struct

from tools.project.tests import test_french_model_variant439 as layout
from tools.project.tests import test_french_model_variant435 as inventory
from tools.project.tests import test_spanish_model_variant460 as instructions
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant439Tests(layout.FrenchModelVariant439Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    resident_name = "SLES_039.51"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    register_writes = staticmethod(instructions.SpanishModelVariant460Tests.register_writes)
    direct_stores = staticmethod(instructions.SpanishModelVariant460Tests.direct_stores)

    def legal_images(self):
        path = inventory.ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                row = self.instances[module["name"]]
                observation = {key: int(row[key]) for key in ("model", "record", "stage", "slot")}
                observation["command"] = int(row["command_word"])
                yield module, observation, data

    def test_wrappers_only_rename_verified_functions(self):
        directory = inventory.ROOT / "src/overlays/french_model_variant"
        for slot in (0, 1):
            for offset, _, role, original in self.helpers:
                if role == "entry":
                    continue
                source = directory / (f"variant439_{role}" + ("_slot1" if slot else "") + ".c")
                self.assertEqual(source.read_text(),
                                 '#include "../../types.h"\n' +
                                 ("#define VERSION_FRENCH\n" if role == "bands" else "") +
                                 f"#define {original} func_{0x8013B000 + slot * 0x40000 + offset:X}\n" +
                                 f'#include "../model_variant/variant422_{role}.c"\n')

    def test_terminal_attempts_cover_every_selected_c_owner(self):
        with (inventory.ROOT / "notes/overlays/spanish-model-variant439-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        expected = {(module["name"], offset): size for module in self.modules
                    for offset, size, _, _ in self.helpers}
        self.assertEqual(len(rows), 70)
        self.assertEqual({(row["module"], int(row["function_offset"], 0)) for row in rows}, set(expected))
        roles = {offset: role for offset, _, role, _ in self.helpers}
        for row in rows:
            offset, slot = int(row["function_offset"], 0), int(row["slot"])
            self.assertEqual(row["slot"], self.instances[row["module"]]["slot"])
            source = inventory.ROOT / "src/overlays/french_model_variant" / (
                f"variant439_{roles[offset]}" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual(int(row["instruction_bytes"]), expected[row["module"], offset])
            self.assertEqual((row["result"], row["different_words"], row["profile"]),
                             ("matched", "0", "gcc_2_8_1_g0_split"))

    def test_original_context_reaches_all_three_delay_slot_dispatches(self):
        dispatches = {0x1094: 0x1E7C, 0x10B0: 0x2878, 0x10C8: 0x1704}
        for module, _, data in self.legal_images():
            base = int(module["load_address"], 0)
            word = lambda pc: struct.unpack_from("<I", data, pc)[0]
            self.assertEqual(word(0xC), 0x00809821)
            self.assertEqual(word(0x14), 0x0260F021)
            for pc, offset in dispatches.items():
                self.assertEqual(word(pc), 0x0C000000 | ((base + offset) >> 2 & 0x3FFFFFF))
                self.assertEqual(word(pc + 4), 0x02602021)
            definitions = {r: set(self.register_writes(data, 4, 0x1230, r)) for r in (4, 19)}
            pending, visited = [(4, None, None)], set()
            reaching = {pc: set() for pc in dispatches}
            while pending:
                pc, root, arg = pending.pop()
                self.assertTrue(4 <= pc < 0x1230 and pc % 4 == 0)
                if (pc, root, arg) in visited:
                    continue
                visited.add((pc, root, arg))
                if pc in definitions[19]:
                    root = pc
                if pc in definitions[4]:
                    arg = pc
                instruction = word(pc)
                op = instruction >> 26
                if op == 0 and instruction & 63 in (8, 9):
                    self.assertEqual(instruction, 0x03E00008)
                if op in (1, 2, 3, 4, 5, 6, 7) or instruction == 0x03E00008:
                    if pc + 4 in definitions[19]:
                        root = pc + 4
                    if pc + 4 in definitions[4]:
                        arg = pc + 4
                    if pc in dispatches:
                        reaching[pc].add((root, arg))
                    if instruction == 0x03E00008:
                        continue
                    if op == 2:
                        targets = [(((base + pc + 4) & 0xF0000000) | ((instruction & 0x3FFFFFF) << 2)) - base]
                    elif op == 3:
                        targets, arg = [pc + 8], None
                    else:
                        displacement = (instruction & 65535) - (65536 if instruction & 32768 else 0)
                        targets = [pc + 8, pc + 4 + displacement * 4]
                    pending.extend((target, root, arg) for target in targets)
                else:
                    pending.append((pc + 4, root, arg))
            self.assertEqual(reaching, {pc: {(0xC, pc + 4)} for pc in dispatches})
            for begin, end, count in ((4, 0x1230, 84), (0x1704, 0x1E7C, 16),
                                       (0x1E7C, 0x2360, 10), (0x2360, 0x2878, 11),
                                       (0x2878, 0x2C90, 9)):
                calls = [pc for pc in range(begin, end, 4) if word(pc) >> 26 == 3]
                self.assertEqual(len(calls), count)
                local = {pc: (((base + pc + 4) & 0xF0000000) | ((word(pc) & 0x3FFFFFF) << 2)) - base
                         for pc in calls
                         if base <= (((base + pc + 4) & 0xF0000000) | ((word(pc) & 0x3FFFFFF) << 2)) < base + 20480}
                self.assertEqual(local, dispatches if begin == 4 else {})

    def test_fifty_complete_preserved_register_write_sets(self):
        rows = (
            (4, 0x1230, (
                [0x1D0, 0x1D4, 0x264, 0x310, 0x540, 0x628, 0x6EC, 0x76C, 0x7C8, 0x864, 0xB10, 0xC64, 0xCB4, 0xD68, 0xDC0, 0xFD8, 0xFFC, 0x10E8, 0x1224],
                [0x5C, 0x240, 0x2F8, 0x514, 0x578, 0x624, 0x710, 0x768, 0x7D0, 0xAAC, 0xB70, 0xC60, 0xCAC, 0xD64, 0xDB8, 0xFC4, 0x1000, 0x1220],
                [0x1A4, 0x508, 0x600, 0x61C, 0x754, 0x760, 0x830, 0xAB0, 0xB4C, 0xC58, 0xD4C, 0xD5C, 0xE24, 0xF70, 0x121C],
                [0xC, 0x1BC, 0x440, 0x4C0, 0x50C, 0x574, 0x620, 0x6F8, 0x764, 0x7CC, 0xA34, 0xAA8, 0xB08, 0xBB8, 0xC50, 0xCE0, 0xD54, 0xDE8, 0x1218],
                [0x1EC, 0x510, 0x570, 0x618, 0xA30, 0xC48, 0x1214],
                [0x1F8, 0x4F8, 0x598, 0x610, 0x724, 0x75C, 0x828, 0x848, 0x9EC, 0xA28, 0xBEC, 0xC5C, 0xCB8, 0xD60, 0xDC4, 0x1210],
                [0x218, 0x504, 0xA9C, 0x120C], [0x4C, 0x2A0, 0x4FC, 0xA68, 0x1208],
                [4, 0x122C], [0x14, 0x1204])),
            (0x1704, 0x1E7C, (
                [0x17D8, 0x17DC, 0x1838, 0x19FC, 0x1A00, 0x1A1C, 0x1A78, 0x1ADC, 0x1E70],
                [0x1748, 0x1E6C], [0x17D4, 0x1AC4, 0x1E68], [0x170C, 0x1E64],
                [0x17A0, 0x1AA0, 0x1AA4, 0x1D60, 0x1E60], [0x17B0, 0x1BE0, 0x1E5C],
                [0x17BC, 0x1A74, 0x1AAC, 0x1D38, 0x1E58], [0x17AC, 0x1AB4, 0x1E54],
                [0x1704, 0x1E78], [0x17A4, 0x1A94, 0x1AA8, 0x1D54, 0x1E50])),
            (0x1E7C, 0x2360, (
                [0x1EC4, 0x2320, 0x2354], [0x1E84, 0x2350], [0x1EB8, 0x234C],
                [0x2004, 0x21B8, 0x2348], [0x2014, 0x21C4, 0x2344],
                [0x1E8C, 0x232C, 0x2340], [0x1EC0, 0x233C], [0x1EBC, 0x231C, 0x2338],
                [0x1E7C, 0x235C], [0x1EB4, 0x2334])),
            (0x2360, 0x2878, (
                [0x2548, 0x2570, 0x25D0, 0x26B0, 0x286C], [0x23A4, 0x2868],
                [0x23E4, 0x281C, 0x2864], [0x2444, 0x24CC, 0x24D8, 0x2860],
                [0x236C, 0x285C], [0x243C, 0x24C0, 0x24D4, 0x2858],
                [0x25E4, 0x2854], [0x2438, 0x24A4, 0x24D0, 0x2850],
                [0x2360, 0x2874], [0x25CC, 0x26CC, 0x284C])),
            (0x2878, 0x2C90, (
                [0x28C0, 0x2900, 0x2948, 0x297C, 0x2AD8, 0x2B90, 0x2C84],
                [0x2880, 0x2C80], [0x28EC, 0x2C7C], [0x2AD4, 0x2B80, 0x2C78],
                [0x2AD0, 0x2B7C, 0x2C74], [0x2A08, 0x2ACC, 0x2B78, 0x2C70],
                [0x29E8, 0x2AC8, 0x2B84, 0x2C6C], [0x290C, 0x2C24, 0x2C68],
                [0x2878, 0x2C8C], [0x28B4, 0x2C2C, 0x2C64])),
        )
        for _, _, data in self.legal_images():
            for start, end, sets in rows:
                for register, expected in zip((*range(16, 24), 29, 30), sets, strict=True):
                    self.assertEqual(self.register_writes(data, start, end, register), expected)

    def test_frames_spills_context_roots_and_packet_fields(self):
        anchors = {
            0x170C: 0x00809821, 0x1748: 0x267117A0,
            0x1E84: 0x00808821, 0x1EB8: 0x263217D4,
            0x236C: 0x0080A021, 0x23A4: 0x269118D0,
            0x2880: 0x00808821, 0x28EC: 0x2632183C,
            0x20E0: 0x27A200D0, 0x20E8: 0x27A200D4,
            0x2618: 0x27A200D0, 0x2620: 0x27A200D4,
            0x2B38: 0x27A200D0, 0x2B40: 0x27A200D4,
        }
        colors = {(offset + channel, 1) for offset in (4, 16, 28, 40) for channel in range(3)}
        screens = {(offset + component, 2) for offset in (8, 20, 32, 44) for component in (0, 2)}
        for _, _, data in self.legal_images():
            for pc, expected in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], expected, hex(pc))
            for begin, end, frame in ((4, 0x1230, 248), (0x1704, 0x1E7C, 296),
                                       (0x1E7C, 0x2360, 256), (0x2360, 0x2878, 288),
                                       (0x2878, 0x2C90, 272)):
                self.assertEqual(struct.unpack_from("<I", data, begin)[0], 0x27BD0000 | (-frame & 65535))
                self.assertEqual(struct.unpack_from("<I", data, end - 4)[0], 0x27BD0000 | frame)
            for begin, end, spill, expected in (
                    (4, 0x1230, 0x94, [0x94, 0xC14]), (4, 0x1230, 252, [0x90]),
                    (0x1704, 0x1E7C, 244, [0x1744]), (0x1704, 0x1E7C, 252, [0x17B8]),
                    (0x2360, 0x2878, 220, [0x23AC]), (0x2878, 0x2C90, 216, [0x28BC])):
                overlaps = [pc for pc, offset, size in self.direct_stores(data, begin, end, 29)
                            if offset < spill + 4 and spill < offset + size]
                self.assertEqual(overlaps, expected)
            for begin, end, register, expected in (
                    (0x1704, 0x1E7C, 17, colors | screens),
                    (0x1E7C, 0x2360, 18, colors),
                    (0x2360, 0x2878, 17, {(0, 4), *((offset, 1) for offset in range(12, 18))}),
                    (0x2878, 0x2C90, 18, colors)):
                self.assertEqual({(off, size) for _, off, size in self.direct_stores(data, begin, end, register)}, expected)

    def test_projection_gates_and_retained_web_flag_semantics(self):
        anchors = {
            0x107C: 0x8C83001C, 0x1080: 0x8FC2193C, 0x1088: 0x0043102B,
            0x108C: 0x14400003, 0x10A4: 0x28420002, 0x10A8: 0x14400003,
            0x10C0: 0x18400003,
            0x1BCC: 0xAE4001A4, 0x1BE4: 0xAEA00000,
            0x1BF0: 0x04400006, 0x1BF8: 0x964601A4,
            0x1D0C: 0xAC6001A4, 0x1D10: 0xAEA00000, 0x1D24: 0x964601A4,
            0x2194: 0x04600008, 0x219C: 0x8FA200D4,
            0x21A4: 0x04400004, 0x21B4: 0x3066FFFF,
            0x2698: 0x18400004, 0x26A8: 0x3046FFFF,
            0x2918: 0x1860009F, 0x2B54: 0x04C00008,
            0x2B5C: 0x8FA200D4, 0x2B64: 0x04400004, 0x2B74: 0x30C6FFFF,
        }
        for _, _, data in self.legal_images():
            for pc, expected in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], expected, hex(pc))
            stack_flag_reads = [pc for pc in range(0x2360, 0x2878, 4)
                                if struct.unpack_from("<I", data, pc)[0] & 0xFFE0FFFF == 0x8FA000D4]
            self.assertEqual(stack_flag_reads, [])

    def test_geometry_and_four_initialized_three_drawn_curtains(self):
        for _, _, data in self.legal_images():
            for pc, expected in layout.FrenchModelVariant439Tests.entry_anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], expected, hex(pc))
        self.assertEqual(3 * 416, 0x4E0)
        self.assertEqual(0x4E0 + 456, 0x6A8)
        self.assertEqual(0x6A8 + 2 * 152, 0x7D8)
        self.assertEqual(0x7D8 + 6 * 144 + 4 * 144 + 144 + 3 * 424, 0x1300)
        self.assertEqual(0x1300 + 4 * 280, 0x1760)
        self.assertEqual(0x1300 + 3 * 280, 0x1648)
        self.assertEqual(17 * 8, 0x88)
        self.assertEqual(0x183C + 52, 0x1870)
        self.assertEqual(0x18D0 + 20, 0x18E4)

    def test_actual_loader_alternates_commands_and_nonzero_divisors(self):
        sectors = (96, 48, 2, 1, 16, 1, 16, 10, 10, 10, 10, 2, 2, 1, 50, 1)
        timings = {
            605000: (60, 100, 112, 200, 240), 605001: (0, 56, 68, 240, 300),
            605002: (10, 56, 64, 120, 140), 605003: (10, 56, 64, 320, 360),
            605004: (40, 180, 192, 360, 420), 605005: (60, 110, 120, 360, 420),
        }
        self.assertEqual(sum(sectors), 276)
        commands, stages = set(), set()
        with (inventory.ROOT / "game/spain/DATA/MODEL.MRG").open("rb") as archive:
            for module, observation, data in self.legal_images():
                model, record = observation["model"], observation["record"]
                stage, slot = observation["stage"], observation["slot"]
                self.assertEqual(record, model - 50 * (model >= 350))
                self.assertEqual(stage % 2, 1 - slot)
                self.assertEqual(module["sector_offset"], record * 276 + sum(sectors[:stage]))
                alternate = (stage - 7) // 2
                self.assertIn(alternate, (0, 1))
                self.assertEqual(stage, 7 + 2 * alternate + slot)
                archive.seek((record * 276 + 275) * 2048 + 0x110 + alternate * 4)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, observation["command"])
                descriptor = 0x2D8C + command % 1000 * 48
                values = struct.unpack_from("<5I", data, descriptor + 0x1C)
                self.assertEqual(values, timings[command])
                self.assertLess(values[0], values[1])
                self.assertLess(values[1], values[2])
                self.assertLess(values[3], values[4])
                table = int(module["load_address"], 0) + 0x2D8C
                self.assertEqual(struct.unpack_from("<I", data, 0xB4)[0], 0x3C020000 | ((table + 0x8000) >> 16 & 65535))
                self.assertEqual(struct.unpack_from("<I", data, 0xB8)[0], 0x24420000 | (table & 65535))
                commands.add(command)
                stages.add(stage)
        self.assertEqual(stages, {7, 8, 9, 10})
        self.assertEqual(commands, set(timings))
