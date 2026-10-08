import pathlib
import re
import subprocess
import sys
import tempfile

binary = str(pathlib.Path(sys.argv[1]).resolve())

def run(*args, ok=True):
    result = subprocess.run([binary, *args], text=True, capture_output=True, timeout=100)
    assert (result.returncode == 0) == ok, (args, result.stdout, result.stderr)
    return result.stdout

assert "--headless" in run("--help")
for args in [("--seed", "-1"), ("--seed", "4294967296"), ("--players", "1"),
             ("--players", "10"), ("--difficulty", "bad"), ("--seed",), ("--unknown",),
             ("--headless", "--humans", "1"), ("--headless", "--smoke", "5"),
             ("--verify", "missing.odr"), ("--screenshot", "unused.png")]:
    run(*args, ok=False)

with tempfile.TemporaryDirectory(prefix="duel-cli-") as directory:
    path = pathlib.Path(directory) / "match.odr"
    first = run("--headless", "--seed", "123", "--rounds", "1", "--difficulty", "easy", "--record", str(path))
    verified = run("--verify", str(path))
    assert re.search(r"hash=(\d+)", first).group(1) == re.search(r"hash=(\d+)", verified).group(1)
    second = run("--headless", "--seed", "123", "--rounds", "1", "--difficulty", "easy")
    assert first.split("ai_candidates=")[0] == second.split("ai_candidates=")[0]
    assert not pathlib.Path(str(path) + ".tmp").exists()
    # Atomic replacement of an existing file must work too.
    run("--headless", "--seed", "123", "--rounds", "1", "--difficulty", "easy", "--record", str(path))
    data = path.read_text()
    path.write_text(data[:-5])
    run("--verify", str(path), ok=False)
print("PASS CLI validation, deterministic matches, recording and replay corruption")
