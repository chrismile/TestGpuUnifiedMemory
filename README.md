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

### RTX 3090 (Ubuntu 24.04, driver 595.58.03)

```
Tested API: Vulkan
#Accesses: 1
Time copy memory type 0: 406.394ms
Time copy memory type 1 (device local): 353.621ms
Time copy memory type 3 (host visible, host coherent): 406.502ms
Time copy memory type 4 (host visible, host coherent, host cached): 405.269ms
Time copy memory type 5 (device local, host visible, host coherent): 362.001ms
#Accesses: 10
Time copy memory type 0: 78.2294ms
Time copy memory type 1 (device local): 37.3238ms
Time copy memory type 3 (host visible, host coherent): 78.6133ms
Time copy memory type 4 (host visible, host coherent, host cached): 78.601ms
Time copy memory type 5 (device local, host visible, host coherent): 37.8709ms
#Accesses: 100
Time copy memory type 0: 45.5563ms
Time copy memory type 1 (device local): 4.968ms
Time copy memory type 3 (host visible, host coherent): 45.5364ms
Time copy memory type 4 (host visible, host coherent, host cached): 45.493ms
Time copy memory type 5 (device local, host visible, host coherent): 4.97106ms

Tested API: CUDA
#Accesses: 1
Time copy cudaMalloc: 43.4185ms
Time copy cudaMallocManaged: 386.792ms
Time copy cudaMallocManaged2: 238.621ms
Time copy cudaMallocHost: 73.2223ms
#Accesses: 10
Time copy cudaMalloc: 5.63693ms
Time copy cudaMallocManaged: 38.8158ms
Time copy cudaMallocManaged2: 25.2372ms
Time copy cudaMallocHost: 45.3252ms
#Accesses: 100
Time copy cudaMalloc: 1.72966ms
Time copy cudaMallocManaged: 5.1667ms
Time copy cudaMallocManaged2: 3.703ms
Time copy cudaMallocHost: 42.5057ms

No HIP device detected

Tested API: SYCL
#Accesses: 1
Time copy malloc_device: 42.4554ms
Time copy malloc_shared: 43.5771ms
Time copy malloc_host: 69.5966ms
#Accesses: 10
Time copy malloc_device: 5.64751ms
Time copy malloc_shared: 5.51246ms
Time copy malloc_host: 44.9697ms
#Accesses: 100
Time copy malloc_device: 1.71078ms
Time copy malloc_shared: 1.79893ms
Time copy malloc_host: 42.3978ms
```

Interesting outcomes:
- When comparing performance of CUDA and SYCL, `cudaMalloc` and `malloc_device` are comparably fast,
  while `malloc_shared` is faster than its CUDA equivalents.
- Vulkan cannot reach performance of CUDA and SYCL with device local memory, even for a large amount of iterations.
- Vulkan has a significant overhead for single-access patterns.
