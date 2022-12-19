
#include "main.h"
#include "cmsis_os.h"
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

extern "C" {

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

}

osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal
};

void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

osThreadId_t telemetryTaskHandle;
const osThreadAttr_t telemetryTask_attributes = {
  .name = "telemetryTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal
};


struct ADCPacket{
  float32_t phaseA;
  float32_t phaseB;
  float32_t phaseC;
  float32_t accumlated_angle;
};

SimulinkReport<ADCPacket,200> reporter;
ADCPacket packet;

void TelemetryTask(void* argument){
  while (true){
    reporter.checkTransmit();
    osDelay(1);
  }
}

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

  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */
  
  // initialize freeRTOS threads
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  telemetryTaskHandle = osThreadNew(TelemetryTask, NULL, &telemetryTask_attributes);

  axis_1_modulator->hardwareEnable();

  osDelay(100);

  axis_1_ch_A->updateVoltage();
  axis_1_ch_B->updateVoltage();
  axis_1_ch_C->updateVoltage();
  
  axis_1_ch_A->config.amplifier_baseline = axis_1_ch_A->sensed_voltage;
  axis_1_ch_B->config.amplifier_baseline = axis_1_ch_B->sensed_voltage;
  axis_1_ch_C->config.amplifier_baseline = axis_1_ch_C->sensed_voltage;

  double th = 0.0;
  double amplitude = 0.5;

  osKernelStart();
  /* USER CODE END 2 */

  /* We should never reach here */
  /* USER CODE BEGIN WHILE */
  while (1) {
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
  
  //reporter.record(packet);
}

extern "C" void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
  HAL_SPI_TxRxCpltCallback(hspi);
}

extern "C" void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi){
  if (hspi == &hspi3){
    // encoder read completed
    axis_1_encoder->encoderReadCompleteCallback();
    packet.accumlated_angle = axis_1_encoder->getAccumulatedPosition();
    reporter.record(packet);
  }
}

extern "C" void EncoderTimer(){
  axis_1_encoder->requestRead();
}