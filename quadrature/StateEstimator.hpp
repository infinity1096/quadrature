#ifndef STATE_ESTIMATOR_HPP
#define STATE_ESTIMATOR_HPP

#include "utils.hpp"
#include "Axis.hpp"

class Axis;

class StateEstimator{
    public:
    
    // information update function from sensors
    void updateSensedAngle(float32_t angle);
    void updateSensedCurrent(float32_t sensedCurrent[3]);

    float32_t getElectricalAngle();
    float32_t getAngle();
    float32_t getVelocity();
    void getDQCurrent(float32_t* Id,  float32_t* Iq);
    void getUnFilteredDQCurrent(float32_t* Id,  float32_t* Iq);
    

    void setAxis(Axis* anAxis);
    
    float32_t Id_complementry_gain = 0.0;
    float32_t Iq_complementry_gain = 0.0;

    protected:
    float32_t Idq0Estimate[3] = {0.0, 0.0, 0.0};
    float32_t Idq0UnFiltered[3] = {0.0, 0.0, 0.0};
    float32_t electrical_angle_estimate = 0.0;
    float32_t mechanical_angle_estimate = 0.0;
    float32_t mechanical_velocity_estimate = 0.0;
    
    float32_t derivative_filter_state = 0.0;
    bool derivative_initialized = false;



    private:
    float32_t N = 300.0;
    Axis* axis;
    float32_t Iab0_buffer[3];
};

#endif