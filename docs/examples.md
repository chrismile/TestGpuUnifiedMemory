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
Tested API: Vulkan
 #Accesses: 1
  Time copy memory type 0 (device local): 214.187ms
  Time copy memory type 0 (device local; host pointer import): 57.7116ms
  Time copy memory type 1 (device local, host visible, host coherent): 112.524ms
  Time copy memory type 2 (device local, host visible, host coherent, host cached): 114.838ms
 #Accesses: 10
  Time copy memory type 0 (device local): 37.7303ms
  Time copy memory type 0 (device local; host pointer import): 23.1568ms
  Time copy memory type 1 (device local, host visible, host coherent): 28.3511ms
  Time copy memory type 2 (device local, host visible, host coherent, host cached): 28.3593ms
 #Accesses: 100
  Time copy memory type 0 (device local): 21.4199ms
  Time copy memory type 0 (device local; host pointer import): 19.8306ms
  Time copy memory type 1 (device local, host visible, host coherent): 20.3037ms
  Time copy memory type 2 (device local, host visible, host coherent, host cached): 20.3616ms
 #Accesses: 100 (upload excluded)
  Time copy memory type 0 (device local): 19.4294ms
  Time copy memory type 0 (device local; host pointer import): 19.4978ms
  Time copy memory type 1 (device local, host visible, host coherent): 19.4827ms
  Time copy memory type 2 (device local, host visible, host coherent, host cached): 19.4388ms

Tested API: SYCL
 #Accesses: 1
  Time copy malloc_device: 36.0671ms
  Time copy malloc_shared: 36.8543ms
  Time copy malloc_host: 36.2221ms
 #Accesses: 10
  Time copy malloc_device: 19.7488ms
  Time copy malloc_shared: 19.5694ms
  Time copy malloc_host: 19.524ms
 #Accesses: 100
  Time copy malloc_device: 17.9118ms
  Time copy malloc_shared: 18.3875ms
  Time copy malloc_host: 17.9383ms
 #Accesses: 100 (upload excluded)
  Time copy malloc_device: 17.7569ms
  Time copy malloc_shared: 17.7693ms
  Time copy malloc_host: 17.7585ms
```

```
Running on NVIDIA GeForce RTX 3080 Ti Laptop GPU
Tested API: Vulkan
 #Accesses: 1
  Time copy memory type 0: 307.032ms
  Time copy memory type 1 (device local): 256.838ms
  Time copy memory type 1 (device local; host pointer import): 101.819ms
  Time copy memory type 3 (host visible, host coherent): 80.0639ms
  Time copy memory type 4 (host visible, host coherent, host cached): 80.3119ms
  Time copy memory type 5 (device local, host visible, host coherent): 60.5383ms
 #Accesses: 10
  Time copy memory type 0: 67.4429ms
  Time copy memory type 1 (device local): 28.3905ms
  Time copy memory type 1 (device local; host pointer import): 13.2914ms
  Time copy memory type 3 (host visible, host coherent): 45.4157ms
  Time copy memory type 4 (host visible, host coherent, host cached): 45.5219ms
  Time copy memory type 5 (device local, host visible, host coherent): 9.28395ms
 #Accesses: 100
  Time copy memory type 0: 44.1774ms
  Time copy memory type 1 (device local): 6.09576ms
  Time copy memory type 1 (device local; host pointer import): 4.54944ms
  Time copy memory type 3 (host visible, host coherent): 41.9646ms
  Time copy memory type 4 (host visible, host coherent, host cached): 42.1283ms
  Time copy memory type 5 (device local, host visible, host coherent): 4.1485ms
 #Accesses: 100 (upload excluded)
  Time copy memory type 0: 41.5882ms
  Time copy memory type 1 (device local): 3.58549ms
  Time copy memory type 1 (device local; host pointer import): 3.5876ms
  Time copy memory type 3 (host visible, host coherent): 41.5801ms
  Time copy memory type 4 (host visible, host coherent, host cached): 41.729ms
  Time copy memory type 5 (device local, host visible, host coherent): 3.58247ms

