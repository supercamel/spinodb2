#include <iostream>
#include <string>
#include "dom_node.h"
#include "parser.h"
#include "index.h"

using namespace std;


int find_first()
{

}


int main()
{
    Spino::GenericIndex idx("count");
    Spino::Parser parser;

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            std::string json;
            json = "{\"count\":";
            json += std::to_string(i);
            json += "}";

            const Spino::DomView& view = parser.parse(json);
            idx.insert(view);
        }
    }

    cout << "printing" << endl;
    idx.print();
    cout << "done" << endl;

/*
    Spino::DomNode node(alloc);
    node.set_uint(3);
    parser.reset();
    cout << "find first" << endl;
    auto *sln = idx.find_first(&node);
    cout << sln << endl;
    cout << sln->dom_node->stringify() << endl;
    cout << "done" << endl;

    node.set_uint(9);
    sln = idx.find_last(&node);
    cout << sln << endl;
    cout << sln->dom_node->stringify() << endl;

    cout << "equal range" << endl;
    auto range = idx.equal_range(&node);
    */

    return 0;
}