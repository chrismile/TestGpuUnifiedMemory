# Example data

## RTX 3090 (Ubuntu 24.04, driver 595.58.03)

```
Tested API: Vulkan
 #Accesses: 1
  Time copy memory type 0: 95.0647ms
  Time copy memory type 1 (device local): 81.5425ms
  Time copy memory type 1 (device local; host pointer import): 35.7759ms
  Time copy memory type 3 (host visible, host coherent): 18.0757ms
  Time copy memory type 4 (host visible, host coherent, host cached): 18.1785ms
  Time copy memory type 5 (device local, host visible, host coherent): 12.3415ms
 #Accesses: 10
  Time copy memory type 0: 19.3972ms
  Time copy memory type 1 (device local): 8.55995ms
  Time copy memory type 1 (device local; host pointer import): 3.73762ms
  Time copy memory type 3 (host visible, host coherent): 11.3639ms
  Time copy memory type 4 (host visible, host coherent, host cached): 11.4815ms
  Time copy memory type 5 (device local, host visible, host coherent): 1.51335ms
 #Accesses: 100
  Time copy memory type 0: 11.3608ms
  Time copy memory type 1 (device local): 1.13181ms
  Time copy memory type 1 (device local; host pointer import): 0.667993ms
  Time copy memory type 3 (host visible, host coherent): 10.5742ms
  Time copy memory type 4 (host visible, host coherent, host cached): 10.5357ms
  Time copy memory type 5 (device local, host visible, host coherent): 0.43996ms
 #Accesses: 100 (upload excluded)
  Time copy memory type 0: 10.4705ms
  Time copy memory type 1 (device local): 0.320045ms
  Time copy memory type 1 (device local; host pointer import): 0.320448ms
  Time copy memory type 3 (host visible, host coherent): 10.4623ms
  Time copy memory type 4 (host visible, host coherent, host cached): 10.4694ms
  Time copy memory type 5 (device local, host visible, host coherent): 0.321045ms

Tested API: CUDA
 #Accesses: 1
  Time copy cudaMalloc: 10.9288ms
  Time copy cudaMallocManaged: 121.715ms
  Time copy cudaMallocManaged2: 11.9156ms
  Time copy cudaMallocHost: 17.5422ms
 #Accesses: 10
  Time copy cudaMalloc: 1.39011ms
  Time copy cudaMallocManaged: 13.0424ms
  Time copy cudaMallocManaged2: 1.48444ms
  Time copy cudaMallocHost: 11.2858ms
 #Accesses: 100
  Time copy cudaMalloc: 0.435943ms
  Time copy cudaMallocManaged: 1.51968ms
  Time copy cudaMallocManaged2: 0.4465ms
  Time copy cudaMallocHost: 10.6081ms
 #Accesses: 100 (upload excluded)
  Time copy cudaMalloc: 0.326108ms
  Time copy cudaMallocManaged: 1.03326ms
  Time copy cudaMallocManaged2: 0.330267ms
  Time copy cudaMallocHost: 10.535ms

Tested API: SYCL
 #Accesses: 1
  Time copy malloc_device: 10.6607ms
  Time copy malloc_shared: 10.9709ms
  Time copy malloc_host: 17.5261ms
 #Accesses: 10
  Time copy malloc_device: 1.3557ms
  Time copy malloc_shared: 1.42527ms
  Time copy malloc_host: 11.2005ms
 #Accesses: 100
  Time copy malloc_device: 0.432429ms
  Time copy malloc_shared: 0.436729ms
  Time copy malloc_host: 10.5896ms
 #Accesses: 100 (upload excluded)
  Time copy malloc_device: 0.329898ms
  Time copy malloc_shared: 0.329332ms
  Time copy malloc_host: 10.5223ms
```



# Old data (512 MiB)

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
  Time copy memory type 0 (device local): 138.542ms
  Time copy memory type 0 (device local; host pointer import): 46.8541ms
  Time copy memory type 1 (device local, host visible, host coherent): 68.465ms
  Time copy memory type 2 (device local, host visible, host coherent, host cached): 67.7944ms
 #Accesses: 10
  Time copy memory type 0 (device local): 31.5628ms
  Time copy memory type 0 (device local; host pointer import): 22.145ms
  Time copy memory type 1 (device local, host visible, host coherent): 24.3731ms
  Time copy memory type 2 (device local, host visible, host coherent, host cached): 24.319ms
 #Accesses: 100
  Time copy memory type 0 (device local): 20.6512ms
  Time copy memory type 0 (device local; host pointer import): 19.8146ms
  Time copy memory type 1 (device local, host visible, host coherent): 19.916ms
  Time copy memory type 2 (device local, host visible, host coherent, host cached): 19.9897ms
 #Accesses: 100 (upload excluded)
  Time copy memory type 0 (device local): 19.4546ms
  Time copy memory type 0 (device local; host pointer import): 19.4608ms
  Time copy memory type 1 (device local, host visible, host coherent): 19.4634ms
  Time copy memory type 2 (device local, host visible, host coherent, host cached): 19.4675ms

