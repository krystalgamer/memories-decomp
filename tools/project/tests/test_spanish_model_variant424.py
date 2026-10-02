import csv
import hashlib
import struct

from tools.project.tests import test_french_model_variant424 as layout
from tools.project.tests import test_french_model_variant435 as inventory
from tools.project.tests import test_spanish_model_variant460 as instructions
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant424Tests(inventory.FrenchModelVariant435Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    family = 424
    source_family = 407
    module_count = 30
    distinct_images = 29
    binding_count = 33
    tail_start = 0x18B4
    spans = ((4, 0x12D8), (0x12D8, 0x18B4))
    helpers = ((0x12D8, 1500, "petals", "func_8013C2DC"),)
    reachable_helpers = {0x12D8}
    local_call_targets = {0x12D8}
    models_by_stage = layout.FrenchModelVariant424Tests.models_by_stage
    entry_anchors = layout.FrenchModelVariant424Tests.entry_anchors
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
                yield module, data

    def test_wrappers_only_rename_verified_functions(self):
        directory = inventory.ROOT / "src/overlays/french_model_variant"
        shared = inventory.ROOT / "src/overlays/model_variant/variant407_petals.c"
        with (inventory.ROOT / "notes/overlays/spanish-model-variant424-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 30)
        self.assertEqual({row["module"] for row in rows}, {module["name"] for module in self.modules})
        for row in rows:
            slot = int(row["slot"])
            self.assertEqual(row["slot"], self.instances[row["module"]]["slot"])
            source = directory / ("variant424_petals" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(source.read_text(), '#include "../../types.h"\n#define VERSION_FRENCH\n'
                             f"#define func_8013C2DC func_{0x8013C2D8 + slot * 0x40000:X}\n"
                             '#include "../model_variant/variant407_petals.c"\n')
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual(row["shared_source_fingerprint"], hashlib.sha256(shared.read_bytes()).hexdigest())
            self.assertEqual((row["function_offset"], row["result"], row["different_words"], row["profile"]),
                             ("0x12D8", "matched", "0", "gcc_2_8_1_g0_split"))
            self.assertEqual(row["instruction_bytes"], "1500")

    def test_original_context_reaches_delay_slot_dispatch(self):
        for module, data in self.legal_images():
            base = int(module["load_address"], 0)
            word = lambda pc: struct.unpack_from("<I", data, pc)[0]
            self.assertEqual(word(0xC), 0x00809021)
            self.assertEqual(word(0x14), 0x0240B021)
            self.assertEqual(word(0x1170), 0x0C000000 | ((base + 0x12D8) >> 2 & 0x3FFFFFF))
            self.assertEqual(word(0x1174), 0x02402021)
            definitions = {r: set(self.register_writes(data, 4, 0x12D8, r)) for r in (4, 18)}
            pending, visited, reaching = [(4, None, None)], set(), set()
            while pending:
                pc, root, arg = pending.pop()
                self.assertTrue(4 <= pc < 0x12D8 and pc % 4 == 0)
                if (pc, root, arg) in visited:
                    continue
                visited.add((pc, root, arg))
                if pc in definitions[18]:
                    root = pc
                if pc in definitions[4]:
                    arg = pc
                instruction = word(pc)
                op = instruction >> 26
                if op == 0 and instruction & 63 in (8, 9):
                    self.assertEqual(instruction, 0x03E00008)
                if op in (1, 2, 3, 4, 5, 6, 7) or instruction == 0x03E00008:
                    if pc + 4 in definitions[18]:
                        root = pc + 4
                    if pc + 4 in definitions[4]:
                        arg = pc + 4
                    if pc == 0x1170:
                        reaching.add((root, arg))
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
            self.assertEqual(reaching, {(0xC, 0x1174)})
            self.assertEqual(sum(word(pc) >> 26 == 3 for pc in range(4, 0x12D8, 4)), 74)
            self.assertEqual(sum(word(pc) >> 26 == 3 for pc in range(0x12D8, 0x18B4, 4)), 16)

    def test_complete_preserved_register_write_sets(self):
        rows = (
            (4, 0x12D8, (
                [0x1D0, 0x1D4, 0x278, 0x31C, 0x6C4, 0x7B8, 0xA70, 0xC50, 0xCA8, 0xD4C, 0xDAC, 0xFCC, 0x10A4, 0x10EC, 0x1110, 0x1190, 0x12CC],
                [0x5C, 0x254, 0x698, 0x6FC, 0xA0C, 0xAD0, 0xC54, 0xCA0, 0xD50, 0xDA4, 0xFD0, 0x10BC, 0x10D8, 0x1114, 0x12C8],
                [0xC, 0x1A4, 0x2CC, 0x68C, 0x78C, 0xA10, 0xAAC, 0xC4C, 0xD38, 0xD48, 0xE0C, 0x12C4],
                [0x1BC, 0x5B4, 0x63C, 0x690, 0x6F8, 0x998, 0xA08, 0xA68, 0xB10, 0xBC8, 0xBF4, 0xC48, 0xCE4, 0xD44, 0xE10, 0x12C0],
                [0x234, 0x694, 0x6F4, 0x990, 0xBB0, 0x12BC],
                [0x230, 0x684, 0x9FC, 0x12B8], [0x14, 0x12B4],
                [0x4C, 0x2B4, 0x67C, 0x9C8, 0x12B0], [4, 0x12D4],
                [0x54, 0x358, 0x994, 0xB00, 0xC40, 0xCE0, 0xD3C, 0xDD4, 0x12AC])),
            (0x12D8, 0x18B4, (
                [0x133C, 0x14A4, 0x1880, 0x18A8], [0x12E0, 0x18A4],
                [0x1460, 0x14CC, 0x18A0], [0x12E8, 0x189C], [0x1494, 0x1898],
                [0x142C, 0x1894], [0x12F0, 0x1890], [0x1424, 0x188C],
                [0x12D8, 0x18B0], [0x1478, 0x1888])),
        )
        for _, data in self.legal_images():
            for start, end, sets in rows:
                for register, expected in zip((*range(16, 24), 29, 30), sets, strict=True):
                    self.assertEqual(self.register_writes(data, start, end, register), expected)

    def test_context_spills_frames_and_stable_packet(self):
        anchors = {
            4: 0x27BDFF08, 0x8C: 0xAFA500FC, 0x94: 0xAFB60080,
            0x9C: 0x04A00375, 0xB8: 0x8FB800FC, 0x12D4: 0x27BD00F8,
            0x12D8: 0x27BDFEB0, 0x12E0: 0x00808821, 0x12E8: 0x02209821,
            0x12F0: 0x26362320, 0x1318: 0x27A90070, 0x132C: 0xAFA90120,
            0x1344: 0xAFA200E8, 0x15D0: 0xAFA000D8, 0x15D8: 0xAFA00090,
            0x1650: 0x27A200E0, 0x1658: 0x27A200E4, 0x18B0: 0x27BD0150,
        }
        for _, data in self.legal_images():
            for pc, expected in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], expected, hex(pc))
            for start, end, spill, expected in (
                    (4, 0x12D8, 128, [0x94]), (4, 0x12D8, 252, [0x8C]),
                    (0x12D8, 0x18B4, 232, [0x1344]), (0x12D8, 0x18B4, 288, [0x132C])):
                overlaps = [pc for pc, offset, size in self.direct_stores(data, start, end, 29)
                            if offset < spill + 4 and spill < offset + size]
                self.assertEqual(overlaps, expected)
            self.assertEqual(self.direct_stores(data, 0x12D8, 0x18B4, 22),
                             [(0x1674, 4, 1), (0x1680, 5, 1), (0x1690, 6, 1)])
        self.assertEqual(248 + 4, 252)
        self.assertEqual(0x2320 + 36, 0x2344)
        self.assertEqual(144 + 80, 224)
        self.assertEqual(112 + 32, 144)

    def test_projection_gates_and_unsigned_timing_transitions(self):
        anchors = {
            0x1360: 0x97A800F0, 0x1368: 0x00081400, 0x136C: 0x00021383,
            0x1374: 0x8C440794, 0x137C: 0x188000D0, 0x1380: 0x28820201,
            0x1424: 0x0002BC03, 0x1428: 0x00171080, 0x14A0: 0x00171900,
            0x14A4: 0x02238021, 0x161C: 0x02672021, 0x1624: 0x02652821,
            0x162C: 0x02663021, 0x1660: 0x02673821,
            0x1664: 0x0C021E56, 0x1688: 0x00403021, 0x1694: 0x8FA200E4,
            0x16A4: 0x8EA20854, 0x16AC: 0x14400004, 0x16B0: 0x02C02021,
            0x16B4: 0x8FA500E8, 0x16B8: 0x0C0210AA, 0x16BC: 0x30C6FFFF,
            0x16D4: 0x8CA40794, 0x16DC: 0x28820400, 0x16E0: 0x10400038,
            0x16E8: 0x8E2229D4, 0x16F4: 0x8E2229CC, 0x16FC: 0x00620018,
            0x170C: 0xACA20794, 0x1710: 0x8E620794, 0x1718: 0x28420200,
            0x1724: 0x8E2229FC, 0x172C: 0x14400002, 0x1730: 0x24020002,
            0x1734: 0xAE2229FC, 0x1754: 0x28A20400, 0x176C: 0x8E2229C4,
            0x1774: 0x0043102B, 0x1778: 0x1040000E, 0x177C: 0x24A2FC00,
            0x1780: 0xAC820794, 0x178C: 0x1CA0000D, 0x179C: 0x8C430028,
            0x17A0: 0x8E2229C4, 0x17A8: 0x0043102B, 0x17AC: 0x14400005,
            0x17B4: 0x24020400, 0x17B8: 0xAC820794, 0x17BC: 0x24020001,
            0x17C0: 0xAC820854, 0x17DC: 0x8C420854, 0x17E4: 0x01020018,
            0x17F4: 0x14620009, 0x1800: 0x15020006, 0x1808: 0x8E2329FC,
            0x1810: 0x14620002, 0x1814: 0x24020005, 0x1818: 0xAE2229FC,
            0x185C: 0xA7A500F0, 0x186C: 0x25080040, 0x187C: 0x1480FEB8,
        }
        for _, data in self.legal_images():
            for pc, expected in {**self.entry_anchors, **anchors}.items():
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], expected, hex(pc))

    def test_actual_commands_both_alternates_and_context_separation(self):
        resident = inventory.ROOT / "game/spain/SLES_039.51"
        if not resident.exists():
            self.skipTest("legal Spanish resident input required")
        pointers = struct.unpack_from("<14I", resident.read_bytes(), 0x800)
        self.assertEqual((pointers[9], pointers[10]), (0x80136000, 0x80176000))
        sectors = (96, 48, 2, 1, 16, 1, 16, 10, 10, 10, 10, 2, 2, 1, 50, 1)
        self.assertEqual(sum(sectors), 276)
        commands, stages = set(), set()
        for module, data in self.legal_images():
            row = self.instances[module["name"]]
            model, record, stage, slot = (int(row[key]) for key in ("model", "record", "stage", "slot"))
            self.assertEqual(record, model - 50 * (model >= 350))
            self.assertEqual(module["sector_offset"], record * 276 + sum(sectors[:stage]))
            alternate = (stage - 7) // 2
            self.assertIn(alternate, (0, 1))
            self.assertEqual(stage, 7 + 2 * alternate + slot)
            with (inventory.ROOT / module["archive"]).open("rb") as archive:
                archive.seek((record * 276 + 275) * 2048 + 0x110 + alternate * 4)
                command, = struct.unpack("<i", archive.read(4))
            self.assertEqual(command, int(row["command_word"]))
            descriptor = 0x19B0 + command % 1000 * 68
            self.assertGreaterEqual(descriptor, self.tail_start)
            self.assertLessEqual(descriptor + 68, len(data))
            base = int(module["load_address"], 0)
            table = base + 0x19B0
            self.assertEqual(struct.unpack_from("<I", data, 0xB0)[0], 0x3C020000 | ((table + 0x8000) >> 16 & 65535))
            self.assertEqual(struct.unpack_from("<I", data, 0xB4)[0], 0x24420000 | (table & 65535))
            widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
            accesses = [(word & 65535) + widths[word >> 26] for word in struct.unpack("<1205I", data[4:0x12D8])
                        if word >> 26 in widths and word >> 21 & 31 == 22 and not word & 0x8000]
            self.assertEqual(max(accesses), 0x2A10)
            for address, size in ((pointers[slot], 96 * 2048), (pointers[3 + slot], 4096), (base, 20480)):
                self.assertTrue(pointers[9 + slot] + 0x2A10 <= address or address + size <= pointers[9 + slot])
            commands.add(command)
            stages.add(stage)
        self.assertEqual(stages, {7, 8, 9, 10})
        self.assertEqual(commands, {590000, 590001, 590002, *range(590004, 590014)})

    def test_five_initialized_arrays_four_projected_arrays_and_48_lanes(self):
        anchors = {
            0x150: 0xAFA000B4, 0x434: 0x00C02821, 0x488: 0x24A30180,
            0x4BC: 0x24A20300, 0x4E8: 0x24A20480, 0x4F8: 0x8FB900B4,
            0x4FC: 0x8FB80080, 0x56C: 0x8FB800B4, 0x574: 0x27180001,
            0x57C: 0xAFB800B4, 0x580: 0xA4A20602, 0x594: 0xA4A20604,
            0x59C: 0x1440FFA6, 0xFCC: 0x02C08021, 0xFD0: 0x02C08821,
            0x10B8: 0x1440FFC6, 0x1324: 0xA7A000F0, 0x1824: 0x25250001,
            0x185C: 0xA7A500F0, 0x187C: 0x1480FEB8,
        }
        for _, data in self.legal_images():
            for pc, expected in {**self.entry_anchors, **anchors}.items():
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], expected, hex(pc))
            self.assertEqual(self.register_writes(data, 0x434, 0x5A4, 5), [0x434, 0x5A0])
            self.assertEqual(self.register_writes(data, 0x438, 0x5A4, 6), [0x58C])
            self.assertEqual(self.direct_stores(data, 0x438, 0x5A4, 6),
                             [(0x534, 0x854, 4), (0x538, 0x914, 4),
                              (0x53C, 0x9D4, 4), (0x550, 0x794, 4)])
            counter_writes = [pc for pc, offset, size in self.direct_stores(data, 0x150, 0x5A4, 29)
                              if offset < 184 and 180 < offset + size]
            self.assertEqual(counter_writes, [0x150, 0x57C])
            self.assertFalse(any(word >> 26 == 3 for word in struct.unpack("<91I", data[0x438:0x5A4])))
        self.assertEqual(48 * 8, 0x180)
        self.assertEqual(5 * 48 * 8, 0x780)
        self.assertEqual(0x794 + 4 * 48 * 4, 0xA94)
        self.assertEqual(0x23A8 + 48 * 16, 0x26A8)
        self.assertEqual(0x26A8 + 48 * 16, 0x29A8)
