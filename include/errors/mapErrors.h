#pragma once

#include <string>
#include "simulationErrors.h"

class MapError : public SimulationError {
public:
    MapError(const std::string& descriptor) : SimulationError(descriptor) {}
    MapError(const std::string& descriptor, const std::string& message) : SimulationError(descriptor, message) {}
};
