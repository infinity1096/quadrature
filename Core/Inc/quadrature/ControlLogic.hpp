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

    void sensedCurrentUpdate();
    void sensedEncoderUpdate();

    ControlMode control_mode = ControlMode::CURRENT;

    PIDController<true> VelocityController;
    PIDController<true> PositionController;

    StateEstimator state_estimator;

    float32_t Id_error = 0; 
    float32_t Iq_error = 0;

    float32_t Id_target = 0.0;    
    float32_t Iq_target = 0.0;

    float32_t Vd_output = 0.0;
    float32_t Vq_output = 0.0;

    float32_t Id_Kp = 0.0;
    float32_t Id_Ki = 0.0;
    float32_t Id_integrator = 0.0;
    
    float32_t Iq_Kp = 0.0;
    float32_t Iq_Ki = 0.0;
    float32_t Iq_integrator = 0.0;
    
    Axis* axis;

};


#endif