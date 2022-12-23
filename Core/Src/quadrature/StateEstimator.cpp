
#include "StateEstimator.hpp"
#include "FOC_math.hpp"

#define STATE_EST_INT_STEPS 1
#define STATE_EST_DT 1e-4

void StateEstimator::updateSensedAngle(float32_t angle){

    electrical_angle_estimate = axis->mechanicalAngleToElectrical(angle);
    mechanical_angle_estimate = angle;

    // derivative computation
    if (!derivative_initialized){
        derivative_initialized = true;
        derivative_filter_state = angle;
    }
    
    // derivative filtering
    for (int i = 0; i < STATE_EST_INT_STEPS; i++){
        derivative_filter_state += STATE_EST_DT * N * (angle - derivative_filter_state) / STATE_EST_INT_STEPS;
    }
    
    mechanical_velocity_estimate = N * (angle - derivative_filter_state);
}

void StateEstimator::updateSensedCurrent(float32_t sensedCurrent[3]){
    
    forwardClarke(sensedCurrent, Iab0_buffer);
    forwardPark(Iab0_buffer, electrical_angle_estimate, Idq0UnFiltered);

    Idq0Estimate[0] = Id_complementry_gain * Idq0Estimate[0] + (1.0 - Id_complementry_gain) * Idq0UnFiltered[0];
    
    Idq0Estimate[1] = Iq_complementry_gain * Idq0Estimate[1] + (1.0 - Iq_complementry_gain) * Idq0UnFiltered[1];

    Idq0Estimate[2] = 0.0;
}

float32_t StateEstimator::getElectricalAngle(){
    return electrical_angle_estimate;
}

float32_t StateEstimator::getAngle(){
    return mechanical_angle_estimate;
}

float32_t StateEstimator::getVelocity(){
    return mechanical_velocity_estimate;
}

void StateEstimator::getDQCurrent(float32_t* Id,  float32_t* Iq){
    *Id = Idq0Estimate[0];
    *Iq = Idq0Estimate[1];
}

void StateEstimator::getUnFilteredDQCurrent(float32_t* Id,  float32_t* Iq){
    *Id = Idq0UnFiltered[0];
    *Iq = Idq0UnFiltered[1];
}

void StateEstimator::setAxis(Axis* anAxis){
    axis = anAxis;
}