Tested API: CUDA
 #Accesses: 1
  Time copy cudaMalloc: 53.9125ms
  Time copy cudaMallocManaged: 270.723ms
  Time copy cudaMallocManaged2: 55.9306ms
  Time copy cudaMallocHost: 80.3793ms
 #Accesses: 10
  Time copy cudaMalloc: 8.56187ms
  Time copy cudaMallocManaged: 29.8884ms
  Time copy cudaMallocManaged2: 8.78143ms
  Time copy cudaMallocHost: 45.4089ms
 #Accesses: 100
  Time copy cudaMalloc: 4.07146ms
  Time copy cudaMallocManaged: 6.2637ms
  Time copy cudaMallocManaged2: 4.10862ms
  Time copy cudaMallocHost: 41.9793ms
 #Accesses: 100 (upload excluded)
  Time copy cudaMalloc: 3.57799ms
  Time copy cudaMallocManaged: 4.51844ms
  Time copy cudaMallocManaged2: 3.5894ms
  Time copy cudaMallocHost: 41.597ms

Tested API: SYCL
 #Accesses: 1
  Time copy malloc_device: 44.8767ms
  Time copy malloc_shared: 45.1878ms
  Time copy malloc_host: 87.8741ms
 #Accesses: 10
  Time copy malloc_device: 7.70984ms
  Time copy malloc_shared: 7.73838ms
  Time copy malloc_host: 46.2323ms
 #Accesses: 100
  Time copy malloc_device: 3.97919ms
  Time copy malloc_shared: 3.98846ms
  Time copy malloc_host: 42.0519ms
 #Accesses: 100 (upload excluded)
  Time copy malloc_device: 3.57019ms
  Time copy malloc_shared: 3.57164ms
  Time copy malloc_host: 41.5738ms
```


## Intel Alder Lake iGPU + RTX 3080 Ti (Windows 11, Intel driver 101.7085, NVIDIA driver 596.36)

```
Running on Intel(R) Iris(R) Xe Graphics
Tested API: Vulkan
 #Accesses: 1
  Time copy memory type 0 (device local): 151.074ms
  Time copy memory type 0 (device local; host pointer import): 61.4366ms
  Time copy memory type 1 (device local, host visible, host coherent): 53.1082ms
  Time copy memory type 2 (device local, host visible, host coherent, host cached): 53.5825ms
 #Accesses: 10
  Time copy memory type 0 (device local): 32.5721ms
  Time copy memory type 0 (device local; host pointer import): 23.4785ms
  Time copy memory type 1 (device local, host visible, host coherent): 22.4417ms
  Time copy memory type 2 (device local, host visible, host coherent, host cached): 22.509ms
 #Accesses: 100
  Time copy memory type 0 (device local): 20.1095ms
  Time copy memory type 0 (device local; host pointer import): 19.4957ms
  Time copy memory type 1 (device local, host visible, host coherent): 20.9516ms
  Time copy memory type 2 (device local, host visible, host coherent, host cached): 19.3482ms
 #Accesses: 100 (upload excluded)
  Time copy memory type 0 (device local): 19.1371ms
  Time copy memory type 0 (device local; host pointer import): 19.1376ms
  Time copy memory type 1 (device local, host visible, host coherent): 19.1536ms
  Time copy memory type 2 (device local, host visible, host coherent, host cached): 19.0192ms

Tested API: SYCL
 #Accesses: 1
  Time copy malloc_device: 38.7907ms
  Time copy malloc_shared: 37.6332ms
  Time copy malloc_host: 37.7336ms
 #Accesses: 10
  Time copy malloc_device: 20.7954ms
  Time copy malloc_shared: 20.7207ms
  Time copy malloc_host: 20.8774ms
 #Accesses: 100
  Time copy malloc_device: 19.1684ms
  Time copy malloc_shared: 19.1305ms
  Time copy malloc_host: 19.0843ms
 #Accesses: 100 (upload excluded)
  Time copy malloc_device: 18.8917ms
  Time copy malloc_shared: 18.988ms
  Time copy malloc_host: 18.9198ms
