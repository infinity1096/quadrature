#ifndef FIELDTREE_HPP
#define FIELDTREE_HPP

#include <map>
#include <string>

class AdjustableField {
    public:
    AdjustableField(std::string aFieldName, float* aFieldLocation) : field_name(aFieldName), field_location(aFieldLocation) {}

    std::string getName() const {
        return field_name;
    }

    virtual float* operator[](std::string aName){
        if (aName == field_name){
            return field_location;
        }else{
            return nullptr;
        }
    }

    protected:
    std::string field_name;
    float* field_location = nullptr;    
};

class FieldTreeNode : public AdjustableField{
    public:

    void addChild(const AdjustableField&& aChildField){
        childrens[aChildField.getName()] = std::move(aChildField);
    }

    virtual float* operator[](std::string aName) override {
        
        auto it = aName.find_first_of(".");
        if (it == std::string::npos){
            return nullptr;
        }

        std::string subFieldName = aName.substr(0, it);
        std::string query = aName.substr(it + 1, aName.size() - 1 - it);

        return childrens[subFieldName][query];
    }

    private:
    std::map<std::string, AdjustableField&> childrens;
};

#endif