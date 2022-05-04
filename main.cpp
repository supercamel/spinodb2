#include <iostream>
#include <chrono>
#include "database.h"

using namespace std;
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;


int main()
{
    Spino::Database db;
    auto& col = db.get_collection("test");
    for(int i = 0; i < 1000000; i++)
        col.append("{\"test field\": 100}");

    db.set_bool_value("lol testing", true);

    cout << db.get_bool_value("lol testing") << endl;

    db.set_string_value("haha", "hello world");
    auto t1 = high_resolution_clock::now();
    db.save_not_bson_collection("test", "db.bson");
    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;

    cout << "bson save time: " << ms_double.count() << endl;

    t1 = high_resolution_clock::now();
    db.load_not_bson_collection("test2", "db.bson");
    t2 = high_resolution_clock::now();
    ms_double = t2 - t1;
    cout << "bson load time: " << ms_double.count() << endl;
}
