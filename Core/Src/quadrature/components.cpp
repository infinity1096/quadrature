#include "quadrature/components.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include "adc.h"

#ifdef __cplusplus
}
#endif

uint16_t adc_fields[3];

CurrentSense* axis_1_ch_A = nullptr;
CurrentSense* axis_1_ch_B = nullptr;
CurrentSense* axis_1_ch_C = nullptr;
VoltageSense* pvcc_sense = nullptr;

Encoder* axis_1_encoder = nullptr;

Modulator* axis_1_modulator = nullptr;

void componentInit(){
    
    VoltageSenseConfig unitVoltageSense;
    CurrentSenseConfig shuntCurrentSense;

    VoltageSenseConfig pvccSenseConfig;
    pvccSenseConfig.resistor_bridge_ratio = 1.0/11.0;


    axis_1_ch_A = new CurrentSense(adc_fields + 0, unitVoltageSense, shuntCurrentSense);
    axis_1_ch_B = new CurrentSense(adc_fields + 1, unitVoltageSense, shuntCurrentSense);
    axis_1_ch_C = new CurrentSense(adc_fields + 2, unitVoltageSense, shuntCurrentSense);
    
    axis_1_modulator = new TimerModulator(&htim1);

    axis_1_modulator->initialize();

    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)(adc_fields), 3);
  

}
