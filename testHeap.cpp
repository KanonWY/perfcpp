#include <gperftools/heap-checker.h>
#include <gperftools/malloc_extension.h>
#include <gperftools/profiler.h>

#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <thread>
#include <vector>

using namespace std;

struct Test
{
    int a[1024];  // 4*1024 = 4k
};

void f1()
{
    int i;
    for (i = 0; i < 1024; ++i) {
        auto sp     = std::make_shared<Test>();
        sp->a[1023] = 100;
    }
}

void f2()
{
    int i;
    for (i = 0; i < 1024; ++i) {
        auto sp     = std::make_shared<Test>();
        sp->a[1023] = 100;
    }
}

static bool WriteSmalleFile(const char* filePath, std::string& content)
{
    FILE* f = fopen(filePath, "w+");
    if (!f) {
        std::cout << "fopen error" << std::endl;
        return false;
    }
    bool ret = true;
    if (fwrite(content.data(), content.size(), 1UL, f) != 1UL) {
        std::cout << "fwrite error" << std::endl;
        ret = false;
    }
    fclose(f);
    return ret;
}

static bool WriteProfFile(const char* filePath, std::string&& content)
{
    FILE* file = fopen(filePath, "w+");
    if (!file) {
        std::cout << "fopen error" << std::endl;
        return false;
    }
    bool ret = true;
    if (fwrite(content.data(), content.size(), 1UL, file) != 1UL) {
        std::cout << "fwrite error" << std::endl;
        ret = false;
    }
    fclose(file);
    return ret;
}

static bool check_TCMALLOC_SAMPLE_PARAMETER()
{
    char* str = getenv("TCMALLOC_SAMPLE_PARAMETER");
    if (str == NULL) {
        return false;
    }
    char* endptr;
    int   val = strtol(str, &endptr, 10);
    return (*endptr == '\0' && val > 0);
}

bool has_TCMALLOC_SAMPLE_PARAMETER()
{
    static bool val = check_TCMALLOC_SAMPLE_PARAMETER();
    return val;
}

std::string GetNowTimeStamp()
{
    auto t1 = std::chrono::system_clock::now();
    auto t2 = std::chrono::system_clock::to_time_t(t1);
    return std::to_string(t2);
}

std::string GetHeapName()
{
    std::string TS = GetNowTimeStamp();
    TS += "_Heap.prof";
    return TS;
}

std::string GetCPUProfilerName()
{
    std::string TS = GetNowTimeStamp();
    TS += "_CPU.prof";
    return TS;
}

std::string GetHeapGrowProfilerName()
{
    std::string TS = GetNowTimeStamp();
    TS += "_GrowHeap.prof";
    return TS;
}

static std::vector<std::string> TcmallocProperty{
    "generic.current_allocated_bytes",
    "generic.heap_size",
    "tcmalloc.pageheap_free_bytes",
    "tcmalloc.pageheap_unmapped_bytes",
    "tcmalloc.slack_bytes",
    "tcmalloc.max_total_thread_cache_bytes",
    "tcmalloc.current_total_thread_cache_bytes"};

void DumpTcmallocProperty()
{
    MallocExtension* malloc_ext = MallocExtension::instance();
    if (malloc_ext == nullptr) {
        std::cout << "error malloc_ext is empty" << std::endl;
    }
    std::map<std::string, size_t> Property_Map;
    for (auto it : TcmallocProperty) {
        size_t value;
        malloc_ext->GetNumericProperty(it.c_str(), &value);
        Property_Map[it] = value;
    }
    for (auto it : Property_Map) {
        printf("%45s %ld\n", it.first.c_str(), it.second);
    }
}

int main()
{
    f1();
    f2();
    std::vector<Test> vec;
    vec.reserve(100000);
    std::vector<Test> vec2;
    vec2.reserve(100000);

    MallocExtension* malloc_ext = MallocExtension::instance();
    if (malloc_ext == nullptr) {
        std::cout << "error malloc_ext is empty" << std::endl;
    }

    std::string obj;
    malloc_ext->GetHeapSample(&obj);
    if (!WriteProfFile(GetHeapName().c_str(), std::move(obj))) {
        std::cout << "WriteProf Error" << std::endl;
    }

    char* str = new char[4096];
    malloc_ext->GetStats(str, 4096);
    std::cout << "Input Malloc Stats:===================>" << std::endl;
    std::cout << str << std::endl;
    delete[] str;

    std::string obj2;
    malloc_ext->GetHeapGrowthStacks(&obj2);
    if (!WriteProfFile(GetHeapGrowProfilerName().c_str(), std::move(obj2))) {
        std::cout << "WriteProf error" << std::endl;
    }

    int Nums = 0;
    malloc_ext->ReadStackTraces(&Nums);

    std::cout << "Nums = " << Nums << std::endl;

    DumpTcmallocProperty();
    HeapLeakChecker::NoGlobalLeaks();
    return 0;
}