#!/usr/bin/env python3

from __future__ import annotations

import sys

from build_baseline import BuildError
from build_regional_baseline import RegionalBuildConfiguration, build
from workspace import WorkspaceError, require_workspace_root


JAPANESE_BUILD = RegionalBuildConfiguration(
    splat_directory="tmp/splat/slpm_86398",
    output_directory="tmp/project-build",
    matching_config="config/slpm_86398/matching_c.json",
    output_name="SLPM_863.98",
    linker_script="slpm_86398.ld",
    asm_directory="tmp/project-build/japanese-asm",
    expected_size=0x1D0800,
    link_symbols="config/slpm_86398/link_symbols.ld",
)


def main() -> int:
    try:
        root = require_workspace_root()
        output = build(root, JAPANESE_BUILD)
    except (BuildError, WorkspaceError, OSError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    print(f"built: {output.relative_to(root)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
