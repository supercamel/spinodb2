#ifndef SPINO_DOM_NODE_H
#define SPINO_DOM_NODE_H

#include <iostream>
using namespace std;


#include <cstdint>
#include <cstdlib>
#include <string>
#include <cstring>
#include <fstream>
#include <map>
#include "dom_arr.h"

namespace Spino
{

    inline std::string escape(const std::string &str)
    {
        std::string result;
        for (char c : str)
        {
            if (c == '"')
            {
                result += "\\\"";
            }
            else if (c == '\\')
            {
                result += "\\\\";
            }
            else if (c == '\n')
            {
                result += "\\n";
            }
            else if (c == '\t')
            {
                result += "\\t";
            }
            else
            {
                result += c;
            }
        }
        return result;
    }

    inline std::string unescape(const std::string &str)
    {
        std::string result;
        unsigned int len = str.length();
        unsigned int i = 0;
        while (i < len)
        {
            char c = str[i++];
            if (c == '\\')
            {
                c = str[i++];
                if (c == 'n')
                {
                    result += '\n';
                }
                else if (c == '\\')
                {
                    result += '\\';
                }
                else if (c == 't')
                {
                    result += '\t';
                }
                else if (c == '"')
                {
                    result += '"';
                }
                else
                {
                    cout << "SpinoDB:: unescape: unknown escape sequence in string.\n"
                        << str << endl;
                    cout << str[i - 1] << endl;
                }
            }
            else
            {
                result += c;
            }
        }
        return result;
    }

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
        int64_t i;
        uint64_t u;
        double d;
        std::string* str;
        bool b;
        void* ptr;
        uint8_t bytes[8];
    };


    class DomView
    {
        public:
            DomView();

            friend class DomNode;

            inline int get_int() const { return value.i; }
            inline uint64_t get_uint() const { return value.u; }
            const std::string& get_string() const;
            double get_double() const;
            bool get_bool() const;
            inline DOM_NODE_TYPE get_type() const { return type; }

            void to_not_bson(std::ofstream &fout) const;

            bool has_member(const std::string& name) const;
            const DomView& get_member(const std::string& name) const;
            const DomView& get_element(uint index) const;

            ElementIterator element_begin() const;
            ElementIterator element_end() const;

            std::string stringify() const;
        protected:
            DOM_NODE_TYPE type;
            DomNodeValue value;

            class DomObject* get_object() const;
            class DomArray* get_array() const;



    };

    class DomNode : public DomView
    {
        public:
            friend class DomObject;
            friend class DomArray;

            DomNode();
            DomNode(DOM_NODE_TYPE _type);
            DomNode(const char *str);
            DomNode(int i);
            DomNode &operator=(DomNode &other);
            ~DomNode();

            bool is_numeric() const;
            double get_numeric_as_double() const;

            bool operator==(const DomNode &other);
            bool operator>=(const DomNode &other);
            bool operator<=(const DomNode &other);
            bool operator>(const DomNode &other);
            bool operator<(const DomNode &other);

            void set_invalid();
            void set_object();
            void set_array();
            void set_int(int64_t i);
            void set_uint(uint64_t u);
            void set_double(double d);
            void set_bool(bool b);
            void set_string(const std::string& c);
            void set_null();

            void copy(const DomView *other);
            void from_not_bson(std::ifstream &fin);

            void add_member(const std::string& name, DomNode &node);
            void push_back(DomNode &node);

            void destroy();
        private:
            DomNode(const DomNode& other);
    };
}

#endif
