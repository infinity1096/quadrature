#ifndef STATE_ESTIMATOR_HPP
#define STATE_ESTIMATOR_HPP

#include "utils.hpp"

class StateEstimator{
    public:

    // information update function from sensors
    void updateSensedPosition(float32_t absolutePosition);
    void updateSensedCurrent(float32_t phaseA, float32_t phaseB, float32_t phaseC);

    private:

};

#endif