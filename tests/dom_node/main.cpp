#include <iostream>
#include <string>
#include <fstream>
#include "dom_node.h"
#include "parser.h"

using namespace std;

bool parsing()
{
    std::string teststr = "{\"arr\":[0, 1, false, true, null, \"hello world\"], \"obj\":{\"subobj\":\"string\"}}";
    //std::string teststr = "{\"hello\":\"world\"}";
    Spino::Parser parser;
    cout << "parsing" << endl;
    Spino::DomNode* doc = parser.parse(teststr.c_str());
    cout << "doc: " << doc->stringify() << endl;

    if (doc->get_member("arr").get_element(1).get_int() != 1)
    {
        cout << "error getting 1 value from sub array" << endl;
        return false;
    }

    if(doc->get_member("arr").get_element(3).get_bool() != true)
    {
        cout << "error getting bool from array" << endl;
        return false;
    }

    int count = 0;
    Spino::ElementIterator arriter = doc->get_member("arr").element_begin();
    while(arriter) {
        count++;
        arriter++;
    }
    if(count != 6) {
        cout << "should have counted 6 members but did not" << endl;
        return false;
    }

    return true;
}

bool building_an_object()
{
    cout << "making the nodes" << endl;
    Spino::DomNode* doc = Spino::dom_node_allocator.make();
    Spino::DomNode* subobj = Spino::dom_node_allocator.make();
    Spino::DomNode* arr = Spino::dom_node_allocator.make();
    Spino::DomNode* node = Spino::dom_node_allocator.make();

    subobj->set_object();
    arr->set_array();

    doc->set_object();
    subobj->set_object();
    node->set_string("hello", 5, true);
    subobj->add_member("subfield", node);

    node->set_bool(true);
    subobj->add_member("subfield2", node);

    doc->add_member("subobj", subobj);

    for (int i = 0; i < 5; i++)
    {
        node->set_int(i);
        arr->push_back(node);
    }
    doc->add_member("arr", arr);
    cout << doc->stringify() << endl;

    return true;
}

bool assign_rules()
{
    return true;
}

void bson_dump()
{
}

int main()
{
    if (!parsing())
    {
        return -1;
    }

    if (!building_an_object())
    {
        return -1;
    }

    if (!assign_rules())
    {
        return -1;
    }

    bson_dump();
    return 0;
}