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
    cout << "here" << endl;
    Spino::Database db;
    cout << "database created" << endl;

    db.set_int_value("testval", 100);

    cout << db.get_int_value("testval") << endl;
}
