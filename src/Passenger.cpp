#include "../include/Passenger.h"

#include <sstream>

namespace bus_reservation {

Passenger::Passenger(const std::string& passengerName, const std::string& passengerContact)
    : name_(passengerName), contact_(passengerContact) {}

std::string Passenger::getId() const {
    return contact_;
}

std::string Passenger::getName() const {
    return name_;
}

void Passenger::setName(const std::string& passengerName) {
    name_ = passengerName;
}

std::string Passenger::getContact() const {
    return contact_;
}

void Passenger::setContact(const std::string& passengerContact) {
    contact_ = passengerContact;
}

std::string Passenger::summary() const {
    std::ostringstream output;
    output << "Passenger{name='" << name_ << "', contact='" << contact_ << "'}";
    return output.str();
}

}  // namespace bus_reservation
