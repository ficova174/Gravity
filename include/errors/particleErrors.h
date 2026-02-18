#pragma once

#include <string>
#include "simulationErrors.h"

class ParticleError : public SimulationError {
public:
    ParticleError(const std::string& descriptor) : SimulationError(descriptor) {}
    ParticleError(const std::string& descriptor, const std::string& message) : SimulationError(descriptor, message) {}
};
