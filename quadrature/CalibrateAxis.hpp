#ifndef CALIBRATE_AXIS
#define CALIBRATE_AXIS

#include "Axis.hpp"
#include "Command.hpp"
#include "FOC_math.hpp"

class CalibrateAxis : public Command {
    public:
    CalibrateAxis(Axis* anAxis): axis(anAxis) {}

    void initialize() {
        
    }

    void execute() {
        loop_idx++;
        float32_t theta_elec = 0.0;
        if (loop_idx < 500){
            // force pull to align D axis
            initial_encoder_value = axis->getRawEncoderValue();
            theta_elec = 0.0;
        }else if (loop_idx <= 2500){
            theta_elec = 2 * M_PI * (loop_idx - 500) / 1000.0;
        }else if (loop_idx <= 3000){
            theta_elec = 4 * M_PI;
            final_encoder_value = axis->getRawEncoderValue();
        }else if (loop_idx <= 5000){
            theta_elec = 2 * M_PI * (2.0 - (loop_idx - 3000) / 1000.0);
        }

        float32_t Vdq0[3] = {1.0, 0.0, 0.0};
        
        inversePark(Vdq0, theta_elec, Vab0);
        inverseClarke(Vab0, Vabc);

        axis->applyCalibrationModulation(Vabc);
    }

    bool isFinished() {
        return loop_idx >= 5000;
    }

    void end(){
        Vabc[0] = 0;
        Vabc[1] = 0;
        Vabc[2] = 0;

        

        axis->applyCalibrationModulation(Vabc);

        float32_t diff = final_encoder_value - initial_encoder_value;

        if (abs(diff) < 0.05){
            // calibration failed
            return;
        }

        // direction
        axis->axis_config.encoder_direction = diff > 0 ? Direction::NORMAL : Direction::INVERTED;

        // FIXME: pole pair calibration

        // phase
        if (axis->axis_config.encoder_direction == Direction::NORMAL){
            axis->axis_config.encoder_phase_offset = - initial_encoder_value * axis->axis_config.pole_pairs;
        }else{
            axis->axis_config.encoder_phase_offset = initial_encoder_value * axis->axis_config.pole_pairs;
        }

        axis->axis_config.encoder_calibrated = true;
    }

    private:
    Axis* axis = nullptr;
    float32_t initial_encoder_value;
    float32_t final_encoder_value;

    float32_t Vabc[3] = {0.0, 0.0, 0.0};
    float32_t Vab0[3] = {0.0, 0.0, 0.0};

    float32_t accumlated_raw_encoder_value = 0.0;
    long num_raw_encoder_value = 0;

    long loop_idx = 0;
};

#endif