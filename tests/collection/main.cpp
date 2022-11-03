#include <iostream>
#include <string>
#include <chrono>
#include "dom_node.h"
#include "parser.h"
#include "collection.h"

using namespace std;

Spino::JournalWriter jw;
Spino::Collection col("testCol", jw);

bool find_one()
{
    col.append("{\"test\":4}");

    // find unindexed field
    cout << col.find_one("{test:4}")->stringify() << endl;

    if(col.find_one("{test:\"lol\"}") != nullptr)
    {
        cout << "find_one returned not nullptr for a query that doesn't match anything" << endl;
    }

    auto start = chrono::high_resolution_clock::now();
    for(int i = 0; i < 1; i++) {
        col.append("{\"test\":4}");
    }
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    cout << "append 1M documents: " << duration.count() << "ms" << endl;

    col.append("{\"hw\":\"jibba jabba\"}");
    col.append("{\"hw\":\"hello world\"}");

    cout << col.find_one("{hw:\"jibba jabba\"}")->stringify() << endl;
    return true;
}

bool cursor()
{
    cout << "Cursor test:" << endl;
    auto cursor = col.find("{}");
    while(cursor->has_next())
    {
        cout << cursor->next_dom()->stringify() << endl;
    }

    cursor = col.find("{idx:{$range:[100, 110]}}");
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
    cout << "drop()" << endl;
    col.append("{\"drop_me\": 100, \"idx\": 100}");

    cout << "drop 1" << endl;
    col.drop("{drop_me:{$exists:true}}");

    cout << "checking..." << endl;
    if(col.find_one("{drop_me:100}") != nullptr)
    {
        cout << "drop failed" << endl;
        return false;
    }

    auto cursor = col.find("{idx:100}");
    while(cursor->has_next()) {
        cout << cursor->next() << endl;
    }
    cout << "done" << endl;
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
        Spino::DomNode* doc = Spino::dom_node_allocator.make();
        Spino::DomNode* idx = Spino::dom_node_allocator.make();
        doc->set_object();
        idx->set_int(i);
        doc->add_member("idx", idx);
        col.append(doc);
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
    // show time take in microseconds
    ms_double = t2 - t1;
    cout << "find time: " << ms_double.count() << endl;

    t1 = high_resolution_clock::now();
    Spino::DomNode idx;
    idx.set_int(9000);
    const Spino::DomView* view = col.find_one("idx", idx);

    t2 = high_resolution_clock::now();
    cout << view->stringify() << endl;
    ms_double = t2-t1;
    cout << "find one time: " << ms_double.count()/1000000.0 << endl;

    // delete performance 
    t1 = high_resolution_clock::now();
    auto cursor = col.find("{}");
    int count = 0;
    while(cursor->has_next()) 
    {
        auto next_dom = cursor->next_dom();
        if(next_dom->has_member("idx")) {
            std::string query = "{idx:" + next_dom->get_member("idx").stringify() + "}";
            col.drop(query);
            count++;
        }
        //cout << cursor->next() << endl;
    }
    t2 = high_resolution_clock::now();
    ms_double = t2 - t1;
    cout << "delete time: " << ms_double.count() << endl;
    cout << "count: " << count << endl;

    return true;
}

bool nin_query() {
    col.append("{\"name\": \"Sheep\"}");
    col.append("{\"name\": \"Mouse\"}");
    auto cursor = col.find("{}");
    while(cursor->has_next()) {
        cout << cursor->next() << endl;
    }
    cout << endl;

    cout << "calling find" << endl;
    size_t n = col.find("{name: {$in: [\"Horse\", \"Camel\", \"Donkey\"]}}")->count();
    cout << n << endl;
    return n == 1;
}

bool upsert() {
    col.append("{\"name\": \"Horse\", \"legs\": 4}");
    col.upsert("{name: \"Horse\"}", "{\"name\": \"Horse\", \"legs\": 3}");

    cout << "Horse: " << col.find_one("{name: \"Horse\"}")->stringify() << endl;
    cout << "end of horse" << endl;
    if(col.find_one("{name: \"Horse\"}")->get_member("legs").get_uint() != 3) {
        cout << "upsert failed" << endl;
        return false;
    }

    return true;
}

int main()
{
    if(!upsert()) {
        cout << "upsert failed" << endl;
        return -1;
    }


    if(!nin_query()) {
        cout << "nin_query failed" << endl;
        return -1;
    }

    if(!find_one()) 
    {
        return -1;
    }

    if(!cursor()) {
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

    col.clear();
    cout << Spino::dom_node_allocator.get_n_allocations() << endl;
    return 0;
}
