#include "../include/Trip.h"

#include <sstream>

namespace bus_reservation {

Trip::Trip(const std::string& tripId,
           const std::string& vehicleId,
           const std::string& from,
           const std::string& to,
           const std::string& departure,
           std::uint32_t fare)
    : id_(tripId), busId_(vehicleId), origin_(from), destination_(to), departureDateTime_(departure), fareInCents_(fare) {}

std::string Trip::getId() const {
    return id_;
}

std::string Trip::getBusId() const {
    return busId_;
}

void Trip::setBusId(const std::string& vehicleId) {
    busId_ = vehicleId;
}

std::string Trip::getOrigin() const {
    return origin_;
}

void Trip::setOrigin(const std::string& from) {
    origin_ = from;
}

std::string Trip::getDestination() const {
    return destination_;
}

void Trip::setDestination(const std::string& to) {
    destination_ = to;
}

std::string Trip::getDepartureDateTime() const {
    return departureDateTime_;
}

void Trip::setDepartureDateTime(const std::string& departure) {
    departureDateTime_ = departure;
}

std::uint32_t Trip::getFareInCents() const {
    return fareInCents_;
}

void Trip::setFareInCents(std::uint32_t fare) {
    fareInCents_ = fare;
}

bool Trip::isSameRoute(const std::string& from, const std::string& to) const {
    return origin_ == from && destination_ == to;
}

std::string Trip::summary() const {
    std::ostringstream output;
    output << "Trip{id='" << id_ << "', busId='" << busId_ << "', route='" << origin_
           << " -> " << destination_ << "', departure='" << departureDateTime_
           << "', fareInCents=" << fareInCents_ << "}";
    return output.str();
}

}  // namespace bus_reservation
