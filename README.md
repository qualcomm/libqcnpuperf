# libqcnpuperf

libqcnpuperf is a lightweight libary providing apis to collect NPU Metrics.
This library can be easily integrated into a simple console application or a profiling tools like NvTop or generate metrics for perfetto or integrate into CI loops or any other applications.
This library provides 3 simple apis.
qcom_dsp_init()
qcom_dsp_get_prof_data()
qcom_dsp_deinit()

## Branches

**main**: Primary development branch. Contributors should develop submissions based on this branch, and submit pull requests to this branch.

## Requirements

 ### System Requirements
- Linux (Debian/Ubuntu)
- Qualcomm platform with DSP/NPU support
### Dependencies
This project depends on FastRPC for CPU ↔ DSP communication.

Install required packages (Debian):

```bash
sudo apt update
sudo apt install libfastrpc-dev
```

Install required packages (Ubuntu):

```bash
sudo apt update
sudo apt install qcom-fastrpc-dev qcom-dspservices-headers-dev
```

## Installation Instructions

```bash
git clone https://github.com/qualcomm/libqcnpuperf.git
cd libqcnpuperf
```

## Build Instructions
This project uses CMake.
### 1. Create a build directory
```bash
mkdir -p build
cd build
```
### 2. Configure
```bash
cmake ..
```
### 3. Build
```bash
make -j$(nproc)
```
### 4. (Optional) Install
```bash
sudo make install
```
### Building without the ncurses sample
If you don't want the sample binary:
```bash
cmake -DQCNPU_PERF_BUILD_CLI=OFF ..
```

### Building without the daemon
```bash
cmake -DQCNPU_PERF_BUILD_DAEMON=OFF ..
```

## Runtime Notes
Ensure FastRPC is available:
```bash
ls /dev/fastrpc*
```

## Usage

```c
qcom_dsp_init();
qcom_dsp_get_prof_data(...);
qcom_dsp_deinit();
```

## qcnpuperfd — Stats Daemon

`qcnpuperfd` is a systemd-native daemon that polls NPU metrics once per second and
writes them to a file, updating it atomically so readers always see a consistent snapshot.

### Output format

Plain text, one metric per line. Default output path: `/tmp/qcnpuperf_stats`.

```
q6_utilization=<float>   # effective Q6 clock vs max Q6 clock (%)
q6_clock_khz=<uint>      # average Q6 clock frequency in KHz
hvx_utilization=<float>  # HVX utilization vs max Q6 clock (%)
hmx_utilization=<float>  # HMX (NPU matrix engine) utilization vs max Q6 clock (%)
```

### Running directly

```bash
qcnpuperfd                   # writes to /tmp/qcnpuperf_stats
qcnpuperfd /run/npu/stats    # custom output path
```

### Running as a systemd service

```bash
sudo systemctl enable qcnpuperfd
sudo systemctl start qcnpuperfd
journalctl -u qcnpuperfd -f  # follow logs
```

The service restarts automatically on failure (`Restart=on-failure`, `RestartSec=5s`).

## Development

How to develop new features/fixes for the software. Maybe different than "usage". Also provide details on how to contribute via a [CONTRIBUTING.md file](CONTRIBUTING.md).

## License

libqcnpuperf is licensed under the [BSD-3-clause License](https://spdx.org/licenses/BSD-3-Clause.html). See [LICENSE.txt](LICENSE.txt) for the full license text.