```

```
Running on NVIDIA GeForce RTX 3080 Ti Laptop GPU
Tested API: Vulkan
 #Accesses: 1
  Time copy memory type 0: 154.722ms
  Time copy memory type 1 (device local): 152.954ms
  Time copy memory type 1 (device local; host pointer import): 68.1271ms
  Time copy memory type 3 (host visible, host coherent): 76.7863ms
  Time copy memory type 4 (host visible, host coherent, host cached): 76.825ms
  Time copy memory type 5 (device local, host visible, host coherent): 61.2005ms
 #Accesses: 10
  Time copy memory type 0: 18.1424ms
  Time copy memory type 1 (device local): 18.7753ms
  Time copy memory type 1 (device local; host pointer import): 9.03518ms
  Time copy memory type 3 (host visible, host coherent): 45.0405ms
  Time copy memory type 4 (host visible, host coherent, host cached): 44.9901ms
  Time copy memory type 5 (device local, host visible, host coherent): 8.32815ms
 #Accesses: 100
  Time copy memory type 0: 4.59708ms
  Time copy memory type 1 (device local): 4.59004ms
  Time copy memory type 1 (device local; host pointer import): 3.18353ms
  Time copy memory type 3 (host visible, host coherent): 42.0226ms
  Time copy memory type 4 (host visible, host coherent, host cached): 42.0476ms
  Time copy memory type 5 (device local, host visible, host coherent): 3.56304ms
 #Accesses: 100 (upload excluded)
  Time copy memory type 0: 2.89529ms
  Time copy memory type 1 (device local): 2.98608ms
  Time copy memory type 1 (device local; host pointer import): 2.53903ms
  Time copy memory type 3 (host visible, host coherent): 41.6453ms
  Time copy memory type 4 (host visible, host coherent, host cached): 41.6783ms
  Time copy memory type 5 (device local, host visible, host coherent): 2.54125ms

Tested API: CUDA
 #Accesses: 1
  Time copy cudaMalloc: 55.3302ms
  Time copy cudaMallocManaged: 4281.01ms
  Time copy cudaMallocManaged2: 47.0591ms
  Time copy cudaMallocHost: 75.1908ms
 #Accesses: 10
  Time copy cudaMalloc: 7.63122ms
  Time copy cudaMallocManaged: 421.099ms
  Time copy cudaMallocManaged2: 7.06152ms
  Time copy cudaMallocHost: 45.3738ms
 #Accesses: 100
  Time copy cudaMalloc: 3.41961ms
  Time copy cudaMallocManaged: 37.5002ms
  Time copy cudaMallocManaged2: 3.43911ms
  Time copy cudaMallocHost: 42.3974ms
 #Accesses: 100 (upload excluded)
  Time copy cudaMalloc: 2.54208ms
  Time copy cudaMallocManaged: 7.45784ms
  Time copy cudaMallocManaged2: 2.97775ms
  Time copy cudaMallocHost: 42.1358ms

Tested API: SYCL
 #Accesses: 1
  Time copy malloc_device: 44.4029ms
  Time copy malloc_shared: 44.6814ms
  Time copy malloc_host: 76.4517ms
 #Accesses: 10
  Time copy malloc_device: 7.15753ms
  Time copy malloc_shared: 7.14263ms
  Time copy malloc_host: 45.1445ms
 #Accesses: 100
  Time copy malloc_device: 3.9883ms
  Time copy malloc_shared: 3.38912ms
  Time copy malloc_host: 42.2294ms
 #Accesses: 100 (upload excluded)
  Time copy malloc_device: 2.97281ms
  Time copy malloc_shared: 2.53931ms
  Time copy malloc_host: 41.9211ms
```
