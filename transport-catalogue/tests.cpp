#include "tests.h"

using namespace std::string_literals;

namespace tr_cat {
namespace tests {
namespace detail {

void AssertImpl(const bool value, const std::string& expr, const std::string& file, const std::string& func,
                unsigned line, std::ostream& stream, const std::string& hint) {
    if (!value) {
        stream << std::boolalpha;
        stream << file << "("s << line << "): "s << func << ": "s << "ASSERT("s << expr << ") failed."s;
        if (!hint.empty()) {
            stream << " Hint: "s << hint << std::endl;;
        }
        abort();
    }
}
}// detail



void TestOutput(const std::string& file_in_make_base, const std::string& file_in_stats,
                const std::string& file_out, const std::string& file_example, std::ostream& stream) {
    {
        std::ifstream inf_make {file_in_make_base};
        std::ifstream inf_stats{file_in_stats};
        std::ofstream outf {file_out};
        size_t size_file;
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
                                            + file_out + '\n' + file_example + '\n', stream);
        }
        stream << "Test without saving graph OK. Saved file is size: "s << size_file << " bytes\n"s;
    }

    {
        std::ifstream inf_make {file_in_make_base};
        std::ifstream inf_stats{file_in_stats};
        std::ofstream outf {file_out};
        size_t size_file;
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
                                            + file_out + '\n' + file_example + '\n', stream);
        }
        stream << "Test with saving graph OK. Saved file is size: "s << size_file << " bytes\n"s;
    }

}

size_t TestCatalogSpeed(const std::string& file_in_make_base, const std::string& file_stats,
                      const std::string& file_out, const std::string& render_file,
                        bool with_graph, std::ostream& stream) {
    std::ifstream inf_make {file_in_make_base};
    std::ofstream outf {file_out};
    aggregations::TransportCatalogue catalog;
    interface::JsonReader reader(catalog, inf_make, outf);
    size_t fsize;
    LOG_DURATION_STREAM("TOTAL WITHOUT CONSTRUCTORS READER AND CATALOG"s, stream);
    {

        {
            LOG_DURATION_STREAM("    READING             "s, stream);
            reader.ReadDocument();
        }
        {
            LOG_DURATION_STREAM("    PARSING             "s, stream);
            reader.ParseDocument();
        }
        {
            LOG_DURATION_STREAM("    ADD STOPS           "s, stream);
            reader.AddStops();
        }
        {
            LOG_DURATION_STREAM("    ADD DISTANCES       ", stream);
            reader.AddDistances();
        }
        {
            LOG_DURATION_STREAM("    ADD BUSES           "s, stream);
            reader.AddBuses();
        }
        {
            LOG_DURATION_STREAM("    RENDER MAP          "s, stream);
            std::ofstream out_render {render_file};
            reader.RenderMap(out_render);
        }
        {
            LOG_DURATION_STREAM("    CREATE GRAPH        "s, stream);
            reader.CreateGraph();
        }
    }
    {

        LOG_DURATION_STREAM("SERIALIZATION   "s, stream);
        fsize = reader.Serialize(with_graph);

    }
    std::ifstream inf_stats {file_stats};
    aggregations::TransportCatalogue catalog2;
    interface::JsonReader reader2(catalog2, inf_stats, outf);
    stream << "\n"s;
    {
        LOG_DURATION_STREAM("    READ & PARSING STATS"s, stream);
        reader2.ReadDocument();
        reader2.ParseDocument();
    }
    {
        LOG_DURATION_STREAM("DESERIALIZATION "s, stream);
        reader2.Deserialize(with_graph);
    }
    {
        {
            LOG_DURATION_STREAM("    GET ANSWERS         "s, stream);
            reader2.GetAnswers();
        }
        {
            LOG_DURATION_STREAM("    PRINT               "s, stream);
            reader2.PrintAnswers();
        }
    }
    stream << "-----------------------------------\n"s;
    return fsize;
}

void Test(const std::string file_in_make_base, const std::string file_in_stats,
          const std::string file_out, const std::string file_example, 
          const std::string file_map, std::ostream& stream = std::cerr) {

    stream << std::endl << "========================================"s << std::endl;
    stream << std::endl << "Testing\nFiles : "s << file_in_make_base;
    stream << ' ' << file_in_stats << ' ' << file_out << ' ' << file_map << std::endl << std::endl;
    TestOutput(file_in_make_base, file_in_stats, file_out, file_example, stream);
    stream << std::endl << "----------------------------------------"s << std::endl;

    stream << std::endl << "Testing Speed without saving graph.\nFiles : "s << file_in_make_base;
    stream << ' ' << file_in_stats << ' ' << file_out << ' ' << file_map << std::endl << std::endl;
    {
        LOG_DURATION_STREAM("TOTAL TIME"s, stream);
        size_t fsize = TestCatalogSpeed (file_in_make_base, file_in_stats, file_out, file_map, false, stream);
        stream << "Size saving file: "s << fsize << " bytes\n"s;
    }

    stream << std::endl << "-----------------------------------------"s << std::endl;

    stream << std::endl << "Testing Speed with saving graph.\nFiles : "s << file_in_make_base;
    stream << ' ' << file_in_stats << ' ' << file_out << ' ' << file_map << std::endl << std::endl;
    {
        LOG_DURATION_STREAM("TOTAL TIME"s, stream);
        size_t fsize = TestCatalogSpeed(file_in_make_base, file_in_stats, file_out, file_map, true, stream);
        stream << "Size saving file: "s << fsize << " bytes\n"s;
    }

    stream << "========================================"s << std::endl;
    stream << std::endl;
}
}//tests
}//tr_cat


        

