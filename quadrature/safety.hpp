#ifndef SAFETY_HPP
#define SAFETY_HPP

enum class ComponentState {DISABLED, NORMAL, ERROR};
enum class InformationValidity{INVALID, VALID};

class SafetyComponent {
    public:
    ComponentState getSafetyState();
    
    protected:
    ComponentState safety_state = ComponentState::DISABLED;
};

#endif