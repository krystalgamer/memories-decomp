from __future__ import annotations

import csv
from pathlib import Path
import shutil
import sys
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import psyq_declaration_contracts


class PsyqDeclarationContractTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        self.root = REPOSITORY / "tmp/test-psyq-declaration-contracts"
        shutil.rmtree(self.root, ignore_errors=True)
        (self.root / "config/slus_01411").mkdir(parents=True)
        (self.root / "src/psyq").mkdir(parents=True)
        (self.root / "src/game").mkdir(parents=True)
        with (self.root / "config/slus_01411/functions.csv").open(
            "w", newline="", encoding="utf-8"
        ) as handle:
            writer = csv.DictWriter(
                handle,
                fieldnames=["address", "size", "name", "status", "module"],
            )
            writer.writeheader()
            writer.writerow(
                {
                    "address": "0x80070000",
                    "size": "0x10",
                    "name": "SdkCall",
                    "status": "sdk_asm",
                    "module": "psyq/sdk",
                }
            )
            writer.writerow(
                {
                    "address": "0x80020000",
                    "size": "0x10",
                    "name": "GameCall",
                    "status": "matching_c",
                    "module": "game",
                }
            )

    def tearDown(self) -> None:
        shutil.rmtree(self.root, ignore_errors=True)

    def test_psyq_header_owns_sdk_declaration(self) -> None:
        (self.root / "src/psyq/libtest.h").write_text(
            "void SdkCall(void);\n", encoding="utf-8"
        )

        errors, stats = psyq_declaration_contracts.validate(self.root)

        self.assertEqual(errors, [])
        self.assertEqual(stats["header_symbols"], 1)

    def test_game_header_cannot_redeclare_sdk_function(self) -> None:
        (self.root / "src/game/test.h").write_text(
            "void SdkCall(void);\n", encoding="utf-8"
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertTrue(any("must be declared by a header" in error for error in errors))

    def test_game_function_declaration_is_out_of_scope(self) -> None:
        (self.root / "src/game/test.h").write_text(
            "void GameCall(void);\n", encoding="utf-8"
        )

        errors, _ = psyq_declaration_contracts.validate(self.root)

        self.assertEqual(errors, [])


if __name__ == "__main__":
    unittest.main()
