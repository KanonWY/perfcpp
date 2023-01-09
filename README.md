## Use gperftools

[gperftools](https://xusenqi.site/2020/12/06/C++Profile%E7%9A%84%E5%A4%A7%E6%9D%80%E5%99%A8_gperftools%E7%9A%84%E4%BD%BF%E7%94%A8/)  
[Doc](https://gperftools.github.io/gperftools/tcmalloc.html)  
[collection](https://zhuanlan.zhihu.com/p/399999297)  
[tcmalloc大空闲内存不释放问题](https://blog.csdn.net/qq_16097611/article/details/118999271)  


### 1.prepare
```bash
sudo apt-get install -y libunwind-dev
git clone https://github.com/gperftools/gperftools.git
cd gperftools
cmake -S . build
cmake --build build
```
### 2.cpu
```cpp
ProfilerStart(filename);

other code

ProfilerStop();
```

### 3.heap
- profile info
more detail in [heapprofile.html](https://gperftools.github.io/gperftools/heapprofile.html) and source code.
```cpp
#include <gperftools/malloc_extension.h>

MallocExtension::instance()->GetStats(buffer, buffer_length);
MallocExtension::instance()->GetHeapSample(&string);
MallocExtension::instance()->GetHeapGrowthStacks(&string);
```
- check
more detail in [heap_checker.html](https://gperftools.github.io/gperftools/heap_checker.html) and source code.
```cpp
#include <gperftools/heap-checker.h>
HeapLeakChecker heap_checker("test_foo");
{
    code that exercises some foo functionality;
    this code should not leak memory;
}
if (!heap_checker.NoLeaks()) assert(NULL == "heap memory leak");
```


### 4.tcmalloc
高级“调整”标志，可以更精确地控制tcmalloc如何从内核分配内存.
