#include "Xenoxy/impl/Types.hpp"
#include <stdexcept>

namespace Xenoxy {
    std::ostream& operator<<(std::ostream& os, const TypeType& type)
    {
        switch(type)
        {
            case TypeType::None:
                os<<"TypeType::None";
                break;
            case TypeType::Primitive:
                os<<"TypeType::Primitive";
                break;
            case TypeType::Composite:
                os<<"TypeType::Composite";
                break;
            default:
                os<<"TypeType::Unknown";
                break;
        }
        return os;
    }

    TypeDef::TypeDef(std::string name, TypeType type, size_t size)
    : type(type), name(name), size(size) {
        this->offsets[""] = 0;
    }

    std::int64_t TypeDef::GetSize() { return this->size; }

    TypeDef& TypeDef::operator[](const std::string& fieldName)
    {
        auto iter = this->types.find(fieldName);
        if(iter == this->types.end())
        {
            throw std::runtime_error("Invalid field");
        }

        return (*iter->second);
    }

    std::int64_t TypeDef::GetOffset(const std::string& fieldName)
    {
        auto iter = this->offsets.find(fieldName);
        if(iter == this->offsets.end())
        {
            throw std::runtime_error("Field does not exist");
        }

        return iter->second;
    }

        // this->types[fieldName] = type;
        // this->offsets[fieldName] = this->size;
        // if(type->type == TypeType::Primitive)
        // {
        //     this->size += type->size;
        //     return;
        // }

        // for(auto p : type->types)
        // {
        //     std::string newField = fieldName + "." + p.first;
        //     this->RecurTypeParser(newField, p.second);
        // }

    void TypeDef::AddField(std::string fieldName, TypeDef* type)
    {
        auto iter = this->offsets.find(fieldName);
        if(iter != this->offsets.end())
        {
            throw std::runtime_error("Field already exists");
        }

        if(type->type == TypeType::None)
        {
            throw std::runtime_error("Invalid TypeType");
        }

        for(auto& p : type->offsets)
        {

            std::string newName;
            if(p.first == "")
            {
                newName = fieldName;
                this->types[newName] = type;
            }
            else
            {
                
                newName = fieldName + "." + p.first;
                this->types[newName] = type->types[p.first];
            }
            this->offsets[newName] = p.second + this->size;
        }
        this->size += type->size;
    }

    bool TypeDef::operator==(const TypeDef& other)
    {
        if(this->offsets.size() != other.offsets.size())
        {
            return false;
        }

        for(auto p : this->offsets)
        {
            auto iter = other.offsets.find(p.first);
            if(iter == other.offsets.end())
            {
                return false;
            }

            if(p.second != iter->second)
            {
                return false;
            }
        }
        return true;
    }

    bool TypeDef::operator!=(const TypeDef& other)
    {
        return !(*this == other);
    }

    void TypeDef::DebugPrint()
    {
        std::cout<<this->name<<' '<<this->size<<std::endl;
        for(auto p: this->offsets)
        {
            std::cout<<p.first<<' '<<p.second<<std::endl;
        }
    }

    ObjectInst::ObjectInst(TypeDef* type, ObjectFlags flags, std::int64_t count, bool owning)
    : type(type), flags(flags), count(count)
    {
        if(!owning) { return; }
        if(count == -1)
        {
            data = malloc(this->type->GetSize());
        }
        else
        {
            data = malloc(this->type->GetSize() * count);
        }
    }

    ObjectInst::~ObjectInst()
    {
        // in the future, once there are owning ObjectInstances, add recursive freeing!
        if(!owning) { return; }
        free(data);
    }

    ObjectInst ObjectInst::operator[](const std::string& fieldName)
    {
        TypeDef* childType = &(*this->type)[fieldName];
        if(childType == nullptr) { throw std::runtime_error("Child type is null"); }
        std::int64_t offset = this->type->GetOffset(fieldName);
        if(this->count == -1) {
            ObjectInst subInst(childType, this->flags, -1, false);
            subInst.data = (void*)((std::int64_t)this->data + offset);
            return subInst;
        }

        throw std::runtime_error("Arrays not to be indexed with operator[]");
    }

    void* ObjectInst::Get()
    {
        if(this->type->type != TypeType::Primitive) { throw std::runtime_error("Trying to get not primitive type"); }
        return this->data;
    }













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
        obj->flags = ObjectFlags(this->flags);
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
