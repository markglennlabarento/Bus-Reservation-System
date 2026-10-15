#pragma once

#include "Entity.h"

#include <cstdint>
#include <string>

namespace bus_reservation {

class Bus : public Entity {
public:
    Bus() = default;
    Bus(const std::string& busId, const std::string& busName, std::uint32_t capacity);

    std::string getId() const override;
    std::string getName() const;
    void setName(const std::string& busName);
    std::uint32_t getSeatCapacity() const;
    void setSeatCapacity(std::uint32_t capacity);
    bool hasCapacity(std::uint32_t requestedSeats = 1) const;
    std::string summary() const override;

private:
    std::string id_;
    std::string name_;
    std::uint32_t seatCapacity_ = 0;
};

}  // namespace bus_reservation
