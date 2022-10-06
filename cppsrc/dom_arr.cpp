#include "dom_arr.h"
#include "dom_node.h"
#include <sstream>

namespace Spino {

    const DomView& ElementIterator::operator*() { DomView* n = *it; return *n; }

    const class DomView* ElementIterator::operator->() { return (*it); }

    bool ElementIterator::has_next() { return it != array->end(); }

    const DomView& ElementIterator::next() { return **it++; }


    DomArray::DomArray() {

    }

    DomArray::~DomArray() {
        for(auto i : elements) {
            delete i;
        }
    }

    const DomView& DomArray::get_index(size_t pos) const
    {
        return *elements.at(pos);
    }

    void DomArray::push_back(DomNode& val)
    {
        DomNode* n = new DomNode();
        *n = val;
        elements.push_back(n);
    }


    ElementIterator DomArray::element_begin() const
    {
        return ElementIterator(&elements, elements.begin());
    }

    ElementIterator DomArray::element_end() const
    {
        return ElementIterator(&elements, elements.end());
    }

    std::string DomArray::stringify() const
    {
        std::stringstream ss;
        ss << "[";
        size_t count = 0;
        size_t size = elements.size();

        for(auto& element : elements) {
            ss << element->stringify();
            if(++count != size) {
                ss << ",";
            }
        }
        ss << "]";
        return ss.str();
    }

}
