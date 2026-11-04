#include "Xenoxy/impl/Types.hpp"
#include <stdexcept>

namespace Xenoxy {
    std::ostream& operator<<(std::ostream& os, const VariableSignature& sig)
    {
        os<<sig.name<<":";
        if(sig.flags.Static) { os<<" static"; }
        if(sig.flags.Const) { os<<" const"; }
        if(sig.flags.Untracked) { os<<" untracked"; }
        os<<" "<<sig.type;
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const FunctionSignature& sig)
    {
        os<<sig.name<<"(";
        bool printed = false;
        for(auto& VS : sig.parameters)
        {
            if(printed) { os<<", "; }
            os<<VS;
            printed=true;
        }
        os<<") ->";
        if(sig.flags.Static) { os<<" static"; }
        if(sig.flags.Const) { os<<" const"; }
        if(sig.flags.Untracked) { os<<" untracked"; }
        os<<" "<<sig.type;
        return os;
    }

    Object* Object::copy() const {
        Object* obj = new Object();
        obj->flags = TypeFlags(this->flags);
        obj->val = this->val->copy();
        return obj;
    }

    Object::~Object() {
        delete val;
    }

    Value Value::operator+(const Value &rhs) {
        // Edge case -> int over/underflow
        const std::int64_t* intVal = std::get_if<std::int64_t>(&this->data);
        const std::int64_t* intVal2 = std::get_if<std::int64_t>(&rhs.data);
        if(intVal != nullptr && intVal2 != nullptr) { return Value{(*intVal) + (*intVal2)}; }
        const double* floatVal = std::get_if<double>(&this->data);
        const double* floatVal2 = std::get_if<double>(&rhs.data);
        if(floatVal != nullptr && floatVal2 != nullptr) { return Value{(*floatVal) + (*floatVal2)}; }
        const std::string* stringVal = std::get_if<std::string>(&this->data);
        const std::string* stringVal2 = std::get_if<std::string>(&rhs.data);
        if(stringVal != nullptr && stringVal2 != nullptr) { return Value{(*stringVal) + (*stringVal2)}; }
        std::cout<<"Addition overflow: "<<"this="<<*this<<','<<"rhs="<<rhs<<std::endl;
        throw std::runtime_error("Invalid addition operands");
    }

    Value Value::operator-(const Value &rhs) {
        // Edge case -> int over/underflow
        const std::int64_t* intVal = std::get_if<std::int64_t>(&this->data);
        const std::int64_t* intVal2 = std::get_if<std::int64_t>(&rhs.data);
        if(intVal != nullptr && intVal2 != nullptr) { return Value{(*intVal) - (*intVal2)}; }
        const double* floatVal = std::get_if<double>(&this->data);
        const double* floatVal2 = std::get_if<double>(&rhs.data);
        if(floatVal != nullptr && floatVal2 != nullptr) { return Value{(*floatVal) - (*floatVal2)}; }
        throw std::runtime_error("Invalid subtraction operands");
    }

    Value Value::operator*(const Value &rhs) {
        // Edge case -> int over/underflow
        const std::int64_t* intVal = std::get_if<std::int64_t>(&this->data);
        const std::int64_t* intVal2 = std::get_if<std::int64_t>(&rhs.data);
        if(intVal != nullptr && intVal2 != nullptr) { return Value{(*intVal) * (*intVal2)}; }
        const double* floatVal = std::get_if<double>(&this->data);
        const double* floatVal2 = std::get_if<double>(&rhs.data);
        if(floatVal != nullptr && floatVal2 != nullptr) { return Value{(*floatVal) * (*floatVal2)}; }
        throw std::runtime_error("Invalid multiplication operands");
    }

