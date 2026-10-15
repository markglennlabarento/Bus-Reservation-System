#include "../include/Bus.h"
#include "../include/Passenger.h"
#include "../include/Reservation.h"
#include "../include/Trip.h"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace bus_reservation {
namespace {

bool readLine(const std::string& prompt, std::string& value) {
    std::cout << prompt;
    return static_cast<bool>(std::getline(std::cin, value));
}

bool readNumber(const std::string& prompt, std::uint32_t& value) {
    std::string input;
    if (!readLine(prompt, input)) {
        return false;
    }

    std::istringstream parser(input);
    unsigned long long parsed = 0;
    char extra = '\0';
    if (!(parser >> parsed) || (parser >> extra) ||
        parsed > std::numeric_limits<std::uint32_t>::max()) {
        std::cout << "Enter a valid non-negative whole number.\n";
        return false;
    }

    value = static_cast<std::uint32_t>(parsed);
    return true;
}

class ReservationSystem {
public:
    int run() {
        if (!loadData()) {
            return 1;
        }

        std::cout << "=== Bus Reservation System ===\n";
        while (true) {
            printMenu();
            std::uint32_t choice = 0;
            if (!readNumber("Select an option: ", choice)) {
                if (!std::cin) {
                    std::cout << "\nInput ended. Exiting.\n";
                    return saveData() ? 0 : 1;
                }
                continue;
            }

            switch (choice) {
                case 1:
                    listTrips();
                    break;
                case 2:
                    bookSeat();
                    break;
                case 3:
                    listReservations();
                    break;
                case 4:
                    cancelReservation();
                    break;
                case 5:
                    addBus();
                    break;
                case 6:
                    addTrip();
                    break;
                case 0:
                    if (!saveData()) {
                        std::cerr << "Could not save application data. The program will remain open so you can retry.\n";
                        break;
                    }
                    std::cout << "Thank you for using the Bus Reservation System.\n";
                    return 0;
                default:
                    std::cout << "Unknown option. Choose a number from 0 to 6.\n";
            }
        }
    }

private:
    std::vector<Bus> buses_;
    std::vector<Trip> trips_;
    std::vector<Reservation> reservations_;
    std::uint32_t nextBusId_ = 2;
    std::uint32_t nextTripId_ = 2;
    std::uint32_t nextReservationId_ = 1;
    const std::string dataFileName_ = "bus_reservation_data.txt";

