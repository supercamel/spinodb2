#ifndef SPINO_PARSER_H
#define SPINO_PARSER_H

#include "dom_node.h"
#include "rapidjson/reader.h"

namespace Spino
{
    class Parser : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, Parser>
    {
    public:
        Parser(DomAllocator &alloc) : alloc(alloc)
        {
            key = nullptr;
        }

        bool Bool(bool b) 
        {
            DomNode* element = make_dom_node(alloc);
            element->set_bool(b);
            element->set_key(key, false);
            key = nullptr;
            nodes.push_back(element);
            return true;
        }

        bool Null()
        {
            DomNode *element = make_dom_node(alloc);
            element->set_null();
            element->set_key(key, false);
            key = nullptr;
            nodes.push_back(element);
            return true;
        }

        bool Int(int i)
        {
            DomNode *element = make_dom_node(alloc);
            element->set_int(i);
            element->set_key(key, false);
            key = nullptr;

            nodes.push_back(element);

            return true;
        }

        bool Uint(unsigned int u)
        {
            DomNode *element = make_dom_node(alloc);
            element->set_uint(u);
            element->set_key(key, false);
            key = nullptr;
            nodes.push_back(element);

            return true;
        }

        bool Int64(int64_t i)
        {
            DomNode *element = make_dom_node(alloc);
            element->set_int(i);
            element->set_key(key, false);
            key = nullptr;
            nodes.push_back(element);

            return true;
        }

        bool Uint64(uint64_t u)
        {
            DomNode *element = make_dom_node(alloc);
            element->set_uint(u);
            element->set_key(key, false);
            key = nullptr;
            nodes.push_back(element);

            return true;
        }

        bool Double(double d)
        {
            DomNode *element = make_dom_node(alloc);
            element->set_double(d);
            element->set_key(key, false);
            key = nullptr;
            nodes.push_back(element);

            return true;
        }

        bool RawNumber(const char *str, size_t length, bool copy)
        {
            return true;
        }

        bool String(const char *str, size_t length, bool copy)
        {
            DomNode *node = make_dom_node(alloc);
            node->set_string(str, length);
            node->set_key(key, false);
            key = nullptr;
            nodes.push_back(node);
            return true;
        }

        bool StartObject()
        {
            DomNode *node = make_dom_node(alloc);
            node->set_object();
            node->set_key(key, false);
            key = nullptr;
            nodes.push_back(node);
            return true;
        }

        bool Key(const char *str, size_t length, bool copy)
        {
            if (length < DOM_ALLOCATOR_CHUNK_SIZE-1)
            {
                key = (char *)alloc.alloc();
                for (size_t i = 0; i < length; i++)
                {
                    key[i] = str[i];
                }
                key[length] = 0;
            }
            else
            {
                key = (char *)malloc(length + 1);
                for (size_t i = 0; i < length; i++)
                {
                    key[i] = str[i];
                }
                key[length] = 0;
            }
            return true;
        }

        bool EndObject(rapidjson::SizeType sz)
        {
            DomNode* obj = nodes[nodes.size()-(sz+1)];
            for (size_t i = 0; i < sz; i++)
            {
                DomNode* element = nodes[nodes.size()-1];
                obj->push(element);
                nodes.pop_back();
                alloc.free(element);
            }
            return true;
        }

        bool StartArray()
        {
            DomNode *node = make_dom_node(alloc);
            node->set_array();
            node->set_key(key, false);
            key = nullptr;
            nodes.push_back(node);

            return true;
        }

        bool EndArray(rapidjson::SizeType sz)
        {
            return EndObject(sz);
        }

        DomNode *parse(const char *str)
        {
            reset();
            rapidjson::StringStream ss(str);
            reader.Parse(ss, *this);
            return nodes[0];
        }

        DomNode *get()
        {
            return nodes[0];
        }

        void reset()
        {
            if (key)
            {
                alloc.free(key);
                key = nullptr;
            }

            nodes.erase(nodes.begin(), nodes.end());
        }

    private:
        DomAllocator &alloc;
        std::vector<DomNode *> nodes;
        rapidjson::Reader reader;
        char *key;
    };
}

#endif