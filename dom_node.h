#ifndef SPINO_DOM_NODE_H
#define SPINO_DOM_NODE_H

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "allocator.h"

namespace Spino
{

    constexpr size_t DOM_ALLOCATOR_CHUNK_SIZE = 40;
    typedef Allocator<DOM_ALLOCATOR_CHUNK_SIZE> DomAllocator;

    enum DOM_NODE_TYPE
    {
        DOM_NODE_TYPE_INVALID,
        DOM_NODE_TYPE_INT,
        DOM_NODE_TYPE_UINT,
        DOM_NODE_TYPE_DOUBLE,
        DOM_NODE_TYPE_STRING,
        DOM_NODE_TYPE_BOOL,
        DOM_NODE_TYPE_OBJECT,
        DOM_NODE_TYPE_ARRAY,
        DOM_NODE_TYPE_NULL,
        DOM_NODE_TYPE_END
    };

    inline const char *dom_node_type_to_string(DOM_NODE_TYPE type)
    {
        switch (type)
        {
        case DOM_NODE_TYPE_INVALID:
            return "invalid";
        case DOM_NODE_TYPE_INT:
            return "int";
        case DOM_NODE_TYPE_UINT:
            return "uint";
        case DOM_NODE_TYPE_DOUBLE:
            return "double";
        case DOM_NODE_TYPE_STRING:
            return "string";
        case DOM_NODE_TYPE_BOOL:
            return "bool";
        case DOM_NODE_TYPE_OBJECT:
            return "object";
        case DOM_NODE_TYPE_ARRAY:
            return "array";
        case DOM_NODE_TYPE_NULL:
            return "null";
        case DOM_NODE_TYPE_END:
            return "";
        }
        return "";
    }

    union DomNodeValue
    {
        int32_t i;
        uint64_t u;
        double d;
        char *str;
        bool b;
        class DomNode *first;
    };

    class DomNode
    {
    public:
        DomNode(DomAllocator &alloc) : alloc(alloc)
        {
            type = DOM_NODE_TYPE_NULL;
            key = nullptr;
        }

        DomNode(DomAllocator &alloc, DOM_NODE_TYPE _type) : alloc(alloc)
        {
            type = _type;
            key = nullptr;
            next = nullptr;
            value.u = 0;
        }

        DomNode(DomAllocator &alloc, const char *str) : alloc(alloc)
        {
            type = DOM_NODE_TYPE_STRING;
            key = nullptr;
            next = nullptr;
            value.str = strdup(str);
        }

        DomNode(DomAllocator &alloc, int i) : alloc(alloc)
        {
            type = DOM_NODE_TYPE_INT;
            key = nullptr;
            next = nullptr;
            value.i = i;
        }

        DomNode &operator=(DomNode &other)
        {
            type = other.type;
            key = other.key;
            value = other.value;
            other.type = DOM_NODE_TYPE_NULL;
            other.value.u = 0;
            return *this;
        }

        void destroy()
        {
            free_self();
            set_null();
        }

        bool is_numeric() const
        {
            switch(type)
            {
                case DOM_NODE_TYPE_DOUBLE:
                case DOM_NODE_TYPE_INT:
                case DOM_NODE_TYPE_UINT:
                case DOM_NODE_TYPE_BOOL:
                    return true;
                default:
                    return false;
            }
        }

        double get_numeric_as_double() const
        {
            switch(type)
            {
                case DOM_NODE_TYPE_DOUBLE:
                    return value.d;
                case DOM_NODE_TYPE_INT:
                    return value.i;
                case DOM_NODE_TYPE_UINT:
                    return value.u;
                case DOM_NODE_TYPE_BOOL:
                    return value.b;
                default:
                    return 0;
            }
        }

        bool operator==(const DomNode &other)
        {
            if(is_numeric() && other.is_numeric())
            {
                return get_numeric_as_double() == other.get_numeric_as_double();
            }
            else if (type == DOM_NODE_TYPE_STRING && other.type == DOM_NODE_TYPE_STRING)
            {
                int len = strlen(value.str);
                int otherlen = strlen(other.value.str);
                if (len != otherlen)
                {
                    return false;
                }

                return strncmp(value.str, other.value.str, len) == 0;
            }
            return false;
        }

        bool operator>=(const DomNode& other)
        {
            if(is_numeric() && other.is_numeric())
            {
                return get_numeric_as_double() >= other.get_numeric_as_double();
            }
            else if (type == DOM_NODE_TYPE_STRING && other.type == DOM_NODE_TYPE_STRING)
            {
                int len = strlen(value.str);
                int otherlen = strlen(other.value.str);
                if (len != otherlen)
                {
                    return false;
                }

                return strncmp(value.str, other.value.str, len) >= 0;
            }
            return false;
        }