    Value Value::operator/(const Value &rhs) {
        // Edge case -> div by 0, intmin/-1
        const std::int64_t* intVal = std::get_if<std::int64_t>(&this->data);
        const std::int64_t* intVal2 = std::get_if<std::int64_t>(&rhs.data);
        if(intVal != nullptr && intVal2 != nullptr) { return Value{(*intVal) / (*intVal2)}; }
        const double* floatVal = std::get_if<double>(&this->data);
        const double* floatVal2 = std::get_if<double>(&rhs.data);
        if(floatVal != nullptr && floatVal2 != nullptr) { return Value{(*floatVal) / (*floatVal2)}; }
        throw std::runtime_error("Invalid division operands");
    }

    Value Value::operator%(const Value &rhs) {
        // Edge case -> mod by negative
        const std::int64_t* intVal = std::get_if<std::int64_t>(&this->data);
        const std::int64_t* intVal2 = std::get_if<std::int64_t>(&rhs.data);
        if(intVal != nullptr && intVal2 != nullptr) { return Value{(*intVal) % (*intVal2)}; }
        throw std::runtime_error("Invalid division operands");
    }

    Value Value::operator==(const Value &rhs) {
        const std::int64_t* intVal = std::get_if<std::int64_t>(&this->data);
        const std::int64_t* intVal2 = std::get_if<std::int64_t>(&rhs.data);
        if(intVal != nullptr && intVal2 != nullptr) { return Value{(*intVal) == (*intVal2)}; }
        const double* floatVal = std::get_if<double>(&this->data);
        const double* floatVal2 = std::get_if<double>(&rhs.data);
        if(floatVal != nullptr && floatVal2 != nullptr) { return Value{(*floatVal) == (*floatVal2)}; }
        const std::string* stringVal = std::get_if<std::string>(&this->data);
        const std::string* stringVal2 = std::get_if<std::string>(&rhs.data);
        if(stringVal != nullptr && stringVal2 != nullptr) { return Value{(*stringVal) == (*stringVal2)}; }
        const bool* boolVal = std::get_if<bool>(&this->data);
        const bool* boolVal2 = std::get_if<bool>(&rhs.data);
        if(boolVal != nullptr && boolVal2 != nullptr) { return Value{(*boolVal) == (*boolVal2)}; }
        std::cout<<*this<<' '<<rhs<<std::endl;
        throw std::runtime_error("Invalid == operands");
    }

    Value Value::operator>=(const Value &rhs) {
        const std::int64_t* intVal = std::get_if<std::int64_t>(&this->data);
        const std::int64_t* intVal2 = std::get_if<std::int64_t>(&rhs.data);
        if(intVal != nullptr && intVal2 != nullptr) { return Value{(*intVal) >= (*intVal2)}; }
        const double* floatVal = std::get_if<double>(&this->data);
        const double* floatVal2 = std::get_if<double>(&rhs.data);
        if(floatVal != nullptr && floatVal2 != nullptr) { return Value{(*floatVal) >= (*floatVal2)}; }
        std::cout<<*this<<' '<<rhs<<std::endl;
        throw std::runtime_error("Invalid >= operands");
    }

    Value Value::operator<=(const Value &rhs) {
        const std::int64_t* intVal = std::get_if<std::int64_t>(&this->data);
        const std::int64_t* intVal2 = std::get_if<std::int64_t>(&rhs.data);
        if(intVal != nullptr && intVal2 != nullptr) { return Value{(*intVal) <= (*intVal2)}; }
        const double* floatVal = std::get_if<double>(&this->data);
        const double* floatVal2 = std::get_if<double>(&rhs.data);
        if(floatVal != nullptr && floatVal2 != nullptr) { return Value{(*floatVal) <= (*floatVal2)}; }
        std::cout<<*this<<' '<<rhs<<std::endl;
        throw std::runtime_error("Invalid <= operands");
    }

