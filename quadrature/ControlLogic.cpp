#include "ControlLogic.hpp"

#include "FOC_math.hpp"

#define CURRENT_LOOP_DT 1.0f/24000.0f
#define POSITION_LOOP_DT 1.0f/10000.0f

void ControlLogic::sensedCurrentUpdate(){
    
    // read sensed current
    float32_t sensed_current[3] = {0.0,0.0,0.0};
    axis->getSensedCurrent(sensed_current);

    // update state estimator
    state_estimator.updateSensedCurrent(sensed_current);

    // check is axis armed
    if (!axis->isArmed()){
        return;
    }

    // fetch reference signal
    float32_t Id_target_local = 0.0;
    float32_t Iq_target_local = 0.0;
    
    // prepare modulation signal output
    float32_t Vab0[3] = {0.0, 0.0, 0.0};

    if (control_mode == ControlMode::NONE){
        // modulate 0 voltage output
        axis->applyModulation(Vab0);
        return;
    }

    // in all other cases, set
    //      - D axis current = 0
    //      - Q axis current set by command or velocity loop
    Id_target_local = 0.0;
    Iq_target_local = Iq_target;
    
    // read sensed electrical angle and Id, Iq from state estimator
    float32_t Id, Iq, electrical_angle;
    state_estimator.getDQCurrent(&Id, &Iq);
    electrical_angle = state_estimator.getElectricalAngle();

    // compute unclipped PID output from current control loop
    float32_t Vdq0[3] = {0.0, 0.0, 0.0};
    float32_t Vdq0_output[3] = {0.0, 0.0, 0.0};
    
    Vdq0[0] = Id_controller.update(Id_target_local, Id, CURRENT_LOOP_DT);  
    Vdq0[1] = Iq_controller.update(Iq_target_local, Iq, CURRENT_LOOP_DT);
    
    // limit modulation voltage with axis's limitation
    axis->limitOutputVoltage(Vdq0, Vdq0_output); 

    // feedback actual voltage output to PID for anti-saturation
    // cautious: even if Ki = 0, if Kp \ne 0, the integrator will still increase if
    // this control loop is called but no voltage can be modulated.     
    Id_controller.antiSaturation(Vdq0_output[0], CURRENT_LOOP_DT);
    Iq_controller.antiSaturation(Vdq0_output[1], CURRENT_LOOP_DT);

    // record output voltage
    Vd_output = Vdq0_output[0];
    Vq_output = Vdq0_output[1];

    // apply modulation
    inversePark(Vdq0_output, electrical_angle, Vab0);
    
    // record output ab0 voltage
    Valpha_output = Vab0[0];
    Vbeta_output = Vab0[1];

    axis->applyModulation(Vab0);
}

void ControlLogic::sensedEncoderUpdate(){
    // read sensed position
    float32_t sensed_angle = axis->getSensedAngle();

    // update state estimator
    state_estimator.updateSensedAngle(sensed_angle);

    if (control_mode == ControlMode::NONE || control_mode == ControlMode::CURRENT){
        // do nothing to current target. Current loop will handle setting target / output.
        return;
    }

    // run position loop
    float32_t position_target_local = position_target;
    if (control_mode == ControlMode::POSITION){
        // read mechanical angle
        float32_t actual_position_signal = state_estimator.getAngle();
        
        // compute PID output of position loop
        float32_t unclipped_velocity_target = position_controller.update(position_target_local, actual_position_signal, POSITION_LOOP_DT);

        // apply saturation from max_velocity limit
        velocity_target = clip(unclipped_velocity_target, -axis->axis_config.velocity_limit, axis->axis_config.velocity_limit);

        // feedback actual output back to PID for anti-saturation
        position_controller.antiSaturation(velocity_target, POSITION_LOOP_DT);
    }
    
    // run velocity loop
    float32_t velocityTargetLocal = velocity_target;
    
    // read mechanical velocity
    float32_t actualVelocity = state_estimator.getVelocity();

    // compute PID output of velocity loop
    float32_t unClippedIqTarget = velocity_controller.update(velocityTargetLocal, actualVelocity, POSITION_LOOP_DT);

    // apply saturation from max current limit
    Iq_target = clip(unClippedIqTarget, -axis->axis_config.current_limit, axis->axis_config.current_limit);

    // feedback actual output back to PID for anti-saturation
    velocity_controller.antiSaturation(unClippedIqTarget, POSITION_LOOP_DT);
}