#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include <vector>
#include <string>

class Statement {
    public:
    bool recognize(char* buf, int len);
    bool parse(char* buf, int len);
    virtual void execute()=0;
    virtual ~Statement() {}

    protected:
    virtual bool recognizeAscii(char* buf, int len)=0;
    virtual bool recognizeBinary(char* buf, int len){
        return false;
    }
    
    virtual bool parseAscii(char* buf, int len)=0;
    virtual bool parseBinary(char* buf, int len){
        return false;
    } 
};

class ArmAxisStatement : public Statement {
    public:
    void execute() override;

    bool recognizeAscii(char* buf, int len) override;
    bool parseAscii(char* buf, int len) override; 
};

class DisarmAxisStatement : public Statement {
    public:
    void execute() override;

    bool recognizeAscii(char* buf, int len) override;
    bool parseAscii(char* buf, int len) override; 
};

class SetParameterStatement : public Statement {
    public:
    void execute() override;

    bool recognizeAscii(char* buf, int len) override;
    bool parseAscii(char* buf, int len) override; 

    private:
    std::string field;
    std::string value;

    float floatVal = 0.0;
};

class FlashSaveStatement : public Statement {
    public:
    void execute() override;

    bool recognizeAscii(char* buf, int len) override;
    bool parseAscii(char* buf, int len) override; 
};

class ListParametersStatement : public Statement {
    public:
    void execute() override;

    bool recognizeAscii(char* buf, int len) override;
    bool parseAscii(char* buf, int len) override; 
};

class SetCurrentStatement : public Statement {
    public:
    void execute() override;

    bool recognizeAscii(char* buf, int len) override;
    bool parseAscii(char* buf, int len) override; 
};

class SetTorqueStatement : public Statement {
    public:
    void execute() override;

    bool recognizeAscii(char* buf, int len) override;
    bool parseAscii(char* buf, int len) override; 
};

class SetVelocityStatement : public Statement {
    public:
    void execute() override;

    bool recognizeAscii(char* buf, int len) override;
    bool parseAscii(char* buf, int len) override; 
};

class SetPositionStatement : public Statement {
    public:
    void execute() override;

    bool recognizeAscii(char* buf, int len) override;
    bool parseAscii(char* buf, int len) override; 
};

class SetTelemetryStatement : public Statement {
    public:
    void execute() override;

    bool recognizeAscii(char* buf, int len) override;
    bool parseAscii(char* buf, int len) override; 
};


#endif