#pragma once

#include "geo.h"

#include <string>
#include <list>
#include <unordered_set>
#include <vector>
#include <tuple>
#include <deque>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <cmath>
#include <utility>

namespace tr_cat {
    namespace aggregations {

        using namespace std::string_literals;

        struct BusOutput {
            std::string_view name;
            int stops_count = 0;
            int unique_stops_count = 0;
            int distance = 0;
            double curvature = 0;
        };

        struct StopOutput {
            std::string_view name;
            std::set<std::string_view> buses;
        };



        class TransportCatalogue {

        public:

            void AddStop (std::string& name, geo::Coordinates coords);

            void AddBus (std::string_view name, std::vector<std::string>& stops, const bool is_ring);

            void AddDistance(const std::string_view lhs, const std::string_view rhs, double distance);

            std::optional<const BusOutput>  GetBusInfo (std::string_view name);

            std::optional<const StopOutput> GetStopInfo (std::string_view name);

            std::optional<std::set<std::string_view>> GetBusesFromStop (std::string_view name);


        private:
            struct Stop;
            struct Bus {
                std::string name;
                std::vector<Stop*> stops;
            };

            struct Stop {
                std::string name;
                geo::Coordinates coordinates;
                std::set<std::string_view> buses;
            };

            class DistanceHasher {
            public:
                size_t operator() (const std::pair<const Stop*, const Stop*> element) const {
                    const size_t shift = (size_t)log2(1 + sizeof(Stop));
                    const size_t result = (size_t)(element.first) >> shift;
                    return result + ((size_t)(element.second) >> shift) * 37;
                }
            };

            class DistanceCompare {
            public:
                bool operator() (const std::pair<const Stop*, const Stop*> lhs, const std::pair<const Stop*, const Stop*> rhs) const {
                    return lhs.first == rhs.first && rhs.second == lhs.second;
                }
            };

            std::unordered_map<std::pair<const Stop*, const Stop*>, int, DistanceHasher, DistanceCompare> distances_;


            std::deque<Stop> stops_data_;
            std::deque<Bus> buses_data_;

            std::unordered_map<std::string_view, Stop*> stops_container_;
            std::unordered_map<std::string_view, Bus*> buses_container_;


            Stop* FindStop (std::string_view name) const;

            Bus* FindBus (std:: string_view name)const;

            int ComputeRouteDistance (std::string_view name) const;

            double ComputeGeoRouteDistance (std::string_view name) const;

            int GetDistance(const Stop* lhs, const Stop* rhs) const;

            int ComputeUniqueStopsCount (std::string_view name) const;

        };
    }//aggregations
}//tr_cat