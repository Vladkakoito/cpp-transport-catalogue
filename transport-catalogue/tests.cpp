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

        void TestOutput() {
            aggregations::TransportCatalogue catalog;
            std::vector<std::string> lines;

            {
                std::ifstream inf {"input.txt"s};
                std::ofstream outf {"output.txt"s};

                
                ReadBase(catalog, inf);
                ReadAndWriteStat(catalog, inf, outf);
            }

            {
                std::ifstream inf {"output.txt"s};

                for (std::string line; getline(inf, line); ) {
                    lines.push_back(line);
                }
            }

            {
                std::ifstream inf {"output_need.txt"s};
                for (size_t i = 0; i < lines.size(); ++i) {
                    std::string line1;
                    getline(inf, line1);
                    ASSERT_HINT(lines[i] == line1,"\noutput: " + lines[i] + "\n"s + "need:   "s + line1 + "\n"s + "LINE: "s + std::to_string(i) + "\n"s);
                }
                std::cerr << "OUTPUT OK"s << std::endl << std::endl;
            }
        }

        void TestSpeed() {
            std::ifstream inf {"input.txt"s};
            std::ofstream outf {"output.txt"s};

            aggregations::TransportCatalogue catalog;
            {
                aggregations::InputReader reader(catalog, inf);
                LOG_DURATION("BASE FILLING"s);

                {
                    LOG_DURATION("    PARSING       "s);
                    reader.ParseBase();
                }
                
                {
                    LOG_DURATION("    ADD STOPS     "s);
                    reader.AddStops();
                }

                {
                    LOG_DURATION("    ADD DISTANCES ");
                    reader.AddDistances();
                }

                {
                    LOG_DURATION("    ADD BUSES     "s);
                    reader.AddBuses();
                }
            }

            {
                aggregations::StatReader reader(catalog, inf, outf);
                LOG_DURATION("QUERY READING AND ANSWERS WRITING"s);
                reader.ParseBase();
            }
        }

        void TestCatalog() {
            TestOutput();
            TestSpeed();
        }
    }
}//tr_cat


        

