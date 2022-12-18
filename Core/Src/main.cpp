
#include "main.h"
#include "adc.h"
#include "cordic.h"
#include "dma.h"
#include "fdcan.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

#include "arm_math.h"
#include "quadrature/communication/simulink_reporter.hpp"
#include "quadrature/components.hpp"

extern "C" void SystemClock_Config(void);

struct ADCPacket{
  float32_t phaseA;
  float32_t phaseB;
  float32_t phaseC;
};

SimulinkReport<ADCPacket,200> reporter;
ADCPacket packet;

int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_FDCAN1_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_ADC2_Init();
  MX_CORDIC_Init();
  MX_SPI3_Init();
  MX_USB_Device_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  componentInit();

  axis_1_modulator->hardwareEnable();

  HAL_Delay(100);

  axis_1_ch_A->updateVoltage();
  axis_1_ch_B->updateVoltage();
  axis_1_ch_C->updateVoltage();
  
  axis_1_ch_A->config.amplifier_baseline = axis_1_ch_A->sensed_voltage;
  axis_1_ch_B->config.amplifier_baseline = axis_1_ch_B->sensed_voltage;
  axis_1_ch_C->config.amplifier_baseline = axis_1_ch_C->sensed_voltage;

  double th = 0.0;
  double amplitude = 0.5;


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    th += 0.3;
    float32_t Vabc[3];
    Vabc[0] = amplitude * arm_sin_f32(th);
    Vabc[1] = amplitude * arm_sin_f32(th + M_PI * 2 / 3);
    Vabc[2] = amplitude * arm_sin_f32(th + M_PI * 4 / 3);

    //axis_1_modulator->modulate(Vabc[0], Vabc[1], Vabc[2], 24);

    float32_t phase_currents_abc[3];

    phase_currents_abc[0] = axis_1_ch_A->sensed_current;
    phase_currents_abc[1] = axis_1_ch_B->sensed_current;
    phase_currents_abc[2] = axis_1_ch_C->sensed_current;

    reporter.checkTransmit();
    HAL_Delay(1);

    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
uint32_t idx = 0;
// implements callback function after DMA transfer
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc){
  axis_1_ch_A->updateCurrent();
  axis_1_ch_B->updateCurrent();
  axis_1_ch_C->updateCurrent();

  packet.phaseA = axis_1_ch_A->sensed_current;
  packet.phaseB = axis_1_ch_B->sensed_current;
  packet.phaseC = axis_1_ch_C->sensed_current;
  
  reporter.record(packet);
}