        bool operator<=(const DomNode& other)
        {
            if(is_numeric() && other.is_numeric())
            {
                return get_numeric_as_double() <= other.get_numeric_as_double();
            }
            else if (type == DOM_NODE_TYPE_STRING && other.type == DOM_NODE_TYPE_STRING)
            {
                int len = strlen(value.str);
                int otherlen = strlen(other.value.str);
                if (len != otherlen)
                {
                    return false;
                }

                return strncmp(value.str, other.value.str, len) <= 0;
            }
            return false;
        }

        bool operator>(const DomNode& other)
        {
            if(is_numeric() && other.is_numeric())
            {
                return get_numeric_as_double() > other.get_numeric_as_double();
            }
            else if (type == DOM_NODE_TYPE_STRING && other.type == DOM_NODE_TYPE_STRING)
            {
                int len = strlen(value.str);
                int otherlen = strlen(other.value.str);
                if (len != otherlen)
                {
                    return false;
                }

                return strncmp(value.str, other.value.str, len) > 0;
            }
            return false;
        }

        bool operator<(const DomNode& other)
        {
            if(is_numeric() && other.is_numeric())
            {
                return get_numeric_as_double() < other.get_numeric_as_double();
            }
            else if (type == DOM_NODE_TYPE_STRING && other.type == DOM_NODE_TYPE_STRING)
            {
                int len = strlen(value.str);
                int otherlen = strlen(other.value.str);
                if (len != otherlen)
                {
                    return false;
                }

                return strncmp(value.str, other.value.str, len) < 0;
            }
            return false;
        }

        void set_key(const char *nkey, bool copy = true)
        {
            if (copy)
            {
                size_t len = strlen(nkey);
                char *ktmp;
                if (len < DOM_ALLOCATOR_CHUNK_SIZE - 1)
                {
                    ktmp = (char *)alloc.alloc();
                }
                else
                {
                    ktmp = (char *)malloc(len + 1);
                }
                strncpy(ktmp, nkey, len);
                ktmp[len] = '\0';
                key = ktmp;
            }
            else
            {
                key = nkey;
            }
        }

        const char *get_key() const
        {
            return key;
        }

        void set_invalid()
        {
            type = DOM_NODE_TYPE_INVALID;
        }

        void set_object()
        {
            type = DOM_NODE_TYPE_OBJECT;
            value.first = 0;
        }

        void set_array()
        {
            type = DOM_NODE_TYPE_ARRAY;
            value.first = 0;
        }

        void set_int(int32_t i)
        {
            type = DOM_NODE_TYPE_INT;
            value.i = i;
        }

        int get_int() const
        {
            return value.i;
        }

        void set_uint(uint64_t u)
        {
            type = DOM_NODE_TYPE_UINT;
            value.u = u;
        }

        uint64_t get_uint() const
        {
            return value.u;
        }

        void set_double(double d)
        {
            type = DOM_NODE_TYPE_DOUBLE;
            value.d = d;
        }

        double get_double() const
        {
            return value.d;
        }

        void set_bool(bool b)
        {
            type = DOM_NODE_TYPE_BOOL;
            value.b = b;
        }

        bool get_bool() const
        {
            return value.b;
        }

        void set_string(const char *c, size_t len)
        {
            type = DOM_NODE_TYPE_STRING;
            if (len < DOM_ALLOCATOR_CHUNK_SIZE)
            {
                value.str = (char *)alloc.alloc();
            }
            else
            {
                value.str = (char *)malloc(len);
            }
            //strncpy(value.str, c, len);
            for(size_t i = 0; i < len; i++)
            {
                value.str[i] = c[i];
            }
            value.str[len] = '\0';
        }

        const char *get_string() const
        {
            return value.str;
        }

        void set_null()
        {
            type = DOM_NODE_TYPE_NULL;
        }

        DOM_NODE_TYPE get_type() const
        {
            return type;
        }

        bool has_member(const char *key, size_t len) const
        {
            DomNode *iter = (DomNode *)value.first;
            while (iter)
            {
                if (strncmp(iter->key, key, len) == 0)
                {
                    return true;
                }
                iter = iter->next;
            }
            return false;
        }

        void push(DomNode *val)
        {
            DomNode* d = push_in_place();
            *d = *val;
        }

        /**
         * @brief constructs a node and pushes it to the object, returns a pointer to the new node
         * @return DomNode* 
         */
        DomNode* push_in_place()
        {
            DomNode* node = (DomNode*)alloc.alloc();
            node = new(node)DomNode(alloc);
            DomNode* old_first = value.first;
            value.first = node;
            node->next = old_first;
            return node;
        }

