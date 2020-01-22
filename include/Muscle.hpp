#pragma once
#include "FES/Muscle.hpp"
#include <string>

class Muscle {
public:
    Muscle(std::string& name_);
    void set_stim(int pulse_width_);
};
