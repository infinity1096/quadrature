#ifndef SIMULINK_REPORTER_HPP
#define SIMULINK_REPORTER_HPP

#include "arm_math.h"
#include "usbd_cdc_if.h"

template<typename T, size_t buf_length>
class SimulinkReport {
    public:
    
    void append(const T& packet){
        if (current_write_idx >= buf_length){
            transmit();
        }

        auto writeAddress = buffer + sizeof(float) + (current_write_idx) * sizeof(T);
        memcpy(writeAddress, &packet, sizeof(T));
        current_write_idx++;
    }

    bool transmit(){
        // prepare package header and end for simulink to recognize
        memcpy(buffer, "SSSS", 4);
        memcpy(buffer + sizeof(float) + (buf_length) * sizeof(T), "EEEE", 4);

        CDC_Transmit_FS(buffer, 2 * sizeof(float) + buf_length * sizeof(T));

        current_write_idx = 0;
    }
    
    bool isFull(){
        return current_write_idx >= buf_length;
    }

    uint8_t buffer[2 * sizeof(float) + buf_length * sizeof(T)];
    
    private:
    size_t current_write_idx = 0;
};

#endif