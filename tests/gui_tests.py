import os
import pathlib
import re
import subprocess
import sys
import tempfile

binary = str(pathlib.Path(sys.argv[1]).resolve())
env = dict(os.environ, SDL_VIDEODRIVER="dummy", SDL_RENDER_DRIVER="software")
with tempfile.TemporaryDirectory(prefix="duel-gui-") as directory:
    root = pathlib.Path(directory)
    script = root / "keys.txt"
    replay = root / "match.odr"

    def run(frames, keys, *extra):
        script.write_text(keys)
        result = subprocess.run([binary, "--seed", "42", "--smoke", str(frames), "--humans", "2",
                                 "--smoke-input", str(script), "--record", str(replay), *extra],
                                env=env, text=True, capture_output=True, timeout=60, cwd=root)
        assert result.returncode == 0, (result.stdout, result.stderr)
        return result.stdout

    # Menu navigation, match setup, pause, controls, return, and restart.
    result = run(12, "0 press Down\n1 press Return\n2 press Up\n3 press Return\n4 press Escape\n5 press Down\n6 press Down\n7 press Return\n8 press Escape\n9 press Down\n10 press Return\n")
    assert "screen=0 players=3 humans=2" in result, result
    # Held aim/power input, double-fire rejection, weapon selection, pause/resume.
    result = run(700, "0 press Return\n1 down Left\n1 down Up\n31 up Left\n31 up Up\n32 press 2\n33 press Space\n34 press Space\n35 press Escape\n45 press Escape\n46 press F1\n47 press F5\n",
                 "--screenshot", str(root / "game.png"))
    assert "completed_shots=1 screen=1" in result, result
    assert (root / "game.png").read_bytes().startswith(b"\x89PNG\r\n\x1a\n")
    rows = replay.read_text().splitlines()
    shot = rows[3].split()
    assert abs(float(shot[2]) - 65) < 0.001, shot
    assert abs(float(shot[3]) - 80) < 0.001, shot
    assert shot[4] == "1", shot  # key 2 is the rocket
    verified = subprocess.run([binary, "--verify", str(replay)], text=True, capture_output=True)
    assert verified.returncode == 0, verified.stderr
    # Replay playback uses the GUI's fixed-step loop and verifies its checkpoint.
    playback = subprocess.run([binary, "--replay", str(replay), "--smoke", "700"], env=env,
                              text=True, capture_output=True, timeout=60, cwd=root)
    assert playback.returncode == 0 and "completed_shots=1" in playback.stdout, playback.stderr
print("PASS SDL menus, human controls, pause, restart, screenshot and replay playback")
