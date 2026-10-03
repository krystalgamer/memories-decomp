import csv
import hashlib
import importlib.util
from pathlib import Path
import re
import struct
import tempfile

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant412Tests(family435.FrenchModelVariant435Tests):
    family = 412
    module_count = 2
    distinct_images = 2
    binding_count = 30
    tail_start = 0x12C4
    spans = ((4, 0xB98), (0xB98, 0x12C4))
    helpers = ((0xB98, 1836, "trails", "func_8013BB98"),)
    reachable_helpers = {0xB98}
    local_call_targets = {0xB98}
    models_by_stage = ((7, (10,)),)
    entry_anchors = {
        0xA0: 0x00181940, 0xA8: 0xAFC31B24,
        0x4D0: 0x24A205D0, 0x4F4: 0x2A42001E,
        0x500: 0xA0660C98, 0x50C: 0xA0660F80,
        0x51C: 0x254A007C, 0x524: 0x2A020006, 0x52C: 0x252900F8,
        0x53C: 0xAC600C1C, 0x540: 0xAC600BA0, 0x548: 0x2A42001E,
        0x55C: 0x27181268, 0xA14: 0x02402021, 0xB98: 0x27BDF8F0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant412_trails_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013BB98 func_8017BB98\n'
                         '#include "variant412_trails.c"\n')
        body = (directory / "variant412_trails.c").read_text()
        self.assertIn('#include "variant412_trails.h"', body)
        self.assertNotRegex(body, r"\b(?:extern|asm|__asm__|register)\b")
        path = family435.ROOT / "notes/overlays/french-model-variant412-attempts.csv"
        with path.open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 38)
        expected = ((1780, 388), (1788, 389), (1800, 354), (1800, 354),
                    (1800, 353), (1856, 368), (1840, 290), (1856, 366),
                    (1840, 291), (1836, 13), (1836, 13), (1836, 6),
                    (1836, 3), (1836, 3), (1836, 3), (1836, 1),
                    (1836, 0), (1836, 0))
        self.assertEqual([(int(row["instruction_bytes"]), int(row["different_words"]))
                          for row in rows[:36]],
                         [pair for pair in expected for _ in range(2)])
        self.assertEqual([row["result"] for row in rows],
                         ["mismatch"] * 32 + ["text_exact"] * 4 + ["matched"] * 2)
        for slot, row in enumerate(rows[-2:]):
            source = directory / ("variant412_trails" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["function_offset"], row["slot"], row["profile"],
                              row["instruction_bytes"], row["different_words"]),
                             ("0xB98", str(slot), "gcc_2_8_1_g0_split", "1836", "0"))

    def test_direct_calls_timing_and_guest_context_separation(self):
        root = family435.ROOT
        archive_path = root / "game/france/DATA/MODEL.MRG"
        resident_path = root / "game/france/SLES_039.48"
        if not archive_path.exists() or not resident_path.exists():
            self.skipTest("legal French MODEL and resident inputs required")
        resident = resident_path.read_bytes()
        self.assertEqual(hashlib.sha256(resident).hexdigest(),
                         "57ecdfb9a9e1faf8b342fe7c7304c23723810861f3ab2fa3bef9eb27b5146b44")
        pointers = struct.unpack_from("<14I", resident, 0x800)
        with (root / "config/sles_03948/functions.csv").open() as handle:
            starts = {int(row["address"], 0) for row in csv.DictReader(handle)}
        with archive_path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, base = int(row["slot"]), int(module["load_address"], 0)
                self.assertEqual(int(row["command_word"]), 578000)
                self.assertEqual(pointers[5 + slot], base)
                context = pointers[9 + slot]
                self.assertEqual(context, 0x80136000 + slot * 0x40000)
                self.assertLess(context + 0x1B68, 1 << 32)
                for start, size in ((pointers[slot], 96 * 2048),
                                    (pointers[3 + slot], 2 * 2048),
                                    (base, 10 * 2048)):
                    self.assertTrue(context + 0x1B68 <= start or start + size <= context)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                call = struct.unpack_from("<I", data, 0xA10)[0]
                self.assertEqual(call >> 26, 3)
                self.assertEqual(0x80000000 | ((call & 0x3FFFFFF) << 2), base + 0xB98)
                high, low = struct.unpack_from("<II", data, 0x90)
                self.assertEqual(high >> 16, 0x3C02)
                self.assertEqual(low >> 16, 0x2442)
                immediate = low & 0xFFFF
                self.assertEqual(((high & 0xFFFF) << 16) +
                                 (immediate - 0x10000 if immediate & 0x8000 else immediate),
                                 base + 0x28A4)
                self.assertEqual(struct.unpack_from("<3I", data, 0x28A4 + 20),
                                 (100, 140, 118))
                external = set()
                for start, end in self.spans:
                    for word, in struct.iter_unpack("<I", data[start:end]):
                        if word >> 26 == 3:
                            target = 0x80000000 | ((word & 0x3FFFFFF) << 2)
                            if not base <= target < base + 20480:
                                external.add(target)
                bindings = (root / module["linker_symbols"]).read_text()
                declared = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                self.assertEqual(external, declared)
                self.assertTrue(external <= starts)
                self.assertIn("GsGetLw = 0x8008A428;", bindings)

    def test_target_compiler_record_layout(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("pyelftools required for target layouts")
        from tools.project.build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        root = family435.ROOT
        profiles = load_compiler_profiles(root)
        profile = "gcc_2_8_1_g0_split"
        if not (root / profiles[profile]["compiler"]).exists():
            self.skipTest("local target compiler required")
        with tempfile.TemporaryDirectory(prefix="variant412-layout-", dir=root / "tmp") as temporary:
            directory = Path(temporary)
            source = directory / "layout.c"
            source.write_text(
                '#include "../../src/types.h"\n'
                '#include "../../src/overlays/french_model_variant/variant412_trails.h"\n'
                '#define OFFSET(type, member) ((u32)&((type *)0)->member)\n'
                'const u32 trail_layout[] = {sizeof(void *), sizeof(u32), sizeof(SVECTOR), '
                'sizeof(CVECTOR), sizeof(POLY_G4), sizeof(Variant412Trail), '
                'OFFSET(Variant412Trail, original), OFFSET(Variant412Trail, moved), '
                'OFFSET(Variant412Trail, angle), OFFSET(Variant412Trail, progress), '
                'OFFSET(Variant412Trail, inner), OFFSET(Variant412Trail, outer), '
                'sizeof(((Variant412Trail *)0)->original[0]), '
                'sizeof(((Variant412Trail *)0)->inner[0]), sizeof(Variant412Timing), '
                'OFFSET(Variant412Timing, capture_start), OFFSET(Variant412Timing, capture_stop), '
                'sizeof(Variant412TimingLink), OFFSET(Variant412TimingLink, timing), '
                'sizeof(Variant412Links), OFFSET(Variant412Links, part)};\n')
            obj = compile_c(root, tool(root, "as"), dict(
                source=str(source.relative_to(root)), object="layout.o", profile=profile),
                profiles, object_directory=str(directory.relative_to(root)),
                asm_directory=str((directory / "asm").relative_to(root)))
            with obj.open("rb") as handle:
                elf = ELFFile(handle)
                symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("trail_layout")
                section = elf.get_section(symbol["st_shndx"])
                self.assertEqual((section.name, symbol["st_value"]), (".rodata", 0))
                self.assertIn(symbol["st_size"], (0, 84))
                self.assertEqual(len(section.data()), 84)
                self.assertEqual(struct.unpack("<21I", section.data()),
                                 (4, 4, 8, 4, 36, 4712, 0, 1488, 2976, 3100, 3224,
                                  3968, 248, 124, 28, 20, 24, 4, 0, 24, 0))
