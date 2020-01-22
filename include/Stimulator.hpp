#pragma once
#include "FES/Muscle.hpp"

class FES {
public:
    Stimulator(std::vector<muscle> muscles_);
    void set_stim(int pulse_width_);
};
