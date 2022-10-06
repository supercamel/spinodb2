#ifndef SPINO_CURSOR_H
#define SPINO_CURSOR_H

#include "index.h"
#include "query_parser.h"
#include "query_executor.h"

namespace Spino
{
    class Cursor
    {
    public:
        Cursor(DomAllocator &alloc, std::vector<unique_ptr<Index>> &indices, const char *query_original) : alloc(alloc), indices(indices), executor(alloc)
        {
            limit = 0;
            n_results = 0;
            query = strdup(query_original);
            QueryParser query_parser(alloc, indices, query);
            instructions = query_parser.parse_query(range);

            executor.set_instructions(&instructions);
            iter = range.first;
            while (iter)
            {
                bool r = executor.execute_query(iter->dom_node);
                if (r)
                {
                    break;
                }
                else
                {
                    if (iter == range.last)
                    {
                        iter = nullptr;
                    }
                    else
                    {
                        iter = iter->get_next();
                    }
                }
            }
        }

        ~Cursor()
        {
            free((void *)query);
        }

        bool has_next()
        {
            if (iter == nullptr)
            {
                return false;
            }
            else
            {
                if (limit == 0 || (n_results < limit))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }

        std::string next()
        {
            const DomNode* result = next_dom();
            if(result != nullptr)
            {
                return result->stringify();
            }
            else
            {
                return "";
            }
        }

        const DomNode *next_dom()
        {
            n_results++;
            const DomNode *ret = iter->dom_node;
            if (iter == range.last)
            {
                iter = nullptr;
            }
            else
            {
                iter = iter->get_next();
                while (iter)
                {
                    bool r = executor.execute_query(iter->dom_node);
                    if (r)
                    {
                        break;
                    }
                    else
                    {
                        if (iter == range.last)
                        {
                            iter = nullptr;
                        }
                        else
                        {
                            iter = iter->get_next();
                        }
                    }
                }
            }
            return ret;
        }

        Cursor *set_limit(size_t l)
        {
            limit = l;
            return this;
        }

    private:
        DomAllocator &alloc;
        std::vector<std::unique_ptr<Index>> &indices;
        Index::Range range;
        const char *query;
        std::vector<Token> instructions;
        Index::SkipListNode *iter;
        QueryExecutor executor;
        size_t limit;
        size_t n_results;
    };
}

#endif
