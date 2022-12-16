#ifndef PID_HPP
#define PID_HPP

#include "utils.hpp"

#define SUB_STEPS 10
class PIDController{
    public:
    PIDController(){
        Kp = 0.0f;
        Ki = 0.0f;
        Kd = 0.0f;
        N = 100;
        back_calculation_coeff = 1.0f;
    }

    float update(float ref, float input, float dt){
        float output = 0.0f;
        float error = ref - input;

        // derivative computation
        if (!derivative_initialized){
            derivative_initialized = true;
            derivative_filter_state = error;
        }
        
        // derivative filtering
        for (int i = 0; i < SUB_STEPS; i++){
            derivative_filter_state += dt * N * (error - derivative_filter_state) / SUB_STEPS;
        }

        // PID output computation
        float error_dot = (error - derivative_filter_state) * N;
        output = Kp * error + I_integrator_state + Kd * error_dot;

        // integrator computation
        I_integrator_state += dt * Ki * error;

        // saturation & anti-windup computation
        float output_saturated = clip(output, -saturation, saturation);
        last_unclipped_output = output;
        
        return output_saturated;
    }

    void antiSaturation(float actualOutput, float dt){
        I_integrator_state += (actualOutput - last_unclipped_output) * back_calculation_coeff * dt;
    }

    void reset(){
        I_integrator_state = 0.0f;
        derivative_initialized = false;
    }

    // Basic PID parameters
    float Kp, Ki, Kd;

    // output
    float last_unclipped_output;

    // Basic PID states
    float I_integrator_state;

    // derivative filtering
    float N;
    float derivative_filter_state;
    bool derivative_initialized;

    // output saturation & anti-windup
    float saturation;
    float back_calculation_coeff;
};

#endif