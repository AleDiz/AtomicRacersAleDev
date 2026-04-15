// chronoAPI.cpp
#include "chronoAPI.hpp"
#include <chrono>
#include <ctime>


std::string obtenerFechaActual() {
    auto ahora = std::chrono::system_clock::now();
    std::time_t tiempo = std::chrono::system_clock::to_time_t(ahora);
    std::tm* fecha = std::localtime(&tiempo);

    std::ostringstream oss;
    oss << std::put_time(fecha, "%Y-%m-%d");  // Formato: 2025-05-02
    return oss.str();
}