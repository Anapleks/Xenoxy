#ifndef Types_hpp
#define Types_hpp
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <iostream>
#include <variant>

namespace Xenoxy {
    enum class TypeType
    {
        // invalid
        None,
        // int, float, char, bool
        Primitive,
        // user defined classes
        Composite
    };

    std::ostream& operator<<(std::ostream& os, const TypeType& type);

    class TypeDef
    {
        private:
        // "" refers to this object, should contain offset 0
        // stores the offsets to different fields (if this type has any)
        // mapped recursively, ie field.field is accessible
        std::unordered_map<std::string, std::int64_t> offsets;

        // "" does not have typedef
        // Stores the types of the different fields (if this type has any)
        // mapped recursively, ie field.field is accessible
        std::unordered_map<std::string, TypeDef*> types;

        // stores the size of this type in bytes
        std::int64_t size=0;

        public:
        // name of this type
        std::string name;

        // type of this TypeDef
        TypeType type;

        TypeDef(std::string name, TypeType type, size_t size=0);

        // returns the size of the type
        std::int64_t GetSize();

        // access TypeDef for given field
        TypeDef& operator[](const std::string& fieldName);

        // access offset for given field
        std::int64_t GetOffset(const std::string& fieldName);

        // adds a new field to this typeDef
        // if the new field is of a composite type, creates recursive mapping
        void AddField(std::string fieldName, TypeDef* type);

        // checks for a perfect match, checking if both the name, the field names and the offsets are equal
        // no need to check types due to the recursive mapping
        bool operator==(const TypeDef& other);
        bool operator!=(const TypeDef& other);

        void DebugPrint();
    };

    struct ObjectFlags
    {
        // set if object is unchangeable
        bool Imut=false;
        // set if binding is unchangeable
        bool Const=false;
        // set if object is not thread-safety tracked by StateManager,
        // default to false for local variables
        bool Untracked=false;
        // set if object is static (allocated once and kept separate)
        bool Static=false;
    };

    class ObjectInst
    {
        // when true ObjectInst owns all allocated objects in its data
        bool owning = false;

        // Type of this object
        // check type equality before assignment
        TypeDef* type;

        // Flags for this object (modification, lifetime)
        ObjectFlags flags;

        // for arrays != -1
        // for typical object instances (not arrays) == -1
        std::int64_t count;

        // pointer to the data directly
        void* data = nullptr;

        public:
        ObjectInst() = delete;
        ObjectInst(TypeDef* type, ObjectFlags flags, std::int64_t count=-1, bool owning = true);
        ~ObjectInst();

        // returns an ObjectInst object for the given fieldName,
        // supports field.field accessing
        ObjectInst operator[](const std::string&);

        // returns an ObjectInst object for the given index in an array,
        // throws error when attempting to index an ObjectInst that is not an array
        ObjectInst operator[](std::int64_t index);

        // returns pointer to this object's data field
        // to be run on ObjectInst objects for fields
        void* Get() const;

        TypeDef* GetType() const;

        ObjectInst operator+(ObjectInst& other);
        ObjectInst operator-(ObjectInst& other);
        ObjectInst operator*(ObjectInst& other);
        ObjectInst operator/(ObjectInst& other);
        ObjectInst operator%(ObjectInst& other);

        ObjectInst operator==(ObjectInst& other);

        // returns true when types match
        bool TypeCheck(std::string typeName) const;
    };

    std::ostream& operator<<(std::ostream& os, const ObjectInst& inst);

    struct VariableSignature
    {
        std::string name;
        std::string type;
        ObjectFlags flags{false, false, false, false};
    };

    std::ostream& operator<<(std::ostream& os, const VariableSignature& sig);

    struct FunctionSignature
    {
        std::string name;
        std::string type;
        ObjectFlags flags;
        std::vector<VariableSignature> parameters;
    };

    std::ostream& operator<<(std::ostream& os, const FunctionSignature& sig);

    struct ObjectRef
    {
        std::string name;
    };

    struct FunctionRef
    {
        std::string name;
    };

    struct ObjectHolder
    {
        ObjectInst* obj;
    };
}

#endif