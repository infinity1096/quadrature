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
    if (floatField != nullptr){
        *floatField = floatVal;
    }
}

bool SetParameterStatement::recognizeAscii(char* buf, int len){
    return len > 4 && strncmp(buf, "set ", 4) == 0;
}

// from https://stackoverflow.com/questions/3825392/string-to-float-conversion
// atof is too big to fit the memory
float small_atof(const char *s)
{
    int f, m, sign, d=1;
    f = m = 0;

    sign = (s[0] == '-') ? -1 : 1;
    if (s[0] == '-' || s[0] == '+') s++;

    for (; *s != '.' && *s; s++) {
            f = (*s-'0') + f*10;
    }
    if (*s == '.')
            for (++s; *s; s++) {
                    m = (*s-'0') + m*10;
                    d *= 10;
            }
    return sign*(f + (float)m/d);
}

bool is_number(std::string value){
    int num_dot = 0;
    for(int i = 0; i < value.size(); i++){
        char ch = value[i];
        if (i == 0 && !(('0' <= ch && ch <= '9') || ch == '.' || ch == '-')){
            return false;
        }

        if (i != 0 && !(('0' <= ch && ch <= '9') || ch == '.')){
            return false;
        }

        if (ch == '.'){
            num_dot++;
        }

        if (num_dot >= 2){
            return false;
        }
    }

    return true;
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

        if (!is_number(value)){
            return false;
        }

        floatVal = small_atof(value.c_str());
    }

    floatField = getParameter(field);
    if (floatField != nullptr){
        return true;
    }

    return false;
}



// ---------- Flash Save Command ----------
void FlashSaveStatement::execute(){
    listParameters();
}

bool FlashSaveStatement::recognizeAscii(char* buf, int len){
    return len == 11 && strncmp(buf, "flash save\n", 11) == 0;
}

bool FlashSaveStatement::parseAscii(char* buf, int len){
    return len == 11 && strncmp(buf, "flash save\n", 11) == 0; 
}

// ---------- List Parameter Command ----------
void ListParametersStatement::execute(){
    listParameters();
}

bool ListParametersStatement::recognizeAscii(char* buf, int len){
    return len == 5 && strncmp(buf, "list\n", 5) == 0;
}

bool ListParametersStatement::parseAscii(char* buf, int len){
    return recognizeAscii(buf, len);
}

// ---------- Set Current Command ----------
void SetCurrentStatement::execute(){
    setCurrent(currentGoal);
}

bool SetCurrentStatement::recognizeAscii(char* buf, int len){
    return len > 9 && strncmp(buf, "current ", 8) == 0;
}

bool SetCurrentStatement::parseAscii(char* buf, int len){
    
    std::string str(buf+8, len-9);
    if (is_number(str)){
        currentGoal = small_atof(str.c_str());
        return true;
    }

    return false;
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
    setVelocity(velocityGoal);
}

bool SetVelocityStatement::recognizeAscii(char* buf, int len){
    return len > 10 && strncmp(buf, "velocity ", 9) == 0;
}

bool SetVelocityStatement::parseAscii(char* buf, int len){
        std::string str(buf+9, len-10);
    if (is_number(str)){
        velocityGoal = small_atof(str.c_str());
        return true;
    }

    return false;
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


