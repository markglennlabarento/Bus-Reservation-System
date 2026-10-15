#include "../include/Reservation.h"

#include <sstream>

namespace bus_reservation {

std::string toString(ReservationStatus status) {
    switch (status) {
        case ReservationStatus::Active:
            return "Active";
        case ReservationStatus::Cancelled:
            return "Cancelled";
        default:
            return "Unknown";
    }
}

Reservation::Reservation(const std::string& reservationId,
                         const std::string& bookingTripId,
                         std::uint32_t seat,
                         const Passenger& traveler,
                         ReservationStatus reservationStatus)
    : id_(reservationId), tripId_(bookingTripId), seatNumber_(seat), passenger_(traveler), status_(reservationStatus) {}

std::string Reservation::getId() const {
    return id_;
}

std::string Reservation::getTripId() const {
    return tripId_;
}

void Reservation::setTripId(const std::string& bookingTripId) {
    tripId_ = bookingTripId;
}

std::uint32_t Reservation::getSeatNumber() const {
    return seatNumber_;
}

void Reservation::setSeatNumber(std::uint32_t seat) {
    seatNumber_ = seat;
}

Passenger Reservation::getPassenger() const {
    return passenger_;
}

void Reservation::setPassenger(const Passenger& traveler) {
    passenger_ = traveler;
}

ReservationStatus Reservation::getStatus() const {
    return status_;
}

void Reservation::setStatus(ReservationStatus reservationStatus) {
    status_ = reservationStatus;
}

bool Reservation::isActive() const {
    return status_ == ReservationStatus::Active;
}

void Reservation::cancel() {
    status_ = ReservationStatus::Cancelled;
}

std::string Reservation::summary() const {
    std::ostringstream output;
    output << "Reservation{id='" << id_ << "', tripId='" << tripId_ << "', seatNumber=" << seatNumber_
           << ", passenger=" << passenger_.summary() << ", status=" << toString(status_) << "}";
    return output.str();
}

}  // namespace bus_reservation
