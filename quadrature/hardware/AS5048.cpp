#include "AS5048.hpp"

#include "safety.hpp"

#define AS5048A_CLEAR_ERROR_FLAG_ADDR               0x0001
#define AS5048A_PROGRAMMING_CONTROL_ADDR            0x0003
#define AS5048A_OTP_REG_ZERO_POS_HIGH_ADDR          0x0016
#define AS5048A_OTP_REG_ZERO_POS_LOW_ADDR           0x0017
#define AS5048A_DIAGNOSTICS_AUTO_GAIN_CONTROL_ADDR  0x3FFD
#define AS5048A_MAGNITUDE_ADDR                      0x3FFE
#define AS5048A_ANGLE_ADDR                          0x3FFF

#define AS5048A_PARITY                              0x8000

#define AS5048A_READ_FLAG                           0x4000
#define AS5048A_WRITE_FLAG                          0x0000

#define AS5048A_CLEAR_ERROR_FLAG_CMD                0b0100000000000001
#define AS5048A_NOP_CMD                             0x0000

#define AS5048A_DATA_MASK                           0b0011111111111111



bool AS5048::initialize() {
    HAL_GPIO_WritePin(cs_GPIOx, cs_GPIO_Pin, GPIO_PIN_SET); // CS <- HIGH
    
    HAL_Delay(10);

    // clear error flag
    HAL_GPIO_WritePin(cs_GPIOx, cs_GPIO_Pin, GPIO_PIN_RESET); // CS <- LOW
    uint16_t clear_error_cmd = AS5048A_CLEAR_ERROR_FLAG_CMD;
    HAL_SPI_Receive(hspix,(uint8_t*)&clear_error_cmd, 2, 1000);
    HAL_GPIO_WritePin(cs_GPIOx, cs_GPIO_Pin, GPIO_PIN_SET); // CS <- HIGH

    HAL_Delay(10);

    requestRead();

    safety_state = ComponentState::NORMAL;

    return true;
}

bool AS5048::requestRead(){
    // set CS pin to low voltage
    HAL_GPIO_WritePin(cs_GPIOx, cs_GPIO_Pin, GPIO_PIN_RESET); // CS <- LOW

    //prepare receive command
    uint16_t transmit_command = AS5048A_PARITY | AS5048A_READ_FLAG | AS5048A_ANGLE_ADDR;

    HAL_StatusTypeDef status = HAL_ERROR;

    // check if DMA is free
    if (hspix->hdmarx->State == HAL_DMA_STATE_READY){
        status = HAL_SPI_TransmitReceive_DMA(hspix, (uint8_t*)&transmit_command,(uint8_t*)(&spi_buffer), 1); //SPI data unit is 2 bytes, so length is 1 
        if (status == HAL_ERROR){
            // error, reset CS pin
            HAL_GPIO_WritePin(cs_GPIOx, cs_GPIO_Pin, GPIO_PIN_RESET); // CS <- HIGH
        }
    }

    return status == HAL_OK;
}

void AS5048::encoderReadCompleteCallback() {
        
    HAL_GPIO_WritePin(cs_GPIOx, cs_GPIO_Pin, GPIO_PIN_SET); // CS <- HIGH

    raw_receive = spi_buffer;

    uint16_t parity = raw_receive;
    
    parity ^= parity >> 8;
    parity ^= parity >> 4;
    parity ^= parity >> 2;
    parity ^= parity >> 1;

    if (!(parity & 0x01) == 0){
        is_data_valid = false;
        return;
    }

    int16_t absolute_angle = (int16_t)(raw_receive & AS5048A_DATA_MASK) - 8192;
    absolute_position = absolute_angle;

    if (last_information_initialized_cnt < 5){
        last_information_initialized_cnt++;
    }else{
        if (absolute_angle - last_absolute_angle > (ENCODER_PERIOD >> 1)){
            num_rotation--;
        }else if(absolute_angle - last_absolute_angle < -(ENCODER_PERIOD >> 1)){
            num_rotation++;
        }
    } 

    last_absolute_angle = absolute_angle;
    is_data_valid = true;
}