#pragma once

#include <string>

namespace bus_reservation {

class Entity {
public:
    virtual ~Entity() = default;
    virtual std::string getId() const = 0;
    virtual std::string summary() const = 0;
};

}  // namespace bus_reservation
