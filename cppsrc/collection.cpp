#include "collection.h"

namespace Spino
{
    Collection::Collection(const std::string& name, JournalWriter &jw) : name(name), jw(jw)
    {
        indexes.push_back(make_unique<UintIndex>("_id"));
        id = 0;
    }

    Collection::~Collection()
    {
        // the DOM nodes must be destroyed because some string values/keys might be
        // allocated using the system allocator. failure to free = leaked memory
        // everything else is free'd when the dom allocator is destroyed
        for(auto& node : nodes )
        {
            node.second.destroy();
        }
    }

    void Collection::create_index(const std::string& name)
    {
        std::vector<std::string> tokens;
        std::string delimiter = ".";

        size_t pos = 0;
        while ((pos = name.find(delimiter)) != std::string::npos) {
            tokens.push_back(s.substr(0, pos));
            s.erase(0, pos + delimiter.length());
        }

        indexes.push_back(make_unique<Index>(name));

        auto &index = indexes[indexes.size() - 1];
        size_t count = 0;
        for(auto& node : nodes)
        {
            DomView view;

            for(auto& token : tokens) {
                if(node.second.has_member(token)) {
                    view = node.second.get_member(token);
                }
            }

            index->map.insert({view, node.second});
        }
    }

    void Collection::drop_index(const std::string& name)
    {
        for (size_t i = 0; i < indexes.size(); i++)
        {
            if (indexes[i]->field_name == name)
            {
                indexes[i] = std::move(indexes[indexes.size() - 1]);
                indexes.resize(indexes.size() - 1);
                return;
            }
        }
    }

    const DomView& Collection::find_one(const std::string& query)
    {
        Spino::Index::Range range;

        Spino::QueryParser qp(get_indices(), query);
        auto &instructions = qp.parse_query(range);

        executor.set_instructions(&instructions);
        auto iter = range.first;
        while (iter)
        {
            bool r = executor.execute_query(iter->dom_node);

            if (r)
            {
                cout << iter->dom_node << endl;
                cout << "find one result: " << iter->dom_node->stringify() << endl;
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

    const DomNode *Collection::find_one(const char *index_key, DomNode *value)
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

    unique_ptr<Cursor> Collection::find(const char *query)
    {
        return make_unique<Cursor>(alloc, get_indices(), query);
    }

    DomNode *Collection::create_node()
    {
        return make_dom_node(alloc);
    }

    size_t Collection::drop(const char *query, size_t limit)
    {
        size_t ret = 0;
        auto cursor = find(query)->set_limit(limit);
        bool jwenabled = jw.get_enabled();
        jw.set_enabled(false);
        while (cursor->has_next())
        {
            const DomNode *dom = cursor->next_dom();
            drop_one_by_id(dom->get_member("_id", 3)->get_uint());
            ret++;
        }
        jw.set_enabled(jwenabled);

        if (jw.get_enabled())
        {
            stringstream ss;
            ss << "{\"cmd\":\"drop\",\"collection\":\"";
            ss << escape(name);
            ss << "\",\"query\":\"" << escape(query);
            ss << "\",\"limit\":" << limit << "}";
            jw.append(ss.str());
        }
        return ret;
    }

    void Collection::upsert(const char *query, DomNode *replacement)
    {
        drop(query, 1);
        append(replacement);

        if (jw.get_enabled())
        {
            stringstream ss;
            ss << "{\"cmd\":\"upsert\",\"collection\":\"";
            ss << escape(name);
            ss << "\",\"query\":\"" << escape(query);
            ss << "\",\"document\":\"";
            ss << escape(replacement->stringify()) << "\"}";
            jw.append(ss.str());
        }
    }

    void Collection::upsert(const char *query, const char *json)
    {
        upsert(query, parser->parse(json));
    }

    void Collection::append(DomNode *node)
    {
        DomNode *actual_node = node;

        DomNode* id_node = actual_node->get_member("_id");
        if(id_node == nullptr)
        {
            id_node = actual_node->push_in_place();
        }
        else
        {
            id_node->destroy();
        }
        id_node->set_uint(id++);
        id_node->set_key("_id", (size_t)3);

        for (auto &index : indexes)
        {
            index->insert(actual_node);
        }

        if (jw.get_enabled())
        {
            stringstream ss;
            ss << "{\"cmd\":\"append\",\"collection\":\"";
            ss << escape(name);
            ss << "\",\"document\":\"";
            ss << escape(node->stringify()) << "\"}";
            jw.append(ss.str());
        }
    }

    bool Collection::append(const char *json)
    {
        append(parser->parse(json));
        return true;
    }

    size_t Collection::size()
    {
        return indexes[0]->size();
    }

    DomAllocator &Collection::get_allocator() { return alloc; }
    std::vector<unique_ptr<Index>> &Collection::get_indices() { return indexes; }

    void Collection::print() { indexes[0]->print(); }

    void Collection::drop_one_by_id(uint64_t id)
    {
        DomNode id_node(alloc);
        id_node.set_uint(id);
        auto result = indexes[0]->find_first(&id_node);

        for (size_t i = 1; i < indexes.size(); i++)
        {
            if (result->dom_node->has_member(indexes[i]->get_key(), strlen(indexes[i]->get_key())))
            {
                indexes[i]->delete_node(result->dom_node);
            }
        }

        indexes[0]->delete_node(result->dom_node);
        result->dom_node->destroy();
    }


}