Tested API: SYCL
 #Accesses: 1
  Time copy malloc_device: 35.7535ms
  Time copy malloc_shared: 36.0161ms
  Time copy malloc_host: 35.775ms
 #Accesses: 10
  Time copy malloc_device: 19.839ms
  Time copy malloc_shared: 20.116ms
  Time copy malloc_host: 20.1226ms
 #Accesses: 100
  Time copy malloc_device: 18.3237ms
  Time copy malloc_shared: 18.397ms
  Time copy malloc_host: 18.46ms
 #Accesses: 100 (upload excluded)
  Time copy malloc_device: 18.081ms
  Time copy malloc_shared: 18.1336ms
  Time copy malloc_host: 18.1797ms
```

```
Running on NVIDIA GeForce RTX 3080 Ti Laptop GPU
Tested API: Vulkan
 #Accesses: 1
  Time copy memory type 0: 207.078ms
  Time copy memory type 1 (device local): 164.266ms
  Time copy memory type 1 (device local; host pointer import): 68.0745ms
  Time copy memory type 3 (host visible, host coherent): 64.3475ms
  Time copy memory type 4 (host visible, host coherent, host cached): 67.8671ms
  Time copy memory type 5 (device local, host visible, host coherent): 60.0565ms
 #Accesses: 10
  Time copy memory type 0: 58.0793ms
  Time copy memory type 1 (device local): 18.6939ms
  Time copy memory type 1 (device local; host pointer import): 9.62151ms
  Time copy memory type 3 (host visible, host coherent): 43.9473ms
  Time copy memory type 4 (host visible, host coherent, host cached): 44.0609ms
  Time copy memory type 5 (device local, host visible, host coherent): 8.65007ms
 #Accesses: 100
  Time copy memory type 0: 43.2078ms
  Time copy memory type 1 (device local): 4.57231ms
  Time copy memory type 1 (device local; host pointer import): 3.63145ms
  Time copy memory type 3 (host visible, host coherent): 41.8386ms
  Time copy memory type 4 (host visible, host coherent, host cached): 41.9683ms
  Time copy memory type 5 (device local, host visible, host coherent): 3.11799ms
 #Accesses: 100 (upload excluded)
  Time copy memory type 0: 41.5928ms
  Time copy memory type 1 (device local): 2.98063ms
  Time copy memory type 1 (device local; host pointer import): 2.98355ms
  Time copy memory type 3 (host visible, host coherent): 41.5782ms
  Time copy memory type 4 (host visible, host coherent, host cached): 41.73ms
  Time copy memory type 5 (device local, host visible, host coherent): 2.98216ms

Tested API: CUDA
 #Accesses: 1
  Time copy cudaMalloc: 45.6662ms
  Time copy cudaMallocManaged: 193.085ms
  Time copy cudaMallocManaged2: 47.0906ms
  Time copy cudaMallocHost: 65.3845ms
 #Accesses: 10
  Time copy cudaMalloc: 6.88128ms
  Time copy cudaMallocManaged: 21.6746ms
  Time copy cudaMallocManaged2: 6.9852ms
  Time copy cudaMallocHost: 43.9471ms
 #Accesses: 100
  Time copy cudaMalloc: 2.97348ms
  Time copy cudaMallocManaged: 4.45794ms
  Time copy cudaMallocManaged2: 2.99226ms
  Time copy cudaMallocHost: 41.8282ms
 #Accesses: 100 (upload excluded)
  Time copy cudaMalloc: 2.54246ms
  Time copy cudaMallocManaged: 3.24558ms
  Time copy cudaMallocManaged2: 2.55015ms
  Time copy cudaMallocHost: 41.5797ms

Tested API: SYCL
 #Accesses: 1
  Time copy malloc_device: 43.8367ms
  Time copy malloc_shared: 43.9119ms
  Time copy malloc_host: 73.9847ms
 #Accesses: 10
  Time copy malloc_device: 6.68261ms
  Time copy malloc_shared: 6.67675ms
  Time copy malloc_host: 44.8081ms
 #Accesses: 100
  Time copy malloc_device: 2.94953ms
  Time copy malloc_shared: 2.94868ms
  Time copy malloc_host: 41.8979ms
 #Accesses: 100 (upload excluded)
  Time copy malloc_device: 2.53722ms
  Time copy malloc_shared: 2.53687ms
  Time copy malloc_host: 41.5691ms
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
