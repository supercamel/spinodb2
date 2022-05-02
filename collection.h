#ifndef SPINO_COLLECTION_H
#define SPINO_COLLECTION_H

#include <string>
#include <vector>
#include <memory>
#include <chrono>

#include "index.h"
#include "parser.h"
#include "query_parser.h"
#include "query_executor.h"
#include "cursor.h"

#include <iostream>

namespace Spino
{

    using namespace std;
    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;

    class Collection
    {
    public:
        Collection(const char *name) : name(name), alloc(DomAllocator(7)), executor(alloc)
        {
            indexes.push_back(make_unique<Index>("_id"));
            parser = make_unique<Parser>(alloc);
            id = 0;
        }

        ~Collection()
        {
            // the DOM nodes must be destroyed because some string values/keys might be 
            // allocated using the system allocator. failure to free = leaked memory
            // everything else is free'd when the dom allocator is destroyed
            auto &pidx = indexes[0];
            auto *iter = pidx->begin();
            while (iter)
            {
                iter->dom_node->destroy();
                iter = (Index::SkipListNode *)iter->next[0];
            }
        }

        const char *get_name()
        {
            return name.c_str();
        }

        void create_index(const char *name)
        {
            indexes.push_back(make_unique<Index>(name));

            auto &index = indexes[indexes.size() - 1];
            auto *iter = indexes[0]->begin();
            while (iter)
            {
                index->insert(iter->dom_node);
                iter = (Index::SkipListNode *)iter->next[0];
            }
        }

        void drop_index(const char *name)
        {
            for (size_t i = 0; i < indexes.size(); i++)
            {
                if (strcmp(indexes[i]->get_key(), name) == 0)
                {
                    indexes[i] = std::move(indexes[indexes.size() - 1]);
                    indexes.resize(indexes.size() - 1);
                    return;
                }
            }
        }

        const DomNode *find_one(const char *query)
        {
            Spino::Index::Range range;

            Spino::QueryParser qp(alloc, get_indices(), query);
            auto &instructions = qp.parse_query(range);

            executor.set_instructions(&instructions);
            auto iter = range.first;
            while (iter)
            {
                bool r = executor.execute_query(iter->dom_node);

                if (r)
                {
                    return iter->dom_node;
                }
                iter = iter->get_next();
                if (iter == range.last)
                {
                    break;
                }
            }
            return nullptr;
        }

        DomNode *find_one(const char *index_key, DomNode *value)
        {
            for (auto &idx : indexes)
            {
                if (strcmp(idx->get_key(), index_key) == 0)
                {
                    auto result = idx->find_first(value);
                    if (result)
                    {
                        return result->dom_node;
                    }
                }
            }
            return nullptr;
        }

        unique_ptr<Cursor> find(const char *query)
        {
            return make_unique<Cursor>(alloc, get_indices(), query);
        }

        DomNode *create_node()
        {
            return make_dom_node(alloc);
        }

        void drop_one_by_id(uint64_t id)
        {
            DomNode id_node(alloc);
            id_node.set_uint(id);
            auto result = indexes[0]->find_first(&id_node);

            for(size_t i = 1; i < indexes.size(); i++)
            {
                if(result->dom_node->has_member(indexes[i]->get_key(), strlen(indexes[i]->get_key())))
                {
                    indexes[i]->delete_node(result->dom_node);
                }
            }

            indexes[0]->delete_node(result->dom_node);
            result->dom_node->destroy();
        }

        size_t drop(const char* query, size_t limit = 0)
        {
            cout << "drop: " << query << endl;
            size_t ret = 0;
            auto cursor = find(query)->set_limit(limit);
            while(cursor->has_next())
            {
                const DomNode* dom = cursor->next_dom();
                cout << "dropping: " << dom->stringify() << endl;
                drop_one_by_id(dom->get_member("_id", 3)->get_uint());
                ret++;
            }
            cout << "drop complete" << endl;
            return ret;
        }

        void upsert(const char* query, DomNode* replacement)
        {
            drop(query, 1);
            append(replacement);
        }

        void upsert(const char* query, const char* json)
        {
            drop(query, 1);
            append(json);
        }

        void append(DomNode *node)
        {
            DomNode *actual_node = create_node();
            *actual_node = *node;

            DomNode *idnode = actual_node->push_in_place();
            idnode->set_uint(id++);
            idnode->set_key("_id");

            for (auto &index : indexes)
            {
                index->insert(actual_node);
            }
        }

        void append(const char *json)
        {
            append(parser->parse(json));
        }

        DomAllocator &get_allocator() { return alloc; }
        std::vector<unique_ptr<Index>> &get_indices() { return indexes; }

        void print() { indexes[0]->print(); }

    private:
        std::string name;
        std::vector<unique_ptr<Index>> indexes;
        uint64_t id;
        DomAllocator alloc;
        unique_ptr<Parser> parser;
        QueryExecutor executor;
    };

}

#endif