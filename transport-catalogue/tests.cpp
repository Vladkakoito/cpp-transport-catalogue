#include "tests.h"

using namespace std::string_literals;

namespace tr_cat {
namespace tests {
namespace detail {

void AssertImpl(const bool value, const std::string& expr, const std::string& file, const std::string& func,
                unsigned line, const std::string& hint) {
    if (!value) {
        std::cerr << std::boolalpha;
        std::cerr << file << "("s << line << "): "s << func << ": "s << "ASSERT("s << expr << ") failed."s;
        if (!hint.empty()) {
            std::cerr << " Hint: "s << hint << std::endl;;
        }
        abort();
    }
}
}// detail



void TestOutput(const std::string& file_in_make_base, const std::string& file_in_stats,
                const std::string& file_out, const std::string& file_example) {
    {
        std::ifstream inf_make {file_in_make_base};
        std::ifstream inf_stats{file_in_stats};
        std::ofstream outf {file_out};
        unsigned long long int size_file;
        {
            aggregations::TransportCatalogue catalog;
            interface::JsonReader reader(catalog, inf_make, outf);
            reader.ReadDocument();
            reader.ParseDocument();
            reader.AddStops();
            reader.AddDistances();
            reader.AddBuses();
            reader.CreateGraph();
            size_file = reader.Serialize(false);
            inf_make.close();
        }
        {
            aggregations::TransportCatalogue catalog;
            interface::JsonReader reader(catalog, inf_stats, outf);
            reader.ReadDocument();
            reader.ParseDocument();
            reader.Deserialize(false);
            reader.GetAnswers();
            reader.PrintAnswers();
            inf_stats.close();
            outf.close();
            ASSERT_HINT(reader.TestingFilesOutput(file_out, file_example),
                                            "\nOutput files not equal. Without graph:\n"s
                                            + file_out + '\n' + file_example + '\n');
        }
        std::cerr << "Test without saving graph OK. Saved file is size: "s << sizeof (size_file) / 8 << "bytes\n"s;
    }

    {
        std::ifstream inf_make {file_in_make_base};
        std::ifstream inf_stats{file_in_stats};
        std::ofstream outf {file_out};
        unsigned long long int size_file;
        {
            aggregations::TransportCatalogue catalog;
            interface::JsonReader reader(catalog, inf_make, outf);
            reader.ReadDocument();
            reader.ParseDocument();
            reader.AddStops();
            reader.AddDistances();
            reader.AddBuses();
            reader.CreateGraph();
            size_file = reader.Serialize(true);
            inf_make.close();
        }
        {
            aggregations::TransportCatalogue catalog;
            interface::JsonReader reader(catalog, inf_stats, outf);
            reader.ReadDocument();
            reader.ParseDocument();
            reader.Deserialize(true);
            reader.GetAnswers();
            reader.PrintAnswers();
            inf_stats.close();
            outf.close();
            ASSERT_HINT(reader.TestingFilesOutput(file_out, file_example),
                                            "\nOutput files not equal. Without graph:\n"s
                                            + file_out + '\n' + file_example + '\n');
        }
        std::cerr << "Test with saving graph OK. Saved file is size: "s << sizeof (size_file) / 8 << "bytes\n"s;
    }

}

void TestRenderSpeed(const std::string& file_in, const std::string& file_out) {
    std::cerr << "Testing Render "s << file_in << std::endl << std::endl;
    LOG_DURATION("TOTAL"s);
    aggregations::TransportCatalogue catalog;
    std::ifstream inf {file_in};
    std::ofstream outf {file_out};
    interface::JsonReader reader(catalog, inf, outf);
    {
        LOG_DURATION("BASE FILLING"s);

        {
            LOG_DURATION("    READING         "s);
            reader.ReadDocument();
        }

        {
            LOG_DURATION("    PARSING         "s);
            reader.ParseDocument();
        }

        {
            LOG_DURATION("    ADD STOPS       "s);
            reader.AddStops();
        }

        {
            LOG_DURATION("    ADD DISTANCES   ");
            reader.AddDistances();
        }

        {
            LOG_DURATION("    ADD BUSES       "s);
            reader.AddBuses();
        }
    }
    {
        LOG_DURATION("RENDERING"s);
        {
            LOG_DURATION("    DRAWING         "s);
            reader.RenderMap(outf);
        }
    }
    std::cerr << "-----------------------------------\n\n"s;
}

size_t TestCatalogSpeed(const std::string& file_in_make_base, const std::string& file_stats,
                      const std::string& file_out, const std::string& render_file, bool with_graph) {
    std::ifstream inf_make {file_in_make_base};
    std::ofstream outf {file_out};
    aggregations::TransportCatalogue catalog;
    interface::JsonReader reader(catalog, inf_make, outf);
    size_t fsize;
    LOG_DURATION("TOTAL WITHOUT CONSTRUCTORS READER AND CATALOG"s);
    {
        LOG_DURATION("BASE FILLING"s);

        {
            LOG_DURATION("    READING         "s);
            reader.ReadDocument();
        }

        {
            LOG_DURATION("    PARSING         "s);
            reader.ParseDocument();
        }

        {
            LOG_DURATION("    ADD STOPS       "s);
            reader.AddStops();
        }

        {
            LOG_DURATION("    ADD DISTANCES   ");
            reader.AddDistances();
        }

        {
            LOG_DURATION("    ADD BUSES       "s);
            reader.AddBuses();
        }
        {
            LOG_DURATION("    RENDER MAP      "s);
            std::ofstream out_render {render_file};
            reader.RenderMap(out_render);
        }
        {
            LOG_DURATION("    CREATE GRAPH    "s);
            reader.CreateGraph();
        }
    }
    {

        LOG_DURATION("    SERIALIZATION       "s);
        fsize = reader.Serialize(with_graph);

    }
    std::ifstream inf_stats {file_stats};
    aggregations::TransportCatalogue catalog2;
    interface::JsonReader reader2(catalog2, inf_stats, outf);
    {
        LOG_DURATION("READ AND PARSING STATS"s);
        reader2.ReadDocument();
        reader2.ParseDocument();
    }
    {
        LOG_DURATION("DESERIALIZATION"s);
        reader2.Deserialize(with_graph);
    }
    {
        LOG_DURATION("ANSWERS  "s);
        {
            LOG_DURATION("    GET ANSWERS     "s);
            reader2.GetAnswers();
        }
        {
            LOG_DURATION("    PRINT           "s);
            reader2.PrintAnswers();
        }
    }
    std::cerr << "-----------------------------------\n\n"s;
    return fsize;
}

void Test(const std::string file_in_make_base, const std::string file_in_stats,
          const std::string file_out, const std::string file_example, const std::string file_map) {
/*
    std::cerr << std::endl << "========================================"s << std::endl;
    std::cerr << std::endl << "Testing "s << std::endl << std::endl;
    TestOutput(file_in_make_base, file_in_stats, file_out, file_example);
    std::cerr << std::endl << "========================================"s << std::endl;
*/
    std::cerr << std::endl << "Testing Speed without saving graph "s << std::endl << std::endl;
    {
        LOG_DURATION("TOTAL TIME"s);
        size_t fsize = TestCatalogSpeed (file_in_make_base, file_in_stats, file_out, file_map, false);
        std::cerr << "Size saving file: "s << fsize << '\n';
    }

    std::cerr << std::endl << "========================================"s << std::endl;
    std::cerr << std::endl;

    std::cerr << std::endl << "Testing Speed with saving graph "s << std::endl << std::endl;
    {
        LOG_DURATION("TOTAL TIME"s);
        size_t fsize = TestCatalogSpeed(file_in_make_base, file_in_stats, file_out, file_map, true);
        std::cerr << "Size saving file: "s << fsize << '\n';
    }

    std::cerr << std::endl << "========================================"s << std::endl;
    std::cerr << std::endl;
}
}//tests
}//tr_cat


        

