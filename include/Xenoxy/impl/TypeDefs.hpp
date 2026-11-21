#ifndef TypeDefs_hpp
#define TypeDefs_hpp

#define integerTypeName "int"
#define floatingPointTypeName "float"
#define booleanTypeName "bool"

#define integerTypeDef TypeDef(integerTypeName, Xenoxy::TypeType::Primitive, 8)
#define floatingPointTypeDef TypeDef(floatingPointTypeName, Xenoxy::TypeType::Primitive, 8)
#define booleanTypeDef TypeDef(booleanTypeName, Xenoxy::TypeType::Primitive, 1)

#endif