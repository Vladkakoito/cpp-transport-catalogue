#include "transport_catalogue.h"
#include "request_handler.h"
#include "json_reader.h"
#include "tests.h"

#include <cassert>
#include <fstream>
#include <iostream>

using namespace std;
using namespace tr_cat;

int main() {
    tests::Test("tests/test_make.json", "tests/test_stats.json"s, "tests/test_out.json"s,
                "tests/test_answer.json"s, "test_files/maps/test5_map.svg"s);

}
