"""Export IDA 9.x names into reviewable memories-decomp symbol files."""

from __future__ import annotations

import csv
import re
from pathlib import Path

import ida_bytes
import ida_funcs
import ida_kernwin
import idautils
import idc


IDENTIFIER = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
SYMBOL_PATTERN = re.compile(
    r"^\s*([A-Za-z_][A-Za-z0-9_]*)\s*=\s*"
    r"(0x[0-9A-Fa-f]+|\d+)\s*;"
)
AUTO_PREFIXES = (
    "sub_",
    "loc_",
    "locret_",
    "byte_",
    "word_",
    "dword_",
    "qword_",
    "unk_",
    "off_",
    "asc_",
    "stru_",
    "algn_",
)
FUNCTION_FIELDS = (
    "address",
    "size",
    "name",
    "status",
    "module",
    "notes",
)
SEMANTIC_FIELDS = (
    "kind",
    "address",
    "name",
    "confidence",
    "evidence",
    "notes",
)

SEMANTIC_MAP = "notes/semantic-symbol-map.csv"
SEMANTIC_MAP_DIRECTORY = "notes/semantic-symbol-map.d"


class ExportError(Exception):
    pass


def find_project_root() -> Path | None:
    scripted = Path(__file__).resolve().parents[2]
    if (scripted / "config/slus_01411/functions.csv").is_file():
        return scripted
    selected = ida_kernwin.ask_file(
        False,
        "functions.csv",
        "Select config/slus_01411/functions.csv",
    )
    if not selected:
        return None
    path = Path(selected).resolve()
    if path.name != "functions.csv":
        raise ExportError("selected file is not functions.csv")
    root = path.parents[2]
    if not (root / SEMANTIC_MAP).is_file():
        raise ExportError("could not locate the memories-decomp root")
    return root


def is_placeholder(name: str, address: int) -> bool:
    if not name or name.startswith(AUTO_PREFIXES):
        return True
    return name.lower() in {
        f"func_{address:08x}",
        f"d_{address:08x}",
    }


def is_user_name(address: int) -> bool:
    try:
        return bool(ida_bytes.has_user_name(ida_bytes.get_flags(address)))
    except (AttributeError, TypeError):
        return not is_placeholder(idc.get_name(address), address)


def load_function_rows(root: Path) -> list[dict[str, str]]:
    path = root / "config/slus_01411/functions.csv"
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        if tuple(reader.fieldnames or ()) != FUNCTION_FIELDS:
            raise ExportError(f"{path}: unexpected function fields")
        return list(reader)


def semantic_map_paths(root: Path) -> list[Path]:
    """Return the main semantic map followed by its per-family files."""
    paths = [root / SEMANTIC_MAP]
    directory = root / SEMANTIC_MAP_DIRECTORY
    if directory.is_dir():
        paths.extend(sorted(directory.glob("*.csv")))
    return paths


def load_semantic_names(root: Path) -> dict[tuple[str, int], str]:
    names: dict[tuple[str, int], str] = {}
    for path in semantic_map_paths(root):
        with path.open("r", encoding="utf-8", newline="") as handle:
            reader = csv.DictReader(handle)
            if tuple(reader.fieldnames or ()) != SEMANTIC_FIELDS:
                raise ExportError(f"{path}: unexpected semantic map fields")
            for row in reader:
                names[(row["kind"], int(row["address"], 0))] = row["name"]
    return names


def load_project_symbol_addresses(root: Path) -> set[int]:
    path = root / "config/slus_01411/symbols.txt"
    addresses: set[int] = set()
    for line in path.read_text(encoding="utf-8").splitlines():
        match = SYMBOL_PATTERN.match(line)
        if match:
            addresses.add(int(match.group(2), 0))
    return addresses


def write_csv(
    path: Path,
    fields: tuple[str, ...],
    rows: list[dict[str, str]],
) -> None:
    with path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fields, lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)


def exported_functions(
    rows: list[dict[str, str]],
) -> tuple[list[dict[str, str]], list[dict[str, str]]]:
    exported = []
    changes = []
    for original in rows:
        row = dict(original)
        address = int(row["address"], 0)
        if ida_bytes.is_mapped(address):
            name = idc.get_name(address)
            function = ida_funcs.get_func(address)
            if (
                function is not None
                and function.start_ea == address
                and IDENTIFIER.fullmatch(name or "")
                and not is_placeholder(name, address)
            ):
                row["name"] = name
        exported.append(row)
        if (
            row["name"] != original["name"]
            and original["module"] == "game"
        ):
            changes.append(
                {
                    "kind": "function",
                    "address": original["address"],
                    "name": row["name"],
                    "confidence": "review",
                    "evidence": "IDA 9.x export",
                    "notes": (
                        f"Review before replacing {original['name']} in the "
                        "semantic registry."
                    ),
                }
            )
    return exported, changes


def exported_names() -> list[tuple[int, str, bool]]:
    result = []
    for address, name in idautils.Names():
        if (
            not ida_bytes.is_mapped(address)
            or not IDENTIFIER.fullmatch(name)
            or is_placeholder(name, address)
            or not is_user_name(address)
        ):
            continue
        function = ida_funcs.get_func(address)
        result.append(
            (
                address,
                name,
                function is not None and function.start_ea == address,
            )
        )
    return sorted(result)


def main() -> None:
    try:
        root = find_project_root()
        if root is None:
            return
        selected = ida_kernwin.ask_file(
            True,
            "*.csv",
            "Choose the exported functions.csv path",
        )
        if not selected:
            return
        functions_path = Path(selected).resolve()
        output = functions_path.parent
        output.mkdir(parents=True, exist_ok=True)

        current_rows = load_function_rows(root)
        functions, semantic_changes = exported_functions(current_rows)
        write_csv(functions_path, FUNCTION_FIELDS, functions)

        names = exported_names()
        symbols_path = output / "symbols.txt"
        with symbols_path.open("w", encoding="utf-8") as handle:
            for address, name, function in names:
                suffix = " // type:func" if function else ""
                handle.write(f"{name} = 0x{address:08X};{suffix}\n")

        accepted = load_semantic_names(root)
        known_global_addresses = load_project_symbol_addresses(root)
        function_addresses = {
            int(row["address"], 0) for row in current_rows
        }
        for address, name, function in names:
            if function or address in function_addresses:
                continue
            if (
                accepted.get(("global", address)) == name
                and address in known_global_addresses
            ):
                continue
            semantic_changes.append(
                {
                    "kind": "global",
                    "address": f"0x{address:08X}",
                    "name": name,
                    "confidence": "review",
                    "evidence": "IDA 9.x export",
                    "notes": "Review address, ownership, and meaning before applying.",
                }
            )
        semantic_changes.sort(
            key=lambda row: (int(row["address"], 0), row["kind"])
        )
        # Review rows are diffed against the merged map (main file plus
        # notes/semantic-symbol-map.d/*.csv) but always written as one file.
        semantic_path = output / "semantic-symbol-map.csv"
        write_csv(semantic_path, SEMANTIC_FIELDS, semantic_changes)

        ida_kernwin.msg(
            "memories-decomp symbol export complete:\n"
            f"  functions: {functions_path}\n"
            f"  symbols: {symbols_path}\n"
            f"  semantic review rows: {semantic_path} "
            f"({len(semantic_changes)})\n"
        )
    except (
        ExportError,
        OSError,
        UnicodeError,
        ValueError,
        KeyError,
        csv.Error,
    ) as error:
        ida_kernwin.warning(f"memories-decomp symbol export failed:\n{error}")


if __name__ == "__main__":
    main()
