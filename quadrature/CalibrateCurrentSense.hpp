#ifndef CALIBRATE_CURRENT_SENSE
#define CALIBRATE_CURRENT_SENSE

#include "ADCSense.hpp"
#include "Command.hpp"

class CalibrateCurrentSense : public Command {
    public:
    CalibrateCurrentSense(CurrentSense* currentSense, long num_sample);

    void initialize() override;

    void execute() override;

    bool isFinished() override;

    void end() override;

    private:
    long num_sample = 100;
    double accumlated_sum = 0;
    long accumlated_idx = 0;

    CurrentSense* current_sense;
};

#endif