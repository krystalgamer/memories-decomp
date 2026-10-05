import csv
import hashlib
import importlib.util
import json
from pathlib import Path
import re
import struct
from unittest import SkipTest
from unittest.mock import patch

from tools.project.tests import test_french_model_variant439 as layout
from tools.project.tests import test_french_model_variant435 as inventory
from tools.project.tests import test_spanish_model_variant460 as instructions
from tools.project.progress import load_spanish_overlay_inventories
from tools.project.overlay_sources import c_segments


class SpanishModelVariant439Tests(layout.FrenchModelVariant439Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    resident_name = "SLES_039.51"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    source_directories = {"rings": "spanish_model_variant"}
    helpers = tuple(sorted((*layout.FrenchModelVariant439Tests.helpers,
                            (0x1230, 1236, "rings", "func_8013C230"))))
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
                if role in ("entry", "rings"):
                    continue
                source = directory / (f"variant439_{role}" + ("_slot1" if slot else "") + ".c")
                self.assertEqual(source.read_text(),
                                 '#include "../../types.h"\n' +
                                 ("#define VERSION_FRENCH\n" if role == "bands" else "") +
                                 f"#define {original} func_{0x8013B000 + slot * 0x40000 + offset:X}\n" +
                                 f'#include "../model_variant/variant422_{role}.c"\n')
        source = inventory.ROOT / "src/overlays/spanish_model_variant/variant439_rings.c"
        self.assertIn("void func_8013C230(u8 *work)", source.read_text())
        self.assertEqual(source.with_name("variant439_rings_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013C230 func_8017C230\n'
                         '#include "variant439_rings.c"\n')

    def test_terminal_attempts_cover_every_selected_c_owner(self):
        with (inventory.ROOT / "notes/overlays/spanish-model-variant439-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        expected = {(module["name"], offset): size for module in self.modules
                    for offset, size, _, _ in self.helpers}
        self.assertEqual(len(rows), 99)
        experiments = [row for row in rows if row["result"] == "mismatch"]
        self.assertEqual([(int(row["instruction_bytes"]), int(row["different_words"]))
                          for row in experiments],
                         [(1264, 294), (1248, 297), (1248, 297), (1240, 286),
                          (1264, 284), (1240, 281), (1248, 266), (1240, 281),
                          (1240, 281), (1248, 282), (1288, 292), (1252, 291),
                          (1240, 286), (1236, 24), (1240, 281)])
        rows = [row for row in rows if row["result"] == "matched"]
        self.assertEqual(len(rows), 84)
        self.assertEqual({(row["module"], int(row["function_offset"], 0)) for row in rows}, set(expected))
        roles = {offset: role for offset, _, role, _ in self.helpers}
        for row in rows:
            offset, slot = int(row["function_offset"], 0), int(row["slot"])
            self.assertEqual(row["slot"], self.instances[row["module"]]["slot"])
            directory = self.source_directories.get(roles[offset], "french_model_variant")
            source = inventory.ROOT / "src/overlays" / directory / (
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
        archive_path = inventory.ROOT / "game/spain/DATA/MODEL.MRG"
        if not archive_path.exists():
            self.skipTest("legal Spanish MODEL input required")
        sectors = (96, 48, 2, 1, 16, 1, 16, 10, 10, 10, 10, 2, 2, 1, 50, 1)
        timings = {
            605000: (60, 100, 112, 200, 240), 605001: (0, 56, 68, 240, 300),
            605002: (10, 56, 64, 120, 140), 605003: (10, 56, 64, 320, 360),
            605004: (40, 180, 192, 360, 420), 605005: (60, 110, 120, 360, 420),
        }
        self.assertEqual(sum(sectors), 276)
        commands, stages = set(), set()
        with archive_path.open("rb") as archive:
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

    def test_retained_rings_geometry_and_update_gates(self):
        anchors = {
            0x1230: 0x27BDFEF0, 0x1238: 0x0080F021,
            0x1240: 0x27D60E08, 0x1248: 0x27D01808,
            0x1278: 0x27D20FAC, 0x1288: 0x8E46FFFC,
            0x1290: 0x18C000EF, 0x129C: 0x00021980,
            0x12B8: 0x8FC218F8, 0x12C4: 0x00031303,
            0x12C8: 0x8FC318FC, 0x12CC: 0x24540080,
            0x12D4: 0x8FC21900, 0x13A8: 0x00159A00,
            0x13AC: 0xA6200004, 0x13BC: 0x2AA20011,
            0x13E0: 0x26660110, 0x13E8: 0x26670118,
            0x142C: 0x284200A0, 0x1448: 0x24040002,
            0x1454: 0x24060140, 0x14C8: 0x24060080,
            0x14D8: 0x240601C0, 0x1554: 0x26660088,
            0x155C: 0x26670090, 0x1620: 0x18400005,
            0x1634: 0x3046FFFF, 0x1644: 0x2AA20010,
            0x1658: 0x28821000, 0x1664: 0x8FC21944,
            0x1674: 0x00031940, 0x1688: 0x8FC2197C,
            0x1690: 0x28420003, 0x169C: 0x24021000,
            0x16B0: 0x24630001, 0x16B8: 0x265201A8,
            0x16C0: 0x26D601A8, 0x16C8: 0x29A20003,
            0x1700: 0x27BD0110,
        }
        for _, _, data in self.legal_images():
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word, hex(offset))
            calls = [word for word, in struct.iter_unpack("<I", data[0x1230:0x1704])
                     if word >> 26 == 3]
            self.assertEqual(len(calls), 18)
            self.assertEqual(len({word & 0x3FFFFFF for word in calls}), 14)
            flag_reads = [pc for pc in range(0x1230, 0x1704, 4)
                          if struct.unpack_from("<I", data, pc)[0] & 0xFFE0FFFF == 0x8FA000D4]
            self.assertEqual(flag_reads, [])
        self.assertNotIn(0x1230, self.local_call_targets)

    def test_target_compiled_ring_context_and_packet_layouts(self):
        from tools.project.build_baseline import compile_c, load_compiler_profiles, tool

        root = inventory.ROOT
        if not (root / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc").is_file():
            self.skipTest("Local GCC2.8.1 toolchain required for target layout checks")
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required for target layout checks")
        from elftools.elf.elffile import ELFFile
        expressions = [
            "sizeof(Variant439EntryScreenRing)",
            *(f"OFF(Variant439EntryScreenRing, {field})"
              for field in ("a", "b", "c", "inner", "outer", "scale", "count")),
            *(f"OFF(Variant439EntryState, {field})"
              for field in ("screen_rings", "extra", "matrix.t[0]", "matrix.t[1]",
                            "matrix.t[2]", "step", "phase")),
            "sizeof(POLY_GT4)",
            *(f"OFF(POLY_GT4, {field})"
              for field in ("x0", "x1", "x2", "x3", "u0", "u1", "u2", "u3", "tpage")),
        ]
        expected = [424, 0, 0x88, 0x110, 0x198, 0x19C, 0x1A0, 0x1A4,
                    0xE08, 0x1808, 0x18F8, 0x18FC, 0x1900, 0x1944, 0x197C,
                    52, 8, 20, 32, 44, 12, 24, 36, 48, 26]
        directory = root / "tmp/model439-ring-layout-test"
        directory.mkdir(parents=True, exist_ok=True)
        source = directory / "layout.c"
        source.write_text(
            '#include "../../src/types.h"\n'
            '#include "../../src/overlays/french_model_variant/variant439_entry.h"\n'
            '#define OFF(T, field) ((u32)&((T *)0)->field)\n'
            'const u32 ring_layout[] = {\n' + ",\n".join(expressions) + "\n};\n")
        obj = compile_c(root, tool(root, "as"),
                        {"source": source.relative_to(root).as_posix(),
                         "object": "layout.o", "profile": "gcc_2_8_1_g0_split"},
                        load_compiler_profiles(root),
                        object_directory=directory.relative_to(root).as_posix(),
                        asm_directory=directory.relative_to(root).as_posix())
        with obj.open("rb") as handle:
            elf = ELFFile(handle)
            section = elf.get_section_by_name(".rodata")
            self.assertEqual(section["sh_flags"], 2)
            self.assertEqual(section.data(), struct.pack("<25I", *expected))
        self.assertEqual(0xE08 + 3 * 424, 0x1300)
        self.assertEqual(0x1808 + 52, 0x183C)

    def test_selected_c_calls_and_complete_raw_owners_when_built(self):
        root = inventory.ROOT
        for module in self.modules:
            if not (root / f"tmp/overlays/{module['name']}/build/{module['name']}.elf").is_file():
                self.skipTest("Build Spanish MODEL439 images before checking real owners")
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required for ELF ownership checks")
        from elftools.elf.elffile import ELFFile

        with (self.config / "functions.csv").open() as handle:
            resident = {int(row["address"], 0) for row in csv.DictReader(handle)}
        for module, _, image in self.legal_images():
            base = int(module["load_address"], 0)
            directory = root / f"tmp/overlays/{module['name']}"
            self.assertEqual((root / module["output"]).read_bytes(), image)
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            selected = c_segments(root, root / module["layout"])
            self.assertEqual(len(selected), 6)
            script = (directory / f"{module['name']}.ld").read_text()
            external = set()
            with (directory / f"build/{module['name']}.elf").open("rb") as final_handle:
                final = ELFFile(final_handle)
                final_symbols = final.get_section_by_name(".symtab")
                for (offset, size, _, _), segment in zip(self.helpers, selected, strict=True):
                    obj = directory / "build" / segment["object"]
                    self.assertIn(f"{obj.relative_to(root)}(.text);", script)
                    with obj.open("rb") as source_handle:
                        source = ELFFile(source_handle)
                        source_symbols = source.get_section_by_name(".symtab")
                        name = f"func_{base + offset:X}"
                        own, = source_symbols.get_symbol_by_name(name)
                        definition, = final_symbols.get_symbol_by_name(name)
                        for symbol, elf, address in ((own, source, 0), (definition, final, base + offset)):
                            self.assertIsInstance(symbol["st_shndx"], int)
                            self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                             (address, size, "STT_FUNC"))
                            self.assertTrue(elf.get_section(symbol["st_shndx"])["sh_flags"] & 4)
                        section = final.get_section(definition["st_shndx"])
                        start = definition["st_value"] - section["sh_addr"]
                        self.assertEqual(section.data()[start:start + size], image[offset:offset + size])
                        text = source.get_section(own["st_shndx"]).data()
                        self.assertEqual(len(text), size)
                        relocations = source.get_section_by_name(".rel.text")
                        self.assertIsNotNone(relocations)
                        for relocation in relocations.iter_relocations():
                            if relocation["r_info_type"] != 4:
                                continue
                            target = source_symbols.get_symbol(relocation["r_info_sym"])
                            site = relocation["r_offset"]
                            addend = (struct.unpack_from("<I", text, site)[0] & 0x3FFFFFF) << 2
                            word = struct.unpack_from("<I", image, offset + site)[0]
                            address = ((base + offset + site + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                            if target["st_shndx"] != "SHN_UNDEF":
                                self.assertEqual(target["st_shndx"], own["st_shndx"])
                                self.assertEqual(address, base + offset + target["st_value"] + addend)
                                self.assertTrue(base + offset <= address < base + offset + size)
                                continue
                            resolved, = final_symbols.get_symbol_by_name(target.name)
                            self.assertEqual(address, resolved["st_value"] + addend, target.name)
                            if base <= address < base + 20480:
                                self.assertEqual(offset, 4)
                                self.assertIn(address - base, self.local_call_targets)
                            else:
                                self.assertIn(address, resident)
                                external.add(address)
                bindings = (root / module["linker_symbols"]).read_text()
                self.assertEqual(external, {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)})
                self.assertEqual(len(external), 36)
                for offset, size, stem in ((0, 4, "module_header"), (0x2C90, 9072, "unclassified_tail")):
                    obj = directory / f"build/tmp/overlays/{module['name']}/asm/data/overlays/{module['name']}/{stem}.data.o"
                    self.assertIn(obj.relative_to(root).as_posix(), script)
                    with obj.open("rb") as raw_handle:
                        raw_elf = ELFFile(raw_handle)
                        for elf, address in ((raw_elf, 0), (final, base + offset)):
                            raw, = elf.get_section_by_name(".symtab").get_symbol_by_name(f"D_{base + offset:X}")
                            self.assertIsInstance(raw["st_shndx"], int)
                            self.assertEqual((raw["st_value"], raw["st_info"]["type"], raw["st_size"]),
                                             (address, "STT_OBJECT", size))
                            section = elf.get_section(raw["st_shndx"])
                            self.assertFalse(section["sh_flags"] & 4)
                            start = raw["st_value"] - section["sh_addr"]
                            self.assertEqual(section.data()[start:start + size], image[offset:offset + size])

    def test_resident_callee_input_and_final_definitions_when_built(self):
        root = inventory.ROOT
        linked = root / "tmp/project-build/SLES_039.51.elf"
        if not linked.is_file() or not (root / "game/spain/SLES_039.51").is_file():
            self.skipTest("Build legal Spanish resident before checking real callee owners")
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required for ELF ownership checks")
        from elftools.elf.elffile import ELFFile

        retail = (root / "game/spain/SLES_039.51").read_bytes()
        self.assertEqual(hashlib.sha256(retail).hexdigest(),
                         "b0fefd88b6510f49af4f01e6180e40371652b7ceaa5f31dcb938c942316fc790")
        self.assertEqual(linked.with_suffix("").read_bytes(), retail)
        directory = root / "tmp/splat/sles_03951"
        script = (directory / "sles_03951.ld").read_text()
        bindings = (root / self.modules[0]["linker_symbols"]).read_text()
        addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
        self.assertEqual(len(addresses), 36)
        with (self.config / "functions.csv").open() as handle:
            functions = {int(row["address"], 0): row for row in csv.DictReader(handle)}
        matching = {int(row["address"], 0): row for row in json.loads(
            (self.config / "matching_c.json").read_text())["functions"]}
        with linked.open("rb") as handle:
            final = ELFFile(handle)
            for address in addresses:
                row = functions[address]
                size = int(row["size"], 0)
                if address in matching:
                    obj = directory / "build" / (matching[address]["source"][:-2] + ".o")
                else:
                    self.assertTrue(address == 0x8005C018 or address >= 0x80073C4C)
                    start = 0x8005C018 if address == 0x8005C018 else 0x80073C4C
                    obj = directory / f"build/tmp/splat/sles_03951/asm/generated/spanish_{start:08x}.o"
                self.assertIn(obj.relative_to(root).as_posix(), script)
                with obj.open("rb") as source_handle:
                    source = ELFFile(source_handle)
                    own, = source.get_section_by_name(".symtab").get_symbol_by_name(row["name"])
                    self.assertIsInstance(own["st_shndx"], int)
                    self.assertEqual((own["st_info"]["type"], own["st_size"]), ("STT_FUNC", size))
                    self.assertTrue(source.get_section(own["st_shndx"])["sh_flags"] & 4)
                definition, = final.get_section_by_name(".symtab").get_symbol_by_name(row["name"])
                self.assertIsInstance(definition["st_shndx"], int)
                self.assertEqual((definition["st_value"], definition["st_size"], definition["st_info"]["type"]),
                                 (address, size, "STT_FUNC"))
                section = final.get_section(definition["st_shndx"])
                self.assertTrue(section["sh_flags"] & 4)
                start = address - section["sh_addr"]
                offset = 0x800 + address - 0x80010000
                self.assertEqual(section.data()[start:start + size], retail[offset:offset + size])

    def test_missing_spanish_archive_skips_before_open(self):
        with patch.object(Path, "exists", return_value=False), patch.object(
                Path, "open", side_effect=AssertionError("missing archive must not be opened")):
            with self.assertRaisesRegex(SkipTest, "legal Spanish MODEL input required"):
                self.test_actual_loader_alternates_commands_and_nonzero_divisors()
