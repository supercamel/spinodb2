#ifndef SPINO_PARSER_H
#define SPINO_PARSER_H

#include "dom_node.h"
#include "rapidjson/reader.h"

namespace Spino
{
        class Parser : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, Parser>
    {
    public:
        Parser()
        {
        }

        bool Bool(bool b) 
        {
            DomNode* node = new DomNode();
            node->set_bool(b);
            nodes.push_back(node);
            return true;
        }

        bool Null()
        {
            DomNode* node = new DomNode();
            node->set_null();
            nodes.push_back(node);
            return true;
        }

        bool Int(int i)
        {
            DomNode* node = new DomNode();
            node->set_int(i);
            nodes.push_back(node);
            return true;
        }

        bool Uint(unsigned int u)
        {
            DomNode* node = new DomNode();
            node->set_uint(u);
            nodes.push_back(node);
            return true;
        }

        bool Int64(int64_t i)
        {
            DomNode* node = new DomNode();
            node->set_int(i);
            nodes.push_back(node);
            return true;
        }

        bool Uint64(uint64_t u)
        {
            DomNode* node = new DomNode();
            node->set_uint(u);
            nodes.push_back(node);
            return true;
        }

        bool Double(double d)
        {
            DomNode* node = new DomNode();
            node->set_double(d);
            nodes.push_back(node);
            return true;
        }

        bool RawNumber(const char *str, size_t length, bool copy)
        {
            return true;
        }

        bool String(const char *str, size_t length, bool copy)
        {
            DomNode* s = new DomNode();
            std::string c(str, length);
            s->set_string(c);
            nodes.push_back(s);
            return true;
        }

        bool StartObject()
        {
            state_stack.push_back(STATE_OBJECT);
            DomNode* obj = new DomNode();
            obj->set_object();
            nodes.push_back(obj);
            n_nodes.push_back(nodes.size());
            return true;
        }

        bool Key(const char *str, size_t length, bool copy)
        {
            if(state_stack.back() == STATE_OBJECT)
            {
                std::string key = std::string(str, length);
                key_stack.push_back(key);
            }
            return true;
        }

        bool EndObject(rapidjson::SizeType sz)
        {
            state_stack.pop_back();
            size_t n = n_nodes.back();
            n_nodes.pop_back();
            for(size_t i = nodes.size() - 1; i >= n; i--)
            {
                nodes[n - 1]->add_member(key_stack.back().c_str(), *(nodes.back()));
                key_stack.pop_back();
                delete nodes.back();
                nodes.pop_back();
            }

            return true;
        }

        bool StartArray()
        {
            state_stack.push_back(STATE_ARRAY);
            DomNode* arr = new DomNode();
            arr->set_array();
            nodes.push_back(arr);
            n_nodes.push_back(nodes.size());
            return true;
        }

        bool EndArray(rapidjson::SizeType sz)
        {
            state_stack.pop_back();
            size_t n = n_nodes.back();
            n_nodes.pop_back();
            auto v2 = std::vector<DomNode*>(nodes.begin() + n, nodes.end());

            size_t i = 0;
            while(nodes.size() > n)
            {
                nodes[n - 1]->push_back(*v2[i]);
                delete v2[i++];
                nodes.pop_back();
            }

            return true;
        }

        DomView& parse(const std::string& str)
        {
            reset();
            rapidjson::StringStream ss(str.c_str());
            reader.Parse(ss, *this);
            return *nodes[0];
        }

        DomView& get()
        {
            return *nodes[0];
        }

        void reset()
        {
            key_stack = std::vector<std::string>();
            state_stack = std::vector<ParserState>();
            n_nodes = std::vector<size_t>();
            for(size_t i = 0; i < nodes.size(); i++)
            {
                delete nodes[i];
            }

            nodes = std::vector<DomNode*>();
        }

    private:
        enum ParserState
        {
            STATE_OBJECT,
            STATE_ARRAY,
            STATE_END
        };


        rapidjson::Reader reader;

        std::vector<string> key_stack;
        std::vector<size_t> n_nodes;
        std::vector<DomNode*> nodes;
        std::vector<ParserState> state_stack;
    };
}

#endif
