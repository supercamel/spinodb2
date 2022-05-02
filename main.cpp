#include <iostream>
#include <chrono>
#include "database.h"

using namespace std;

int main()
{
    Spino::Database db;
    auto& col = db.get_collection("test");
    col.append("{\"test field\": 100}");

    db.set_bool_value("lol testing", true);

    cout << db.get_bool_value("lol testing") << endl;

    db.set_string_value("haha", "hello world");
    db.save("db.db");
}
