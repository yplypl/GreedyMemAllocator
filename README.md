##功能介绍
本内存分配器，设计用于最大化的重复使用已经分配的内存，采用定时最久未使用算法，归还长期没有被重用的内存，从而大幅度的提高内存的重复利用率。本内存分配器在高运算环境下，能够达到99%以上的内存重复使用。极大提升程序内存分配和释放的性能。

##代码说明
``
BaseMemManager，内存分配器的接口类。规范子类行为。
StdMemManager，内存分配器的操作系统api实现，直接调用malloc和free。用于和GreedyMemManager对比验证性能。
GreedyMemManager，贪婪内存分配器，会缓存释放的内存，采用定时最久未使用算法，归还长期没有被重用的内存。（正在工作的内存分配器）
``

##性能测试结果
### 窄带内存范围测试
循环的测试10万次内存分配和释放，每次分配的内存为某个内存大小范围内的随机值。
```
random bytes length begin：65, end:1024, test times:100000, system api cost：266, greedyMemory cost:47,totalMallocCount:100000, blockReuseCount:99996, call system malloc count:4
random bytes length begin：1025, end:10240, test times:100000, system api cost：688, greedyMemory cost:31,totalMallocCount:100000, blockReuseCount:99996, call system malloc count:4
random bytes length begin：10241, end:102400, test times:100000, system api cost：4187, greedyMemory cost:31,totalMallocCount:100000, blockReuseCount:99996, call system malloc count:4
random bytes length begin：102401, end:1048576, test times:100000, system api cost：19922, greedyMemory cost:47,totalMallocCount:100000, blockReuseCount:99996, call system malloc count:4
random bytes length begin：1048576, end:5242880, test times:10000, system api cost：9922, greedyMemory cost:15,totalMallocCount:10000, blockReuseCount:9996, call system malloc count:4
```

即便是最小的65byte-1K性能都有好几倍的提升，随着分配的内存块越来越大，性能提升会成百上千倍。

### 宽带内存范围测试
循环的测试1万次内存分配和释放，每次分配的内存为1byte-5M的随机值。
```
random bytes length begin：1, end:5242880, test times:10000, system api cost：8891, greedyMemory cost:15,totalMallocCount:9999, blockReuseCount:9983, call system malloc count:16
```
如此宽范围不定长的内存分配，此内存分配器性能提升依然到达了(8891/15) = 593倍。内存重复利用率也高达：99.83%