#ifndef CURRENT_SENSE_HPP
#define CURRENT_SENSE_HPP

#include "utils.hpp"

struct CurrentSenseConfig{
    uint16_t amplifier_baseline = 2047;
    float32_t shunt_resistor = 0.001;
    float32_t amplification = 20;
};

struct VoltageSenseConfig{
    float32_t AVCC_voltage = 3.30f;
    float32_t ADC_RANGE = (1 << 12);
    float32_t resistor_bridge_ratio = 1.0; // sensed_voltage / actual voltage
};

class VoltageSense {
    public:
    VoltageSense(uint16_t* adcDataField, VoltageSenseConfig aConfig) : adc_field(adcDataField), config(aConfig) {}

    float32_t convertUnitToVoltage(uint16_t rawUnit){
        return rawUnit / config.ADC_RANGE * config.AVCC_voltage / config.resistor_bridge_ratio;
    }

    float32_t convertVoltageToUnit(float32_t voltage){
        return config.ADC_RANGE * config.resistor_bridge_ratio / config.AVCC_voltage * voltage;
    }

    VoltageSense& updateVoltage(){
        sensed_voltage = convertUnitToVoltage(*adc_field);
        return *this;
    }

    VoltageSenseConfig config;
    float32_t sensed_voltage;
    uint16_t* adc_field; 
};

class CurrentSense : public VoltageSense {
    public:
    CurrentSense(uint16_t* adcDataField, VoltageSenseConfig voltageConfig, CurrentSenseConfig currentConfig) : VoltageSense(adcDataField, voltageConfig), config(currentConfig) {}

    // current * shunt_res * amplification = (ADC_sense - ADC_base) / 4096 * AVCC_voltage
    float32_t convertVoltageToCurrent(float32_t voltage){
        return voltage / config.shunt_resistor / config.amplification;
    }

    float32_t convertCurrentToVoltage(float32_t current){
        return current * config.shunt_resistor * config.amplification;
    }

    void updateCurrent(){
        updateVoltage();
        sensed_current = convertVoltageToCurrent(sensed_voltage);
    }

    CurrentSenseConfig config;
    float32_t sensed_current;
};

#endif