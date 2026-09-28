cmake_minimum_required(VERSION 3.31)
project(dtop VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)

# общее между демоном и клиентом, например, структура пакета обмена данными через сокет или tty
# команды, настройки IPC итд
add_subdirectory(dcommon)

# код демона
add_subdirectory(ddaemon)

# код UI-клиента
add_subdirectory(dclient)
