# Verification

Measured locally on 2026-10-08, Linux x86_64, Intel Core i5-1145G7, GCC 16.2.1,
Clang 23.1.1, CMake 4.4.4. The installed SDL2 is sdl2-compat 2.32.74 backed by SDL3;
graphical automation used the dummy video driver and software renderer.

## Checks performed

| Check | Result |
|---|---|
| GCC release build, warnings treated as errors | Passed |
| Core simulation suite | Passed, 976 assertions |
| CLI integration suite | Passed |
| SDL input and rendering integration suite | Passed |
| Clang release headless build and tests | Passed |
| ASan, UBSan and leak checks, core, CLI and SDL GUI suites | Passed outside the tracing sandbox |
| 12 three-round AI matches | Completed: 2, 5 and 9 players, seeds 0, 1, 42 and 4294967295, easy difficulty |
| GUI-recorded three-round demo | Five recorded shots, all checkpoints verified headlessly |

The core suite compares optimized terrain with a full-grid reference after
**every gravity tick**, checks all three terrain styles and both world edges,
verifies simulation results under 30/60/144 FPS presentation schedules, exercises
all five weapons, and covers malformed input, illegal commands, exhausted turn
limits, AI search reproducibility, nine-player elimination, replay mismatches,
partial replays, and repeated restarts.

The SDL suite injects normal SDL key events to navigate setup, change player
count, start a human match, hold angle/power keys, select a weapon, fire twice,
pause/resume, open controls, return to setup, capture a PNG, and play back a saved
shot. It asserts the actual recorded angle, power, weapon, and single-shot count.
Menus, controls, the battlefield, and the winner screen were visually inspected.

## Terrain benchmark

Command: `./build/release/terrain_bench` in the optimized GCC release build.
Scenario: a 1067×600 grid, flat surface at y=300, a radius-45 underground crater
at (533, 365), settling completely in 92 ticks.

| Implementation | Cells visited | Sample elapsed time |
|---|---:|---:|
| Full-grid reference | 58,800,236 | 77.813 ms |
| Dirty-column implementation | 3,864,748 | 20.886 ms |

Both produced identical cells and tick counts. This workload visits **15.21× fewer
cells**, with **3.73× lower elapsed time** in this representative run. Times depend
on hardware and load; this is a terrain-gravity benchmark, not an overall FPS claim.
The benchmark checks equivalence and reports timings without a flaky speed
threshold. Rendering separately caches a texture and skips terrain updates when
column revision counters are unchanged.

## Reproduction and limits

```sh
make test
make sanitize
make benchmark
SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software ./program \
  --seed 42 --smoke 1800 --record build/demo.odr
./program --verify build/demo.odr
```

LeakSanitizer cannot operate under the sandbox's process tracing. Tests were
rerun outside that sandbox with leak detection enabled; it was not disabled to
obtain a passing result.

Native desktop input, fullscreen transitions, GPU rendering, Windows/macOS,
and cross-machine replay determinism have not been manually verified. The Linux
archive requires compatible host runtime libraries; it is not a self-contained
portable binary. The GitHub Actions workflow is configured but has not been run
on a hosted runner in this session. No network multiplayer is implemented.
