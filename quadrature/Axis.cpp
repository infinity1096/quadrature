#include "Axis.hpp"

#include "FOC_math.hpp"

Axis::Axis(AxisConfig axisConfig) : axis_config(axisConfig) {}

bool Axis::requestArm() {

    // check is components valid and initialized
    if (encoder == nullptr || encoder->getSafetyState() != ComponentState::NORMAL){
        return false;
    }

    if (modulator == nullptr || modulator->getSafetyState() != ComponentState::NORMAL){
        return false;
    }

    for (int i = 0; i < 3; i++){
        if (currentSense[i] == nullptr){
            return false;
        }
    }

    if (pvccSense == nullptr){
        return false;
    }

    // check is axis calibrated
    if (isCalibrated()){
        is_armed = true;
        modulator->hardwareEnable();
    }else{
        is_armed = false;
    }

    return is_armed;
}

void Axis::disarm() {
    is_armed = false;
    modulator->hardwareDisable();
}

bool Axis::isCalibrated() {
    return axis_config.encoder_calibrated && axis_config.motor_calibrated && axis_config.encoder_direction != Direction::UNKNOWN;
}

void Axis::applyModulation(float32_t Vab0[3]){
    if (is_armed){
        modulator->modulate(Vab0, 24); // FIXME: pvccSense not updated
    }
}

void Axis::limitOutputVoltage(float32_t aVdq0Input[3], float32_t aVdq0Output[3]){
    float32_t theMaxModulationVoltage = fmin(pvccSense->sensed_voltage * 2.0f / 3.0f, axis_config.voltage_limit);
    float32_t theRequestedModulationVoltageSquared = aVdq0Input[0] * aVdq0Input[0] + aVdq0Input[1] * aVdq0Input[1];

    if (theMaxModulationVoltage * theMaxModulationVoltage < theRequestedModulationVoltageSquared){
        // output should be limited
        float32_t requestedModulationVoltage = 1.0f;
        arm_sqrt_f32(theRequestedModulationVoltageSquared, &requestedModulationVoltage);

        aVdq0Output[0] = aVdq0Input[0] * theMaxModulationVoltage / requestedModulationVoltage;
        aVdq0Output[1] = aVdq0Input[1] * theMaxModulationVoltage / requestedModulationVoltage;
        aVdq0Output[2] = aVdq0Input[2];
    }else{
        // output should not be limited
        memcpy(aVdq0Output, aVdq0Input, 3 * sizeof(float32_t));
    }
}

void Axis::attachEncoder(Encoder* aEncoder){
    encoder = aEncoder;
}

void Axis::attachModulator(Modulator* aModulator){
    modulator = aModulator;
}

void Axis::attachCurrentSense(long idx, CurrentSense* aSensor) {
    if (0 <= idx && idx <= 2){
        currentSense[idx] = aSensor;
    }
}

void Axis::attachVoltageSense(VoltageSense* aSensor) {
    pvccSense = aSensor;
}

float32_t Axis::getSensedAngle() {
    if (axis_config.encoder_direction == Direction::NORMAL){
        return encoder->getAccumulatedPosition();
    }else{
        return -encoder->getAccumulatedPosition();
    }
}

float32_t Axis::getRawEncoderValue() {
    if (encoder == nullptr){
        return 0.0;
    }else{
        return encoder->getAccumulatedPosition();
    }
}

void Axis::applyCalibrationModulation(float32_t Vabc[3]){
    if (abs(Vabc[0]) > 2.0 || abs(Vabc[1]) > 2.0 || abs(Vabc[2]) > 2.0){
        return;
    }

    modulator->modulate(Vabc, 24); // FIXME: pvccSense not updated
}

float32_t Axis::getSensedElectricalAngle(){
    return mechanicalAngleToElectrical(getSensedAngle());
}

void Axis::getSensedCurrent(float32_t Iabc[3]){
    Iabc[0] = currentSense[0]->sensed_current;
    Iabc[1] = currentSense[1]->sensed_current;
    Iabc[2] = currentSense[2]->sensed_current;
}

float32_t Axis::electricalAngleToMechanical(float32_t electrical){
    return (electrical - axis_config.encoder_phase_offset) / axis_config.pole_pairs;
}

float32_t Axis::mechanicalAngleToElectrical(float32_t mechanical){
    return axis_config.pole_pairs * mechanical + axis_config.encoder_phase_offset;
}