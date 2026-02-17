#pragma once

#include <string>
#include "simulationErrors.h"

class MapError : public SimulationError {
public:
    MapError(const std::string& msg) : SimulationError(msg) {}
};
