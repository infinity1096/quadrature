#include "ControlLogic.hpp"

#include "FOC_math.hpp"

void ControlLogic::sensedCurrentUpdate(){
    
    // read sensed current
    float32_t sensed_current[3] = {0.0,0.0,0.0};
    axis->getSensedCurrent(sensed_current);

    // update state estimator
    state_estimator.updateSensedCurrent(sensed_current);

    // fetch reference signal
    float32_t Id_target_local = 0.0;
    float32_t Iq_target_local = 0.0;
    
    switch (control_mode){
        case ControlMode::NONE:
            return;
        case ControlMode::CURRENT:
            // fetch current reference from buffer
            //Id_target_local = 0.0;
            /*
            if (buffer.getOccupied() == 0){
                control_mode = ControlMode::NONE;
                return;
            }

            buffer.read(&Iq_target_local, 1);
            */
            break;
        case ControlMode::VELOCITY:
        case ControlMode::POSITION:
            // use target from ControlLogic
            Id_target_local = 0.0;
            Iq_target_local = Iq_target;
            break;
    }

    Id_target_local = Id_target;
    Iq_target_local = Iq_target;

    // read sensed electrical angle and IDQ from state estimator
    float32_t Id, Iq, electrical_angle;
    state_estimator.getDQCurrent(&Id, &Iq);
    electrical_angle = state_estimator.getElectricalAngle();

    // compute unclipped PID output from current control loop
    float32_t Vdq0[3] = {0.0, 0.0, 0.0};
    float32_t Vd = 0.0, Vq = 0.0;

    Id_error = Id_target_local - Id;
    Iq_error = Iq_target_local - Iq;
    
    Vd = Id_Kp * Id_error + Id_integrator;
    Vq = Iq_Kp * Iq_error + Iq_integrator;
    
    Id_integrator += Id_Ki * Id_error/24000.0;
    Iq_integrator += Iq_Ki * Iq_error/24000.0;

    


    // limit Vdq0 control voltage according to axis config FIXME: 
    // FIXME: system response testing
    Vdq0[0] = clip(Vd, -4, 4);
    Vdq0[1] = clip(Vq, -4, 4);

    //Id_integrator += (Vdq0[0] - Vd) / 24000.0;
    //Iq_integrator += (Vdq0[1] - Vq) / 24000.0;
    
    // record output voltage
    Vd_output = Vdq0[0];
    Vq_output = Vdq0[1];

    float32_t Vab0[3] = {0.0, 0.0, 0.0};

    inversePark(Vdq0, electrical_angle, Vab0);

    axis->applyModulation(Vab0);
}

void ControlLogic::sensedEncoderUpdate(){
    // read sensed position
    float32_t sensed_angle = axis->getSensedAngle();

    // update state estimator
    state_estimator.updateSensedAngle(sensed_angle);

    float32_t position_target_local;
    float32_t velocity_target_local = 0.0;

    switch (control_mode) {
        case ControlMode::NONE: 
        case ControlMode::CURRENT: 
            return;
        case ControlMode::POSITION:
            


            break;

        case ControlMode::VELOCITY:

            break;
    };

    // run position loop
    if (control_mode == ControlMode::POSITION){
        float32_t actual_position_signal = state_estimator.getAngle();
        
        velocity_target_local = PositionController.update(position_target_local, actual_position_signal, 1.0/10000.0);

        velocity_target_local = clip(velocity_target_local, -axis->axis_config.velocity_limit, axis->axis_config.velocity_limit);
    }

    // run velocity loop
    float32_t actual_velocity_signal = state_estimator.getVelocity();

    float32_t Iq_target_local = VelocityController.update(velocity_target_local, actual_velocity_signal, 1.0/10000.0);

    Iq_target_local = clip(Iq_target_local, -axis->axis_config.current_limit, axis->axis_config.current_limit);

    // apply current 
    Iq_target = Iq_target_local;
}