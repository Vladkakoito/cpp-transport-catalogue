#include "transport_catalogue.h"

namespace tr_cat {
    namespace aggregations {

        void TransportCatalogue::AddStop (std::string& name, geo::Coordinates coords) {
            stops_data_.push_back({name, coords, {}});
            stops_container_[stops_data_.back().name] = &(stops_data_.back());
        }

        void TransportCatalogue::AddBus (std::string_view name, std::vector<std::string>& stops, const bool is_ring) {
            buses_data_.push_back({static_cast<std::string>(name), {}});

            for (auto stop : stops) {
                stops_container_[stop]->buses.insert(buses_data_.back().name);
            }
            
            std::vector<Stop*> tmp_stops(stops.size());

            std::transform(stops.begin(), stops.end(), tmp_stops.begin(), [&] (std::string_view element) {
                return FindStop(element);});

            if (tmp_stops.empty()) {
                buses_container_.insert({buses_data_.back().name, &(buses_data_.back())});
                return;
            }

            if (is_ring) {
                tmp_stops.reserve(tmp_stops.size()*2-1);
                for (auto it = tmp_stops.end() - 2; it != tmp_stops.begin(); --it) {
                    tmp_stops.push_back(*it);
                }
                tmp_stops.push_back(tmp_stops.front());
            }

            buses_data_.back().stops = move(tmp_stops);

            buses_container_.insert({buses_data_.back().name, &(buses_data_.back())});
        }

        void TransportCatalogue::AddDistance(const std::string_view lhs_name, const std::string_view rhs_name, double distance) {

            const Stop* lhs = FindStop(lhs_name);
            const Stop* rhs = FindStop(rhs_name);

            distances_[{lhs, rhs}] = distance;
        }

        std::optional<const BusOutput> TransportCatalogue::GetBusInfo (std::string_view name) {

            BusOutput bus_to_output;
            const Bus* bus = FindBus(name);

            if (!bus) {
                return std::nullopt;
            }

            bus_to_output.name = name;
            bus_to_output.stops_count = bus->stops.size();
            bus_to_output.unique_stops_count = ComputeUniqueStopsCount(name);
            bus_to_output.distance = ComputeRouteDistance(name);
            bus_to_output.curvature = bus_to_output.distance / ComputeGeoRouteDistance(name);

            return bus_to_output;
        }

        std::optional<const StopOutput> TransportCatalogue::GetStopInfo (std::string_view name) {

            StopOutput stop_to_output;
            const Stop* stop = FindStop(name);

            if (!stop) {
                return std::nullopt;
            }

            stop_to_output.name = name;
            stop_to_output.buses = stop->buses;

            return stop_to_output;
        }


        std::optional<std::set<std::string_view>> TransportCatalogue::GetBusesFromStop (std::string_view name) {
            
            const Stop* stop = FindStop(name);
            if (!stop) {
                return std::nullopt;
            }
            return stop->buses;
        }


        TransportCatalogue::Stop* TransportCatalogue::FindStop (std::string_view name) const {
            if (!stops_container_.count(name)) {
                return nullptr;
            }
            return stops_container_.at(name);
        }

        TransportCatalogue::Bus* TransportCatalogue::FindBus (std:: string_view name) const {
            if (!buses_container_.count(name)) {
                return nullptr;
            }
            return buses_container_.at(name);
        }

        int TransportCatalogue::GetDistance(const Stop* lhs, const Stop* rhs) const{

            if (distances_.count ({lhs, rhs})) {
                return distances_.at({lhs, rhs});
            }

            if (distances_.count ({rhs, lhs})) {
                return distances_.at({rhs, lhs});
            }

            return static_cast<int>(geo::ComputeDistance(lhs->coordinates, rhs->coordinates));
        }


        int TransportCatalogue::ComputeRouteDistance (std::string_view name) const {
            
            const Bus* bus = FindBus(name);
            int distance = 0;
            const std::vector<Stop*>& stops = bus->stops;

            for (size_t i = 1; i < stops.size(); ++i) {
                distance += GetDistance(stops[i-1], stops[i]);
            }
            return distance;
        }

        double TransportCatalogue::ComputeGeoRouteDistance (std::string_view name) const {
            const Bus* bus = FindBus(name);
            double distance = 0;
            const std::vector<Stop*>& stops = bus->stops;

            for (size_t i = 1; i < stops.size(); ++i) {
                distance += geo::ComputeDistance(stops[i-1]->coordinates, stops[i]->coordinates);
            }
            return distance;
        }

        int TransportCatalogue::ComputeUniqueStopsCount (std::string_view name) const {

            std::set<std::string_view> stops;

            for (const Stop* stop : FindBus(name)->stops) {
                stops.insert(stop->name);
            }

            return static_cast<int>(stops.size());
        }
    }//aggregations
}//tr_cat