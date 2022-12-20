
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
#include "quadrature/CalibrateCurrentSense.hpp"
#include "quadrature/CalibrateAxis.hpp"

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
    osDelay(100);
    
    CalibrateCurrentSense phaseACalibrator(axis_1_ch_A, 100);
    CalibrateCurrentSense phaseBCalibrator(axis_1_ch_B, 100);
    CalibrateCurrentSense phaseCCalibrator(axis_1_ch_C, 100);

    CalibrateAxis axis_calibrator(axis_1);
    
    while (!phaseACalibrator.isProcessEnded()){
      axis_1_ch_A->updateCurrent();
      phaseACalibrator.step();
      osDelay(1);
    }

    while (!phaseBCalibrator.isProcessEnded()){
      axis_1_ch_B->updateCurrent();
      phaseBCalibrator.step();
      osDelay(1);
    }

    while (!phaseCCalibrator.isProcessEnded()){
      axis_1_ch_C->updateCurrent();
      phaseCCalibrator.step();
      osDelay(1);
    }

    axis_1_modulator->hardwareEnable();

    while (!axis_calibrator.isProcessEnded()){
      axis_calibrator.step();
      osDelay(1);
    }

    axis_1->requestArm();
    
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


struct TelemetryPacket{
  float32_t Iq;
  float32_t Id;
  float32_t mech_angle;
  float32_t elec_angle;
  float32_t velocity;
};

SimulinkReport<TelemetryPacket,200> reporter;
TelemetryPacket packet;

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
  
  axis_1_control_logic.sensedCurrentUpdate();
}

extern "C" void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
  HAL_SPI_TxRxCpltCallback(hspi);
}

extern "C" void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi){
  if (hspi == &hspi3){
    // encoder read completed
    axis_1_encoder->encoderReadCompleteCallback();
    //reporter.record(packet);
    axis_1_control_logic.sensedEncoderUpdate();

    axis_1_control_logic.state_estimator.getDQCurrent(&packet.Id, &packet.Iq);

    packet.mech_angle = axis_1_control_logic.state_estimator.getAngle();
    packet.elec_angle = axis_1_control_logic.state_estimator.getElectricalAngle();
    packet.velocity = axis_1_control_logic.state_estimator.getVelocity();
  }
}

extern "C" void EncoderTimer(){
  axis_1_encoder->requestRead();
  reporter.record(packet);
}