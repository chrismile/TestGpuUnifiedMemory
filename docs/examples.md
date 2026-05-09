# Example data

## RTX 3090 (Ubuntu 24.04, driver 595.58.03)

```
Running on NVIDIA GeForce RTX 3090
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
  Time copy cudaMallocManaged2: 48.1439ms
  Time copy cudaMallocHost: 73.2223ms
 #Accesses: 10
  Time copy cudaMalloc: 5.63693ms
  Time copy cudaMallocManaged: 38.8158ms
  Time copy cudaMallocManaged2: 5.93454ms
  Time copy cudaMallocHost: 45.3252ms
 #Accesses: 100
  Time copy cudaMalloc: 1.72966ms
  Time copy cudaMallocManaged: 5.1667ms
  Time copy cudaMallocManaged2: 1.77026ms
  Time copy cudaMallocHost: 42.5057ms

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
- Vulkan cannot reach performance of CUDA and SYCL with device local memory, even for a large amount of iterations.
- Vulkan has a significant overhead for single-access patterns.


## Intel Alder Lake iGPU + RTX 3080 Ti (Fedora 44, Mesa 26.0.6, NVIDIA driver 595.71.05)

```
Running on Intel(R) Iris(R) Xe Graphics (ADL GT2)
Tested API: Vulkan
 #Accesses: 1
  Time copy memory type 0 (device local): 155.376ms
  Time copy memory type 1 (device local, host visible, host coherent): 179.414ms
  Time copy memory type 2 (device local, host visible, host coherent, host cached): 169.648ms
 #Accesses: 10
  Time copy memory type 0 (device local): 32.7735ms
  Time copy memory type 1 (device local, host visible, host coherent): 37.2377ms
  Time copy memory type 2 (device local, host visible, host coherent, host cached): 32.8768ms
 #Accesses: 100
  Time copy memory type 0 (device local): 20.9291ms
  Time copy memory type 1 (device local, host visible, host coherent): 20.9711ms
  Time copy memory type 2 (device local, host visible, host coherent, host cached): 20.9377ms

Tested API: SYCL
 #Accesses: 1
  Time copy malloc_device: 35.8211ms
  Time copy malloc_shared: 36.3648ms
  Time copy malloc_host: 36.1415ms
 #Accesses: 10
  Time copy malloc_device: 19.9553ms
  Time copy malloc_shared: 19.5859ms
  Time copy malloc_host: 19.9491ms
 #Accesses: 100
  Time copy malloc_device: 18.1217ms
  Time copy malloc_shared: 18.1452ms
  Time copy malloc_host: 18.1924ms
```

```
Running on NVIDIA GeForce RTX 3080 Ti Laptop GPU
Tested API: Vulkan
 #Accesses: 1
  Time copy memory type 0: 306.749ms
  Time copy memory type 1 (device local): 262.424ms
  Time copy memory type 3 (host visible, host coherent): 286.59ms
  Time copy memory type 4 (host visible, host coherent, host cached): 266.762ms
  Time copy memory type 5 (device local, host visible, host coherent): 240.789ms
 #Accesses: 10
  Time copy memory type 0: 71.8811ms
  Time copy memory type 1 (device local): 32.8247ms
  Time copy memory type 3 (host visible, host coherent): 68.2201ms
  Time copy memory type 4 (host visible, host coherent, host cached): 70.5359ms
  Time copy memory type 5 (device local, host visible, host coherent): 23.0477ms
 #Accesses: 100
  Time copy memory type 0: 44.206ms
  Time copy memory type 1 (device local): 5.65606ms
  Time copy memory type 3 (host visible, host coherent): 43.6592ms
  Time copy memory type 4 (host visible, host coherent, host cached): 43.7632ms
  Time copy memory type 5 (device local, host visible, host coherent): 4.97956ms

Tested API: CUDA
 #Accesses: 1
  Time copy cudaMalloc: 46.2025ms
  Time copy cudaMallocManaged: 204.937ms
  Time copy cudaMallocManaged2: 47.1493ms
  Time copy cudaMallocHost: 74.4302ms
 #Accesses: 10
  Time copy cudaMalloc: 6.93021ms
  Time copy cudaMallocManaged: 22.1757ms
  Time copy cudaMallocManaged2: 6.98052ms
  Time copy cudaMallocHost: 43.9352ms
 #Accesses: 100
  Time copy cudaMalloc: 2.96946ms
  Time copy cudaMallocManaged: 4.39886ms
  Time copy cudaMallocManaged2: 2.98552ms
  Time copy cudaMallocHost: 41.7786ms

Tested API: SYCL
 #Accesses: 1
  Time copy malloc_device: 43.8534ms
  Time copy malloc_shared: 43.9168ms
  Time copy malloc_host: 84.731ms
 #Accesses: 10
  Time copy malloc_device: 6.67084ms
  Time copy malloc_shared: 6.6778ms
  Time copy malloc_host: 44.7929ms
 #Accesses: 100
  Time copy malloc_device: 2.94792ms
  Time copy malloc_shared: 2.94323ms
  Time copy malloc_host: 42.0256ms
```

