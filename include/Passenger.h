#pragma once

#include "Entity.h"

#include <string>

namespace bus_reservation {

class Passenger : public Entity {
public:
    Passenger() = default;
    Passenger(const std::string& passengerName, const std::string& passengerContact);

    std::string getId() const override;
    std::string getName() const;
    void setName(const std::string& passengerName);
    std::string getContact() const;
    void setContact(const std::string& passengerContact);
    std::string summary() const override;

private:
    std::string name_;
    std::string contact_;
};

}  // namespace bus_reservation
