#ifndef __COMMAND_HPP
#define __COMMAND_HPP

class Command{
    public:
    
    // sub-class this interface to create your own command for the quadcoptor
    // this class provided a pointer to the PX4Quadcoptor [quad] for use.

    // initialize is called only once when your command runs for the first time.
    virtual void initialize() {}

    // execute is called periodically until your [isFinished] function returns true
    virtual void execute() {}

    // return whether the command should finish.
    virtual bool isFinished() = 0;

    // called once when the command finishes.
    virtual void end(){}

    // main logic flow for commands. do not touch.
    void step(){
        if (process_ends){
            return;
        }

        if (!initialized){
            initialize();
            initialized = true;
        }

        if (!isFinished()){
            execute();
        }else{
            end();
            process_ends = true;
        }
    }

    bool isProcessEnded(){
        return process_ends;
    }

    private:
    bool initialized = false;
    bool process_ends = false;
};


#endif