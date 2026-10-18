"""Guard the named requester's grouped bindings and unchanged descriptor API."""

import csv
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
NAME = "CardPreview_RequestResourceTransfer"
REGIONS = (
    ("slus_01411", 0x80029164, 0x7C, 0x348, ""),
    ("slpm_86398", 0x80028F44, 0x7C, 0x348, "japanese/"),
    ("sles_03947", 0x80029198, 0x78, 0x36C, "european/"),
    ("sles_03948", 0x800292DC, 0x78, 0x36C, "european/"),
    ("sles_03949", 0x800292DC, 0x78, 0x36C, "european/"),
    ("sles_03950", 0x800292DC, 0x78, 0x36C, "european/"),
    ("sles_03951", 0x800292DC, 0x78, 0x36C, "european/"),
)
COMPILER = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"


class CardPreviewResourceRequestNameTests(unittest.TestCase):
    def test_seven_region_names_preserve_grouped_owner_and_profile(self):
        for config, address, size, constructor_size, tier in REGIONS:
            with self.subTest(config=config):
                directory = ROOT / "config" / config
                with (directory / "functions.csv").open(newline="") as handle:
                    rows = list(csv.DictReader(handle))
                entry, = [row for row in rows if int(row["address"], 0) == address]
                self.assertEqual((entry["name"], entry["status"], entry["module"]),
                                 (NAME, "matching_c", "game"))
                self.assertEqual(int(entry["size"], 0), size)
                matching = json.loads((directory / "matching_c.json").read_text())["functions"]
                owner = "src/game/" + tier + "duel_effect_resource_setup.c"
                group = [row for row in matching if row["source"] == owner]
                self.assertEqual(
                    [(int(row["address"], 0), int(row["size"], 0), row["profile"]) for row in group],
                    [(address, size, "gcc_2_8_1_g8_split"),
                     (address + size, constructor_size, "gcc_2_8_1_g8_split")],
                )
                self.assertTrue((ROOT / owner).is_file())
                self.assertRegex((directory / "symbols.txt").read_text(),
                                 rf"(?m)^{NAME}\s*=\s*0x{address:08X};.*type:func")
                self.assertNotRegex((directory / "symbols.txt").read_text(),
                                    r"(?m)^func_80029164\s*=")
                if config != "slus_01411":
                    self.assertIn(owner.removeprefix("src/").removesuffix(".c"),
                                  (directory / "split.yaml").read_text())

    def test_registered_callback_and_existing_regional_disc_math(self):
        source = (ROOT / "src/game/duel_effect_resource_setup.c").read_text()
        self.assertIn(f"FileTransferDescriptor *{NAME}(s32 slot, s32 card_id)", source)
        self.assertIn("D_800EA0E8[slot].card_id = card_id;", source)
        self.assertIn("0, 0, DUEL_RESOURCE_TRANSFER_POSITION(card_id),", source)
        self.assertIn("DUEL_RESOURCE_TRANSFER_SECTORS, CardPreview_HandleResourceTransfer, 0, 0);", source)
        self.assertIn("object->callback_data = (void *)slot;", source)
        self.assertIn("object->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;", source)
        self.assertIn("    return object;", source)
        self.assertIn("(((value) - 1) * 7 + CARD_COUNT)", source)
        japanese = (ROOT / "src/game/japanese/duel_effect_resource_setup.c").read_text()
        self.assertIn("#define D_8009B0F4_abs gJapanese_FileTransferFlags", japanese)
        european = (ROOT / "src/game/european/duel_effect_resource_setup.c").read_text()
        self.assertIn("((value) * 8 + CARD_COUNT - 8)", european)
        self.assertIn("#define DUEL_RESOURCE_TRANSFER_SECTORS 8", european)
        for wrapper in (japanese, european):
            self.assertIn('#include "../duel_effect_resource_setup.c"', wrapper)
        with (ROOT / "notes/semantic-symbol-map.csv").open(newline="") as handle:
            entries = [row for row in csv.DictReader(handle) if row["name"] == NAME]
        entry, = entries
        self.assertEqual((entry["kind"], entry["address"], entry["confidence"]),
                         ("function", "0x80029164", "confirmed"))

    @unittest.skipUnless(COMPILER.is_file(), "requires supported GCC 2.8.1")
    def test_descriptor_return_and_full_word_argument_contract(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="preview-request-name-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for result_type, parameters, accepted in (
                ("FileTransferDescriptor *", "s32, s32", True),
                ("void *", "s32, s32", False),
                ("u8 *", "s32, s32", False),
                ("FileTransferDescriptor *", "s32, u16", False),
                ("FileTransferDescriptor *", "s32", False),
            ):
                with self.subTest(result_type=result_type, parameters=parameters):
                    probe = directory / "probe.c"
                    probe.write_text(
                        '#include "../../src/game/duel_effect_resource_setup.h"\n'
                        f"{result_type} (*checked)({parameters}) = {NAME};\n"
                    )
                    result = subprocess.run(
                        [str(COMPILER), "-S", "-O2", "-G8", "-Werror", "-o",
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
