#include <iostream>
#include <string>
#include "dom_node.h"
#include "parser.h"
#include "query_executor.h"
#include "query_parser.h"
#include "query_printer.h"

using namespace std;

Spino::DomAllocator alloc;
Spino::Parser parser(alloc);

bool bfc()
{
    Spino::DomNode *doc = parser.parse("{\"count\":105}");

    Spino::QueryParser qp("{count:105}");
    auto querynode = qp.parse_expression();

    Spino::QueryExecutor exec(alloc, doc);
    if (exec.resolve(querynode) == false)
    {
        cout << "basic numeric comparison query did not match the document" << endl;
    }

    doc->set_null();
    alloc.free(doc);

    if (alloc.get_allocations() != 0)
    {
        cout << "memory leak in bfc()" << endl;
        return false;
    }

    return true;
}

int main()
{
    if (bfc() != true)
    {
        return -1;
    }
    return 0;
}