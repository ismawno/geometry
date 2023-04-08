import subprocess
import os
from argparse import ArgumentParser
from typing import Union


def add_dependencies(
    dependencies: Union[str, list[str]], exec: str, branch: str, folder: str = "."
) -> None:
    if isinstance(dependencies, str):
        dependencies = [dependencies]

    for dependency in dependencies:
        if os.path.exists(dependency):
            print(f"{dependency} dependency already satisfied. Skipping...")
            continue
        subprocess.run(
            [
                "git",
                "submodule",
                "add",
                f"https://github.com/ismawno/{dependency}.git",
                f"{folder}/{dependency}",
            ]
        )
        prev_dir = os.getcwd()
        os.chdir(f"{folder}/{dependency}")
        subprocess.run(["git", "checkout", branch])
        os.chdir(prev_dir)

        if not os.path.exists(f"{folder}/{dependency}/scripts"):
            print(f"No further dependencies for {dependency}")
            continue

        subprocess.run(
            [
                exec,
                f"{folder}/{dependency}/scripts/fetch_dependencies.py",
                "--python-executable",
                exec,
                "--branch",
                branch,
                ".",
            ]
        )


def main() -> None:
    parser = ArgumentParser(
        description="Fetch the library dependencies as git submodules."
    )
    parser.add_argument("src", metavar="root-relpath", type=str)
    parser.add_argument(
        "--python-executable",
        dest="exec",
        default="python",
        type=str,
        help="the python executable to be used. Defaults to 'python'",
    )
    parser.add_argument(
        "--branch",
        dest="branch",
        default="master",
        type=str,
        help="the branch all added submodules will switch to when added. Defaults to 'master'",
    )

    args = parser.parse_args()
    root_path = os.path.abspath(args.src)
    os.chdir(root_path)

    add_dependencies(["vec-2D", "debug-toolg", "ini-parser"], args.exec, args.branch)


if __name__ == "__main__":
    main()
