#ifndef AXIS_HPP
#define AXIS_HPP

#include "Motor.hpp"
#include "Modulator.hpp"
#include "Encoder.hpp"
#include "safety.hpp"
#include "ADCSense.hpp"
#include "StateEstimator.hpp"

enum class Direction {NORMAL, INVERTED, UNKNOWN};

struct AxisConfig{
    // relationship between components
    Direction axis_direction = Direction::NORMAL;
    float32_t mechanical_offset = 0.0;

    Direction encoder_direction = Direction::UNKNOWN; // encoder direction w.r.t. axis
    float32_t encoder_offset;
    
    // maximum limits
    float32_t current_limit;
    float32_t velocity_limit;
    float32_t position_limit_pos;
    float32_t position_limit_neg;
};

class Axis{
    public:
    Axis(Encoder* encoder, Modulator* modulator, MotorConfig motorConfig);

    // interface to axis from control logic
    bool requestArm();
    void disarm();

    bool isCalibrated();

    ModulatorOutput applyModulation(float32_t Vd, float32_t Vq, float32_t Vdc, float32_t electrical_angle);
    void updateEncoder();
    void updateVoltageSense();

    MotorConfig motorConfig;
    AxisConfig axisConfig;
        
    private:
    bool is_armed = false;
    bool is_calibrated = false;

    Encoder* encoder = nullptr;
    Modulator* modulator = nullptr;
    CurrentSense* currentSense[3];
    VoltageSense* pvccSense;
};

#endif