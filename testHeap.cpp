#include "gperftoolsWrap.h"

#include <memory>

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

int main()
{
    f1();
    f2();
    std::vector<Test> vec;
    vec.reserve(100000);
    std::vector<Test> vec2;
    vec2.reserve(100000);
    return 0;
}