# Bus Reservation System

A menu-driven bus reservation application written in C++17. It allows a user to manage buses and trips, reserve seats, view reservations, and cancel bookings from a console.

## Features

- View trips and remaining seats in a formatted table.
- Book a seat for a passenger.
- Prevent booking the same seat twice on a trip.
- View reservations in a formatted table.
- Cancel active reservations; cancelled seats become available again.
- Add buses and trips while the program is running.
- Validate numeric menu, fare, capacity, and seat inputs.
- Start with one sample bus (`B-1`) and trip (`T-1`).

## Object-oriented design

- `Entity` is an abstract base class defining a common ID and summary interface.
- `Bus`, `Passenger`, `Trip`, and `Reservation` encapsulate their data and implement the entity interface.
- `Reservation` contains a `Passenger`, demonstrating composition.
- `ReservationStatus` models a reservation's active/cancelled state.
- The `ReservationSystem` in `src/main.cpp` coordinates the application workflow and keeps its collections private.

## Project structure

```text
include/
  Bus.h
  Entity.h
  Passenger.h
  Reservation.h
  Trip.h
src/
  Bus.cpp
  Passenger.cpp
  Reservation.cpp
  Trip.cpp
  main.cpp
.vscode/
  c_cpp_properties.json
  launch.json
  settings.json
  tasks.json
build/
```

## Requirements

- A C++17-compatible compiler. The provided commands and VS Code configuration use `g++`.
- On Windows, install a GCC toolchain such as MinGW-w64 and ensure `g++` is available on `PATH`.

## Build and run from PowerShell

Run these commands from the project directory:

```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic -static-libgcc -static-libstdc++ -Iinclude src\Bus.cpp src\Passenger.cpp src\Trip.cpp src\Reservation.cpp src\main.cpp -o build\bus_reservation_demo.exe
.\build\bus_reservation_demo.exe
```

The VS Code build task is named **Build C++ Project**. You can run the debugger configuration **C++: Run Bus Reservation System** with F5; it builds the program first and launches it in an external console.

## Menu

```text
1. View trips
2. Book a seat
3. View reservations
4. Cancel a reservation
5. Add a bus
6. Add a trip
0. Exit
```

When booking, enter an existing trip ID, a seat number within the bus capacity, and the passenger's name and contact information. When adding a trip, assign it to an existing bus.

## Data storage

Buses, trips, and reservations are saved to `bus_reservation_data.txt` in the program's current working directory. The application loads this file when it starts and saves each successful booking, cancellation, bus addition, and trip addition immediately. It also saves once more when exiting normally.

The first run creates the data file with the sample bus and trip. Keep the data file to preserve your records; deleting it resets the application to the sample data on the next run. The data file uses a quoted, versioned text format and can be moved or backed up while the application is closed.
