#include "encoder.hpp"

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

bool AS5048::initialize(){
    CS_pin.write(true);

    HAL_Delay(10);

    // clear error flag
    CS_pin.write(false);
    uint16_t clear_error_cmd = AS5048A_CLEAR_ERROR_FLAG_CMD;
    HAL_SPI_Receive(spix,(uint8_t*)&clear_error_cmd, 2, 1000);
    CS_pin.write(true);

    HAL_Delay(10);

    // test communication
    CS_pin.write(false);
    uint16_t nop_cmd = AS5048A_NOP_CMD;
    auto error_code = HAL_SPI_Receive(spix,(uint8_t*)&nop_cmd, 2, 1000);
    CS_pin.write(true);

    if(error_code != HAL_OK || nop_cmd != 0x00){
        return false;
    }

    return true;
}

bool AS5048::initiateEncoderRead(){
    
    // set CS pin to low voltage
    CS_pin.write(false);

    //prepare receive command
    uint16_t transmit_command = AS5048A_PARITY | AS5048A_READ_FLAG | AS5048A_ANGLE_ADDR;

    
    HAL_StatusTypeDef status = HAL_ERROR;

    // check if DMA is free
    if (spix->hdmarx->State == HAL_DMA_STATE_READY){
        status = HAL_SPI_TransmitReceive_DMA(spix, (uint8_t*)&transmit_command,(uint8_t*)(&raw_buffer), 1); //SPI data unit is 2 bytes, so length is 1 
        if (status == HAL_ERROR){
            // error, reset CS pin
            CS_pin.write(true);
        }
    }

    return status == HAL_OK;
}

int32_t AS5048::getRawReceive(){
        return raw_receive & AS5048A_DATA_MASK;
}


bool AS5048::encoderReadCompleteCallback(){
    // parity check
    // parity computed according to https://www.geeksforgeeks.org/finding-the-parity-of-a-number-efficiently/

    CS_pin.write(true);

    raw_receive = raw_buffer;

    uint16_t parity = raw_receive;
    
    parity ^= parity >> 8;
    parity ^= parity >> 4;
    parity ^= parity >> 2;
    parity ^= parity >> 1;
    
    if ((parity & 0x01) == 0){
        //correct parity, data valid.
        int16_t raw_angle = raw_receive & AS5048A_DATA_MASK;
        absolute_angle = (raw_angle - 8192) * PI / 8192.0f;

        if(last_absolute_angle == ABSOLUTE_ANGLE_NOT_INITIALIZED){
            accumulated_angle = accumulated_angle_turns * PI + absolute_angle;
        }else{
            if (absolute_angle - last_absolute_angle > PI){
                accumulated_angle_turns--;
            }else if (absolute_angle - last_absolute_angle < -PI){
                accumulated_angle_turns++;
            }
            accumulated_angle = accumulated_angle_turns * 2.0 * PI + absolute_angle;
        }

        is_valid = true;
        last_absolute_angle = absolute_angle;

        return true;
    }else{
        //incorrect parity, data invalid
        return false;
    }
}