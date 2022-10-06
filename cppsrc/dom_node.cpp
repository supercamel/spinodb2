#include "dom_node.h"
#include "dom_obj.h"
#include "dom_arr.h"

namespace Spino
{
    DomView::DomView() {
        type = DOM_NODE_TYPE_NULL;
        value.u = 0;
    }

    DomNode::DomNode()
    {
    }

    DomNode::DomNode(const DomNode& other) 
    {
    }

    DomNode& DomNode::operator=(DomNode &other)
    {
        type = other.type;
        value = other.value;
        other.type = DOM_NODE_TYPE_INVALID;
        other.value.u = 0;
        return *this;
    }

    bool DomNode::is_numeric() const
    {
        switch (type)
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

    double DomNode::get_numeric_as_double() const
    {
        switch (type)
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

    bool DomNode::operator==(const DomNode &other)
    {
        if (is_numeric() && other.is_numeric())
        {
            return get_numeric_as_double() == other.get_numeric_as_double();
        }
        else if (type == DOM_NODE_TYPE_STRING && other.type == DOM_NODE_TYPE_STRING)
        {
            return *(value.str) == *(other.value.str);
        }
        return false;
    }

    bool DomNode::operator>=(const DomNode &other)
    {
        if (is_numeric() && other.is_numeric())
        {
            return get_numeric_as_double() >= other.get_numeric_as_double();
        }
        else if (type == DOM_NODE_TYPE_STRING && other.type == DOM_NODE_TYPE_STRING)
        {
            return *(value.str) >= *(other.value.str);
        }
        return false;
    }

    bool DomNode::operator<=(const DomNode &other)
    {
        if (is_numeric() && other.is_numeric())
        {
            return get_numeric_as_double() <= other.get_numeric_as_double();
        }
        else if (type == DOM_NODE_TYPE_STRING && other.type == DOM_NODE_TYPE_STRING)
        {
            return *(value.str) <= *(other.value.str);
        }
        return false;
    }

    bool DomNode::operator>(const DomNode &other)
    {
        if (is_numeric() && other.is_numeric())
        {
            return get_numeric_as_double() > other.get_numeric_as_double();
        }
        else if (type == DOM_NODE_TYPE_STRING && other.type == DOM_NODE_TYPE_STRING)
        {
            return *(value.str) > *(other.value.str);
        }
        return false;
    }

    bool DomNode::operator<(const DomNode &other)
    {
        if (is_numeric() && other.is_numeric())
        {
            return get_numeric_as_double() < other.get_numeric_as_double();
        }
        else if (type == DOM_NODE_TYPE_STRING && other.type == DOM_NODE_TYPE_STRING)
        {
            return *(value.str) <  *(other.value.str);
        }
        return false;
    }

    void DomNode::set_invalid()
    {
        type = DOM_NODE_TYPE_INVALID;
    }

    void DomNode::set_object()
    {
        type = DOM_NODE_TYPE_OBJECT;
        value.ptr = (void*)(new DomObject());
    }

    DomObject* DomView::get_object() const
    {
        return (DomObject*)(value.ptr);
    }

    bool DomView::has_member(const std::string &name) const
    {
        return get_object()->has_member(name);
    }

    const DomView& DomView::get_member(const std::string& name) const
    {
        if(type == DOM_NODE_TYPE_OBJECT)
        {
            return get_object()->get_member(name);
        }
        else 
        {
            throw "DomView::get_member() called on non-object";
        }
    }

    const DomView& DomView::get_element(uint index) const
    {
        if(type == DOM_NODE_TYPE_ARRAY)
        {
            return get_array()->get_index(index);
        }
        else 
        {
            throw "DomView::get_element() called on non-array";
        }
    }

    ElementIterator DomView::element_begin() const
    {
        if(type == DOM_NODE_TYPE_ARRAY)
        {
            return get_array()->element_begin();
        }
        else 
        {
            throw "DomView::element_begin() called on non-array";
        }
    }

    ElementIterator DomView::element_end() const
    {
        if(type == DOM_NODE_TYPE_ARRAY)
        {
            return get_array()->element_end();
        }
        else 
        {
            throw "DomView::element_end() called on non-array";
        }
    }


    void DomNode::set_array()
    {
        type = DOM_NODE_TYPE_ARRAY;
        value.ptr = (void*)(new DomArray());
    }

    DomArray* DomView::get_array() const
    {
        return (DomArray*)(value.ptr);
    }

    void DomNode::set_int(int64_t i)
    {
        type = DOM_NODE_TYPE_INT;
        value.i = i;
    }

    void DomNode::set_uint(uint64_t u)
    {
        type = DOM_NODE_TYPE_UINT;
        value.u = u;
    }

    void DomNode::set_double(double d)
    {
        type = DOM_NODE_TYPE_DOUBLE;
        value.d = d;
    }

    double DomView::get_double() const
    {
        return value.d;
    }

    void DomNode::set_bool(bool b)
    {
        type = DOM_NODE_TYPE_BOOL;
        value.b = b;
    }

    bool DomView::get_bool() const
    {
        return value.b;
    }

    void DomNode::set_string(const std::string& c)
    {
        type = DOM_NODE_TYPE_STRING;
        value.str = new std::string(c);
    }

    const std::string& DomView::get_string() const
    {
        return *value.str;
    }

    void DomNode::set_null()
    {
        type = DOM_NODE_TYPE_NULL;
    }

    void DomNode::copy(const DomView *other)
    {
        destroy();
        type = other->get_type();
        if (type == DOM_NODE_TYPE_STRING)
        {
            set_string(other->get_string());
        }
        else if (type == DOM_NODE_TYPE_BOOL)
        {
            set_bool(other->get_bool());
        }
        else if (type == DOM_NODE_TYPE_DOUBLE)
        {
            set_double(other->get_double());
        }
        else if (type == DOM_NODE_TYPE_INT)
        {
            set_int(other->get_int());
        }
        else if (type == DOM_NODE_TYPE_UINT)
        {
            set_uint(other->get_uint());
        }
        else if(type == DOM_NODE_TYPE_ARRAY)
        {
            set_array();
            DomArray* self = get_array();
            DomArray* other_arr = other->get_array();
            for (auto it = other_arr->element_begin(); it != other_arr->element_end(); it++)
            {
                DomNode n;
                n.copy(&(*it));
                self->push_back(n);
            }
        }
        else if(type == DOM_NODE_TYPE_OBJECT)
        {
            set_object();
            DomObject* self = get_object();
            DomObject* other_obj = other->get_object();
            for (auto it = other_obj->member_begin(); it != other_obj->member_end(); it++)
            {
                DomNode n;
                n.copy(&(it.get_value()));
                self->append(it.get_key(), n);
            }
        }
    }

    void DomNode::from_not_bson(std::ifstream &fin)
    {
        switch (type)
        {
            case DOM_NODE_TYPE_OBJECT: // object
                {
                    do
                    {
                        char typec = fin.get();
                        DomNode child;
                        child.type = (DOM_NODE_TYPE)typec;
                        std::string key_string;
                        while (fin.peek() != 0x00)
                        {
                            key_string += fin.get();
                        }
                        fin.get();

                        child.from_not_bson(fin);
                        get_object()->append(key_string, child);
                    } while (fin.peek() != 0x00);
                    fin.get(); // read the end of object null char
                }
                break;

            case DOM_NODE_TYPE_ARRAY:  // array
                {
                    do
                    {
                        char typec = fin.get();
                        DomNode child;
                        child.type = (DOM_NODE_TYPE)typec;
                        std::string key_string;
                        while (fin.peek() != 0x00)
                        {
                            key_string += fin.get();
                        }
                        fin.get();

                        child.from_not_bson(fin);
                        get_array()->push_back(child);
                    } while (fin.peek() != 0x00);
                    fin.get(); // read the end of object null char
                }
                break;
            case DOM_NODE_TYPE_BOOL:
                {
                    set_bool(fin.get());
                }
                break;
            case DOM_NODE_TYPE_INT:
                {
                    fin.read((char *)value.bytes, 4);
                }
                break;
            case DOM_NODE_TYPE_UINT:
                {
                    fin.read((char *)&value.bytes[0], 8);
                }
                break;
            case DOM_NODE_TYPE_DOUBLE:
                {
                    fin.read((char *)value.bytes, 8);
                }
                break;
            case DOM_NODE_TYPE_STRING:
                {
                    DomNodeValue val;
                    fin.read((char *)val.bytes, 4);
                    char *pstr = new char[val.i + 1];
                    fin.read(pstr, val.i);
                    set_string(pstr);
                    delete pstr;
                }
                break;
            default:
                break;
        }
    }

    void DomView::to_not_bson(std::ofstream &fout) const
    {
        switch (get_type())
        {
            case DOM_NODE_TYPE_INVALID:
            case DOM_NODE_TYPE_END:
            case DOM_NODE_TYPE_NULL:
                break;
            case DOM_NODE_TYPE_BOOL:
                {
                    if (get_bool())
                    {
                        fout.put(0x01);
                    }
                    else
                    {
                        fout.put(0x00);
                    }
                }
                break;
            case DOM_NODE_TYPE_INT:
                {
                    const uint8_t *pi = &value.bytes[0];
                    fout.write((char *)pi, 4);
                }
                break;
            case DOM_NODE_TYPE_UINT:
                {
                    const uint8_t *pi = &value.bytes[0];
                    fout.write((char *)pi, 8);
                }
                break;
            case DOM_NODE_TYPE_DOUBLE:
                {
                    const uint8_t *pi = &value.bytes[0];
                    fout.write((char *)pi, 8);
                }
                break;
            case DOM_NODE_TYPE_STRING:
                {
                    DomNodeValue lenval;
                    lenval.i = value.str->length() + 1;
                    const uint8_t *pi = &lenval.bytes[0];
                    fout.write((char *)pi, 4);
                    fout.write(value.str->c_str(), lenval.i);
                }
                break;
            case DOM_NODE_TYPE_ARRAY:
                {
                    const DomArray* arr = get_array();
                    auto it = arr->element_begin();
                    while (it != arr->element_end())
                    {
                        fout.put(it->get_type());
                        fout.put(0x00);
                        it->to_not_bson(fout);
                        it++;
                    }
                    fout.put(0x00);
                }
                break;
            case DOM_NODE_TYPE_OBJECT:
                {
                    const DomObject* obj = get_object();
                    auto it = obj->member_begin();
                    while (it != obj->member_end())
                    {
                        fout.put(it.get_value().get_type());
                        fout.write(it.get_key().c_str(), it.get_key().length());
                        fout.put(0x00);
                        it.get_value().to_not_bson(fout);
                        ++it;
                    }
                    fout.put(0x00);
                }
        }
    }

    std::string DomView::stringify() const {
        switch(type) 
        {
            case DOM_NODE_TYPE_INT:
                return to_string(value.i);
            case DOM_NODE_TYPE_UINT:
                return to_string(value.u);
            case DOM_NODE_TYPE_DOUBLE:
                return to_string(value.d);
            case DOM_NODE_TYPE_BOOL:
                if(value.b) {
                    return "true";
                }
                else {
                    return "false";
                }
            case DOM_NODE_TYPE_NULL:
                return "null";
            case DOM_NODE_TYPE_STRING:
            {
                std::string r = "\"" + escape(*value.str) + "\"";
                return r;
            }
            case DOM_NODE_TYPE_OBJECT:
                return get_object()->stringify();
            case DOM_NODE_TYPE_ARRAY:
                return get_array()->stringify();
        }
        return "undefined";
    }

    DomNode::~DomNode()
    {
        destroy();
    }

    void DomNode::destroy() 
    {
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
                    delete value.str;
                }
                break;
            case DOM_NODE_TYPE_ARRAY:
                {
                    delete get_array();
                }
                break;
            case DOM_NODE_TYPE_OBJECT:
                {
                    delete get_object();
                }
                break;
        }
    }


    void DomNode::add_member(const std::string& name, DomNode &node) 
    {
        if (type == DOM_NODE_TYPE_OBJECT)
        {
            get_object()->append(name, node);
        }
        else 
        {
            throw("add_member: not an object");
        }
    }

    void DomNode::push_back(DomNode &node) 
    {
        if (type == DOM_NODE_TYPE_ARRAY)
        {
            get_array()->push_back(node);
        }
        else 
        {
            throw("push_back: not an array");
        }
    }
}

