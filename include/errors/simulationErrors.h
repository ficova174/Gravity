#pragma once

#include <exception>
#include <string>

class SimulationError : public std::exception {
private:
    std::string m_message;

public:
    SimulationError(const std::string& descriptor) : m_message(descriptor) {}
    SimulationError(const std::string& descriptor, const std::string& message) : m_message(descriptor + message) {}

    const char* what() const noexcept override {
        return m_message.c_str();
    }
};
