#include <iostream>
#include <string>
#include <fstream>
#include "dom_node.h"
#include "parser.h"

using namespace std;

Spino::DomAllocator alloc;

bool parsing()
{
    std::string teststr = "{\"arr\":[0, 1, false, true, null, \"hello world\"], \"obj\":{\"subobj\":\"string\"}}";
    Spino::Parser parser(alloc);
    cout << "parsing" << endl;
    Spino::DomNode *doc = parser.parse(teststr.c_str());

    cout << doc->stringify() << endl;
    cout << "running the checks" << endl;
    Spino::DomNode* val = doc->get_member("arr", 3)->get_index(0);
    if (doc->get_member("arr", 3)->get_index(1)->get_int() != 1)
    {
        cout << "error getting 1 value from sub array" << endl;
        return false;
    }

    if(doc->get_member("arr", 3)->get_index(3)->get_bool() != true) 
    {
        cout << "error getting bool from array" << endl;
        return false;
    }

    int count = 0;
    Spino::DomNode* arrmember = doc->get_member("arr", 3)->begin();
    while(arrmember) {
        count++;
        arrmember = arrmember->get_next();
    }
    if(count != 6) {
        cout << "should have counted 6 members but did not" << endl;
        return false;
    }

    doc->destroy();
    alloc.free(doc);
    
    cout << alloc.get_allocations() << endl;
    return true;
}

bool building_an_object()
{
    cout << "making the nodes" << endl;
    Spino::DomNode doc(alloc);
    Spino::DomNode subobj(alloc);
    Spino::DomNode subobjfield(alloc);

    cout << "setting to obj" << endl;
    doc.set_object();
    subobj.set_object();
    cout << "pushing things" << endl;
    subobjfield.set_string("hello", 6);
    subobjfield.set_key("subfield");
    subobj.push(&subobjfield);
    subobj.set_key("subobj");
    doc.push(&subobj);

    Spino::DomNode dfield(alloc);
    dfield.set_double(10.23);
    dfield.set_key("dval");

    Spino::DomNode ufield(alloc);
    ufield.set_uint(1432345);
    ufield.set_key("ui");
    doc.push(&ufield);
    doc.push(&dfield);

    cout << "checking the things" << endl;
    string result = "{\"dval\":10.23,\"ui\":1432345,\"subobj\":{\"subfield\":\"hello\"}}";
    if (result != doc.stringify().c_str())
    {
        cout << "document didnt' stringify into expected text" << endl;
        cout << result << endl;
        cout << doc.stringify() << endl;
        return false;
    }
    cout << doc.stringify() << endl;

    doc.destroy();
    if (alloc.get_allocations() != 0)
    {
        cout << "memory leak after demolishing doc" << endl;
    }
    return true;
}

bool assign_rules() 
{
    Spino::DomNode doc(alloc);
    doc.set_object();

    Spino::DomNode strnode(alloc);
    strnode.set_string("Hello world!", 11);
    strnode.set_key("str");
    doc.push(&strnode);

    cout << Spino::dom_node_type_to_string(strnode.get_type());

    return true;
}

void bson_dump()
{
    ofstream out("dump.bson", ios::binary | ios::out);

    Spino::Parser parser(alloc);
    Spino::DomNode* node = parser.parse("{\"test\":{\"name\":\"Shazdawg\",\"age\":400},\"gamerskillz\":9001.3}");
    std::vector<uint8_t> bson;
    node->to_bson(bson);

    out.write((char*)&bson[0], bson.size());
    out.close();
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

    if(!assign_rules()) 
    {
        return -1;
    }

    bson_dump();
    return 0;
}