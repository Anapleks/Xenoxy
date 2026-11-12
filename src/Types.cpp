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

    void* ObjectInst::Get() const
    {
        if(this->type->type != TypeType::Primitive) { throw std::runtime_error("Trying to get not primitive type"); }
        return this->data;
    }

    TypeDef* ObjectInst::GetType() const
    {
        return this->type;
    }

    ObjectInst ObjectInst::operator+(ObjectInst& other)
    {
        if(!this->TypeCheck(other.type->name))
        {
            throw std::runtime_error("operator+ type mismatch");
        }

        if(this->type->name == "int")
        {
            ObjectInst ret = ObjectInst(this->type, ObjectFlags{false, false, false, false});
            *(std::int64_t*)ret.Get() = *(std::int64_t*)this->Get() + *(std::int64_t*)other.Get();
            return ret;
        }
        else if(this->type->name == "float")
        {
            ObjectInst ret = ObjectInst(this->type, ObjectFlags{false, false, false, false});
            *(double*)ret.Get() = *(double*)this->Get() + *(double*)other.Get();
            return ret;
        }
        else if(this->type->name == "bool")
        {
            ObjectInst ret = ObjectInst(this->type, ObjectFlags{false, false, false, false});
            *(bool*)ret.Get() = *(bool*)this->Get() + *(bool*)other.Get();
            return ret;
        }
        throw std::runtime_error("Invalid type");
    }
    
    ObjectInst ObjectInst::operator-(ObjectInst& other)
    {
        if(!this->TypeCheck(other.type->name))
        {
            throw std::runtime_error("operator- type mismatch");
        }

        if(this->type->name == "int")
        {
            ObjectInst ret = ObjectInst(this->type, ObjectFlags{false, false, false, false});
            *(std::int64_t*)ret.Get() = *(std::int64_t*)this->Get() - *(std::int64_t*)other.Get();
            return ret;
        }
        else if(this->type->name == "float")
        {
            ObjectInst ret = ObjectInst(this->type, ObjectFlags{false, false, false, false});
            *(double*)ret.Get() = *(double*)this->Get() - *(double*)other.Get();
            return ret;
        }
        throw std::runtime_error("Invalid type");
    }
    
    ObjectInst ObjectInst::operator*(ObjectInst& other)
    {
        if(!this->TypeCheck(other.type->name))
        {
            throw std::runtime_error("operator* type mismatch");
        }

        if(this->type->name == "int")
        {
            ObjectInst ret = ObjectInst(this->type, ObjectFlags{false, false, false, false});
            *(std::int64_t*)ret.Get() = *(std::int64_t*)this->Get() * *(std::int64_t*)other.Get();
            return ret;
        }
        else if(this->type->name == "float")
        {
            ObjectInst ret = ObjectInst(this->type, ObjectFlags{false, false, false, false});
            *(double*)ret.Get() = *(double*)this->Get() * *(double*)other.Get();
            return ret;
        }
        throw std::runtime_error("Invalid type");
    }
    
    ObjectInst ObjectInst::operator/(ObjectInst& other)
    {
        if(!this->TypeCheck(other.type->name))
        {
            throw std::runtime_error("operator/ type mismatch");
        }

        if(this->type->name == "int")
        {
            ObjectInst ret = ObjectInst(this->type, ObjectFlags{false, false, false, false});
            *(std::int64_t*)ret.Get() = *(std::int64_t*)this->Get() / *(std::int64_t*)other.Get();
            return ret;
        }
        else if(this->type->name == "float")
        {
            ObjectInst ret = ObjectInst(this->type, ObjectFlags{false, false, false, false});
            *(double*)ret.Get() = *(double*)this->Get() / *(double*)other.Get();
            return ret;
        }
        throw std::runtime_error("Invalid type");
    }
    
    ObjectInst ObjectInst::operator%(ObjectInst& other)
    {
        if(!this->TypeCheck(other.type->name))
        {
            throw std::runtime_error("operator% type mismatch");
        }

        if(this->type->name == "int")
        {
            ObjectInst ret = ObjectInst(this->type, ObjectFlags{false, false, false, false});
            *(std::int64_t*)ret.Get() = *(std::int64_t*)this->Get() % *(std::int64_t*)other.Get();
            return ret;
        }
        throw std::runtime_error("Invalid type");
    }

    ObjectInst ObjectInst::operator==(ObjectInst& other)
    {
        if(!this->TypeCheck(other.type->name))
        {
            throw std::runtime_error("operator== type mismatch");
        }

        if(this->type->name == "int")
        {
            ObjectInst ret = ObjectInst(this->type, ObjectFlags{false, false, false, false});
            *(std::int64_t*)ret.Get() = *(std::int64_t*)this->Get() == *(std::int64_t*)other.Get();
            return ret;
        }
        else if(this->type->name == "float")
        {
            ObjectInst ret = ObjectInst(this->type, ObjectFlags{false, false, false, false});
            *(double*)ret.Get() = *(double*)this->Get() == *(double*)other.Get();
            return ret;
        }
        else if(this->type->name == "bool")
        {
            ObjectInst ret = ObjectInst(this->type, ObjectFlags{false, false, false, false});
            *(bool*)ret.Get() = *(bool*)this->Get() == *(bool*)other.Get();
            return ret;
        }
        throw std::runtime_error("Invalid type");
    }

    bool ObjectInst::TypeCheck(std::string typeName) const
    {
        return this->type->name == typeName;
    }

    std::ostream& operator<<(std::ostream& os, const ObjectInst& inst)
    {
        std::cout<<inst.GetType()->name<<' ';

        if(inst.GetType()->name == "int")
        {
            os<<*(std::int64_t*)inst.Get();
        }
        else if(inst.GetType()->name == "float")
        {
            os<<*(double*)inst.Get();
        }
        else if(inst.GetType()->name == "bool")
        {
            std::string value = *(bool*)inst.Get() ? "True" : "False";
            os<<value;
        }
        return os;
    }
}
