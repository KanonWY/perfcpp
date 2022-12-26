#include <gperftools/heap-checker.h>
#include <gperftools/malloc_extension.h>
#include <gperftools/profiler.h>

#include <iostream>
#include <thread>
#include <vector>

using namespace std;

struct Test {
  int a[1024];  // 4*1024 = 4k
};

void f1() {
  int i;
  for (i = 0; i < 1024; ++i) {
    void *p2 = new Test;
    // delete[] p2;
  }
}

void f2() {
  int i;
  for (i = 0; i < 1024; ++i) {
    void *p2 = new Test;
    // delete[] p2;
  }
}

static bool WriteSmalleFile(const char *filePath, std::string &content) {
  FILE *f = fopen(filePath, "w+");
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
  return true;
}

static bool check_TCMALLOC_SAMPLE_PARAMETER() {
  char *str = getenv("TCMALLOC_SAMPLE_PARAMETER");
  if (str == NULL) {
    return false;
  }
  char *endptr;
  int val = strtol(str, &endptr, 10);
  return (*endptr == '\0' && val > 0);
}

bool has_TCMALLOC_SAMPLE_PARAMETER() {
  static bool val = check_TCMALLOC_SAMPLE_PARAMETER();
  return val;
}

int main() {
  //   std::thread t2(f2);
  f1();
  f2();
  std::vector<Test> vec;
  vec.reserve(100000);
  std::vector<Test> vec2;
  vec2.reserve(100000);
  //   if (t2.joinable()) {
  //     t2.join();
  //   }
  MallocExtension *malloc_ext = MallocExtension::instance();
  if (malloc_ext == nullptr) {
    std::cout << "error malloc_ext is empty" << std::endl;
  }
  std::string obj;
  const char *perfName = "testHeap.prof";
  malloc_ext->GetHeapSample(&obj);
  if (!WriteSmalleFile(perfName, obj)) {
    std::cout << "writeFile error" << std::endl;
  }

  char *str = new char[4096];
  malloc_ext->GetStats(str, 4096);
  std::cout << str << std::endl;
  delete[] str;

  std::string obj2;
  malloc_ext->GetHeapGrowthStacks(&obj2);
  const char *growHeap = "testHeapGrowp.prof";
  if (!WriteSmalleFile(growHeap, obj2)) {
    std::cout << "writeFile error" << std::endl;
  }
  HeapLeakChecker::NoGlobalLeaks();
  return 0;
}