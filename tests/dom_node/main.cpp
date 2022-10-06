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
    Spino::DomView doc = parser.parse(teststr.c_str());
    cout << "doc: " << doc.stringify() << endl;

    if (doc.get_member("arr").get_element(1).get_int() != 1)
    {
        cout << "error getting 1 value from sub array" << endl;
        return false;
    }

    if(doc.get_member("arr").get_element(3).get_bool() != true)
    {
        cout << "error getting bool from array" << endl;
        return false;
    }

    int count = 0;
    Spino::ElementIterator arriter = doc.get_member("arr").element_begin();
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
    Spino::DomNode doc;
    Spino::DomNode subobj;
    Spino::DomNode arr;
    Spino::DomNode node;

    subobj.set_object();
    arr.set_array();

    doc.set_object();
    subobj.set_object();
    node.set_string("hello");
    subobj.add_member("subfield", node);

    node.set_bool(true);
    subobj.add_member("subfield2", node);

    doc.add_member("subobj", subobj);

    for (int i = 0; i < 5; i++)
    {
        node.set_int(i);
        arr.push_back(node);
    }
    doc.add_member("arr", arr);
    cout << doc.stringify() << endl;

    return true;
}

bool assign_rules()
{
    /*
    Spino::DomNode doc(alloc);
    doc.set_object();

    Spino::DomNode strnode(alloc);
    strnode.set_string("Hello world!", 11);
    strnode.set_key("str");
    doc.push(&strnode);

    cout << Spino::dom_node_type_to_string(strnode.get_type());

*/
    return true;
}

void bson_dump()
{
    /*
    ofstream out("dump.bson", ios::binary | ios::out);

    Spino::Parser parser(alloc);
    Spino::DomNode* node = parser.parse("{\"test\":{\"name\":\"Shazdawg\",\"age\":400},\"gamerskillz\":9001.3}");
    */
    //    std::vector<uint8_t> bson;
    //    node->to_not_bson(bson);

    //    out.write((char*)&bson[0], bson.size());
    //    out.close();
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