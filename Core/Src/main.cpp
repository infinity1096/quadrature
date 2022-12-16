
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

typedef struct {
    uint16_t channelA;
    uint16_t channelB;
    uint16_t channelC;
} ADCSampling;

SimulinkReport<ADCSampling, 200> reporter;
ADCSampling packet;

extern "C" void SystemClock_Config(void);

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

  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
  
  htim1.Instance->CCR1 = 874;
  htim1.Instance->CCR2 = 874;
  htim1.Instance->CCR3 = 874;

  double th = 0.0;
  double amplitude = 50;

  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)(&packet), 3);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    th += 0.03;

    htim1.Instance->CCR1 = 874 + amplitude * arm_sin_f32(th);
    htim1.Instance->CCR2 = 874 + amplitude * arm_sin_f32(th + M_PI * 2 / 3);
    htim1.Instance->CCR3 = 874 + amplitude * arm_sin_f32(th + M_PI * 4 / 3);

    HAL_Delay(1);    

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
uint32_t idx = 0;
// implements callback function after DMA transfer
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc){
  if (idx % 1 == 0){
    reporter.append(packet);
  }
  idx++;
}