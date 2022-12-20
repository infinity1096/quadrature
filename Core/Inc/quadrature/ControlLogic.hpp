#ifndef CONTROL_LOGIC_HPP
#define CONTROL_LOGIC_HPP

enum class ControlMode {NONE, CURRENT, VELOCITY, POSITION};

#include "PID.hpp"
#include "Axis.hpp"
#include "Ringbuffer.hpp"

class ControlLogic {
    public:
    ControlLogic() : buffer(1000) {}

    void setAxis(Axis* aAxis){
        axis = aAxis;
        state_estimator.setAxis(aAxis);
    }

    void sensedCurrentUpdate();
    void sensedEncoderUpdate();

    ControlMode control_mode = ControlMode::CURRENT;

    PIDController IqController;
    PIDController IdController;
    PIDController VelocityController;
    PIDController PositionController;

    StateEstimator state_estimator;

    float32_t Iq_target = 0.0;

    Axis* axis;
    Ringbuffer<float32_t> buffer;
};


#endif