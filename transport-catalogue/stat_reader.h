#pragma once 

#include "transport_catalogue.h"

#include <iostream>
#include <iomanip>

namespace tr_cat {
    namespace aggregations {

        class StatReader {
        public:

            explicit StatReader(TransportCatalogue& catalog)
            :catalog_(catalog), output_(std::cout), input_(std::cin){}

            StatReader(TransportCatalogue& catalog, std::istream& input, std::ostream& output)
            :catalog_(catalog), output_(output), input_(input){}

            void ParseBase() {
                ParseBase(input_);
            }

            void ParseBase(std::istream& input);

        private:

            TransportCatalogue& catalog_;
            std::ostream& output_;
            std::istream& input_;

            std::string_view ParseBus (std::string_view line);

            std::string_view ParseStop (std::string_view line);

            void WriteBusInfo(std::string_view name) {
                WriteBusInfo(output_, name);
            }

            void WriteStopInfo(std::string_view name) {
                WriteStopInfo(output_, name);
            }

            void WriteBusInfo(std::ostream& output, std::string_view name);

            void WriteStopInfo(std::ostream& output, std::string_view name);

        };
    }//aggregations

    void ReadAndWriteStat(aggregations::TransportCatalogue& catalog, std::istream& input, std::ostream& output);

    void ReadAndWriteStat(aggregations::TransportCatalogue& catalog);

}//tr_cat