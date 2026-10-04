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
    binding_count = 35
    tail_start = 0x27A8
    spans = ((4, 0xB98), (0xB98, 0x12C4), (0x12C4, 0x1874),
             (0x1874, 0x1EC8), (0x1EC8, 0x2364), (0x2364, 0x27A8))
    helpers = ((0xB98, 1836, "trails", "func_8013BB98"),
               (0x12C4, 1456, "strip", "func_8013C2C4"),
               (0x1874, 1620, "ribbons", "func_8013C874"),
               (0x1EC8, 1180, "rings", "func_8013CEC8"),
               (0x2364, 1092, "bands", "func_8013D364"))
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
        self.assertEqual(len(rows), 71)
        ribbon_rows = rows[45:51]
        ring_rows = rows[38:45]
        rows = rows[:38]
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
        self.assertEqual((directory / "variant412_rings_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013CEC8 func_8017CEC8\n'
                         '#include "variant412_rings.c"\n')
        ring_body = (directory / "variant412_rings.c").read_text()
        self.assertEqual(ring_body,
                         '#include "../../types.h"\n#include "variant412_rings.h"\n\n'
                         "/* Load shared declarations before selecting this renderer's measured view. */\n"
                         "#define Family402RingView Variant412RingView\n"
                         "#define func_8013C69C func_8013CEC8\n"
                         '#include "variant402_rings.c"\n')
        self.assertNotRegex(ring_body, r"\b(?:extern|asm|__asm__|register)\b")
        self.assertEqual([row["result"] for row in ring_rows],
                         ["compile_error"] + ["text_exact"] * 4 + ["matched"] * 2)
        self.assertEqual((ring_rows[0]["instruction_bytes"], ring_rows[0]["different_words"]),
                         ("", ""))
        self.assertIn("before either slot", ring_rows[0]["reason"])
        for row in ring_rows:
            self.assertEqual(row["function_offset"], "0x1EC8")
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
        for slot, row in enumerate(ring_rows[-2:]):
            source = directory / ("variant412_rings" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["slot"], row["instruction_bytes"], row["different_words"]),
                             (str(slot), "1180", "0"))
        self.assertEqual((directory / "variant412_ribbons.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define MODEL_VARIANT412_RIBBONS\n'
                         '#define func_8013C048 func_8013C874\n'
                         '#include "variant402_ribbons.c"\n')
        self.assertEqual((directory / "variant412_ribbons_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013C874 func_8017C874\n'
                         '#include "variant412_ribbons.c"\n')
        self.assertEqual([row["result"] for row in ribbon_rows],
                         ["text_exact"] * 4 + ["matched"] * 2)
        for row in ribbon_rows:
            self.assertEqual((row["function_offset"], row["profile"],
                              row["instruction_bytes"], row["different_words"]),
                             ("0x1874", "gcc_2_8_1_g0_split", "1620", "0"))
        for slot, row in enumerate(ribbon_rows[-2:]):
            source = directory / ("variant412_ribbons" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["slot"], str(slot))
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertNotIn(f"D_{base + 0x12C4:X}", symbols)
            self.assertNotIn(f"D_{base + 0x2364:X}", symbols)
            self.assertNotIn("unclassified_prefix", layout.read_text())
            self.assertIn(f"D_{base + 0x27A8:X} = 0x{base + 0x27A8:X}; "
                          "// type:u8 size:0x2858 defined:true", symbols)
            self.assertNotIn("size:0x3D3C", symbols)

    def test_shared_strip_bands_wrappers_and_attempt_history(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        with (family435.ROOT / "notes/overlays/french-model-variant412-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 71)
        for label, original, selected, offset, size, start, measured in (
                ("strip", "8013BA5C", "8013C2C4", "0x12C4", 1456, 51,
                 ((1476, 361), (1448, 325), (1456, 13), (1456, 0), (1456, 0))),
                ("bands", "8013CB38", "8013D364", "0x2364", 1092, 63,
                 ((1092, 54), (1092, 0), (1092, 0)))):
            wrapper = '#include "../../types.h"\n'
            if label == "strip":
                wrapper += '#include "variant412_rings.h"\n'
            wrapper += f"#define MODEL_VARIANT412_{label.upper()}\n"
            if label == "strip":
                wrapper += "#define Family402RingView Variant412RingView\n"
            wrapper += f'#define func_{original} func_{selected}\n#include "variant402_{label}.c"\n'
            self.assertEqual((directory / f"variant412_{label}.c").read_text(), wrapper)
            self.assertEqual((directory / f"variant412_{label}_slot1.c").read_text(),
                             '#include "../../types.h"\n'
                             f'#define func_{selected} func_{int(selected, 16) + 0x40000:X}\n'
                             f'#include "variant412_{label}.c"\n')
            self.assertNotRegex((directory / f"variant402_{label}.c").read_text(),
                                r"\b(?:extern|asm|__asm__|register|volatile)\b")
            experiments = rows[start:start + 2 * len(measured)]
            self.assertEqual([(int(row["instruction_bytes"]), int(row["different_words"]))
                              for row in experiments], [pair for pair in measured for _ in range(2)])
            for row in experiments:
                self.assertEqual((row["function_offset"], row["profile"], row["result"]),
                                 (offset, "gcc_2_8_1_g0_split",
                                  "mismatch" if int(row["different_words"]) else "text_exact"))
            terminals = rows[start + len(experiments):start + len(experiments) + 2]
            for slot, row in enumerate(terminals):
                source = directory / (f"variant412_{label}" + ("_slot1" if slot else "") + ".c")
                self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
                self.assertEqual((row["function_offset"], row["slot"], row["instruction_bytes"],
                                  row["different_words"], row["result"], row["profile"]),
                                 (offset, str(slot), str(size), "0", "matched", "gcc_2_8_1_g0_split"))

    def test_retained_code_spans_and_unresolved_reachability(self):
        from overlay_function_inventory import walk_function

        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                for start, size, frame in ((0x12C4, 1456, 256), (0x1874, 1620, 296),
                                           (0x1EC8, 1180, 272), (0x2364, 1092, 304)):
                    cfg = walk_function(data, base, start, size)
                    self.assertTrue(cfg["closed"])
                    self.assertEqual(cfg["extent"], size)
                    self.assertEqual(set(cfg["visited"]), set(range(start, start + size, 4)))
                    self.assertEqual(cfg["calls"], set())
                    self.assertEqual(struct.unpack_from("<I", data, start)[0],
                                     0x27BD0000 | ((-frame) & 0xFFFF))
                self.assertNotIn(base + 0x1EC8, struct.unpack("<5120I", data))
                self.assertNotIn(base + 0x1874, struct.unpack("<5120I", data))
                self.assertEqual(struct.unpack_from("<I", data, 0x1ED8)[0], 0x261E12C0)
                self.assertEqual(struct.unpack_from("<I", data, 0x1F00)[0], 0x26111978)
                self.assertEqual(struct.unpack_from("<I", data, 0x1F08)[0], 0x26121340)
                self.assertEqual(struct.unpack_from("<I", data, 0x2318)[0], 0x26520090)
                self.assertEqual(struct.unpack_from("<I", data, 0x2328)[0], 0x29020002)

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
                self.assertLess(context + 0x1B7A, 1 << 32)
                for start, size in ((pointers[slot], 96 * 2048),
                                    (pointers[3 + slot], 2 * 2048),
                                    (base, 10 * 2048)):
                    self.assertTrue(context + 0x1B7A <= start or start + size <= context)
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
                self.assertIn("RotTransPers = 0x80087868;", bindings)
                self.assertNotIn("func_french_80087868", bindings)

    def test_shared_ribbon_layout_selectors(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("pyelftools required for target layouts")
        from tools.project.build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        root = family435.ROOT
        profiles = load_compiler_profiles(root)
        profile = "gcc_2_8_1_g0_split"
        if not (root / profiles[profile]["compiler"]).exists():
            self.skipTest("local target compiler required")
        offsets = (0x58, 0x178, 0x668, 0x828, 0x82C, 0x830, 0x83C, 0x840,
                   0x844, 0x850, 0x858, 0x868, 0x874, 0x87C, 0x890,
                   0x8A6, 0x8A8, 0x8AC, 0x8C0)
        selected = (0x12C0, 0x13E0, 0x1904, 0x1AC4, 0x1AC8, 0x1ACC,
                    0x1AD8, 0x1ADC, 0x1AE0, 0x1AEC, 0x1AF4, 0x1B04,
                    0x1B14, 0x1B24, 0x1B48, 0x1B5E, 0x1B60, 0x1B64, 0x1B78)
        expressions = [
            "sizeof(Family402Ribbon)", "sizeof(SVECTOR)", "sizeof(VECTOR)",
            "sizeof(MATRIX)", "sizeof(GsCOORDINATE2)", "sizeof(PSXLONG)", "sizeof(POLY_G3)",
            *[f"(u32)&((Family402Ribbon *)0)->{field}"
              for field in ("a", "sa", "angle", "b", "sb", "width", "otz", "ox", "oy")],
            *[f"RIBBON_WORK_{offset:04X}" for offset in offsets], "RIBBON_CONFIG_COUNT",
        ]
        record = (88, 8, 16, 32, 80, 4, 28, 0, 16, 24, 32, 48, 56, 64, 72, 76)
        for family, expected in ((402, record + offsets + (0xC,)),
                                 (412, record + selected + (0x10,))):
            with self.subTest(family=family), tempfile.TemporaryDirectory(
                    prefix="variant-ribbon-layout-", dir=root / "tmp") as temporary:
                directory = Path(temporary)
                source = directory / "layout.c"
                source.write_text(
                    '#include "../../src/types.h"\n'
                    + ('#define MODEL_VARIANT412_RIBBONS\n' if family == 412 else '')
                    + '#include "../../src/overlays/french_model_variant/variant402_ribbons.c"\n'
                    + 'const u32 ribbon_layout[] = {' + ", ".join(expressions) + '};\n')
                obj = compile_c(root, tool(root, "as"), dict(
                    source=str(source.relative_to(root)), object="layout.o", profile=profile),
                    profiles, object_directory=str(directory.relative_to(root)),
                    asm_directory=str((directory / "asm").relative_to(root)))
                with obj.open("rb") as handle:
                    elf = ELFFile(handle)
                    symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("ribbon_layout")
                    section = elf.get_section(symbol["st_shndx"])
                    self.assertEqual((section.name, symbol["st_value"]), (".rodata", 0))
                    self.assertEqual(len(section.data()), len(expected) * 4)
                    self.assertEqual(struct.unpack(f"<{len(expected)}I", section.data()), expected)

    def test_shared_strip_bands_layout_selectors(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("pyelftools required for target layouts")
        from tools.project.build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        root = family435.ROOT
        profiles = load_compiler_profiles(root)
        profile = "gcc_2_8_1_g0_split"
        if not (root / profiles[profile]["compiler"]).exists():
            self.skipTest("local target compiler required")
        strip_offsets = (0x84E, 0x84C, 0x6A8, 0x8AC, 0x8A4, 0x828, 0x82C,
                         0x830, 0x83C, 0x840, 0x844, 0x8A6, 0x890, 0x874)
        strip_selected = (0x1AEA, 0x1AE8, 0x1944, 0x1B64, 0x1B5C, 0x1AC4, 0x1AC8,
                          0x1ACC, 0x1AD8, 0x1ADC, 0x1AE0, 0x1B5E, 0x1B48, 0x1B14)
        band_offsets = (0x438, 0x844, 0x83C, 0x840, 0x744, 0x868, 0x834, 0x836, 0x838,
                        0x894, 0x895, 0x896, 0x898, 0x899, 0x89A, 0x874, 0x8AC)
        band_selected = (0x16A0, 0x1AE0, 0x1AD8, 0x1ADC, 0x1A64, 0x1B04,
                         0x1AD0, 0x1AD2, 0x1AD4, 0x1B4C, 0x1B4D, 0x1B4E,
                         0x1B50, 0x1B51, 0x1B52, 0x1B14, 0x1B64)
        sdk = ["sizeof(SVECTOR)", "sizeof(VECTOR)", "sizeof(MATRIX)",
               "sizeof(GsCOORDINATE2)", "sizeof(PSXLONG)"]
        expressions = {
            "strip_layout": [
                "sizeof(Family402Strip)", *sdk, "sizeof(POLY_GT4)",
                *[f"(u32)&((Family402Strip *)0)->{field}" for field in ("points", "projected", "depth")],
                *[f"STRIP_WORK_{offset:04X}" for offset in strip_offsets], "STRIP_RECORD_BASE",
            ],
            "band_layout": [
                "sizeof(Family402Band)", *sdk, "sizeof(POLY_FT4)",
                *[f"(u32)&((Family402Band *)0)->{field}" for field in ("inner", "outer", "scale", "cycles")],
                *[f"(u32)&((POLY_FT4 *)0)->x{index}" for index in range(4)],
                *[f"BAND_WORK_{offset:04X}" for offset in band_offsets], "sizeof(Family402BandPacket)",
            ],
        }
        for family in (402, 412):
            expected = {
                "strip_layout": (88, 8, 16, 32, 80, 4, 52, 0, 0x30, 0x50)
                + (strip_selected if family == 412 else strip_offsets)
                + (0x1268 if family == 412 else 0,),
                "band_layout": (280, 8, 16, 32, 80, 4, 40, 0, 0x88, 0x110, 0x114, 8, 16, 24, 32)
                + (band_selected if family == 412 else band_offsets)
                + (40 if family == 412 else 52,),
            }
            with self.subTest(family=family), tempfile.TemporaryDirectory(
                    prefix="variant-strip-band-layout-", dir=root / "tmp") as temporary:
                directory = Path(temporary)
                source = directory / "layout.c"
                source.write_text(
                    '#include "../../src/types.h"\n'
                    + ("#define MODEL_VARIANT412_STRIP\n#define MODEL_VARIANT412_BANDS\n"
                       if family == 412 else "")
                    + '#include "../../src/overlays/french_model_variant/variant402_strip.h"\n'
                    + '#include "../../src/overlays/french_model_variant/variant402_bands.h"\n'
                    + "".join(f'const u32 {name}[] = {{' + ", ".join(values) + '};\n'
                              for name, values in expressions.items()))
                obj = compile_c(root, tool(root, "as"), dict(
                    source=str(source.relative_to(root)), object="layout.o", profile=profile),
                    profiles, object_directory=str(directory.relative_to(root)),
                    asm_directory=str((directory / "asm").relative_to(root)))
                with obj.open("rb") as handle:
                    elf = ELFFile(handle)
                    self.assertEqual(elf.get_section_by_name(".rodata")["sh_size"], 232)
                    expected_offset = 0
                    for name, values in expected.items():
                        symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name(name)
                        section = elf.get_section(symbol["st_shndx"])
                        self.assertEqual((section.name, symbol["st_value"]), (".rodata", expected_offset))
                        start = symbol["st_value"]
                        self.assertEqual(struct.unpack(f"<{len(values)}I",
                                                       section.data()[start:start + 4 * len(values)]), values)
                        expected_offset += 4 * len(values)

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

    def test_retained_renderer_target_layout(self):
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
            "sizeof(Family402Ring)": 0x90,
            "(u32)&((Family402Ring *)0)->scale": 0x80,
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(GsCOORDINATE2)": 80, "sizeof(PSXLONG)": 4, "sizeof(POLY_GT4)": 52,
            "(u32)&((Variant412RingConfig *)0)->part_count": 0x10,
            "(u32)&((Variant412RingConfig *)0)->duration": 0x14,
            "sizeof(Variant412RingView)": 0x1B68,
        }
        for member, offset in (("rings", 0x12C0), ("quad", 0x1978), ("transform", 0x1AB0),
                               ("target", 0x1AD0), ("velocity", 0x1AD8), ("frame", 0x1B04),
                               ("elapsed", 0x1B0C), ("step", 0x1B14), ("config", 0x1B24),
                               ("selected_part", 0x1B48), ("interpolation", 0x1B5E),
                               ("phase", 0x1B64)):
            checks[f"(u32)&((Variant412RingView *)0)->{member}"] = offset
        with tempfile.TemporaryDirectory(prefix="variant412-rings-", dir=root / "tmp") as temporary:
            directory = Path(temporary)
            source = directory / "layout.c"
            source.write_text(
                '#include "../../src/types.h"\n'
                '#include "../../src/overlays/french_model_variant/variant412_rings.h"\n'
                "u32 layouts[] = {" + ", ".join(checks) + "};\n")
            obj = compile_c(root, tool(root, "as"), dict(
                source=str(source.relative_to(root)), object="layout.o", profile=profile),
                profiles, object_directory=str(directory.relative_to(root)),
                asm_directory=str((directory / "asm").relative_to(root)))
            with obj.open("rb") as handle:
                elf = ELFFile(handle)
                symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layouts")
                section = elf.get_section(symbol["st_shndx"])
                self.assertEqual(symbol["st_value"], 0)
                self.assertEqual(struct.unpack("<23I", section.data()), tuple(checks.values()))
