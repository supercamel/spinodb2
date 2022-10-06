#ifndef SPINO_COLLECTION_H
#define SPINO_COLLECTION_H

#include <string>
#include <vector>
#include <memory>
#include <chrono>

#include "dom_node.h"
#include "parser.h"
#include "query_parser.h"
#include "query_executor.h"
#include "cursor.h"
#include "journal.h"

#include <iostream>

namespace Spino
{

    using namespace std;
    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;

    class Index {
        public:
            Index(const std::string& field_name) : field_name(field_name) {}

            std::string field_name;
            std::multimap<DomView, DomView> map;
    };

    class Collection
    {
    public:
        Collection(const std::string& name, JournalWriter &jw);
        ~Collection();
        const std::string& get_name() { return name; }
        void create_index(const std::string& name);
        void drop_index(const std::string& name);
        const DomView& find_one(const std::string& query);
        const DomView& find_one(const std::string& index_key, const DomView& value);
        unique_ptr<Cursor> find(const std::string& query);
        size_t drop(const std::string& query, size_t limit = 1);
        void upsert(const std::string& query, DomNode *replacement);
        void upsert(const std::string& query, const std::string& json);
        void append(DomNode *node);
        bool append(const std::string& json);
        size_t size();
        std::vector<unique_ptr<Index>> &get_indices();
        void print();

    private:
        void drop_one_by_id(uint64_t id);

        std::string name;
        std::multimap<size_t, DomNode> nodes;
        std::vector<unique_ptr<Index>> indexes;
        uint64_t id;
        QueryExecutor executor;
        JournalWriter &jw;
    };

}

#endif
