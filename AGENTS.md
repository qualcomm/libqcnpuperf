# AGENTS.md

Single-library C project providing NPU/DSP performance metrics on Qualcomm Linux platforms via FastRPC. No monorepo, no test suite, no scripting layer beyond CMake.

## Build

```bash
# Prerequisites (aarch64 Debian with Qualcomm BSP)
sudo apt install libfastrpc-dev libncurses-dev

(aarch64 Ubuntu with Qualcomm BSP)
sudo apt install qcom-fastrpc1 qcom-fastrpc-dev qcom-dspservices-headers-dev

mkdir -p build && cd build
cmake ..
make -j$(nproc)
sudo make install   # optional
```

Key CMake options:

| Option | Default | Effect |
|---|---|---|
| `QCNPU_PERF_BUILD_CLI` | `ON` | ncurses TUI sample (`qcnpuperf_cli`) |
| `QCNPU_PERF_BUILD_DAEMON` | `ON` | stats daemon (`qcnpuperfd`) |
| `FASTRPC_INCLUDE_DIR` | auto | override path to `AEEStdDef.h` |
| `FASTRPC_LIBRARY` | auto | override path to `libcdsprpc` |

**README.md documents `-DQCNPU_PERF_BUILD_TUI=OFF` — that flag does not exist. The correct flag is `-DQCNPU_PERF_BUILD_CLI=OFF`. Trust CMakeLists.txt over README.**

**Outputs:** `libqcnpuperf.so` (SOVERSION 1), `qcnpuperf.pc`, `inc/qcom_dsp.h`, optional `qcnpuperf_cli`, optional `qcnpuperfd`.

### Debian package

```bash
dpkg-buildpackage -us -uc -b
```

The `.deb` build uses `cmake` with `QCNPU_PERF_BUILD_CLI=OFF` and `QCNPU_PERF_BUILD_DAEMON=ON`. The CLI binary is **not** included in any `.deb` package; only `qcnpuperfd` is packaged as a binary.

## Testing

**No automated test suite exists.** `ctest` is not configured. The PR template mentions tests but none have been written yet.

Runtime verification requires real Qualcomm hardware with FastRPC loaded:
```bash
ls /dev/fastrpc*          # must exist before the library can open a DSP session
./build/qcnpuperf_cli     # ncurses TUI sample
```

## Generated Files — Do Not Hand-Edit

`src/dspquery_stub.c` and `inc/dspquery_stub.h` are QAIC (Qualcomm AI Compiler Interface) generated FastRPC stubs. The IDL source is **not in the repo** — only the generated outputs are checked in. Regenerate with the `qaic` tool when the DSP interface changes.

## CI Checks (GitHub Actions)

All PRs run `qcom-preflight-checks.yml` and `build-deb.yml`:

- **Semgrep** static security scan — must pass before merge
- **Copyright header check** — every source file must carry the Qualcomm BSD-3-Clause header block
- **Commit email check** — non-noreply address required
- Dependency review, repolinter
- **Debian package build** — runs on a real `ubuntu-24.04-arm` runner (not emulated); installs `qcom-fastrpc-dev qcom-dspservices-headers-dev` from the `ubuntu-qcom-iot/qcom-ppa` PPA

No local linter config (no `.clang-format`, `.clang-tidy`, pre-commit hooks).

## Contribution Requirements

- DCO sign-off on every commit: `git commit -s -m "message"`
- Branch from `main`, PR to `main`
- Copyright header required on all new source files (CI enforced)

## Runtime Requirements

- Linux on Qualcomm aarch64 (Debian/Ubuntu)
- `/dev/fastrpc*` device nodes (FastRPC kernel driver)
- `libcdsprpc.so` at runtime
- `libsysmonquery_skel.so` loadable on the DSP side (vendor BSP); loaded via URI `file:///libsysmonquery_skel.so?sysmonquery_skel_handle_invoke&_modver=1.0`

## Code Quirks

- **`DSP_MAX` is not a count** — it equals `CDSP_DOMAIN_ID` (the last valid index), so the static arrays in `qcom_dsp.c` are sized `[DSP_MAX]` meaning two slots: index 0 (ADSP) and index 1 (CDSP/NPU0).
- **`qcnpuperf_cli.c` has unreachable cleanup** — the `while(true)` poll loop has no exit, so `qcom_dsp_deinit` and `endwin()` after it are dead code.
- **`inc/dspquery_stub.h` declares five `local_remote_handle64_*` function pointer externs** that are never defined or used anywhere — QAIC boilerplate, ignore them.
- Consumers must link against both `-lqcnpuperf` and `-lcdsprpc` (both are listed in `qcnpuperf.pc`).
