#pragma once

#include "Entity.h"

#include <cstdint>
#include <string>

namespace bus_reservation {

class Trip : public Entity {
public:
    Trip() = default;
    Trip(const std::string& tripId,
         const std::string& vehicleId,
         const std::string& from,
         const std::string& to,
         const std::string& departure,
         std::uint32_t fare);

    std::string getId() const override;
    std::string getBusId() const;
    void setBusId(const std::string& vehicleId);
    std::string getOrigin() const;
    void setOrigin(const std::string& from);
    std::string getDestination() const;
    void setDestination(const std::string& to);
    std::string getDepartureDateTime() const;
    void setDepartureDateTime(const std::string& departure);
    std::uint32_t getFareInCents() const;
    void setFareInCents(std::uint32_t fare);
    bool isSameRoute(const std::string& from, const std::string& to) const;
    std::string summary() const override;

private:
    std::string id_;
    std::string busId_;
    std::string origin_;
    std::string destination_;
    std::string departureDateTime_;
    std::uint32_t fareInCents_ = 0;
};

}  // namespace bus_reservation
