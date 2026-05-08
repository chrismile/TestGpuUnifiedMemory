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
- CUDA


## How to build

```sh
git submodule update --init --recursive
mkdir build
cd build
cmake ..
cmake --build .
```
