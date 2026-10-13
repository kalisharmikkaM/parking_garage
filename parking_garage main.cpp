#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include <algorithm>

// Structure representing a single Car
struct Car {
    std::string licensePlate;
    std::string carModel;
    std::string color;

    Car(const std::string& plate, const std::string& model, const std::string& col)
        : licensePlate(plate), carModel(model), color(col) {}
};

// Stack-based Narrow Parking Garage (LIFO: Last-In, First-Out)
class ParkingGarage {
private:
    std::vector<Car> garage; // Stack container (back of vector is top of stack)
    size_t maxCapacity;

    // Helper for case-insensitive license plate comparison
    static bool equalsIgnoreCase(const std::string& a, const std::string& b) {
        if (a.length() != b.length()) return false;
        return std::equal(a.begin(), a.end(), b.begin(), [](char ch1, char ch2) {
            return std::tolower(static_cast<unsigned char>(ch1)) == 
                   std::tolower(static_cast<unsigned char>(ch2));
        });
    }

public:
    explicit ParkingGarage(size_t capacity = 5) : maxCapacity(capacity) {}

    bool isFull() const {
        return garage.size() >= maxCapacity;
    }

    bool isEmpty() const {
        return garage.empty();
    }

    size_t getCarCount() const {
        return garage.size();
    }

    size_t getCapacity() const {
        return maxCapacity;
    }

    // 1. PUSH: A car enters the garage
    bool parkCar(const std::string& plate, const std::string& model, const std::string& color) {
        if (isFull()) {
            std::cout << "\n[Garage Full] Cannot park \"" << plate 
                      << "\". Maximum capacity of " << maxCapacity << " cars reached!\n";
            return false;
        }

        // Check for duplicate license plate
        for (const auto& car : garage) {
            if (equalsIgnoreCase(car.licensePlate, plate)) {
                std::cout << "\n[Error] Car with license plate \"" << plate 
                          << "\" is already inside the garage!\n";
                return false;
            }
        }

        garage.emplace_back(plate, model, color);
        std::cout << "\n[Parked] Car \"" << plate << "\" (" << color << " " << model 
                  << ") entered and parked successfully!\n";
        std::cout << "Available parking spots remaining: " << (maxCapacity - garage.size()) << "\n";
        return true;
    }

    // 2. POP: The most recently entered car leaves (Top of the Stack)
    bool departNearestCar() {
        if (isEmpty()) {
            std::cout << "\n[Garage Empty] No cars in the garage to depart.\n";
            return false;
        }

        Car departingCar = garage.back();
        garage.pop_back();

        std::cout << "\n[Departed] Car \"" << departingCar.licensePlate << "\" (" 
                  << departingCar.color << " " << departingCar.carModel 
                  << ") has exited the garage.\n";
        std::cout << "Available parking spots remaining: " << (maxCapacity - garage.size()) << "\n";
        return true;
    }

    // 3. PEEK: Look at the car closest to the exit (Top of the Stack)
    void peekNearestCar() const {
        if (isEmpty()) {
            std::cout << "\n[Garage Empty] No cars are currently parked.\n";
            return;
        }

        const Car& topCar = garage.back();
        std::cout << "\n--- Next Car Ready to Exit (Top of Stack) ---\n";
        std::cout << "License Plate : " << topCar.licensePlate << "\n";
        std::cout << "Model         : " << topCar.carModel << "\n";
        std::cout << "Color         : " << topCar.color << "\n";
        std::cout << "Position      : Nearest to the exit gate\n";
    }

