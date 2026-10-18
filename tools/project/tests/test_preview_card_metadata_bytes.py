"""Check catalogue-backed preview bytes without executing the renderer or engine."""

import csv
import os
from pathlib import Path
import platform
import re
import shutil
import struct
import subprocess
import sys
import tempfile
import unittest

from tools.project.tests.test_library_display_objects import START


ROOT = Path(__file__).resolve().parents[3]
HEADERS = r"""
#include "src/types.h"
#include "src/game/card_constants.h"
#include "src/game/duel_card.h"
#include "src/game/duel_effect_resource_record.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef struct { u8 before[16]; DuelEffectResourceRecord record; u8 after[16]; } GuardedRecord;
typedef struct { u8 before[16]; u8 bytes[CARD_COUNT + 1]; u8 after[16]; } GuardedTable;
extern GuardedRecord record_block;
extern GuardedTable table_block;
#define gDuel_abCardLevelAttr table_block.bytes
"""
WITNESS = r"""
GuardedRecord record_block, expected_record;
GuardedTable table_block, expected_table;
void copy_metadata(s32 card_id);
int main(void)
{
    u32 id, packed, i, cases = 0;
    for (id = 1; id <= CARD_COUNT; id++) {
        for (packed = 0; packed < 256; packed++) {
            for (i = 0; i < sizeof(record_block); i++)
                ((u8 *)&record_block)[i] = i * 37 + id + packed;
            for (i = 0; i < sizeof(table_block); i++)
                ((u8 *)&table_block)[i] = i * 19 + id + packed;
            table_block.bytes[id] = packed;
            for (i = 0; i < sizeof(record_block); i++)
                ((u8 *)&expected_record)[i] = ((u8 *)&record_block)[i];
            for (i = 0; i < sizeof(table_block); i++)
                ((u8 *)&expected_table)[i] = ((u8 *)&table_block)[i];
            ((u8 *)&expected_record.record)[0x3A] = packed & 15;
            ((u8 *)&expected_record.record)[0x3B] = packed / 16;
            copy_metadata(id);
            for (i = 0; i < sizeof(record_block); i++)
                if (((u8 *)&record_block)[i] != ((u8 *)&expected_record)[i]) return 1;
            for (i = 0; i < sizeof(table_block); i++)
                if (((u8 *)&table_block)[i] != ((u8 *)&expected_table)[i]) return 1;
            cases++;
        }
    }
    return cases == 184832 ? 0 : 2;
}
"""


