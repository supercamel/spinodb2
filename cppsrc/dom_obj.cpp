#include "dom_obj.h"
#include <sstream>

namespace Spino {

    DomObject::DomObject() {

    }

    DomObject::~DomObject() {
        for(auto i : members) {
            delete i.second;
        }
    }

    bool DomObject::has_member(const std::string& key) const
    {
        if(members.find(key) != members.end())
        {
            return true;
        }
        else 
        {
            return false;
        }
    }

    void DomObject::append(const std::string& key, DomNode& val)
    {
        DomNode* n = new DomNode();
        *n = val;
        members.insert({key, n});
    }

    const DomView& DomObject::get_member(const std::string& key, size_t len) const
    {
        return *(members.find(key)->second);
    }

    MemberIterator DomObject::member_begin() const
    {
        return MemberIterator(&members, members.begin());
    }

    MemberIterator DomObject::member_end() const
    {
        return MemberIterator(&members, members.end());
    }


    std::string DomObject::stringify() const
    {
        std::stringstream ss;
        ss << "{";
        size_t count = 0;
        size_t sz = members.size();

        for(auto& i : members) {
            ss << "\"" << escape(i.first) << "\":" << i.second->stringify();
            if(++count != sz) {
                ss << ",";
            }
        }
        ss << "}";
        return ss.str();
    }

}


