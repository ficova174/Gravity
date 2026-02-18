#pragma once

#include <string>
#include "simulationErrors.h"

class ViewportError : public SimulationError {
public:
    ViewportError(const std::string& descriptor) : SimulationError(descriptor) {}
    ViewportError(const std::string& descriptor, const std::string& message) : SimulationError(descriptor, message) {}
};
