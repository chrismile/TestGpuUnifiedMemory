# Example data

## RTX 3090 (Ubuntu 24.04, driver 595.58.03)

```
Running on NVIDIA GeForce RTX 3090
Tested API: Vulkan
 #Accesses: 1
  Time copy memory type 0: 363.182ms
  Time copy memory type 1 (device local): 319.081ms
  Time copy memory type 1 (device local; host pointer import): 124.698ms
  Time copy memory type 3 (host visible, host coherent): 69.7461ms
  Time copy memory type 4 (host visible, host coherent, host cached): 71.2197ms
  Time copy memory type 5 (device local, host visible, host coherent): 49.079ms
 #Accesses: 10
  Time copy memory type 0: 74.6425ms
  Time copy memory type 1 (device local): 34.2998ms
  Time copy memory type 1 (device local; host pointer import): 14.2461ms
  Time copy memory type 3 (host visible, host coherent): 45.0781ms
  Time copy memory type 4 (host visible, host coherent, host cached): 45.1069ms
  Time copy memory type 5 (device local, host visible, host coherent): 6.06605ms
 #Accesses: 100
  Time copy memory type 0: 45.275ms
  Time copy memory type 1 (device local): 4.57078ms
  Time copy memory type 1 (device local; host pointer import): 2.5281ms
  Time copy memory type 3 (host visible, host coherent): 42.3353ms
  Time copy memory type 4 (host visible, host coherent, host cached): 42.3344ms
  Time copy memory type 5 (device local, host visible, host coherent): 1.74017ms
 #Accesses: 100 (upload excluded)
  Time copy memory type 0: 42.0212ms
  Time copy memory type 1 (device local): 1.26382ms
  Time copy memory type 1 (device local; host pointer import): 1.26671ms
  Time copy memory type 3 (host visible, host coherent): 42.181ms
  Time copy memory type 4 (host visible, host coherent, host cached): 42.0488ms
  Time copy memory type 5 (device local, host visible, host coherent): 1.26629ms

Tested API: CUDA
 #Accesses: 1
  Time copy cudaMalloc: 43.5038ms
  Time copy cudaMallocManaged: 389.818ms
  Time copy cudaMallocManaged2: 47.2049ms
  Time copy cudaMallocHost: 72.2424ms
 #Accesses: 10
  Time copy cudaMalloc: 5.92232ms
  Time copy cudaMallocManaged: 41.0296ms
  Time copy cudaMallocManaged2: 6.13827ms
  Time copy cudaMallocHost: 45.3507ms
 #Accesses: 100
  Time copy cudaMalloc: 1.73097ms
  Time copy cudaMallocManaged: 5.24812ms
  Time copy cudaMallocManaged2: 1.76442ms
  Time copy cudaMallocHost: 42.608ms
 #Accesses: 100 (upload excluded)
  Time copy cudaMalloc: 1.33474ms
  Time copy cudaMallocManaged: 3.10659ms
  Time copy cudaMallocManaged2: 1.32277ms
  Time copy cudaMallocHost: 42.3079ms

Tested API: SYCL
 #Accesses: 1
  Time copy malloc_device: 42.4771ms
  Time copy malloc_shared: 44.272ms
  Time copy malloc_host: 69.4724ms
 #Accesses: 10
  Time copy malloc_device: 5.60671ms
  Time copy malloc_shared: 5.57383ms
  Time copy malloc_host: 45.0528ms
 #Accesses: 100
  Time copy malloc_device: 1.72982ms
  Time copy malloc_shared: 1.72967ms
  Time copy malloc_host: 42.5118ms
 #Accesses: 100 (upload excluded)
  Time copy malloc_device: 1.31518ms
  Time copy malloc_shared: 1.29934ms
  Time copy malloc_host: 42.2655ms
```

Interesting outcomes:
- Allocating the Vulkan staging buffer for the "device local" memory type has a significant overhead.
- Using VK_EXT_external_memory_host to import the host pointer improves performance considerably, but has some overhead.
- When only accessed once, using non-device-local memory is faster than both approaches above.
- The performance winner for Vulkan is device local, host visible memory. This memory type is only available with ReBAR.
- The device local memory types have equal performance when excluding data upload (independently of host visible or not).


## Intel Alder Lake iGPU + RTX 3080 Ti (Fedora 44, Mesa 26.0.6, NVIDIA driver 595.71.05)

```
Running on Intel(R) Iris(R) Xe Graphics (ADL GT2)
TODO
```

```
Running on NVIDIA GeForce RTX 3080 Ti Laptop GPU
TODO
```


## Intel Alder Lake iGPU + RTX 3080 Ti (Windows 11, Intel driver 101.7085, NVIDIA driver 596.36)

```
Running on Intel(R) Iris(R) Xe Graphics
TODO
```

```
Running on NVIDIA GeForce RTX 3080 Ti Laptop GPU
TODO
```
