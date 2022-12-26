#ifndef AXIS_HPP
#define AXIS_HPP

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
    float32_t encoder_phase_offset = 0.0;
    bool encoder_calibrated = false;

    // maximum limits
    float32_t voltage_limit = 4;
    float32_t current_limit = 20;
    float32_t velocity_limit = 100;

    bool is_position_limited = false;
    float32_t position_limit_pos = 0;
    float32_t position_limit_neg = 0;

    // motor parameters
    float32_t pole_pairs;
    float32_t phase_resistance;
    float32_t phase_inductance;
    float32_t torque_constant;
    bool motor_calibrated = false;
};

class Axis{
    public:
    Axis(AxisConfig axisConfig);

    // interface to axis from control logic
    bool requestArm();
    void disarm();

    bool isArmed();
    
    bool isCalibrated();

    void applyModulation(float32_t Vabc[3]);
    void limitOutputVoltage(float32_t aVdq0Input[3], float32_t aVdq0Output[3]);

    // link components to axis
    void attachEncoder(Encoder* aEncoder);
    void attachModulator(Modulator* aModulator);
    void attachCurrentSense(long idx, CurrentSense* aSensor);
    void attachVoltageSense(VoltageSense* aSensor);
    
    // transform encoder data into motor direction encoder data
    float32_t getSensedAngle();
    float32_t getSensedElectricalAngle();
    void      getSensedCurrent(float32_t Iabc[3]);

    // used for calibration only
    float32_t getRawEncoderValue();
    void applyCalibrationModulation(float32_t Vabc[3]);

    // internal conversion
    float32_t electricalAngleToMechanical(float32_t electrical);
    float32_t mechanicalAngleToElectrical(float32_t mechanical);

    AxisConfig axis_config;
        
    private:
    bool is_armed = false;

    Encoder* encoder = nullptr;
    Modulator* modulator = nullptr;
    CurrentSense* currentSense[3] = {nullptr, nullptr, nullptr};
    VoltageSense* pvccSense = nullptr;
};

#endif