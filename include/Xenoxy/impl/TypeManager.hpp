#ifndef TypeManager_hpp
#define TypeManager_hpp
#include <vector>
#include "Xenoxy/impl/Types.hpp"

namespace Xenoxy
{
    class TypeManager
    {
        private:
        // name <-> TypeDef
        std::unordered_map<std::string, TypeDef*> types;

        public:
        TypeDef* AddTypeDef(TypeDef type);
        TypeDef* GetTypeDef(std::string name);
        ObjectInst CreateObjectOfType(std::string name, ObjectFlags flags);
        ObjectInst CreateArrayOfType(std::string name, ObjectFlags flags, std::int64_t count);
    };
}

#endif