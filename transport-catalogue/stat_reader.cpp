#include "stat_reader.h"
#include <ios>

using namespace std::string_literals;

namespace tr_cat {
    namespace aggregations {

        void StatReader::ParseBase(std::istream& input) {
            int n;
            input >> n;

            {
                std::string _;
                getline(input, _);
            }

            for (int i = 0; i < n; ++i) {
                std::string line;
                getline(input, line);

                std::string word = line.substr(0, line.find_first_of(' '));
                line.erase(0, word.size());
                line.erase(0, line.find_first_not_of(' '));

                if (word == "Bus"s) {
                    WriteBusInfo(ParseBus(line));
                }

                if (word == "Stop"s) {
                    WriteStopInfo(ParseStop(line));
                }
            }
        }

        void StatReader::WriteBusInfo(std::ostream& output, std::string_view name) {

            std::optional<BusOutput> bus = catalog_.GetBusInfo(name);

            if (!bus) {
                output << "Bus "s << name << ": not found\n"s;
                return;
            }

            output << "Bus "s << bus->name << ": "s << bus->stops_count << " stops on route, "s << bus->unique_stops_count << " unique stops, "s;
            output << std::setprecision(6) << static_cast<double>(bus->distance) << " route length, "s << bus->curvature << " curvature\n"s;
        }

        void StatReader::WriteStopInfo(std::ostream& output, std::string_view name) {

            std::optional<StopOutput> stop = catalog_.GetStopInfo(name);

            if (!stop) {
                output << "Stop "s << name << ": not found"s << std::endl;
                return;
            }

            if (stop->buses.empty()) {
                output << "Stop " << name << ": no buses"s << std::endl;
                return;
            }

            output << "Stop "s << name << ": buses"s;

            for (std::string_view bus : stop->buses) {
                output << " "s << bus;
            }

            output << std::endl;
        }

        std::string_view StatReader::ParseStop (std::string_view line) {
            line.remove_suffix(line.size() - line.find_last_not_of(' ') - 1);
            return line;
        }

        std::string_view StatReader::ParseBus(std::string_view line) {
            line.remove_suffix(line.size() - line.find_last_not_of(' ') - 1);
            return line;
        }
    }//aggregations

    void ReadAndWriteStat(aggregations::TransportCatalogue& catalog, std::istream& input, std::ostream& output) {
        aggregations::StatReader reader(catalog, input, output);
        reader.ParseBase();
    }

    void ReadAndWriteStat(aggregations::TransportCatalogue& catalog) {
        ReadAndWriteStat(catalog, std::cin, std::cout);
    }
}//tr_cat