    bool loadData() {
        std::ifstream input(dataFileName_);
        if (!input) {
            if (std::filesystem::exists(dataFileName_)) {
                std::cerr << "Could not open data file for reading: " << dataFileName_ << '\n';
                return false;
            }

            buses_.emplace_back("B-1", "Metro Express", 40);
            trips_.emplace_back("T-1", "B-1", "Manila", "Cebu", "2026-10-20 08:30", 1850);
            return saveData();
        }

        std::string fileHeader;
        unsigned int fileVersion = 0;
        if (!(input >> fileHeader >> fileVersion) ||
            fileHeader != "BUS_RESERVATION_DATA" || fileVersion != 1) {
            std::cerr << "The data file has an invalid or unsupported format: " << dataFileName_ << '\n';
            return false;
        }

        std::vector<Bus> loadedBuses;
        std::vector<Trip> loadedTrips;
        std::vector<Reservation> loadedReservations;
        std::string recordType;
        while (input >> recordType) {
            if (recordType == "BUS") {
                std::string id;
                std::string name;
                std::uint32_t capacity = 0;
                if (!(input >> std::quoted(id) >> std::quoted(name) >> capacity) ||
                    id.empty() || name.empty() || capacity == 0) {
                    std::cerr << "Invalid bus record in " << dataFileName_ << '\n';
                    return false;
                }
                loadedBuses.emplace_back(id, name, capacity);
            } else if (recordType == "TRIP") {
                std::string id;
                std::string busId;
                std::string origin;
                std::string destination;
                std::string departure;
                std::uint32_t fare = 0;
                if (!(input >> std::quoted(id) >> std::quoted(busId) >> std::quoted(origin) >>
                      std::quoted(destination) >> std::quoted(departure) >> fare) ||
                    id.empty() || busId.empty() || origin.empty() || destination.empty() || departure.empty()) {
                    std::cerr << "Invalid trip record in " << dataFileName_ << '\n';
                    return false;
                }
                loadedTrips.emplace_back(id, busId, origin, destination, departure, fare);
            } else if (recordType == "RESERVATION") {
                std::string id;
                std::string tripId;
                std::uint32_t seat = 0;
                unsigned int statusValue = 0;
                std::string passengerName;
                std::string passengerContact;
                if (!(input >> std::quoted(id) >> std::quoted(tripId) >> seat >> statusValue >>
                      std::quoted(passengerName) >> std::quoted(passengerContact)) ||
                    id.empty() || tripId.empty() || seat == 0 || passengerName.empty() ||
                    passengerContact.empty() || statusValue > 1) {
                    std::cerr << "Invalid reservation record in " << dataFileName_ << '\n';
                    return false;
                }
                const ReservationStatus status = statusValue == 0
                    ? ReservationStatus::Active
                    : ReservationStatus::Cancelled;
                loadedReservations.emplace_back(
                    id, tripId, seat, Passenger{passengerName, passengerContact}, status);
            } else {
                std::cerr << "Unknown record '" << recordType << "' in " << dataFileName_ << '\n';
                return false;
            }
        }
        if (!input.eof()) {
            std::cerr << "Error while reading data file: " << dataFileName_ << '\n';
            return false;
        }

        for (const Trip& trip : loadedTrips) {
            if (std::none_of(loadedBuses.begin(), loadedBuses.end(), [&trip](const Bus& bus) {
                    return bus.getId() == trip.getBusId();
                })) {
                std::cerr << "Trip '" << trip.getId() << "' references a missing bus in "
                          << dataFileName_ << '\n';
                return false;
            }
        }
        for (const Reservation& reservation : loadedReservations) {
            const auto trip = std::find_if(loadedTrips.begin(), loadedTrips.end(),
                [&reservation](const Trip& candidate) {
                    return candidate.getId() == reservation.getTripId();
                });
            if (trip == loadedTrips.end()) {
                std::cerr << "Reservation '" << reservation.getId() << "' references a missing trip in "
                          << dataFileName_ << '\n';
                return false;
            }
            const auto bus = std::find_if(loadedBuses.begin(), loadedBuses.end(),
                [&trip](const Bus& candidate) {
                    return candidate.getId() == trip->getBusId();
                });
            if (bus == loadedBuses.end() || reservation.getSeatNumber() > bus->getSeatCapacity()) {
                std::cerr << "Reservation '" << reservation.getId() << "' has an invalid seat in "
                          << dataFileName_ << '\n';
                return false;
            }
        }

        buses_ = std::move(loadedBuses);
        trips_ = std::move(loadedTrips);
        reservations_ = std::move(loadedReservations);
        nextBusId_ = static_cast<std::uint32_t>(buses_.size()) + 1;
        nextTripId_ = static_cast<std::uint32_t>(trips_.size()) + 1;
        nextReservationId_ = static_cast<std::uint32_t>(reservations_.size()) + 1;
        return true;
    }

    bool saveData() const {
        std::ofstream output(dataFileName_, std::ios::trunc);
        if (!output) {
            std::cerr << "Could not open data file for writing: " << dataFileName_ << '\n';
            return false;
        }

        output << "BUS_RESERVATION_DATA 1\n";
        for (const Bus& bus : buses_) {
            output << "BUS " << std::quoted(bus.getId()) << ' '
                   << std::quoted(bus.getName()) << ' ' << bus.getSeatCapacity() << '\n';
        }
        for (const Trip& trip : trips_) {
            output << "TRIP " << std::quoted(trip.getId()) << ' '
                   << std::quoted(trip.getBusId()) << ' '
                   << std::quoted(trip.getOrigin()) << ' '
                   << std::quoted(trip.getDestination()) << ' '
                   << std::quoted(trip.getDepartureDateTime()) << ' '
                   << trip.getFareInCents() << '\n';
        }
        for (const Reservation& reservation : reservations_) {
            const Passenger passenger = reservation.getPassenger();
            const unsigned int statusValue =
                reservation.getStatus() == ReservationStatus::Active ? 0 : 1;
            output << "RESERVATION " << std::quoted(reservation.getId()) << ' '
                   << std::quoted(reservation.getTripId()) << ' '
                   << reservation.getSeatNumber() << ' ' << statusValue << ' '
                   << std::quoted(passenger.getName()) << ' '
                   << std::quoted(passenger.getContact()) << '\n';
        }
        output.flush();
        if (!output) {
            std::cerr << "Failed while writing data file: " << dataFileName_ << '\n';
            return false;
        }
        return true;
    }

