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



        void TestOutput(const std::string& file_in, const std::string& file_out, const std::string& file_example) {
            std::ifstream inf {file_in};
            std::ofstream outf {file_out};
            aggregations::TransportCatalogue catalog;
            interface::JsonReader reader(catalog, inf, outf);
            reader.ReadDocument();
            reader.ParseDocument();
            reader.AddStops();
            reader.AddDistances();
            reader.AddBuses();
            reader.CreateGraph();
            reader.GetAnswers();
            reader.PrintAnswers();
            inf.close();
            outf.close();
            ASSERT_HINT(reader.TestingFilesOutput(file_out, file_example), "\nOutput files not equal:\n"s 
                                                + file_out + '\n' + file_example + '\n');
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

        void TestCatalogSpeed(const std::string& file_in, const std::string& file_out, const std::string& render_file) {
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
                LOG_DURATION("ANSWERS  "s);
                {
                    LOG_DURATION("    GET ANSWERS     "s);
                    reader.GetAnswers();
                }
                {
                    LOG_DURATION("    PRINT           "s);
                    reader.PrintAnswers();
                }
            }
            std::cerr << "-----------------------------------\n\n"s;
        }

        void Test(const std::string file_in, const std::string file_out, const std::string file_example,
                                                                        const std::string file_map) {
                                                                            
            std::cerr << std::endl << "========================================"s << std::endl;
            std::cerr << std::endl << "Testing "s << file_in << std::endl << std::endl;
            RUN_TEST(TestOutput, file_in, file_out, file_example);
            std::cerr << std::endl << "========================================"s << std::endl;

            std::cerr << std::endl << "Testing Speed "s << file_in << std::endl << std::endl;
            RUN_TEST(TestCatalogSpeed, file_in, file_out, file_map);
            std::cerr << std::endl << "========================================"s << std::endl;
            std::cerr << std::endl;
        }
    }//tests
}//tr_cat


        

