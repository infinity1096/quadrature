#include "Statement.hpp"
#include "CommandInterface.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include "string.h"

#ifdef __cplusplus
}
#endif

bool Statement::recognize(char* buf, int len){
    if (recognizeAscii(buf, len) || recognizeBinary(buf, len)){
        return true;
    }

    return false;
}

bool Statement::parse(char* buf, int len){
    if (recognizeAscii(buf, len)){
        return parseAscii(buf, len);
    }else if (recognizeBinary(buf, len)){
        return parseBinary(buf, len);
    }else{
        return false;
    }
}

// ---------- Arm axis command ----------

void ArmAxisStatement::execute(){
    armAxis();
}

bool ArmAxisStatement::recognizeAscii(char* buf, int len){
    return len == 4;
}

bool ArmAxisStatement::parseAscii(char* buf, int len){
    return len == 4 && strncmp(buf, "arm\n", 4) == 0;
}

// ---------- Disarm axis command ----------

void DisarmAxisStatement::execute(){
    disarmAxis();
}

bool DisarmAxisStatement::recognizeAscii(char* buf, int len){
    return len == 7;
}

bool DisarmAxisStatement::parseAscii(char* buf, int len){
    return len == 7 && strncmp(buf, "disarm\n", 7) == 0;
}

// ---------- Set parameter command ----------

void SetParameterStatement::execute(){
    // TODO:  s
}

bool SetParameterStatement::recognizeAscii(char* buf, int len){
    return len > 4 && strncmp(buf, "set ", 4) == 0;
}

bool SetParameterStatement::parseAscii(char* buf, int len){
    // command syntax is "set <field> <value>", need to find second ' ' 
    //                               ^
    //                         second_space
    
    std::string str = std::string(buf, len);
    auto secondSpace = str.find_first_of(' ', 5);

    if (secondSpace != std::string::npos){
        field = str.substr(4, secondSpace - 4);
        value = str.substr(secondSpace+1, (len - secondSpace - 2));
        return true;
    }

    return false; // FIXME: 
}

// ---------- Flash Save Command ----------
void FlashSaveStatement::execute(){
    // TODO: 
}

bool FlashSaveStatement::recognizeAscii(char* buf, int len){
    return len == 11 && strncmp(buf, "flash save\n", 11) == 0;
}

bool FlashSaveStatement::parseAscii(char* buf, int len){
    return len == 11 && strncmp(buf, "flash save\n", 11) == 0; 
}

// ---------- List Parameter Command ----------
void ListParametersStatement::execute(){
    // TODO: 
}

bool ListParametersStatement::recognizeAscii(char* buf, int len){
    return len == 5 && strncmp(buf, "list\n", 5) == 0;
}

bool ListParametersStatement::parseAscii(char* buf, int len){
    return len == 5 && strncmp(buf, "list\n", 5) == 0;
}

// ---------- Set Current Command ----------
void SetCurrentStatement::execute(){
    // TODO: 
}

bool SetCurrentStatement::recognizeAscii(char* buf, int len){
    return len == 5 && strncmp(buf, "list\n", 5) == 0;
}

bool SetCurrentStatement::parseAscii(char* buf, int len){
    return len == 5 && strncmp(buf, "list\n", 5) == 0;
}

// ---------- Set Torque Command ----------
void SetTorqueStatement::execute(){
    // TODO: 
}

bool SetTorqueStatement::recognizeAscii(char* buf, int len){
    return len == 5 && strncmp(buf, "list\n", 5) == 0;
}

bool SetTorqueStatement::parseAscii(char* buf, int len){
    return len == 5 && strncmp(buf, "list\n", 5) == 0;
}

// ---------- Set Velocity Command ----------
void SetVelocityStatement::execute(){
    // TODO: 
}

bool SetVelocityStatement::recognizeAscii(char* buf, int len){
    return len == 5 && strncmp(buf, "list\n", 5) == 0;
}

bool SetVelocityStatement::parseAscii(char* buf, int len){
    return len == 5 && strncmp(buf, "list\n", 5) == 0;
}

// ---------- Set Position Command ----------
void SetPositionStatement::execute(){
    // TODO: 
}

bool SetPositionStatement::recognizeAscii(char* buf, int len){
    return len == 5 && strncmp(buf, "list\n", 5) == 0;
}

bool SetPositionStatement::parseAscii(char* buf, int len){
    return len == 5 && strncmp(buf, "list\n", 5) == 0;
}

// ---------- Set Telemetry Command ----------
void SetTelemetryStatement::execute(){
    // TODO: 
}

bool SetTelemetryStatement::recognizeAscii(char* buf, int len){
    return len == 5 && strncmp(buf, "list\n", 5) == 0;
}

bool SetTelemetryStatement::parseAscii(char* buf, int len){
    return len == 5 && strncmp(buf, "list\n", 5) == 0;
}