        DomNode *get_member(const char *key, int len) const
        {
            DomNode *iter = (DomNode *)value.first;
            if(iter == nullptr) 
            {
                return nullptr;
            }
            if(strncmp(iter->key, key, len) == 0)
            {
                return iter;
            }
            iter = (DomNode*)iter->next;

            if(iter == nullptr)
            {
                return nullptr;
            }
            if(strncmp(iter->key, key, len)== 0)
            {
                return iter;
            }
            iter = (DomNode*)iter->next;

            while (iter)
            {
                if (strncmp(iter->key, key, len) == 0)
                {
                    return iter;
                }
                iter = (DomNode *)iter->next;
            }
            return nullptr;
        }

        DomNode *get_index(size_t pos) const
        {
            DomNode *iter = (DomNode *)value.first;
            while (iter && pos--)
            {
                iter = (DomNode *)iter->next;
            }
            return iter;
        }

        DomNode *begin() const
        {
            return value.first;
        }

        DomNode *get_next() const
        {
            return (DomNode *)next;
        }

        std::string stringify() const
        {
            rapidjson::StringBuffer s;
            rapidjson::Writer<rapidjson::StringBuffer> writer(s);
            stringify_to_buf(writer, false);
            return s.GetString();
        }

        void copy(DomNode* other)
        {
            destroy();
            type = other->type;
            if(type == DOM_NODE_TYPE_STRING)
            {
                set_string(other->get_string(), strlen(other->get_string()));
            }
            else if(type == DOM_NODE_TYPE_BOOL)
            {
                set_bool(other->get_bool());
            }
            else if(type == DOM_NODE_TYPE_DOUBLE)
            {
                set_double(other->get_double());
            }
            else if(type == DOM_NODE_TYPE_INT)
            {
                set_int(other->get_int());
            }
            else if(type == DOM_NODE_TYPE_UINT)
            {
                set_uint(other->get_uint());
            }
            else
            {
                value.first = nullptr;
            }
        }

    private:

        void free_self()
        {
            if (key)
            {
                alloc.free((void *)key);
            }
            key = nullptr;

            switch (type)
            {
            case DOM_NODE_TYPE_INT:
            case DOM_NODE_TYPE_UINT:
            case DOM_NODE_TYPE_DOUBLE:
            case DOM_NODE_TYPE_BOOL:
            case DOM_NODE_TYPE_NULL:
            case DOM_NODE_TYPE_INVALID:
            case DOM_NODE_TYPE_END:
                break;
            case DOM_NODE_TYPE_STRING:
            {
                alloc.free(value.str);
            }
            break;
            case DOM_NODE_TYPE_ARRAY:
            case DOM_NODE_TYPE_OBJECT:
            {
                DomNode *iter = (DomNode *)value.first;
                while (iter)
                {
                    iter->destroy();
                    alloc.free(iter);
                    iter = iter->next; // LA LA LA
                }
                value.first = nullptr;
                type = DOM_NODE_TYPE_NULL;
            }
            break;
            }
        }

        void stringify_to_buf(rapidjson::Writer<rapidjson::StringBuffer> &writer, bool write_key) const
        {
            if (write_key)
            {
                writer.Key(key);
            }
            switch (get_type())
            {
            case DOM_NODE_TYPE_INVALID:
            case DOM_NODE_TYPE_END:
                break;
            case DOM_NODE_TYPE_BOOL:
                writer.Bool(value.b);
                break;
            case DOM_NODE_TYPE_DOUBLE:
                writer.Double(value.d);
                break;
            case DOM_NODE_TYPE_INT:
                writer.Int(value.i);
                break;
            case DOM_NODE_TYPE_UINT:
                writer.Uint64(value.u);
                break;
            case DOM_NODE_TYPE_ARRAY:
            {
                writer.StartArray();
                DomNode *iter = (DomNode *)value.first;
                while (iter)
                {
                    iter->stringify_to_buf(writer, false);
                    iter = iter->next;
                }
                writer.EndArray();
            }
            break;
            case DOM_NODE_TYPE_OBJECT:
            {
                writer.StartObject();
                DomNode *iter = (DomNode *)value.first;
                while (iter)
                {
                    iter->stringify_to_buf(writer, true);
                    iter = iter->next;
                }
                writer.EndObject();
            }
            break;
            case DOM_NODE_TYPE_STRING:
                writer.String(value.str);
                break;
            case DOM_NODE_TYPE_NULL:
                writer.Null();
                break;
            }
        }

        DomAllocator &alloc;
        DOM_NODE_TYPE type;
        DomNodeValue value;
        const char *key;
        DomNode *next;
    };

    inline DomNode *make_dom_node(DomAllocator &alloc)
    {
        DomNode *node = (DomNode *)alloc.alloc();
        node = new (node) DomNode(alloc);
        return node;
    }
}

#endif
