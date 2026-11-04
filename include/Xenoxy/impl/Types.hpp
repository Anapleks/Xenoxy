#ifndef Types_hpp
#define Types_hpp
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <cstdint>
#include <iostream>

namespace Xenoxy {
    struct TypeFlags {
        bool Imut=false;
        bool Const=false;
        bool Untracked=false;
        bool Static=false;
    };

    struct VariableSignature {
        std::string name;
        std::string type;
        TypeFlags flags{false, false, false, false};
    };

    std::ostream& operator<<(std::ostream& os, const VariableSignature& sig);

    struct FunctionSignature {
        std::string name;
        std::string type;
        TypeFlags flags;
        std::vector<VariableSignature> parameters;
    };

    std::ostream& operator<<(std::ostream& os, const FunctionSignature& sig);

    struct ObjectRef {
        std::string name;
    };

    struct FunctionRef {
        std::string name;
    };

    class Value;
    class Object {
        public:
        Value* val;
        TypeFlags flags;
        // also mutexes, atomics, etc
        Object* copy() const;
        ~Object();
    };

    class Value {
        public:
        std::variant<
            std::unordered_map<std::string, Object*>,
            std::string,
            std::int64_t,
            bool,
            double> data;

        Value* copy() const;
        ~Value();
        Value operator+(const Value& rhs);
        Value operator-(const Value &rhs);
        Value operator*(const Value &rhs);
        Value operator/(const Value &rhs);
        Value operator%(const Value &rhs);
        Value operator==(const Value &rhs);
        Value operator>=(const Value &rhs);
        Value operator<=(const Value &rhs);
        Value operator>(const Value &rhs);
        Value operator<(const Value &rhs);
        Value operator!();
        Value operator!=(const Value &rhs);
    };

    std::ostream& operator<<(std::ostream& os, const Object& value);
    std::ostream& operator<<(std::ostream& os, const Value& value);
}

#endif