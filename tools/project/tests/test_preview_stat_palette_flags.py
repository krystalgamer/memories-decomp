"""Exercise exact flag writers and CLUT conditionals, not the complete renderer."""

import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest

from tools.project.tests.test_library_display_objects import START


ROOT = Path(__file__).resolve().parents[3]
FIXTURE = r"""
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef struct {
    u8 before[16];
    DuelEffectResourceRecord records[2];
    u8 gap[16];
    SpritePrim primitive;
    u8 after[16];
} GuardedState;
extern GuardedState state;
"""
WITNESS = r"""
GuardedState state, expected;
static u32 cases;
void apply_palette_fragment(s32 which);
static int run_case(u32 which, u32 flags, u32 row)
{
    u32 i, selected, wanted;
    for (i = 0; i < sizeof(state); i++) ((u8 *)&state)[i] = i * 37 + flags + row;
    state.records[0].stat_palette_flags = flags;
    state.records[1].stat_palette_flags = selected = flags ^ 0xA5;
    state.primitive.cxcy.h.cy = row;
    for (i = 0; i < sizeof(state); i++) ((u8 *)&expected)[i] = ((u8 *)&state)[i];
    if (which == 0) ((u8 *)&expected.records[0])[0x3C] = 0;
    else if (which == 1) ((u8 *)&expected.records[0])[0x3C] = flags | 0x40;
    else if (which == 2) ((u8 *)&expected.records[1])[0x3C] = selected | 0x80;
    else if (which == 3) ((u8 *)&expected.records[1])[0x3C] = selected | 0x40;
    else {
        wanted = row;
        if (which == 4 && (selected & 0x80)) wanted = (row + 1) & 0xFFFF;
        if (which == 5 && (selected & 0x40)) wanted = 0xF9;
        if (which == 6 && (selected & 0x80)) wanted = 0xF9;
        if (which == 7 && (selected & 0x40)) wanted = 0xF9;
        expected.primitive.cxcy.h.cy = wanted;
    }
    apply_palette_fragment(which);
    for (i = 0; i < sizeof(state); i++)
        if (((u8 *)&state)[i] != ((u8 *)&expected)[i]) return 1;
    cases++;
    return 0;
}
int main(void)
{
    static const u16 rows[4] = {0, 0xF8, 0x7FFF, 0xFFFF};
    u32 flags, which, i, row;
    for (which = 0; which < 8; which++)
        for (flags = 0; flags < 256; flags++)
            for (i = 0; i < 4; i++)
                if (run_case(which, flags, rows[i])) return 1;
    for (row = 0; row < 65536; row++) {
        if (run_case(4, 0, row)) return 1;
        if (run_case(4, 0x80, row)) return 1;
    }
    return cases == 139264 ? 0 : 2;
}
"""


class PreviewStatPaletteFlagTests(unittest.TestCase):
    def fragments(self):
        constructor = (ROOT / "src/game/duel_effect_resource_setup.c").read_text()
        clear = "entry->stat_palette_flags = 0;"
        self.assertEqual(constructor.count(clear), 1)
        battle = (ROOT / "src/game/duel_scene_battle.c").read_text()
        writers = re.findall(r"effects\[[01]\]\.stat_palette_flags \|= 0x(?:40|80);", battle)
        self.assertEqual(writers, [
            "effects[0].stat_palette_flags |= 0x40;",
            "effects[1].stat_palette_flags |= 0x80;",
            "effects[1].stat_palette_flags |= 0x40;",
        ])
        renderer = (ROOT / "src/game/card_preview_draw_detail_panel.c").read_text()
        readers = re.findall(
            r"if \(rec->stat_palette_flags & 0x(?:80|40)\) \{\s*"
            r"PRM->cxcy\.h\.cy = [^;]+;\s*\}", renderer,
        )
        self.assertEqual(len(readers), 4)
        self.assertIn("PRM->cxcy.h.cy + 1", readers[0])
        for reader in readers[1:]:
            self.assertIn("FUNC_80028B08_CLUT_Y + 1", reader)
        prefix = renderer[:renderer.index("void CardPreview_DrawDetailPanel(")]
        return prefix, [clear, *writers, *readers]


    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires a Linux x86 compiler and ILP32 execution",
    )
    def test_actual_writer_and_palette_fragments_with_regression_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        prefix, original = self.fragments()
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "lost-unused-bits"),
            ("-O2", "wrong-stat-palette"), ("-O2", "wide-flags-clear"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                fragments = original.copy()
                if mutation == "lost-unused-bits":
                    fragments[1] = fragments[1].replace("|=", "=")
                elif mutation == "wrong-stat-palette":
                    fragments[4] = fragments[4].replace("& 0x80", "& 0x40")
                elif mutation == "wide-flags-clear":
                    fragments[0] = "*(u16 *)&entry->stat_palette_flags = 0;"
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(fragments, original)
                body = "\n".join(
                    "case " + str(index) + ": " + fragment + "\n break;"
                    for index, fragment in enumerate(fragments)
                )
                with tempfile.TemporaryDirectory(prefix="preview-palette-flags-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "operations.c"
                    operations.write_text(
                        prefix + FIXTURE + "\nvoid apply_palette_fragment(s32 which) {\n"
                        "DuelEffectResourceRecord *entry = &state.records[0];\n"
                        "DuelEffectResourceRecord *effects = state.records;\n"
                        "DuelEffectResourceRecord *rec = &state.records[1];\n"
                        "SpritePrim *PRM = &state.primitive;\n switch (which) {\n"
                        + body + "\n}\n}\n"
                    )
                    witness = directory / "witness.c"
                    witness.write_text(prefix + FIXTURE + WITNESS)
                    start = directory / "start.S"
                    start.write_text(START)
                    binary = directory / "witness"
                    environment = dict(os.environ, TMPDIR=str(directory))
                    result = subprocess.run(
                        [shutil.which("cc"), "-m32", "-std=gnu89", optimization,
                         "-nostdlib", "-fno-pie", "-no-pie", "-fno-stack-protector",
                         "-ffreestanding", "-fno-builtin", "-fno-strict-aliasing",
                         "-Werror=incompatible-pointer-types", "-I", str(ROOT / "src/game"),
                         str(operations), str(witness), str(start), "-o", str(binary)],
                        cwd=ROOT, env=environment, text=True, capture_output=True, timeout=60,
                    )
                    self.assertEqual(result.returncode, 0, result.stderr[-4000:])
                    result = subprocess.run([str(binary)], cwd=ROOT, env=environment, timeout=180)
                    self.assertEqual(result.returncode, 1 if mutation else 0)


if __name__ == "__main__":
    unittest.main()
