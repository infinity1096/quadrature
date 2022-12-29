#ifndef CONTROL_LOGIC_HPP
#define CONTROL_LOGIC_HPP

enum class ControlMode {NONE, CURRENT, VELOCITY, POSITION};

#include "PID.hpp"
#include "Axis.hpp"
#include "Ringbuffer.hpp"

class ControlLogic {
    public:
    ControlLogic() {}

    void setAxis(Axis* aAxis){
        axis = aAxis;
        state_estimator.setAxis(aAxis);
    }

    // function called by Timers/ADC callback periodically
    void sensedCurrentUpdate();
    void sensedEncoderUpdate();

    ControlMode control_mode = ControlMode::NONE;

    PIDController<false> Id_controller;
    PIDController<false> Iq_controller;

    PIDController<true> velocity_controller;
    PIDController<true> position_controller;

    StateEstimator state_estimator;

    float32_t Iq_target = 0.0;
    float32_t velocity_target = 0;
    float32_t position_target = 0;

    float32_t Vd_output = 0;
    float32_t Vq_output = 0;

    Axis* axis;
};


#endif