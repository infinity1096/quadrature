#ifndef MOTOR_HPP
#define MOTOR_HPP

#include "safety.hpp"
#include "utils.hpp"

struct MotorConfig{
    // physical & electrical parameters
    int16_t pole_pairs;
    float32_t phase_resistance;
    float32_t phase_inductance;
    float32_t torque_constant;
};

#endif