    bool saveOrRollback(const std::string& failureMessage,
                        const std::function<void()>& rollback) {
        if (saveData()) {
            return true;
        }
        rollback();
        std::cout << failureMessage << " The change was not kept.\n";
        return false;
    }

    static void printMenu() {
        std::cout << "\n1. View trips\n"
                     "2. Book a seat\n"
                     "3. View reservations\n"
                     "4. Cancel a reservation\n"
                     "5. Add a bus\n"
                     "6. Add a trip\n"
                     "0. Exit\n";
    }

    const Bus* findBus(const std::string& id) const {
        for (const Bus& bus : buses_) {
            if (bus.getId() == id) {
                return &bus;
            }
        }
        return nullptr;
    }

    const Trip* findTrip(const std::string& id) const {
        for (const Trip& trip : trips_) {
            if (trip.getId() == id) {
                return &trip;
            }
        }
        return nullptr;
    }

    Reservation* findReservation(const std::string& id) {
        for (Reservation& reservation : reservations_) {
            if (reservation.getId() == id) {
                return &reservation;
            }
        }
        return nullptr;
    }

    std::uint32_t activeReservationCount(const std::string& tripId) const {
        std::uint32_t count = 0;
        for (const Reservation& reservation : reservations_) {
            if (reservation.getTripId() == tripId && reservation.isActive()) {
                ++count;
            }
        }
        return count;
    }

    bool seatIsBooked(const std::string& tripId, std::uint32_t seatNumber) const {
        for (const Reservation& reservation : reservations_) {
            if (reservation.getTripId() == tripId && reservation.isActive() &&
                reservation.getSeatNumber() == seatNumber) {
                return true;
            }
        }
        return false;
    }

    void listTrips() const {
        if (trips_.empty()) {
            std::cout << "No trips are available.\n";
            return;
        }

        std::cout << "\nAvailable trips:\n"
                  << std::left
                  << std::setw(8) << "ID"
                  << std::setw(12) << "Bus ID"
                  << std::setw(20) << "Origin"
                  << std::setw(22) << "Destination"
                  << std::setw(22) << "Departure"
                  << std::setw(14) << "Fare (cents)"
                  << std::setw(18) << "Bus"
                  << std::setw(12) << "Seats left" << '\n'
                  << std::string(128, '-') << '\n';
        for (const Trip& trip : trips_) {
            const Bus* bus = findBus(trip.getBusId());
            const std::uint32_t capacity = bus == nullptr ? 0 : bus->getSeatCapacity();
            const std::uint32_t booked = activeReservationCount(trip.getId());
            std::cout << std::left
                      << std::setw(8) << trip.getId()
                      << std::setw(12) << trip.getBusId()
                      << std::setw(20) << trip.getOrigin()
                      << std::setw(22) << trip.getDestination()
                      << std::setw(22) << trip.getDepartureDateTime()
                      << std::setw(14) << trip.getFareInCents()
                      << std::setw(18) << (bus == nullptr ? "Unknown" : bus->getName())
                      << std::setw(12) << (capacity - booked) << '\n';
        }
    }

    void bookSeat() {
        if (trips_.empty()) {
            std::cout << "No trips are available to book.\n";
            return;
        }
        listTrips();

        std::string tripId;
        if (!readLine("Enter trip ID: ", tripId)) {
            return;
        }
        const Trip* trip = findTrip(tripId);
        if (trip == nullptr) {
            std::cout << "Trip not found.\n";
            return;
        }

        const Bus* bus = findBus(trip->getBusId());
        if (bus == nullptr) {
            std::cout << "The bus assigned to this trip was not found.\n";
            return;
        }
        if (activeReservationCount(tripId) >= bus->getSeatCapacity()) {
            std::cout << "This trip is fully booked.\n";
            return;
        }

        std::uint32_t seatNumber = 0;
        if (!readNumber("Enter seat number (1-" + std::to_string(bus->getSeatCapacity()) + "): ",
                        seatNumber)) {
            return;
        }
        if (seatNumber == 0 || seatNumber > bus->getSeatCapacity()) {
            std::cout << "Seat number is outside this bus's capacity.\n";
            return;
        }
        if (seatIsBooked(tripId, seatNumber)) {
            std::cout << "That seat is already booked for this trip.\n";
            return;
        }

        std::string name;
        std::string contact;
        if (!readLine("Passenger name: ", name) ||
            !readLine("Passenger contact: ", contact)) {
            return;
        }
        if (name.empty() || contact.empty()) {
            std::cout << "Passenger name and contact cannot be empty.\n";
            return;
        }

        const Passenger passenger{name, contact};
        const std::string reservationId = "R-" + std::to_string(nextReservationId_++);
        reservations_.emplace_back(reservationId, tripId, seatNumber, passenger);
        if (!saveOrRollback("Booking could not be saved.",
                            [this]() {
                                reservations_.pop_back();
                                --nextReservationId_;
                            })) {
            return;
        }
        std::cout << "Booking confirmed: " << reservations_.back().summary() << '\n';
    }

