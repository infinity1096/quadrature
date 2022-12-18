#ifndef CONTROL_LOGIC_HPP
#define CONTROL_LOGIC_HPP

enum class ControlMode {NONE, OPENLOOP, CURRENT, TORQUE, VELOCITY, POSITION};

#include "PID.hpp"
#include "Axis.hpp"

class ControlLogic {
    public:
    ControlLogic(Axis* axis);

    void sensedCurrentUpdate();
    void sensedEncoderUpdate();

    private:
    PIDController IqController;
    PIDController IdController;
    PIDController VelocityController;
    PIDController PositionController;

    StateEstimator state_estimator;

    Axis* axis;
};


#endif