class PreviewCardMetadataByteTests(unittest.TestCase):
    @unittest.skipUnless((ROOT / "game/SLUS_014.11").is_file(), "requires legally obtained NA executable")
    def test_verified_table_against_complete_local_catalogue(self):
        sys.path.insert(0, str(ROOT / "tools/project"))
        from psx_exe import load_verified_executable

        source, _, _, header = load_verified_executable(ROOT)
        data = source.read_bytes()
        start = header.vram_to_file_offset(0x801D5332)
        table = data[start:start + 723]
        self.assertEqual(len(table), 723)
        stats_offset = header.vram_to_file_offset(0x801D4244)
        with (ROOT / "notes/card-catalog.csv").open(newline="") as handle:
            cards = list(csv.DictReader(handle))
        self.assertEqual(len(cards), 722)
        attributes = ("Light", "Dark", "Earth", "Water", "Fire", "Wind")
        monsters = nonmonsters = 0
        for card_id, card in enumerate(cards, 1):
            with self.subTest(card_id=card_id):
                self.assertEqual(int(card["id"]), card_id)
                kind = (struct.unpack_from("<I", data, stats_offset + (card_id - 1) * 4)[0] >> 26) & 31
                icon, level = table[card_id] >> 4, table[card_id] & 15
                if kind < 20:
                    self.assertLess(icon, 6)
                    self.assertEqual(attributes[icon], card["attribute"])
                    self.assertEqual(level, int(card["level"]))
                    self.assertTrue(1 <= level <= 12)
                    monsters += 1
                else:
                    self.assertIn(kind, (20, 21, 22, 23))
                    self.assertIn(icon, (6, 7))
                    self.assertEqual(level, 0)
                    self.assertEqual(card["attribute"], "")
                    self.assertEqual(card["level"], "0")
                    nonmonsters += 1
        self.assertEqual((monsters, nonmonsters), (621, 101))

    def test_actual_producer_and_icon_consumer_boundaries(self):
        source = (ROOT / "src/game/duel_effect_resource_setup.c").read_text()
        self.assertIn("entry->attribute_icon = (u8)(gDuel_abCardLevelAttr[card_id] >> 4);", source)
        self.assertIn("entry->level = (u8)(gDuel_abCardLevelAttr[card_id] & 0xF);", source)
        renderer = (ROOT / "src/game/card_preview_draw_detail_panel.c").read_text()
        for text in (
            "if (rec->level != 0)", "while (i < (s32)rec->level);",
            "lo = rec->attribute_icon << 4;",
            "PRM->uv.b.lo = lo;", "tile = PRM->uv.b.lo;",
            "PRM->cxcy.h.cx = win->field_40.h.field_40 + tile;",
        ):
            self.assertIn(text, renderer)
        self.assertLess(renderer.index("PRM->uv.b.lo = lo;"), renderer.index("tile = PRM->uv.b.lo;"))
        self.assertNotIn("rec->field_3A", renderer)
        self.assertNotIn("rec->field_3B", renderer)

    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires a Linux x86 compiler and ILP32 execution",
    )
    def test_actual_producer_fragment_and_regression_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/duel_effect_resource_setup.c").read_text()
        match = re.search(
            r"entry->attribute_icon = \(u8\)\(gDuel_abCardLevelAttr\[card_id\] >> 4\);\s*"
            r"entry->level = \(u8\)\(gDuel_abCardLevelAttr\[card_id\] & 0xF\);", source,
        )
        self.assertIsNotNone(match)
        original = match.group()
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "wrong-card-index"),
            ("-O2", "swapped-nibbles"), ("-O2", "wide-level-store"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                code = original
                if mutation == "wrong-card-index":
                    code = code.replace("[card_id]", "[card_id - 1]")
                elif mutation == "swapped-nibbles":
                    code = code.replace("entry->attribute_icon", "entry->temporary_member")
                    code = code.replace("entry->level", "entry->attribute_icon")
                    code = code.replace("entry->temporary_member", "entry->level")
                elif mutation == "wide-level-store":
                    code = code.replace("entry->level =", "*(u16 *)&entry->level =")
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(code, original)
                with tempfile.TemporaryDirectory(prefix="preview-card-bytes-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "operations.c"
                    operations.write_text(
                        HEADERS + "\nvoid copy_metadata(s32 card_id) {\n"
                        "DuelEffectResourceRecord *entry = &record_block.record;\n" + code + "\n}\n"
                    )
                    witness = directory / "witness.c"
                    witness.write_text(HEADERS + WITNESS)
                    start = directory / "start.S"
                    start.write_text(START)
                    binary = directory / "witness"
                    environment = dict(os.environ, TMPDIR=str(directory))
                    result = subprocess.run(
                        [shutil.which("cc"), "-m32", "-std=gnu89", optimization,
                         "-nostdlib", "-fno-pie", "-no-pie", "-fno-stack-protector",
                         "-ffreestanding", "-fno-builtin", "-fno-strict-aliasing",
                         "-Werror=incompatible-pointer-types", "-I", str(ROOT),
                         str(operations), str(witness), str(start), "-o", str(binary)],
                        cwd=ROOT, env=environment, text=True, capture_output=True, timeout=60,
                    )
                    self.assertEqual(result.returncode, 0, result.stderr[-4000:])
                    result = subprocess.run([str(binary)], cwd=ROOT, env=environment, timeout=180)
                    self.assertEqual(result.returncode, 1 if mutation else 0)


if __name__ == "__main__":
    unittest.main()