    void listReservations() const {
        if (reservations_.empty()) {
            std::cout << "There are no reservations yet.\n";
            return;
        }

        std::cout << "\nReservations:\n"
                  << std::left
                  << std::setw(12) << "ID"
                  << std::setw(10) << "Trip"
                  << std::setw(8) << "Seat"
                  << std::setw(24) << "Passenger"
                  << std::setw(22) << "Contact"
                  << std::setw(12) << "Status" << '\n'
                  << std::string(88, '-') << '\n';
        for (const Reservation& reservation : reservations_) {
            const Passenger passenger = reservation.getPassenger();
            std::cout << std::left
                      << std::setw(12) << reservation.getId()
                      << std::setw(10) << reservation.getTripId()
                      << std::setw(8) << reservation.getSeatNumber()
                      << std::setw(24) << passenger.getName()
                      << std::setw(22) << passenger.getContact()
                      << std::setw(12) << toString(reservation.getStatus()) << '\n';
        }
    }

    void cancelReservation() {
        std::string reservationId;
        if (!readLine("Enter reservation ID to cancel: ", reservationId)) {
            return;
        }
        Reservation* reservation = findReservation(reservationId);
        if (reservation == nullptr) {
            std::cout << "Reservation not found.\n";
            return;
        }
        if (!reservation->isActive()) {
            std::cout << "That reservation is already cancelled.\n";
            return;
        }

        reservation->cancel();
        if (!saveOrRollback("Cancellation could not be saved.",
                            [reservation]() { reservation->setStatus(ReservationStatus::Active); })) {
            return;
        }
        std::cout << "Reservation " << reservationId << " cancelled.\n";
    }

    void addBus() {
        std::string name;
        if (!readLine("Bus name: ", name)) {
            return;
        }
        std::uint32_t capacity = 0;
        if (name.empty() || !readNumber("Seat capacity: ", capacity)) {
            if (name.empty()) {
                std::cout << "Bus name cannot be empty.\n";
            }
            return;
        }
        if (capacity == 0) {
            std::cout << "Seat capacity must be greater than zero.\n";
            return;
        }

        const std::string busId = "B-" + std::to_string(nextBusId_++);
        buses_.emplace_back(busId, name, capacity);
        if (!saveOrRollback("Bus could not be saved.",
                            [this]() {
                                buses_.pop_back();
                                --nextBusId_;
                            })) {
            return;
        }
        std::cout << "Bus added: " << buses_.back().summary() << '\n';
    }

    void addTrip() {
        if (buses_.empty()) {
            std::cout << "Add a bus before creating a trip.\n";
            return;
        }

        std::cout << "\nBuses:\n";
        for (const Bus& bus : buses_) {
            std::cout << bus.summary() << '\n';
        }

        std::string busId;
        std::string origin;
        std::string destination;
        std::string departure;
        if (!readLine("Bus ID: ", busId)) {
            return;
        }
        if (findBus(busId) == nullptr) {
            std::cout << "Bus not found.\n";
            return;
        }
        if (!readLine("Origin: ", origin) ||
            !readLine("Destination: ", destination) ||
            !readLine("Departure date and time: ", departure)) {
            return;
        }
        if (origin.empty() || destination.empty() || departure.empty()) {
            std::cout << "Route and departure details cannot be empty.\n";
            return;
        }

        std::uint32_t fare = 0;
        if (!readNumber("Fare in cents: ", fare)) {
            return;
        }

        const std::string tripId = "T-" + std::to_string(nextTripId_++);
        trips_.emplace_back(tripId, busId, origin, destination, departure, fare);
        if (!saveOrRollback("Trip could not be saved.",
                            [this]() {
                                trips_.pop_back();
                                --nextTripId_;
                            })) {
            return;
        }
        std::cout << "Trip added: " << trips_.back().summary() << '\n';
    }
};

}  // namespace
}  // namespace bus_reservation

int main() {
    bus_reservation::ReservationSystem system;
    return system.run();
}
