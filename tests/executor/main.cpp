#include <iostream>
#include <string>
#include "dom_node.h"
#include "parser.h"
#include "query_executor.h"
#include "query_parser.h"

using namespace std;

Spino::Parser parser;


int main()
{
    if (bfc() != true)
    {
        return -1;
    }
    return 0;
}