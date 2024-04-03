#ifndef DATABASE_SERVICE_H
#define DATABASE_SERVICE_H

#include <pqxx/pqxx>

#include <iostream>
#include <optional>

class Database {
public:
    Database(const std::string& connection_string);
    void connect();

    template<typename... Args>
    std::optional<bool> call_procedure(const std::string& proc_name, Args... args) {
        try {
            pqxx::work W(C);
            std::string query = "SELECT " + proc_name + "(" + format_args(args...) + ")";
            pqxx::result R = W.exec(query);
            W.commit();
            return !R.empty() ? std::optional<bool>(R[0][0].as<bool>()) : std::nullopt;
        } catch (const pqxx::sql_error& e) {
            std::cerr << "\033[1;31m" << "Ошибка SQL: " << e.what() << "\033[0m" << std::endl;
            throw;
        } catch (const std::exception& e) {
            std::cerr << "\033[1;31m" << "Произошла ошибка: " << e.what() << "\033[0m" << std::endl;
            throw;
        }
    }

    template<typename T, typename... Args>
    std::string format_args(T first_arg, Args... rest_args) {
        std::ostringstream ss;
        if constexpr (std::is_same_v<T, std::string>) {
            ss << "'" << first_arg << "'";
        } else {
            ss << first_arg;
        }
        std::string str_args = format_args(rest_args...);
        return ss.str() + (str_args.empty() ? "" : ", " + str_args);
    }

    template<typename T>
    std::string format_args(T last_arg) {
        std::ostringstream ss;
        if constexpr (std::is_same_v<T, std::string>) {
            ss << "'" << last_arg << "'";
        } else {
            ss << last_arg;
        }
        return ss.str();
    }

private:
    std::string connection_string_;
    pqxx::connection C;
};

#endif