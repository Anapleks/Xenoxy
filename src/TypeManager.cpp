#include "Xenoxy/impl/TypeManager.hpp"

namespace Xenoxy
{
    TypeDef* TypeManager::AddTypeDef(TypeDef type)
    {
        TypeDef* typePtr = new TypeDef(type);
        auto iter = this->types.find(typePtr->name);
        if(iter != this->types.end())
        {
            throw std::runtime_error("Type already defined");
        }

        this->types[typePtr->name] = typePtr;
        return typePtr;
    }

    TypeDef* TypeManager::GetTypeDef(std::string name)
    {
        auto iter = this->types.find(name);
        if(iter == this->types.end())
        {
            throw std::runtime_error("Type not found");
        }

        return iter->second;
    }

    ObjectInst TypeManager::CreateObjectOfType(std::string name, ObjectFlags flags)
    {
        return ObjectInst(this->GetTypeDef(name), flags);
    }

    ObjectInst TypeManager::CreateArrayOfType(std::string name, ObjectFlags flags, std::int64_t count)
    {
        return ObjectInst(this->GetTypeDef(name), flags, count);
    }
}