    Value Value::operator>(const Value &rhs) {
        const std::int64_t* intVal = std::get_if<std::int64_t>(&this->data);
        const std::int64_t* intVal2 = std::get_if<std::int64_t>(&rhs.data);
        if(intVal != nullptr && intVal2 != nullptr) { return Value{(*intVal) > (*intVal2)}; }
        const double* floatVal = std::get_if<double>(&this->data);
        const double* floatVal2 = std::get_if<double>(&rhs.data);
        if(floatVal != nullptr && floatVal2 != nullptr) { return Value{(*floatVal) > (*floatVal2)}; }
        std::cout<<*this<<' '<<rhs<<std::endl;
        throw std::runtime_error("Invalid > operands");
    }

    Value Value::operator<(const Value &rhs) {
        const std::int64_t* intVal = std::get_if<std::int64_t>(&this->data);
        const std::int64_t* intVal2 = std::get_if<std::int64_t>(&rhs.data);
        if(intVal != nullptr && intVal2 != nullptr) { return Value{(*intVal) < (*intVal2)}; }
        const double* floatVal = std::get_if<double>(&this->data);
        const double* floatVal2 = std::get_if<double>(&rhs.data);
        if(floatVal != nullptr && floatVal2 != nullptr) { return Value{(*floatVal) < (*floatVal2)}; }
        std::cout<<*this<<' '<<rhs<<std::endl;
        throw std::runtime_error("Invalid < operands");
    }

    Value Value::operator!() {
        const bool* boolVal = std::get_if<bool>(&this->data);
        if(boolVal != nullptr) { return Value{!(*boolVal)}; }
        throw std::runtime_error("Invalid inversion operand");
    }

    Value Value::operator!=(const Value &rhs) {
        return !(*this == rhs);
    }

    Value* Value::copy() const {
        using memberList = std::unordered_map<std::string, Object*>;
        Value* val = new Value();
        const std::int64_t* intVal = std::get_if<std::int64_t>(&this->data);
        if(intVal) { val->data = *intVal; return val; }
        const double* floatVal = std::get_if<double>(&this->data);
        if(floatVal) { val->data = *floatVal; return val; }
        const bool* boolVal = std::get_if<bool>(&this->data);
        if(boolVal) { val->data = *boolVal; return val; }
        const std::string* stringVal = std::get_if<std::string>(&this->data);
        if(stringVal) { val->data = std::string(*stringVal); return val; }
        const memberList* members = std::get_if<memberList>(&this->data);
        if(members) {
            memberList membersCopy = memberList();
            for(auto k : (*members)) {
                membersCopy[k.first] = k.second->copy();
            }
            val->data = membersCopy;
            return val;
        }
        throw std::runtime_error("You shouldn't be seeing this, uninitialized value");
    }

    Value::~Value() {
        using memberList = std::unordered_map<std::string, Object*>;
        const memberList* members = std::get_if<memberList>(&this->data);
        if(members) {
            for(auto k : (*members)) {
                delete k.second;
            }
        } else {
            return;
        }
    }

    std::ostream& operator<<(std::ostream& os, const Object& value) {
        std::string flags = value.flags.Const ? "const, " : "";
        flags += value.flags.Imut ? "imut " : "";
        flags += value.flags.Static ? "static " : "";
        flags += value.flags.Untracked ? "untracked " : "";
        os<<"("<<flags<<"Value: "<<*value.val<<")";
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const Value& value) {
        using memberList = std::unordered_map<std::string, Object*>;
        const std::int64_t* intVal = std::get_if<std::int64_t>(&value.data);
        if(intVal) { os<<"int "<< *intVal; }
        const double* floatVal = std::get_if<double>(&value.data);
        if(floatVal) { os<<"float "<<*floatVal; }
        const bool* boolVal = std::get_if<bool>(&value.data);
        if(boolVal) { os<<"bool "<<*boolVal; }
        const std::string* stringVal = std::get_if<std::string>(&value.data);
        if(stringVal) { os<<"string "<<std::string(*stringVal); }
        const memberList* members = std::get_if<memberList>(&value.data);
        if(members) { os<<"memberList"; }
        if(!(intVal || floatVal || boolVal || stringVal || members)) {
            os<<"none";
        }
        return os;
    }
}
