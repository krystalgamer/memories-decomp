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
    helpers = ((4, 2964, "entry", "func_8013B004"),
               (0xB98, 2460, "ribbons", "func_8013BB98"),
               (0x1534, 1680, "sheets", "func_8013C534"),
               (0x1BC4, 1300, "rings", "func_8013CBC4"),
               (0x20D8, 2068, "streamers", "func_8013D0D8"))
    reachable_helpers = {0xB98, 0x1534, 0x1BC4, 0x20D8}
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
        0x444: 0x2703023A, 0x47C: 0x28820003, 0x484: 0x24630394,
        0x7EC: 0xA6C02E8C, 0xB98: 0x27BDFED0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        for label, address in (("rings", 0x8013CBC4), ("sheets", 0x8013C534),
                               ("ribbons", 0x8013BB98), ("streamers", 0x8013D0D8)):
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
        self.assertEqual(len(rows), 109)
        ribbons = [row for row in rows if row["function_offset"] == "0xB98"]
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
        self.assertEqual([row["result"] for row in ribbons],
                         ["mismatch"] * 24 + ["text_exact"] * 2 + ["matched"] * 2)
        expected = ((2444, 393), (2444, 388), (2472, 354), (2460, 125),
                    (2460, 112), (2460, 112), (2456, 276), (2456, 276),
                    (2460, 15), (2460, 9), (2460, 2), (2460, 6), (2460, 0))
        self.assertEqual([(row["instruction_bytes"], row["different_words"])
                          for row in ribbons[:26]],
                         [(str(size), str(words)) for size, words in expected for _ in range(2)])
        for slot, row in enumerate(ribbons[-2:]):
            source = directory / ("variant335_ribbons" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["function_offset"], row["slot"], row["profile"],
                              row["instruction_bytes"], row["different_words"]),
                             ("0xB98", str(slot), "gcc_2_8_1_g0_split", "2460", "0"))

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
                    self.assertTrue(context + 0x2EB4 <= start or start + size <= context)
                self.assertEqual(0xD1C + 3 * 424, 0x1214)
                self.assertEqual(0xABC + 4 * 152, 0xD1C)
                self.assertEqual(3 * 0x394, 0xABC)
                self.assertEqual(0x350 + 17 * 2, 0x372)
                self.assertEqual(0x372 + 17 * 2, 0x394)
                self.assertEqual(0x2CB0 + 52, 0x2CE4)
                self.assertEqual(0x2CE4 + 52, 0x2D18)
                self.assertEqual(0x2D18 + 2 * 40, 0x2D68)
                self.assertEqual(0x254C + 2 * 0x378, 0x2C3C)
                self.assertEqual(0x2D68 + 2 * 40, 0x2DB8)
                self.assertLessEqual(0x2D68, 0x2E18)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<I", data, 0x20D8)[0], 0x27BDFEB8)
                self.assertEqual(struct.unpack_from("<I", data, 0x28EC)[0], 0x27BDFED0)
                self.assertEqual(struct.unpack_from("<2I", data, 0x2FB4),
                                 (0x03E00008, 0x27BD0130))
                for caller, target in ((0x9F0, 0xB98), (0x9F8, 0x1534),
                                       (0xA14, 0x20D8), (0xA1C, 0x1BC4)):
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

    def test_streamer_attempts_and_branch_structure(self):
        root = family435.ROOT
        directory = root / "src/overlays/french_model_variant"
        body = (directory / "variant335_streamers.c").read_text()
        self.assertEqual(body.count("streamer->ox[k] ="), 2)
        self.assertEqual(body.count("streamer->oy[k] ="), 2)
        self.assertIn("sizeof(Variant321Streamer)", body)
        with (root / "notes/overlays/french-model-variant335-attempts.csv").open() as handle:
            rows = [row for row in csv.DictReader(handle) if row["function_offset"] == "0x20D8"]
        self.assertEqual(len(rows), 49)
        self.assertEqual(sum(row["result"] == "mismatch" for row in rows), 44)
        failure, = [row for row in rows if row["result"] == "compile_error"]
        self.assertEqual((failure["slot"], failure["instruction_bytes"], failure["different_words"]),
                         ("0", "", ""))
        self.assertIn("no candidate object or byte comparison", failure["reason"])
        self.assertEqual([row["result"] for row in rows[-4:]],
                         ["text_exact", "text_exact", "matched", "matched"])
        for slot, row in enumerate(rows[-2:]):
            source = directory / ("variant335_streamers" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["slot"], row["profile"], row["instruction_bytes"], row["different_words"]),
                             (str(slot), "gcc_2_8_1_g0_split", "2068", "0"))

    def test_entry_wrapper_bindings_and_attempts(self):
        root = family435.ROOT
        directory = root / "src/overlays/french_model_variant"
        renames = (
            ("func_8013B004", "func_8017B004"), ("func_8013BB98", "func_8017BB98"),
            ("func_8013C534", "func_8017C534"), ("func_8013CBC4", "func_8017CBC4"),
            ("func_8013D0D8", "func_8017D0D8"), ("D_8013D8EC", "D_8017D8EC"),
        )
        self.assertEqual((directory / "variant335_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         + "".join(f"#define {old} {new}\n" for old, new in renames)
                         + '#include "variant335_entry.c"\n')
        self.assertNotRegex((directory / "variant335_entry.c").read_text(),
                            r"\b(?:extern|asm|__asm__)\b")
        aliases = {"GetClut": 0x80082D28, "SetPolyFT4": 0x80082EA8,
                   "SetPolyG3": 0x80082E48, "SetPolyG4": 0x80082EC8,
                   "Square0": 0x80089BC8, "SquareRoot0": 0x80086DD8,
                   "GsGetLwUnit": 0x8008A428}
        paths = [root / "config/sles_03948/overlays/model_variant335_linker_symbols.txt"]
        paths.extend(root / "config/sles_03948/overlays" /
                     (module["name"].removeprefix("french_") + "_symbols.txt")
                     for module in self.modules)
        for path in paths:
            text = path.read_text()
            for name, address in aliases.items():
                self.assertIn(f"{name} = 0x{address:X};", text)
                self.assertNotIn(f"func_french_{address:X}", text)
        with (root / "notes/overlays/french-model-variant335-attempts.csv").open() as handle:
            rows = [row for row in csv.DictReader(handle) if row["function_offset"] == "0x4"]
        self.assertEqual([row["result"] for row in rows],
                         ["mismatch"] * 8 + ["text_exact"] * 2 + ["matched"] * 2)
        expected = ((3064, 707), (3064, 707), (3084, 706), (2968, 126), (2964, 0))
        self.assertEqual([(row["instruction_bytes"], row["different_words"]) for row in rows[:10]],
                         [(str(size), str(words)) for size, words in expected for _ in range(2)])
        self.assertIn("SDK alias calibration", rows[0]["reason"])
        for slot, row in enumerate(rows[-2:]):
            source = directory / ("variant335_entry" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["slot"], row["profile"], row["instruction_bytes"], row["different_words"]),
                             (str(slot), "gcc_2_8_1_g0_split", "2964", "0"))

    def test_target_compiler_entry_layout(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("pyelftools required for target layouts")
        from tools.project.build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        root = family435.ROOT
        profiles = load_compiler_profiles(root)
        profile = "gcc_2_8_1_g0_split"
        if not (root / profiles[profile]["compiler"]).exists():
            self.skipTest("local target compiler required")
        checks = {
            "sizeof(Model335EntryConfig)": 0x30, "sizeof(Model335EntryRecord)": 0x394,
            "sizeof(Model335EntrySheet)": 0x98, "sizeof(Variant335Ring)": 0x1A8,
            "sizeof(Model335EntryShortStreamer)": 0x334,
            "sizeof(Variant337EntryStreamer)": 0x378, "sizeof(Model335EntryState)": 0x2EB4,
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(PSXLONG)": 4, "sizeof(POLY_FT4)": 40,
        }
        members = {
            "Model335EntryConfig": {"part": 4, "start": 0xC, "end": 0x2C},
            "Model335EntryRecord": {"inner": 0x220, "outer": 0x224, "scale": 0x228,
                                    "field_238": 0x238, "field_23A": 0x23A, "field_23C": 0x23C},
            "Model335EntryShortStreamer": {"color": 0x264, "field_2A8": 0x2A8},
            "Model335EntryState": {
                "records": 0, "sheets": 0xABC, "rings": 0xD1C, "short_streamers": 0x1214,
                "streamers": 0x254C, "triangle": 0x2C3C, "quad": 0x2C58, "textured": 0x2C7C,
                "flat_textured": 0x2D18, "extra_textured": 0x2D68, "matrix": 0x2DC8,
                "origin": 0x2DE8, "end": 0x2DF8, "delta": 0x2E08, "target": 0x2E18,
                "direction": 0x2E20, "screen_delta": 0x2E30, "view_delta": 0x2E34,
                "angles": 0x2E44, "frame_count": 0x2E4C, "frame": 0x2E50,
                "animation_frame": 0x2E54, "step": 0x2E58, "fade": 0x2E5C, "config": 0x2E60,
                "part": 0x2E68, "field_2E6C": 0x2E6C, "field_2E70": 0x2E70,
                "field_2E74": 0x2E74, "field_2E78": 0x2E78, "rotation": 0x2E7C,
                "extent": 0x2E88, "count": 0x2E8C, "width": 0x2E92, "wave0": 0x2E98,
                "wave1": 0x2E9C, "phase": 0x2EA0, "brightness": 0x2EAC,
                "slot": 0x2EB0, "command": 0x2EB2,
            },
        }
        for typename, fields in members.items():
            for member, offset in fields.items():
                checks[f"(u32)&(({typename} *)0)->{member}"] = offset
        self.assertEqual(len(checks), 63)
        with tempfile.TemporaryDirectory(prefix="variant335-entry-layout-", dir=root / "tmp") as temporary:
            directory = Path(temporary)
            source = directory / "layout.c"
            source.write_text(
                '#include "../../src/types.h"\n'
                '#include "../../src/overlays/french_model_variant/variant335_entry.h"\n'
                "const u32 entry_layout[] = {" + ", ".join(checks) + "};\n")
            obj = compile_c(root, tool(root, "as"), dict(
                source=str(source.relative_to(root)), object="layout.o", profile=profile),
                profiles, object_directory=str(directory.relative_to(root)),
                asm_directory=str((directory / "asm").relative_to(root)))
            with obj.open("rb") as handle:
                elf = ELFFile(handle)
                symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("entry_layout")
                section = elf.get_section(symbol["st_shndx"])
                self.assertEqual((section.name, symbol["st_value"]), (".rodata", 0))
                self.assertEqual(len(section.data()), 252)
                self.assertEqual(struct.unpack("<63I", section.data()), tuple(checks.values()))

    def test_native_single_point_projection_binding(self):
        root = family435.ROOT
        bindings = (root / "config/sles_03948/overlays/model_variant335_linker_symbols.txt").read_text()
        self.assertIn("RotTransPers = 0x80087868;", bindings)
        self.assertNotIn("func_french_80087868", bindings)
        for module in self.modules:
            stem = module["name"].removeprefix("french_")
            symbols = (root / f"config/sles_03948/overlays/{stem}_symbols.txt").read_text()
            self.assertIn("RotTransPers = 0x80087868;", symbols)
            self.assertNotIn("func_french_80087868", symbols)
        with (root / "config/sles_03948/functions.csv").open() as handle:
            row, = [row for row in csv.DictReader(handle) if int(row["address"], 0) == 0x80087868]
        self.assertEqual((int(row["size"], 0), row["status"]), (44, "sdk_asm"))
        resident = root / "game/france/SLES_039.48"
        if not resident.exists():
            self.skipTest("legal French resident input required")
        data = resident.read_bytes()
        self.assertEqual(struct.unpack_from("<11I", data, 0x800 + 0x80087868 - 0x80010000),
                         (0xC8800000, 0xC8810004, 0, 0x4A180001, 0xE8AE0000, 0xE8C80000,
                          0x4843F800, 0x48029800, 0xACE30000, 0x03E00008, 0x00021083))

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
                '#include "../../src/overlays/french_model_variant/variant335_ribbons.h"\n'
                '#include "../../src/overlays/model_variant/variant321_streamers.h"\n'
                '#define OFFSET(member) ((u32)&((Variant335Ring *)0)->member)\n'
                '#define SHEET_OFFSET(member) ((u32)&((ModelVariantSheet *)0)->member)\n'
                '#define RIBBON_OFFSET(member) ((u32)&((Model335Ribbon *)0)->member)\n'
                '#define STREAMER_OFFSET(member) ((u32)&((Variant321Streamer *)0)->member)\n'
                'const u32 ring_layout[] = {sizeof(Variant335Ring), sizeof(SVECTOR), '
                'sizeof(POLY_GT4), OFFSET(a), OFFSET(b), OFFSET(c), OFFSET(inner), '
                'OFFSET(outer), OFFSET(scale), OFFSET(cycles), '
                'sizeof(ModelVariantSheet), sizeof(SVECTOR), sizeof(VECTOR), sizeof(MATRIX), '
                'sizeof(GsCOORDINATE2), sizeof(PSXLONG), sizeof(POLY_GT4), '
                'SHEET_OFFSET(v0), SHEET_OFFSET(v1), SHEET_OFFSET(v2), SHEET_OFFSET(v3), '
                'SHEET_OFFSET(outer), SHEET_OFFSET(inner), SHEET_OFFSET(size), '
                'sizeof(Model335Ribbon), sizeof(Model335Screen), sizeof(SVECTOR), sizeof(VECTOR), '
                'sizeof(MATRIX), sizeof(GsCOORDINATE2), sizeof(PSXLONG), sizeof(POLY_FT4), '
                'RIBBON_OFFSET(a), RIBBON_OFFSET(sa), RIBBON_OFFSET(angle), RIBBON_OFFSET(b), '
                'RIBBON_OFFSET(sb), RIBBON_OFFSET(width), RIBBON_OFFSET(color), '
                'RIBBON_OFFSET(otz), RIBBON_OFFSET(flag), RIBBON_OFFSET(ox), RIBBON_OFFSET(oy), '
                'sizeof(Variant321Streamer), sizeof(SVECTOR), sizeof(VECTOR), sizeof(MATRIX), '
                'sizeof(GsCOORDINATE2), sizeof(PSXLONG), sizeof(POLY_FT4), '
                'STREAMER_OFFSET(a), STREAMER_OFFSET(sa), STREAMER_OFFSET(angle), STREAMER_OFFSET(b), '
                'STREAMER_OFFSET(sb), STREAMER_OFFSET(width), STREAMER_OFFSET(color), STREAMER_OFFSET(otz), '
                'STREAMER_OFFSET(flag), STREAMER_OFFSET(ox), STREAMER_OFFSET(oy)};\n')
            obj = compile_c(root, tool(root, "as"), dict(
                source=str(source.relative_to(root)), object="layout.o", profile=profile),
                profiles, object_directory=str(directory.relative_to(root)),
                asm_directory=str((directory / "asm").relative_to(root)))
            with obj.open("rb") as handle:
                elf = ELFFile(handle)
                symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("ring_layout")
                section = elf.get_section(symbol["st_shndx"])
                self.assertEqual((section.name, symbol["st_value"]), (".rodata", 0))
                self.assertIn(symbol["st_size"], (0, 244))
                self.assertEqual(len(section.data()), 244)
                self.assertEqual(struct.unpack("<61I", section.data()),
                                 (424, 8, 52, 0, 136, 272, 408, 412, 416, 420,
                                  152, 8, 16, 32, 80, 4, 52, 0, 32, 64, 96, 128, 132, 136,
                                  916, 4, 8, 16, 32, 80, 4, 40, 0, 136, 204, 272, 408,
                                  476, 544, 712, 780, 848, 882,
                                  888, 8, 16, 32, 80, 4, 40, 0, 136, 204, 272, 408, 476,
                                  612, 752, 684, 820, 854))
