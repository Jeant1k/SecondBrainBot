#include "../include/DatabaseService.hpp"

Database::Database(const std::string& connection_string) : connection_string_(connection_string), C(connection_string) {}

void Database::connect() {
    try {
        if (C.is_open()) {
            std::cout << "\033[1;32m" << "База данных открыта успешно: " << C.dbname() << "\033[0m" << std::endl;
        } else {
            std::cout << "\033[1;31m" << "Не могу открыть базу данных" << "\033[0m" << std::endl;
            exit(1);
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
}
