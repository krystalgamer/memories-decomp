import csv
import hashlib
import importlib.util
from pathlib import Path
import struct
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments
from tools.project.tests import test_spanish_model_variant405 as family
from tools.project.tests import test_spanish_model_variant460 as lifetimes


def reaching_definitions(image, base, register, target):
    definitions = set(lifetimes.SpanishModelVariant460Tests.register_writes(image, 4, 0xD00, register))
    pending, visited, reaching = [(4, None)], set(), set()
    while pending:
        pc, definition = pending.pop()
        if not (4 <= pc < 0xD00 and pc % 4 == 0):
            raise AssertionError(f"Entry edge escapes its span: {pc:#x}")
        if (pc, definition) in visited:
            continue
        visited.add((pc, definition))
        if pc in definitions:
            definition = pc
        if pc == target:
            reaching.add(definition)
        word, = struct.unpack_from("<I", image, pc)
        op = word >> 26
        if op in (1, 2, 3, 4, 5, 6, 7) or word == 0x03E00008:
            if pc + 4 in definitions:
                definition = pc + 4
            if word == 0x03E00008:
                continue
            if op == 2:
                targets = [((base + pc + 4) & 0xF0000000 | ((word & 0x3FFFFFF) << 2)) - base]
            elif op == 3:
                targets = [pc + 8]
            else:
                displacement = (word & 65535) - (65536 if word & 32768 else 0)
                targets = [pc + 8, pc + 4 + displacement * 4]
            pending.extend((next_pc, definition) for next_pc in targets)
        else:
            pending.append((pc + 4, definition))
    return reaching


