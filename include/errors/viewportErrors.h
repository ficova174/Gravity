#pragma once

#include <string>
#include "simulationErrors.h"

class ViewportError : public SimulationError {
public:
    ViewportError(const std::string& msg) : SimulationError(msg) {}
};