    // 4. Depart a specific car from the middle using an auxiliary street stack
    bool departSpecificCar(const std::string& plate) {
        if (isEmpty()) {
            std::cout << "\n[Garage Empty] No cars inside the garage.\n";
            return false;
        }

        // Check if car exists
        int targetIndex = -1;
        for (int i = static_cast<int>(garage.size()) - 1; i >= 0; --i) {
            if (equalsIgnoreCase(garage[i].licensePlate, plate)) {
                targetIndex = i;
                break;
            }
        }

        if (targetIndex == -1) {
            std::cout << "\n[Not Found] Car with license plate \"" << plate 
                      << "\" is not in the garage.\n";
            return false;
        }

        // If it's already at the exit (top of stack)
        if (targetIndex == static_cast<int>(garage.size()) - 1) {
            return departNearestCar();
        }

        // Auxiliary stack simulating temporary street/holding area
        std::vector<Car> temporaryStreet;
        int carsMoved = 0;

        std::cout << "\n[Operation] Car \"" << plate << "\" is blocked by other cars.\n";
        std::cout << "Moving blocking cars out to the street temporarily...\n";

        // Pop blocking cars to the temporary street
        while (!garage.empty() && !equalsIgnoreCase(garage.back().licensePlate, plate)) {
            std::cout << "  -> Moving out car \"" << garage.back().licensePlate 
                      << "\" to the temporary street.\n";
            temporaryStreet.push_back(garage.back());
            garage.pop_back();
            carsMoved++;
        }

        // Target car can now exit
        Car targetCar = garage.back();
        garage.pop_back();
        std::cout << "\n[Success] Target Car \"" << targetCar.licensePlate 
                  << "\" has successfully departed!\n";

        // Push blocking cars back into the garage in reverse order (restoring original order)
        std::cout << "\nDriving temporarily moved cars back into the garage...\n";
        while (!temporaryStreet.empty()) {
            std::cout << "  <- Returning car \"" << temporaryStreet.back().licensePlate 
                      << "\" back into the garage.\n";
            garage.push_back(temporaryStreet.back());
            temporaryStreet.pop_back();
        }

        std::cout << "[Complete] Total blocking cars temporarily moved: " << carsMoved << "\n";
        return true;
    }

    // 5. Search for a car and determine how many cars block its exit
    void searchCar(const std::string& plate) const {
        if (isEmpty()) {
            std::cout << "\n[Garage Empty] Garage is empty.\n";
            return;
        }

        for (size_t i = 0; i < garage.size(); ++i) {
            if (equalsIgnoreCase(garage[i].licensePlate, plate)) {
                size_t blockingCars = (garage.size() - 1) - i;
                std::cout << "\n[Found] Car \"" << garage[i].licensePlate 
                          << "\" (" << garage[i].color << " " << garage[i].carModel << ")\n";
                std::cout << "Depth in garage : Slot #" << (i + 1) << " of " << garage.size() << "\n";
                if (blockingCars == 0) {
                    std::cout << "Status          : At the exit gate! Can leave immediately.\n";
                } else {
                    std::cout << "Status          : " << blockingCars 
                              << " car(s) must back out first before this car can exit.\n";
                }
                return;
            }
        }

        std::cout << "\n[Not Found] Car \"" << plate << "\" is not parked in this garage.\n";
    }

    // 6. Display visual garage diagram and status
    void displayGarage() const {
        std::cout << "\n=======================================================\n";
        std::cout << "          NARROW PARKING GARAGE (LIFO STACK)           \n";
        std::cout << "=======================================================\n";

        if (isEmpty()) {
            std::cout << "               [ GARAGE IS EMPTY ]\n";
            std::cout << "        +-------------------------------+\n";
            std::cout << " EXIT   |           (EMPTY)             |\n";
            std::cout << " GATE   |                               |\n";
            std::cout << "        +-------------------------------+\n";
            std::cout << "        |=========== BACK WALL =========|\n";
            std::cout << "=======================================================\n";
            std::cout << "Capacity: 0 / " << maxCapacity << " spots occupied.\n";
            return;
        }

        std::cout << "        === [ EXIT / ENTRANCE GATE ] ===\n";
        std::cout << "                       |\n";
        std::cout << "                       v\n";

        // Display stack from top (nearest exit) to bottom (deepest spot)
        for (int i = static_cast<int>(garage.size()) - 1; i >= 0; --i) {
            const auto& car = garage[i];
            std::string tag = (i == static_cast<int>(garage.size()) - 1) ? " [TOP / EXIT READY]" : "";
            std::string bottomTag = (i == 0) ? " [DEEPEST SPOT]" : "";

            std::cout << "  Slot #" << (i + 1) << " : [ " 
                      << std::left << std::setw(12) << car.licensePlate
                      << " | " << std::setw(16) << (car.color + " " + car.carModel)
                      << " ]" << tag << bottomTag << "\n";
        }

        std::cout << "        +-----------------------------------------------+\n";
        std::cout << "        |================== BACK WALL ==================|\n";
        std::cout << "=======================================================\n";
        std::cout << "Occupancy: " << garage.size() << " / " << maxCapacity 
                  << " spots occupied (" << (maxCapacity - garage.size()) << " available)\n";
        std::cout << "Rule     : Last car in must be the first car out (LIFO).\n";
        std::cout << "=======================================================\n";
    }
};