class SpanishModelVariant405OuterBandsTests(unittest.TestCase):
    setUp = family.SpanishModelVariant405Tests.setUp
    legal_images = family.SpanishModelVariant405Tests.legal_images

    def test_original_context_records_growth_and_packet_bounds(self):
        anchors = {
            0xC: 0x00809021, 0x14: 0x0240A821, 0x24: 0x26B80864,
            0x2C: 0x26B90BC4, 0x6C: 0xAFB50080, 0x40: 0xAFB8008C,
            0x368: 0x2AE20010, 0x374: 0x27180034, 0x378: 0x27390034,
            0x394: 0x27110068, 0x444: 0xAE20FFFC, 0x44C: 0x26310074,
            0x454: 0x2A420003, 0x458: 0x27390074,
            0x72C: 0x2712011C, 0x7A4: 0x2AE20011, 0x7C0: 0xAE40FFFC,
            0x7C4: 0xAE400000, 0x7D0: 0x2A620003, 0x7D4: 0x27390120,
            0xAF8: 0x02402021, 0xB14: 0xA6A331FE,
            0x1B6C: 0x00809821, 0x1B70: 0x26680864,
            0x1BD4: 0x2C420FFF, 0x1D10: 0x866231DC,
            0x1D20: 0x866231DE, 0x1D2C: 0x866231E0,
            0x1EC8: 0x8D220064, 0x1ED0: 0x28420400,
            0x1EF0: 0x8E633224, 0x1EF8: 0x00031080,
            0x1EFC: 0x00431021, 0x1F00: 0x00021140,
            0x1F40: 0x8E633254, 0x1F50: 0xAE623254,
            0x1F70: 0x25080120, 0x1F7C: 0x25290074,
        }
        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            for call, offset in ((0xB10, 0x1B64), (0xB18, 0x12CC)):
                self.assertEqual(struct.unpack_from("<I", image, call)[0],
                                 0x0C000000 | ((base + offset) >> 2 & 0x3FFFFFF))
            self.assertEqual(reaching_definitions(image, base, 18, 0xAF8), {0xC})
            self.assertEqual(reaching_definitions(image, base, 4, 0xB10), {0xAF8})
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
            self.assertEqual(lifetimes.SpanishModelVariant460Tests.register_writes(
                image, 0x1B64, 0x1FBC, 19), [0x1B6C, 0x1FA4])
            self.assertEqual(lifetimes.SpanishModelVariant460Tests.register_writes(
                image, 0x1B64, 0x1FBC, 16), [0x1D30, 0x1EB0, 0x1FB0])
            stores = sorted((off, size) for _, off, size in
                            lifetimes.SpanishModelVariant460Tests.direct_stores(image, 0x1B64, 0x1FBC, 16))
            self.assertEqual(stores, [(off, 1) for off in (-38, -37, -36, -26, -25, -24,
                                                         -14, -13, -12, -2, -1, 0)])
        self.assertEqual(3 * 0x74, 0x15C)
        self.assertEqual(0x864 + 3 * 0x120, 0xBC4)
        self.assertEqual(0x2E08 + 16 * 52, 0x3148)
        self.assertLessEqual(16 * 8 + 8, 17 * 8)
        for slot in (0, 1):
            context = 0x80136000 + slot * 0x40000
            for bank, size in ((0x80100000, 96 * 2048), (0x8013A000, 4096), (0x8013B000, 20480)):
                bank += slot * 0x40000
                self.assertTrue(context + 0x3258 <= bank or bank + size <= context)

    def test_target_compiled_private_and_shared_layout(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from build_baseline import compile_c, load_compiler_profiles, tool
        from elftools.elf.elffile import ELFFile

        expressions = """
sizeof(SVECTOR), sizeof(VECTOR), sizeof(MATRIX), sizeof(GsCOORDINATE2),
sizeof(POLY_GT4), sizeof(Outer405Primary), OFF(Outer405Primary, progress),
sizeof(Bands405Band), OFF(Bands405Band, outer),
OFF(Bands405Band, inner_color), OFF(Bands405Band, outer_color),
OFF(Bands405Band, size), OFF(Bands405Band, completed),
sizeof(Bands405Descriptor), OFF(Bands405Descriptor, count),
OFF(Outer405State, primary), OFF(Outer405State, bands),
OFF(Outer405State, polygons), OFF(Outer405State, origin),
OFF(Outer405State, step), OFF(Outer405State, descriptor),
OFF(Outer405State, phase), sizeof(Outer405State),
sizeof(((Outer405State *)0)->descriptor),
OFF(POLY_GT4, x0), OFF(POLY_GT4, x1), OFF(POLY_GT4, x2), OFF(POLY_GT4, x3),
sizeof(((Outer405State *)0)->primary),
OFF(Outer405State, bands) + sizeof(((Outer405State *)0)->bands),
OFF(Outer405State, polygons) + sizeof(((Outer405State *)0)->polygons)
"""
        expected = (8, 16, 32, 80, 52, 0x74, 0x64, 0x120, 0x88, 0x110, 0x114,
                    0x118, 0x11C, 16, 12, 0, 0x864, 0x2E08, 0x31DC, 0x3224,
                    0x3230, 0x3254, 0x3258, 4, 8, 20, 32, 44, 0x15C, 0xBC4, 0x3148)
        directory = ROOT / "tmp/model405-outer-layout-tests"
        directory.mkdir(exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n'
                          '#include "../../src/overlays/spanish_model_variant/variant405_outer_bands.h"\n'
                          '#define OFF(t, f) ((u32)&((t *)0)->f)\n'
                          'u32 layout[] = {' + expressions + "};\n")
        obj = compile_c(ROOT, tool(ROOT, "as"),
                        {"source": str(source.relative_to(ROOT)), "profile": "gcc_2_8_1_g0_split", "object": "layout.o"},
                        load_compiler_profiles(ROOT), object_directory=str(directory.relative_to(ROOT)),
                        asm_directory=str(directory.relative_to(ROOT)))
        with obj.open("rb") as handle:
            elf = ELFFile(handle)
            symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layout")
            section = elf.get_section(symbol["st_shndx"])
            self.assertEqual((symbol["st_value"], section["sh_size"]), (0, len(expected) * 4))
            self.assertEqual(section.data(), struct.pack("<" + "I" * len(expected), *expected))

    def test_outer_c_owner_and_all_relocations_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        for module, image in self.legal_images():
            base = int(module["load_address"], 0)
            directory = ROOT / f"tmp/overlays/{module['name']}"
            linked_path = directory / f"build/{module['name']}.elf"
            if not linked_path.is_file():
                self.skipTest("Build MODEL405 overlays before checking C ownership")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"])
                         if "variant405_outer_bands" in s["source"]]
            path = directory / "build" / selected["object"]
            self.assertIn(f"{path.relative_to(ROOT)}(.text);", (directory / f"{module['name']}.ld").read_text())
            with linked_path.open("rb") as final_handle, path.open("rb") as obj_handle:
                linked, compiled = ELFFile(final_handle), ELFFile(obj_handle)
                symbols = compiled.get_section_by_name(".symtab")
                own, = symbols.get_symbol_by_name(f"func_{base+0x1B64:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]), (0, 1112, "STT_FUNC"))
                section = compiled.get_section(own["st_shndx"])
                self.assertTrue(section["sh_flags"] & 4)
                text = section.data()
                callees, local = [], []
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    symbol = symbols.get_symbol(relocation["r_info_sym"])
                    site = relocation["r_offset"]
                    self.assertLess(site, 1112)
                    word, = struct.unpack_from("<I", image, 0x1B64 + site)
                    address = ((base + 0x1B64 + site + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                    addend = (struct.unpack_from("<I", text, site)[0] & 0x3FFFFFF) << 2
                    if symbol["st_shndx"] == "SHN_UNDEF":
                        resolved, = linked.get_section_by_name(".symtab").get_symbol_by_name(symbol.name)
                        self.assertEqual((word >> 26, address), (3, resolved["st_value"] + addend))
                        callees.append(address)
                    else:
                        self.assertEqual(symbol["st_shndx"], own["st_shndx"])
                        self.assertEqual((word >> 26, address),
                                         (2, base + 0x1B64 + symbol["st_value"] + addend))
                        local.append((site, address-base-0x1B64))
                self.assertEqual(local, [(0x148, 0x198)])
                self.assertEqual(len(callees), 10)
                self.assertEqual(set(callees), {0x8005C018, 0x800842A8, 0x80085558, 0x80086258,
                                               0x800872A8, 0x800875F8, 0x80087738, 0x80087958, 0x80087CB8})

    def test_terminal_source_and_transitive_layout_fingerprints(self):
        with (ROOT / "notes/overlays/spanish-model-variant405-outer-bands-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual([int(r["different_words"]) for r in rows], [13] + [0] * 10)
        body = ROOT / "src/overlays/spanish_model_variant/variant405_outer_bands.c"
        shared = body.with_name("variant405_bands.h")
        dependency = hashlib.sha256(body.read_bytes() + body.with_suffix(".h").read_bytes() +
                                    shared.read_bytes()).hexdigest()
        for module in self.modules:
            row = [r for r in rows if r["module"] == module["name"]][-1]
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"])
                         if "variant405_outer_bands" in s["source"]]
            self.assertEqual((row["result"], row["instruction_bytes"], row["different_words"]), ("matched", "1112", "0"))
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
            self.assertEqual(row["fingerprint"], hashlib.sha256((ROOT / selected["source"]).read_bytes()).hexdigest())
            self.assertEqual(row["dependency_fingerprint"], dependency)


if __name__ == "__main__":
    unittest.main()
