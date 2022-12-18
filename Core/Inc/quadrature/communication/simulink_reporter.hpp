#ifndef SIMULINK_REPORTER_HPP
#define SIMULINK_REPORTER_HPP

#ifdef __cplusplus
extern "C" {
#endif

#include "arm_math.h"
#include "usbd_cdc_if.h"

#ifdef __cplusplus
}
#endif

#include "libs/Ringbuffer.hpp"

template<typename T, size_t buf_length>
class SimulinkReport {
    public:
    SimulinkReport() : receive_buffer(1.5 * buf_length) {
        memcpy(transmitBuffer, "SSSS", 4);
        memcpy(transmitBuffer +  sizeof(float) + buf_length * sizeof(T), "EEEE", 4);
    }

    void record(const T& data){
        receive_buffer.write(&data, 1);
    }

    void checkTransmit(){
        if (receive_buffer.getOccupied() >= buf_length){
            // copy to transmit buffer and send
            receive_buffer.read((T*)(transmitBuffer + sizeof(float)),buf_length);
            CDC_Transmit_FS(transmitBuffer, 2 * sizeof(float) + buf_length * sizeof(T));
        }
    }

    uint8_t transmitBuffer[2 * sizeof(float) + buf_length * sizeof(T)];
    Ringbuffer<T> receive_buffer;

    private:
    size_t current_write_idx = 0;
};

#endif