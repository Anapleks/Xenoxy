#ifndef StateManager_hpp
#define StateManager_hpp
#include <cstdint>
#include <unordered_map>
#include <string>

#include "Xenoxy/impl/Types.hpp"
#include "Xenoxy/impl/EAST.hpp"

namespace Xenoxy {
    // s_ - static
    // g_ - global
    // t_ - temporary

    struct Function {
        // static objects
        std::unordered_map<std::string, Object*> s_Objects;
        // sub functions
        std::unordered_map<std::string, Function*> s_Functions;
        Node* scopeEAST;
        FunctionSignature signature;

        ~Function();
    };

    struct ScopeInstance {
        ScopeInstance* parentScopeInstance = nullptr;
        Function* functionMetadata = nullptr;
        std::unordered_map<std::string, Object*> t_Objects;

        ~ScopeInstance();
    };
    std::ostream& operator<<(std::ostream& os, const ScopeInstance& inst);

    // Calling a function:
    // save scopeID
    // call newScopeInstance(function_name) -> returns new scopeID

    // returning from a function
    // call deleteScopeInstance(scopeID)
    // get old scopeID

    // Entering subscope (conditional, loop, etc)
    // call enterSubscope(scopeID)

    // Leaving subscope (conditional, loop, etc)
    // call leaveSubscope(scopeID)

    // Creating variable (not static)
    // call CreateVariable(scopeID)
    // automatically makes it part of the scope it was created in,
    // deleted on scope destruction automatically

    // Declaring function (global)
    // call G_CreateFunction(Statement, FunctionSignature)

    // Declaring variable (global)
    // call G_CreateVariabe(VariableSignature)

    // Getting function* (from global)
    // call G_GetFunction(function_name)

    class StateManager {
        public:
        //                 name         pointer to
        std::unordered_map<std::string, Function*> g_Functions;
        std::unordered_map<std::string, Object*> g_Objects;

        std::int64_t nextID=1;
        std::unordered_map<std::int64_t, ScopeInstance*> scopeInstances;

        std::int64_t CreateScope(FunctionRef& ref);
        void DeleteScope(std::int64_t scopeID);
        void EnterSubscope(std::int64_t scopeID);
        void LeaveSubscope(std::int64_t scopeID);
        Object* CreateVariable(std::int64_t scopeID, VariableSignature& sig);
        Object* GetVariable(std::int64_t scopeID, ObjectRef& ref);
        void G_CreateFunction(Node* EAST, FunctionSignature signature);
        void G_CreateVariable(VariableSignature signature);
        Function* G_GetFunction(std::string name);
        
        ~StateManager();
    };
    std::ostream& operator<<(std::ostream& os, const StateManager& inst);
}

#endif