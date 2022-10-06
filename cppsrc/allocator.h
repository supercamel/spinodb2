#ifndef SPINO_ALLOCATOR_H
#define SPINO_ALLOCATOR_H

#include <vector>
#include <cstdint>
#include <cstdlib>

#include <iostream>
using namespace std;

namespace Spino
{
    template <size_t SZ> class Allocator
    {
    public:
        Allocator() { }

        ~Allocator() { }

        void *alloc()
        {
            return malloc(SZ);
        }

        void free(void *ptr)
        {
            ::free(ptr);
        }
    };

}

#endif
