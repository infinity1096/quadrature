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
            Id_target_local = 0.0;
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

    // read sensed electrical angle and IDQ from state estimator
    float32_t Id, Iq, electrical_angle;
    state_estimator.getDQCurrent(&Id, &Iq);
    electrical_angle = state_estimator.getElectricalAngle();

    // compute unclipped PID output from current control loop
    float32_t Vdq0[3] = {0.0, 0.0, 0.0};

    Vdq0[0] = IdController.update(Id_target_local, Id, 1.0/48000.0);
    Vdq0[1] = IqController.update(Iq_target_local, Iq, 1.0/48000.0);
    // Vdq0[2] = 0.0;

    // limit Vdq0 control voltage according to axis config FIXME: 
    Vdq0[0] = 0.0;
    Vdq0[1] = 1.0;

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
            Iq_target = 0.0;
            return;
        case ControlMode::POSITION:
            
            if (buffer.getOccupied() == 0){
                control_mode = ControlMode::NONE;
                return;
            }
            buffer.read(&position_target_local, 1);

            break;

        case ControlMode::VELOCITY:

            if (buffer.getOccupied() == 0){
                control_mode = ControlMode::NONE;
                return;
            }
            buffer.read(&velocity_target_local, 1);
            
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