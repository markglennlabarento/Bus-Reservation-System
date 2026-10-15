#pragma once

#include "Entity.h"
#include "Passenger.h"

#include <cstdint>
#include <string>

namespace bus_reservation {

enum class ReservationStatus {
    Active,
    Cancelled
};

std::string toString(ReservationStatus status);

class Reservation : public Entity {
public:
    Reservation() = default;
    Reservation(const std::string& reservationId,
                const std::string& bookingTripId,
                std::uint32_t seat,
                const Passenger& traveler,
                ReservationStatus reservationStatus = ReservationStatus::Active);

    std::string getId() const override;
    std::string getTripId() const;
    void setTripId(const std::string& bookingTripId);
    std::uint32_t getSeatNumber() const;
    void setSeatNumber(std::uint32_t seat);
    Passenger getPassenger() const;
    void setPassenger(const Passenger& traveler);
    ReservationStatus getStatus() const;
    void setStatus(ReservationStatus reservationStatus);
    bool isActive() const;
    void cancel();
    std::string summary() const override;

private:
    std::string id_;
    std::string tripId_;
    std::uint32_t seatNumber_ = 0;
    Passenger passenger_;
    ReservationStatus status_ = ReservationStatus::Active;
};

}  // namespace bus_reservation
