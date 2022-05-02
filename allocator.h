#ifndef SPINO_ALLOCATOR_H
#define SPINO_ALLOCATOR_H

#include <vector>
#include <cstdint>
#include <cstdlib>

#include <iostream>
using namespace std;

namespace Spino
{
    template <size_t CHUNK_SIZE>
    class Allocator
    {
    public:
        Allocator(int reserve_pages = 3, size_t chunks_per_page = 1024*1024) : chunks_per_page(chunks_per_page)
        {
            for (int i = 0; i < reserve_pages; i++)
            {
                Page *page = new Page(chunks_per_page);
                pages.push_back(page);
            }
            current_page = 0;
        }

        ~Allocator()
        {
            for (size_t i = 0; i < pages.size(); i++)
            {
                delete pages[i];
            }
        }

        void *alloc()
        {
            allocations++;
            void *r = pages[current_page]->alloc();
            if (r == nullptr)
            {
                for (size_t i = 0; i < pages.size(); i++)
                {
                    r = pages[i]->alloc();
                    if (r != nullptr)
                    {
                        current_page = i;
                        return r;
                    }
                }
                Page *page = new Page(chunks_per_page);
                pages.push_back(page);
                current_page = pages.size() - 1;
                return page->alloc();
            }
            else
            {
                return r;
            }
        }

        void free(void *ptr)
        {
            allocations--;
            for (size_t i = 0; i < pages.size(); i++)
            {
                if (pages[i]->free(ptr))
                {
                    return;
                }
            }
            ::free(ptr);
        }

        size_t get_allocations()
        {
            return allocations;
        }

    private:
        union Block
        {
            Block *next;
            uint8_t bytes[CHUNK_SIZE];
        };

        class Page
        {
        public:
            Page(uint32_t reserve_blocks)
            {
                start = new Block[reserve_blocks];
                free_head = start;
                end = &start[reserve_blocks];

                uint32_t count = 0;
                while (count <= reserve_blocks)
                {
                    start[count].next = &start[count + 1];
                    count++;
                }
                start[reserve_blocks - 1].next = 0;
            }

            void *alloc()
            {
                Block *n = free_head;
                if (n != 0)
                {
                    free_head = free_head->next;
                }
                return n;
            }

            bool free(void *ptr)
            {
                if ((ptr >= start) && (ptr <= end))
                {
                    Block *bptr = (Block *)ptr;
                    bptr->next = free_head;
                    free_head = bptr;
                    return true;
                }
                return false;
            }

        private:
            Block *start;
            Block *free_head;
            Block *end;
        };

        uint32_t current_page;
        std::vector<Page *> pages;
        size_t allocations = 0;
        size_t chunks_per_page;
    };
}

#endif
