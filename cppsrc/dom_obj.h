#ifndef DOM_OBJ_INCLUDED
#define DOM_OBJ_INCLUDED

#include <map>
#include <string>
#include "dom_node.h"

namespace Spino {
    class MemberIterator 
    {
        public:
            MemberIterator(const std::map<std::string, DomNode*> *members, std::map<std::string, DomNode*>::const_iterator it) :
                members(members), it(it)
            {
            }

            bool operator==(const MemberIterator &other) const
            {
                return (*it == *(other.it));
            }

            bool operator!=(const MemberIterator &other)
            {
                return *it != *(other.it);
            }

            const DomView& operator*() const
            {
                return *(it->second);
            }

            const DomView* operator->() const
            {
                return it->second;
            }

            void operator++(int i)
            {
                ++it;
            }

            void operator++()
            {
                ++it;
            }

            bool has_next()
            {
                return *it != *(members->end());
            }

            std::string get_key()
            {
                std::string result = it->first;
                return result;
            }

            const DomView& get_value()
            {
                return *(it->second);
            }
        private:
            const std::map<std::string, DomNode*> *members;
            std::map<std::string, DomNode*>::const_iterator it;

    };


    class DomObject
    {
        public:
            DomObject();
            ~DomObject();
            bool has_member(const std::string& name) const;
            void append(const std::string& member, DomNode& val);

            const DomView& get_member(const std::string& key, size_t len = 0) const;

            MemberIterator member_begin() const;
            MemberIterator member_end() const;

            std::string stringify() const;

        private:
            std::map<std::string, DomNode*> members;
    };


}

#endif

