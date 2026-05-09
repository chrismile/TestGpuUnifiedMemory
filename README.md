# TestGpuUnifiedMemory

Test app for performance of unified memory on GPUs.

The app tests the following access pattern:
- Write to a buffer on the CPU.
- Read from a buffer on the GPU.

Performance is tested for multiple scenarios:
- Read from a buffer once or multiple times on the GPU after writing on the CPU.
- Use unified memory or two buffers with explicit host-to-device copies.

Currently, the following apps are supported:
- Vulkan
- SYCL (Intel, NVIDIA, AMD)
- CUDA (NVIDIA)
- CUDA (AMD; NVIDIA support broken in ROCm SDK as of 2026-05-09)


## How to build

```sh
git submodule update --init --recursive
mkdir build
cd build
cmake ..
cmake --build .
```


### SYCL support

For SYCL support, please add the CMake arguments below when configuring the project.
`ONEAPI_PATH` needs to point to a DPC++ compiler build obtained from https://github.com/intel/llvm.
`ONEAPI_SYCL_TARGETS` can be adapted to the required SYCL targets depending on the tested GPUs.

```sh
-DSUPPORT_ONEAPI=ON -DONEAPI_PATH=<...> -DONEAPI_SYCL_TARGETS="spir64;nvptx64-nvidia-cuda"
```


### HIP support

For HIP support, the ROCm SDK needs to be obtained. One option on Linux is to follow the guide at
https://rocm.docs.amd.com/projects/install-on-linux/en/latest/install/rocm-runfile-installer.html.

```sh
wget https://repo.radeon.com/rocm/installer/rocm-runfile-installer/rocm-rel-7.2.3/ubuntu/24.04/rocm-installer_1.2.8.70203-61-90~24.04.runmkdir build
bash rocm-installer_1.2.8.70203-61-90~24.04.run untar <...>/rocm-dir
```

Afterward, please add the CMake arguments below when configuring the project.
`CMAKE_HIP_PLATFORM` can be set to `amd`, `nvidia` or `spirv`.
As of 2026-05-09, `CMAKE_HIP_PLATFORM=nvidia` seems to be broken for ROCm SDK version 7.2.3 with the NVIDIA platform.
`hipGetDeviceCount` returns `hipErrorNoDevice`, even though ignoring the error leads to normal program flow.

```sh
-Dhip_DIR=<...>/rocm-dir/rocm-7.2.3/lib/cmake/hip -DCMAKE_HIP_COMPILER_ROCM_ROOT=<...>/rocm-dir/rocm-7.2.3 -DCMAKE_HIP_PLATFORM=nvidia
```


## Example data

Example data can be found in the file `docs/examples.md`.
