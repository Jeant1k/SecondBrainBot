cmake_minimum_required(VERSION 3.10)
project(TelegramBot)

set(CMAKE_CXX_STANDARD 17)

# Укажите путь к заголовочным файлам
include_directories(${CMAKE_SOURCE_DIR}/src)

# Добавьте исходные файлы
file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp" "src/*/*/*.cpp")

add_executable(TelegramBot ${SOURCES})

# Свяжите библиотеки с вашим исполняемым файлом
target_link_libraries(TelegramBot TgBot boost_system ssl crypto pthread pqxx pq)
