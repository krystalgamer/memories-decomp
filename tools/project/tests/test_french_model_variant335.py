import csv
import hashlib
import importlib.util
from pathlib import Path
import re
import struct
import tempfile

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant335Tests(family435.FrenchModelVariant435Tests):
    family = 335
    module_count = 4
    distinct_images = 4
    binding_count = 34
    tail_start = 0x28EC
    spans = ((4, 0xB98), (0xB98, 0x1534), (0x1534, 0x1BC4),
             (0x1BC4, 0x20D8), (0x20D8, 0x28EC))
    helpers = ((0x1534, 1680, "sheets", "func_8013C534"),
               (0x1BC4, 1300, "rings", "func_8013CBC4"))
    reachable_helpers = {0x1534, 0x1BC4}
    local_call_targets = {0xB98, 0x1534, 0x1BC4, 0x20D8}
    models_by_stage = ((7, (44, 558)),)
    entry_anchors = {
        0xC: 0x00809021, 0x14: 0x0240B021,
        0x20: 0x26D80D1C, 0x620: 0x271401A4, 0x6D0: 0x2A220011,
        0x708: 0x271801A8, 0x710: 0xAE800000, 0x720: 0xAE82FFFC,
        0x724: 0x2AA20003, 0x72C: 0x269401A8, 0x1BC4: 0x27BDFEF0,
        0x18: 0x26D80ABC, 0x4A4: 0x27100020, 0x4A8: 0x270F0040,
        0x5FC: 0x24840098, 0x604: 0x2AA20004, 0x608: 0x27180098,
        0x1534: 0x27BDFEF0, 0x1544: 0x265E0ABC,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        for label, address in (("rings", 0x8013CBC4), ("sheets", 0x8013C534)):
            self.assertEqual((directory / f"variant335_{label}_slot1.c").read_text(),
                             '#include "../../types.h"\n'
                             f'#define func_{address:X} func_{address + 0x40000:X}\n'
                             f'#include "variant335_{label}.c"\n')
            self.assertNotRegex((directory / f"variant335_{label}.c").read_text(),
                                r"\b(?:extern|asm|__asm__)\b")
        body = (directory / "variant335_rings.c").read_text()
        self.assertIn('#include "variant335_rings.h"', body)
        path = family435.ROOT / "notes/overlays/french-model-variant335-attempts.csv"
        with path.open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 20)
        sheets = [row for row in rows if row["function_offset"] == "0x1534"]
        rows = [row for row in rows if row["function_offset"] == "0x1BC4"]
        self.assertEqual(len(rows), 10)
        self.assertEqual([(row["instruction_bytes"], row["different_words"])
                          for row in rows[:8]],
                         [("1300", "34")] * 2 + [("1300", "23")] * 2
                         + [("1324", "298")] * 2 + [("1300", "0")] * 2)
        self.assertEqual([row["result"] for row in rows],
                         ["mismatch"] * 6 + ["text_exact"] * 2 + ["matched"] * 2)
        for slot, row in enumerate(rows[-2:]):
            source = directory / ("variant335_rings" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["function_offset"], row["slot"], row["profile"],
                              row["instruction_bytes"], row["different_words"]),
                             ("0x1BC4", str(slot), "gcc_2_8_1_g0_split", "1300", "0"))
        self.assertEqual([row["result"] for row in sheets],
                         ["mismatch"] * 6 + ["text_exact"] * 2 + ["matched"] * 2)
        self.assertEqual([(row["instruction_bytes"], row["different_words"])
                          for row in sheets[:8]],
                         [("1680", "12")] * 4 + [("1680", "95")] * 2 + [("1680", "0")] * 2)
        for slot, row in enumerate(sheets[-2:]):
            source = directory / ("variant335_sheets" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["function_offset"], row["slot"], row["profile"],
                              row["instruction_bytes"], row["different_words"]),
                             ("0x1534", str(slot), "gcc_2_8_1_g0_split", "1680", "0"))

    def test_direct_calls_and_context_separation(self):
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
                self.assertEqual(int(row["command_word"]), 501000)
                self.assertEqual(pointers[5 + slot], base)
                context = pointers[9 + slot]
                self.assertEqual(context, 0x80136000 + slot * 0x40000)
                for start, size in ((pointers[slot], 96 * 2048),
                                    (pointers[3 + slot], 2 * 2048),
                                    (base, 10 * 2048)):
                    self.assertTrue(context + 0x2EA4 <= start or start + size <= context)
                self.assertEqual(0xD1C + 3 * 424, 0x1214)
                self.assertEqual(0xABC + 4 * 152, 0xD1C)
                self.assertEqual(0x2CB0 + 52, 0x2CE4)
                self.assertLessEqual(0x2CE4 + 52, 0x2E18)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                for caller, target in ((0x9F8, 0x1534), (0xA1C, 0x1BC4)):
                    call = struct.unpack_from("<I", data, caller)[0]
                    self.assertEqual(call >> 26, 3)
                    self.assertEqual(0x80000000 | ((call & 0x3FFFFFF) << 2), base + target)
                    self.assertEqual(struct.unpack_from("<I", data, caller + 4)[0], 0x02402021)
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
        with tempfile.TemporaryDirectory(prefix="variant335-layout-", dir=root / "tmp") as temporary:
            directory = Path(temporary)
            source = directory / "layout.c"
            source.write_text(
                '#include "../../src/types.h"\n'
                '#include "../../src/overlays/french_model_variant/variant335_rings.h"\n'
                '#define OFFSET(member) ((u32)&((Variant335Ring *)0)->member)\n'
                '#define SHEET_OFFSET(member) ((u32)&((ModelVariantSheet *)0)->member)\n'
                'const u32 ring_layout[] = {sizeof(Variant335Ring), sizeof(SVECTOR), '
                'sizeof(POLY_GT4), OFFSET(a), OFFSET(b), OFFSET(c), OFFSET(inner), '
                'OFFSET(outer), OFFSET(scale), OFFSET(cycles), '
                'sizeof(ModelVariantSheet), sizeof(SVECTOR), sizeof(VECTOR), sizeof(MATRIX), '
                'sizeof(GsCOORDINATE2), sizeof(PSXLONG), sizeof(POLY_GT4), '
                'SHEET_OFFSET(v0), SHEET_OFFSET(v1), SHEET_OFFSET(v2), SHEET_OFFSET(v3), '
                'SHEET_OFFSET(outer), SHEET_OFFSET(inner), SHEET_OFFSET(size)};\n')
            obj = compile_c(root, tool(root, "as"), dict(
                source=str(source.relative_to(root)), object="layout.o", profile=profile),
                profiles, object_directory=str(directory.relative_to(root)),
                asm_directory=str((directory / "asm").relative_to(root)))
            with obj.open("rb") as handle:
                elf = ELFFile(handle)
                symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("ring_layout")
                section = elf.get_section(symbol["st_shndx"])
                self.assertEqual((section.name, symbol["st_value"]), (".rodata", 0))
                self.assertIn(symbol["st_size"], (0, 96))
                self.assertEqual(len(section.data()), 96)
                self.assertEqual(struct.unpack("<24I", section.data()),
                                 (424, 8, 52, 0, 136, 272, 408, 412, 416, 420,
                                  152, 8, 16, 32, 80, 4, 52, 0, 32, 64, 96, 128, 132, 136))
