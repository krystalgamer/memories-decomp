#!/usr/bin/env python3

from __future__ import annotations

import argparse
import shutil
import sys
from pathlib import Path

from workspace import WorkspaceError, require_workspace_root, resolve_within


class CleanError(RuntimeError):
    pass


CLEAN_TARGETS = {
    "extract": "tmp/extract",
    "generated": "tmp/generated",
    "overlays": "tmp/overlays",
    "overlay-objects": "tmp/overlay-object-cache",
    "splat": "tmp/splat",
    "project-build": "tmp/project-build",
    "incremental": "tmp/incremental",
    "reports": "tmp/reports",
}

# Each image's own generated output, so building one image leaves the others'
# split and build products in place. North America splits into the tmp/splat
# root; every regional image keeps its split beneath tmp/splat/<image>.
REGIONAL_IMAGES = {
    "slpm_86398": ("SLPM_863.98", "japanese"),
    "sles_03947": ("SLES_039.47", "european"),
    "sles_03948": ("SLES_039.48", "french"),
    "sles_03949": ("SLES_039.49", "german"),
    "sles_03950": ("SLES_039.50", "italian"),
    "sles_03951": ("SLES_039.51", "spanish"),
}
NORTH_AMERICAN_IMAGE = "slus_01411"
IMAGES = (NORTH_AMERICAN_IMAGE, *REGIONAL_IMAGES)
IMAGE_GROUPS = ("splat", "project-build")


def executable_outputs(name: str) -> list[str]:
    return [
        f"tmp/project-build/{name}{suffix}" for suffix in ("", ".elf", ".map", ".ld")
    ]


def image_paths(root: Path, group: str, image: str) -> list[str]:
    if group == "splat":
        if image in REGIONAL_IMAGES:
            return [f"tmp/splat/{image}"]
        splat = root / CLEAN_TARGETS["splat"]
        if not splat.is_dir():
            return []
        return [
            f"tmp/splat/{entry.name}"
            for entry in sorted(splat.iterdir())
            if entry.name not in REGIONAL_IMAGES
        ]
    if image in REGIONAL_IMAGES:
        name, language = REGIONAL_IMAGES[image]
        return [f"tmp/project-build/{language}-asm", *executable_outputs(name)]
    return [
        "tmp/project-build/obj",
        "tmp/project-build/asm",
        *executable_outputs("SLUS_014.11"),
    ]


def remove_path(root: Path, relative_path: str) -> None:
    resolve_within(root, relative_path)
    path = root / relative_path
    if not path.exists() and not path.is_symlink():
        print(f"clean: absent {relative_path}")
        return
    if path.is_symlink() or path.is_file():
        path.unlink()
    elif path.is_dir():
        shutil.rmtree(path)
    else:
        raise CleanError(f"unsupported generated path type: {relative_path}")
    print(f"clean: removed {relative_path}")


def remove_target(root: Path, name: str) -> None:
    group, separator, image = name.partition(":")
    if not separator:
        remove_path(root, CLEAN_TARGETS[name])
        return
    paths = image_paths(root, group, image)
    if not paths:
        print(f"clean: absent {group} output for {image}")
    for path in paths:
        remove_path(root, path)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Remove only explicitly known generated project paths."
    )
    parser.add_argument(
        "targets",
        nargs="+",
        choices=sorted(
            [*CLEAN_TARGETS]
            + [f"{group}:{image}" for group in IMAGE_GROUPS for image in IMAGES]
        ),
        help="generated path groups to remove; group:image limits splat or "
        "project-build to one executable's output",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        root = require_workspace_root()
        for target in dict.fromkeys(args.targets):
            remove_target(root, target)
    except (CleanError, WorkspaceError, OSError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
