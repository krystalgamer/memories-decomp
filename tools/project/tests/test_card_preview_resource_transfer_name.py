"""Check confirmed callback ownership and type, without executing VRAM uploads."""

import csv
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
NAME = "CardPreview_HandleResourceTransfer"
REGIONS = (
    ("slus_01411", 0x800289BC, 0x14C, ""),
    ("slpm_86398", 0x8002879C, 0x14C, "japanese/"),
    ("sles_03947", 0x8002895C, 0x16C, "european/"),
    ("sles_03948", 0x80028AA0, 0x16C, "european/"),
    ("sles_03949", 0x80028AA0, 0x16C, "european/"),
    ("sles_03950", 0x80028AA0, 0x16C, "european/"),
    ("sles_03951", 0x80028AA0, 0x16C, "european/"),
)
COMPILER = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"


class CardPreviewResourceTransferNameTests(unittest.TestCase):
    def test_seven_region_bindings(self):
        for config, address, size, tier in REGIONS:
            with self.subTest(config=config):
                directory = ROOT / "config" / config
                with (directory / "functions.csv").open(newline="") as handle:
                    rows = list(csv.DictReader(handle))
                entry, = [row for row in rows if int(row["address"], 0) == address]
                self.assertEqual((entry["name"], entry["status"], entry["module"]),
                                 (NAME, "matching_c", "game"))
                self.assertEqual(int(entry["size"], 0), size)
                rows = json.loads((directory / "matching_c.json").read_text())["functions"]
                entry, = [row for row in rows if int(row["address"], 0) == address]
                owner = "src/game/" + tier + "card_preview_handle_resource_transfer.c"
                self.assertEqual((entry["source"], entry["profile"], int(entry["size"], 0)),
                                 (owner, "gcc_2_8_1_g0_no_split", size))
                self.assertEqual(sum(row["source"] == owner for row in rows), 1)
                self.assertTrue((ROOT / owner).is_file())
                self.assertRegex((directory / "symbols.txt").read_text(),
                                 rf"(?m)^{NAME}\s*=\s*0x{address:08X};.*type:func")
                if config != "slus_01411":
                    self.assertIn(owner.removeprefix("src/").removesuffix(".c"),
                                  (directory / "split.yaml").read_text())

    def test_registered_callback_and_preserved_phase_boundaries(self):
        source = (ROOT / "src/game/card_preview_handle_resource_transfer.c").read_text()
        self.assertIn("void CardPreview_HandleResourceTransfer(FileTransferDescriptor *object, s32 mode)", source)
        self.assertIn("if (mode == 0)", source)
        self.assertIn("D_8009B0F4 &= 0xFFDCFFFF;", source)
        self.assertIn("e = &D_800EA0E8[(s32)object->callback_data];", source)
        self.assertEqual(source.count("LoadImage(rect, VRAM_UPLOAD_WORDS(b));"), 4)
        positions = [source.index(f"rect = &e->rects[{index}];") for index in range(4)]
        self.assertEqual(positions, sorted(positions))
        requester = (ROOT / "src/game/duel_effect_resource_setup.c").read_text()
        self.assertIn("DUEL_RESOURCE_TRANSFER_SECTORS, CardPreview_HandleResourceTransfer, 0, 0);", requester)
        japanese = (ROOT / "src/game/japanese/card_preview_handle_resource_transfer.c").read_text()
        self.assertIn("#define D_8009B0F4 gJapanese_FileTransferFlags", japanese)
        self.assertIn('#include "../card_preview_handle_resource_transfer.c"', japanese)
        european = (ROOT / "src/game/european/card_preview_handle_resource_transfer.c").read_text()
        self.assertIn("#define FUNC_800289BC_PHASE_SIZE 0x4000", european)
        self.assertIn("#define FUNC_800289BC_FOURTH_IMAGE_OFFSET 0x3560", european)
        for text in (european, (ROOT / "src/game/european/duel_effect_resource_setup.c").read_text()):
            self.assertNotIn("#define CardPreview_HandleResourceTransfer", text)
        with (ROOT / "notes/semantic-symbol-map.csv").open(newline="") as handle:
            entries = [row for row in csv.DictReader(handle) if row["name"] == NAME]
        self.assertEqual(len(entries), 1)
        self.assertEqual((entries[0]["kind"], entries[0]["address"], entries[0]["confidence"]),
                         ("function", "0x800289BC", "confirmed"))

    @unittest.skipUnless(COMPILER.is_file(), "requires supported GCC 2.8.1")
    def test_callback_contract_keeps_descriptor_and_opaque_word(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="resource-callback-name-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for parameters, accepted in (
                ("FileTransferDescriptor *, s32", True), ("u8 *, s32", False),
                ("FileTransferDescriptor *", False),
            ):
                with self.subTest(parameters=parameters):
                    probe = directory / "probe.c"
                    probe.write_text(
                        '#include "../../src/game/duel_effect_resource_setup.h"\n'
                        f"void (*checked)({parameters}) = {NAME};\n"
                    )
                    result = subprocess.run(
                        [str(COMPILER), "-S", "-O2", "-G0", "-Werror", "-o",
                         str(directory / "probe.s"), str(probe)],
                        cwd=ROOT, env=dict(os.environ, TMPDIR=str(directory)),
                        text=True, capture_output=True, timeout=60,
                    )
                    if accepted:
                        self.assertEqual(result.returncode, 0, result.stderr)
                    else:
                        self.assertNotEqual(result.returncode, 0, result.stderr)
                        self.assertIn("incompatible pointer type", result.stderr)


if __name__ == "__main__":
    unittest.main()
