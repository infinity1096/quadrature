#include "CommandInterface.hpp"

#include "components.hpp"

bool armAxis(){
    return axis_1->requestArm();
}

bool disarmAxis(){
    axis_1->disarm();
    return true;
}

bool setParameter(std::string name, std::string value){
    return false;
}

bool saveConfig(){
    return false;
}

bool dumpParameters(){
    return false;
}

bool setCurrent(float32_t value){
    return false;
}

bool setTorque(float32_t value){
    return false;
}

bool setVelocity(float32_t value){
    return false;
}

bool setPosition(float32_t value){
    return false;
}

bool setTelemetry(bool enabled){
    return false;
}