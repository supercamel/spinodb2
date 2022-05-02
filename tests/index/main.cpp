#include <iostream>
#include <string>
#include "dom_node.h"
#include "parser.h"
#include "index.h"

using namespace std;

Spino::DomAllocator alloc;

int main()
{
    Spino::Index idx("count");
    Spino::Parser parser(alloc);

    char json[128];
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            sprintf(json, "{\"count\":%i}", i);
            auto *doc = parser.parse(json);
            idx.insert(doc);
        }
    }

    idx.print();

    sprintf(json, "{\"count\":3}");
    parser.reset();
    auto *doc = parser.parse(json);
    auto *sln = idx.find_first(doc);
    cout << sln << endl;
    cout << sln->dom_node->stringify() << endl;

    sprintf(json, "{\"count\": 8}");
    doc = parser.parse(json);
    sln = idx.find_last(doc);
    cout << sln << endl;
    cout << sln->dom_node->stringify() << endl;
    sln = sln->get_next();
    cout << sln->dom_node->stringify() << endl;

    return 0;
}