#include <iostream>
#include <string>
#include <chrono>
#include "dom_node.h"
#include "parser.h"
#include "collection.h"

using namespace std;


Spino::Collection col("testCol");

bool find_one()
{
    col.append("{\"test\":4}");

    // find unindexed field
    cout << col.find_one("{test:4}")->stringify() << endl;

    if(col.find_one("{test:\"lol\"}") != nullptr)
    {
        cout << "find_one returned not nullptr for a query that doesn't match anything" << endl;
    }

    col.append("{\"test\":12}");
    cout << col.find_one("{test:{$gt:5}}")->stringify() << endl;
    cout << col.find_one("{test:{$lt:5}}")->stringify() << endl;
    cout << col.find_one("{test:{$range:[8,15]}}")->stringify() << endl;
    cout << col.find_one("{test:{$type:number}}")->stringify() << endl;
    cout << col.find_one("{test:{$exists: true}}")->stringify() << endl;

    col.append("{\"hw\":\"jibba jabba\"}");
    col.append("{\"hw\":\"hello world\"}");

    cout << col.find_one("{hw:\"jibba jabba\"}")->stringify() << endl;
    return true;
}

bool cursor()
{
    auto cursor = col.find("{idx:{$range:[100, 110]}}");
    while(cursor->has_next()) 
    {
        cout << cursor->next() << endl;
    }

    cursor = col.find("{test:{$exists:true}}");
    while(cursor->has_next())
    {
        cout << cursor->next() << endl;
    }
    return true;
}

bool drop()
{
    col.append("{\"drop_me\": 100, \"idx\": 100}");
    col.drop("{drop_me:{$exists:true}}");

    if(col.find_one("{drop_me:100}") != nullptr)
    {
        cout << "drop failed" << endl;
        return false;
    }

    auto cursor = col.find("{idx:100}");
    while(cursor->has_next()) {
        cout << cursor->next() << endl;
    }
    return true;
}

bool performance()
{
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto t1 = high_resolution_clock::now();

    char json[128];
    for(int i = 0; i < 1000000; i++) 
    {
        Spino::DomNode doc(col.get_allocator());
        Spino::DomNode idx(col.get_allocator());
        doc.set_object();
        idx.set_int(i);
        idx.set_key("idx");
        doc.push(&idx);
        col.append(&doc);
    }

    //col.print();

    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;

    cout << "append time: " << ms_double.count() << endl;
    col.create_index("idx");

    t1 = high_resolution_clock::now();

    sprintf(json, "{idx:900000}");
    for(int i = 0; i < 1000000; i++) 
    {
        col.find_one(json);
    }
    t2 = high_resolution_clock::now();
    ms_double = t2-t1;
    cout << "find one time: " << ms_double.count()/1000.0 << endl;

    t1 = high_resolution_clock::now();
    Spino::DomNode idx(col.get_allocator());
    col.find_one("idx", &idx);
    t2 = high_resolution_clock::now();
    ms_double = t2-t1;
    cout << "find one time: " << ms_double.count()/1000.0 << endl;

    return true;
}

int main()
{
    cout << sizeof(Spino::DomNode) << endl;
    if(!find_one()) 
    {
        return -1;
    }

    if(!drop())
    {
        return -1;
    }

    if(!performance())
    {
        return -1;
    }

    if(!cursor())
    {
        return -1;
    }

    return 0;
}