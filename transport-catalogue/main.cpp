#include "transport_catalogue.h"
#include "request_handler.h"
#include "json_reader.h"
#include "tests.h"

#include <cassert>
#include <fstream>
#include <iostream>

using namespace std;
using namespace tr_cat;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests|test]\n"sv;
    stream << "Or   : transport_catalogue [make_base|process_requests|test] [saving_graph=OFF]\n"sv;
}

const int TEST_COUNT = 10;

int main(int argc, char* argv[]) {
    
    ostream& stream = cout;
    
    if ((argc != 2) && (argc != 3)) {
        PrintUsage(stream);
        return 1;
    }
    
    const std::string_view mode(argv[1]);
    bool saving_graph = true;
    if (argc == 3) {
        if (argv[2] == "saving_graph=OFF"sv) {
            saving_graph = false;
        } else {
            PrintUsage(stream);
            return 1;
        }
    }
    
    if (mode == "make_base"sv) {
        aggregations::TransportCatalogue catalog;
        interface::JsonReader reader(catalog);
        reader.ReadDocument ();
        reader.ParseDocument ();
        reader.AddStops ();
        reader.AddDistances ();
        reader.AddBuses ();
        reader.CreateGraph();
        reader.Serialize (saving_graph);
    } else if (mode == "process_requests"sv) {
        aggregations::TransportCatalogue catalog;
        interface::JsonReader reader(catalog);
        reader.ReadDocument ();
        reader.ParseDocument ();
        reader.Deserialize (saving_graph);
        reader.GetAnswers ();
        reader.PrintAnswers ();
    } else if (mode == "test"sv) {
        for (int i = 0; i < TEST_COUNT; ++i) {
            string s = to_string(i);
            stream << "Test "s << s << endl;
            tests::Test("tests/"s + to_string(i) + "make_base.json", "tests/"s + s + "process_requests.json"s,
                        "tests/"s + s + "out.json"s, "tests/"s + s + "out_original.json"s, 
                        "tests/maps/"s + s + "map.svg"s, stream);
        } 
    } else {
        PrintUsage(stream);
        return 1;
    }
    return 0;
}
