#include "../include/Bus.h"

#include <sstream>

namespace bus_reservation {

Bus::Bus(const std::string& busId, const std::string& busName, std::uint32_t capacity)
    : id_(busId), name_(busName), seatCapacity_(capacity) {}

std::string Bus::getId() const {
    return id_;
}

std::string Bus::getName() const {
    return name_;
}

void Bus::setName(const std::string& busName) {
    name_ = busName;
}

std::uint32_t Bus::getSeatCapacity() const {
    return seatCapacity_;
}

void Bus::setSeatCapacity(std::uint32_t capacity) {
    seatCapacity_ = capacity;
}

bool Bus::hasCapacity(std::uint32_t requestedSeats) const {
    return requestedSeats <= seatCapacity_;
}

std::string Bus::summary() const {
    std::ostringstream output;
    output << "Bus{id='" << id_ << "', name='" << name_ << "', seatCapacity=" << seatCapacity_ << "}";
    return output.str();
}

}  // namespace bus_reservation
