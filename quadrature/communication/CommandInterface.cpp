#include "CommandInterface.hpp"

#include "components.hpp"
#include "usbd_cdc_if.h"
#include <map>

#define OUTPUT_BUFFER_SIZE 400

Ringbuffer<char> buffer(OUTPUT_BUFFER_SIZE);
char output_buffer[OUTPUT_BUFFER_SIZE];

bool armAxis(){
    return axis_1->requestArm();
}

bool disarmAxis(){
    axis_1->disarm();
    return true;
}

float* getParameter(std::string name){
    return field_map[name];
}

bool saveConfig(){
    return false;
}

 void floatToString(float fl , char *str, uint8_t d) {
    int     integer = 0;     //整数部分
    float   decimal  = 0;    
    int     temp;
    uint8_t i, n;
    
    integer = (int)(fl);
    temp = integer;
    n = 0;
    
    do {
        n++;
        temp = (int)(temp / 10);  //计算有几位整数位 n
    }while(temp != 0);
 
    temp = integer;
    
    for(i = 0; i < n ; i++){
        *(str + n - 1 - i) = (uint8_t)(temp % 10) + '0'; //此处跳出
        temp = (int)(temp / 10);
    }
    
    if( d == 0 ) {
        *(str + i) = '\0';
    } else {
        *(str + i) = '.';
    }
    
    decimal = fl - integer;
    for(i = n + 1; i < d + n + 1 ; i++)
    {
        decimal = decimal * 10;
        *(str + i) = (uint8_t)(decimal) + '0';
        decimal = decimal - (uint8_t)(decimal);
    }
    *(str + i) = '\0';
}

bool listParameters(){
    char buf[16];
    buffer.write("----------\n", sizeof("----------\n") - 1);
    for (auto kv : field_map){
        floatToString(*kv.second, buf, 2);
        buffer.write(kv.first.c_str(), kv.first.size());
        buffer.write(": ", 2);
        buffer.write(buf, strlen(buf));
        buffer.write("\n", 1);
    }

    int size = buffer.getOccupied();
    buffer.read(output_buffer, size);
    CDC_Transmit_FS((uint8_t*)output_buffer, size);
    return true;
}

bool setCurrent(float32_t value){
    axis_1_control_logic.control_mode = ControlMode::CURRENT;
    axis_1_control_logic.Iq_target = clip(value, -axis_1->axis_config.current_limit, axis_1->axis_config.current_limit);
    return true;
}

bool setTorque(float32_t value){
    return false;
}

bool setVelocity(float32_t value){
    axis_1_control_logic.control_mode = ControlMode::VELOCITY;
    axis_1_control_logic.velocity_target = clip(value, -axis_1->axis_config.velocity_limit, axis_1->axis_config.velocity_limit);
    return true;
}

bool setPosition(float32_t value){
    return false;
}

bool setTelemetry(bool enabled){
    return false;
}