// Input validation helpers
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int getValidInt(const std::string& prompt, int minVal = 1) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= minVal) {
            clearInputBuffer();
            return value;
        }
        std::cout << "[Invalid Input] Please enter a valid number (>= " << minVal << ").\n";
        clearInputBuffer();
    }
}

std::string getValidString(const std::string& prompt) {
    std::string text;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, text);

        size_t first = text.find_first_not_of(" \t");
        if (first != std::string::npos) {
            size_t last = text.find_last_not_of(" \t");
            return text.substr(first, (last - first + 1));
        }
        std::cout << "[Invalid Input] Entry cannot be blank. Please try again.\n";
    }
}

void printMenu() {
    std::cout << "\n=========================================\n";
    std::cout << "     NARROW PARKING GARAGE CONTROLLER    \n";
    std::cout << "=========================================\n";
    std::cout << " 1. Display Garage Status (Stack View)\n";
    std::cout << " 2. Park a Car (Push to Garage)\n";
    std::cout << " 3. Depart Nearest Car (Pop from Garage)\n";
    std::cout << " 4. View Next Car to Exit (Peek Top)\n";
    std::cout << " 5. Depart Specific Car (Handles Blocking Cars)\n";
    std::cout << " 6. Search for a Car & Check Blocking Cars\n";
    std::cout << " 7. Exit\n";
    std::cout << "=========================================\n";
}

int main() {
    // Garage with maximum capacity of 5 cars
    ParkingGarage garage(5);

    // Initial cars entering in chronological order:
    // Car 1 enters first (deepest spot at the back wall)
    garage.parkCar("DL-01-AB-1234", "Honda City", "White");
    garage.parkCar("MH-02-CD-5678", "Hyundai Creta", "Red");
    garage.parkCar("KA-03-EF-9012", "Toyota Fortuner", "Black");
    garage.parkCar("HR-26-GH-3456", "Tesla Model 3", "Blue");

    int choice = 0;
    while (choice != 7) {
        printMenu();
        choice = getValidInt("Enter your choice (1-7): ", 1);

        switch (choice) {
            case 1: {
                garage.displayGarage();
                break;
            }
            case 2: {
                if (garage.isFull()) {
                    std::cout << "\n[Garage Full] Cannot park any more cars.\n";
                    break;
                }
                std::cout << "\n--- Park New Car ---\n";
                std::string plate = getValidString("Enter License Plate: ");
                std::string model = getValidString("Enter Car Model (e.g., Civic, Mustang): ");
                std::string color = getValidString("Enter Color: ");
                garage.parkCar(plate, model, color);
                break;
            }
            case 3: {
                garage.departNearestCar();
                break;
            }
            case 4: {
                garage.peekNearestCar();
                break;
            }
            case 5: {
                if (garage.isEmpty()) {
                    std::cout << "\n[Garage Empty] No cars in garage.\n";
                    break;
                }
                std::cout << "\n--- Depart Specific Car ---\n";
                std::string plate = getValidString("Enter License Plate of the car: ");
                garage.departSpecificCar(plate);
                break;
            }
            case 6: {
                if (garage.isEmpty()) {
                    std::cout << "\n[Garage Empty] No cars in garage.\n";
                    break;
                }
                std::cout << "\n--- Search Car Status ---\n";
                std::string plate = getValidString("Enter License Plate to check: ");
                garage.searchCar(plate);
                break;
            }
            case 7: {
                std::cout << "\nExiting Garage Simulation. Have a great day!\n";
                break;
            }
            default: {
                std::cout << "\n[Error] Invalid choice! Please select an option between 1 and 7.\n";
                break;
            }
        }
    }

    return 0;
}
