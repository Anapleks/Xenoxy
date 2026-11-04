#include "Xenoxy/impl/StateManager.hpp"
#include <iostream>

namespace Xenoxy {
    Function::~Function() {
        delete this->scopeEAST;
        for(auto k : this->s_Functions) {
            delete k.second;
        }
        for(auto k : this->s_Objects) {
            delete k.second;
        }
    }

    ScopeInstance::~ScopeInstance() {
        for(auto k : this->t_Objects) {
            delete k.second;
        }
    }

    std::ostream& operator<<(std::ostream& os, const ScopeInstance& inst)
    {
        os<<"{";
        bool printed=false;
        for(auto k : inst.t_Objects)
        {
            if(printed) {os<<',';}
            os<<"["<<k.first<<"]="<<*k.second;
            printed=true;
        }
        ScopeInstance* ptr = inst.parentScopeInstance;
        while(true)
        {
            if(ptr == nullptr) { break; }
            for(auto k : ptr->t_Objects)
            {
                if(printed) {os<<',';}
                os<<"["<<k.first<<"]="<<*k.second;
                printed=true;
            }
            ptr = ptr->parentScopeInstance;
        }
        os<<"}";
        return os;
    }

    std::int64_t StateManager::CreateScope(FunctionRef& ref) {
        int64_t id = this->nextID++;

        this->scopeInstances[id] = new ScopeInstance();
        this->scopeInstances[id]->functionMetadata = this->g_Functions[ref.name];

        return id;
    }

    void StateManager::DeleteScope(std::int64_t scopeID) {
        ScopeInstance* inst = this->scopeInstances[scopeID];
        ScopeInstance* parent = inst->parentScopeInstance;
        delete inst;
        while(parent != nullptr) {
            inst = parent;
            parent = inst->parentScopeInstance;
            delete inst;
        }
        this->scopeInstances.erase(scopeID);
    }

    void StateManager::EnterSubscope(std::int64_t scopeID) {
        ScopeInstance* inst = this->scopeInstances.at(scopeID);
        ScopeInstance* new_inst = new ScopeInstance();
        new_inst->parentScopeInstance = inst;
        this->scopeInstances[scopeID] = new_inst;
    }

    void StateManager::LeaveSubscope(std::int64_t scopeID) {
        ScopeInstance* inst = this->scopeInstances.at(scopeID);
        ScopeInstance* parent_inst = inst->parentScopeInstance;
        if(parent_inst == nullptr) { throw std::runtime_error("Tried to exit root scope"); }
        this->scopeInstances[scopeID] = parent_inst;
        delete inst;
    }

    Object* StateManager::CreateVariable(std::int64_t scopeID, VariableSignature& sig) {
        ObjectRef ref = ObjectRef{sig.name};
        if(this->GetVariable(scopeID, ref) != nullptr) { throw std::runtime_error("Double declaration of variable"); }
        ScopeInstance* inst = this->scopeInstances.at(scopeID);
        inst->t_Objects[ref.name] = new Object();
        // std::cout<<*inst<<std::endl;
        return inst->t_Objects[ref.name];
    }
    
    Object* StateManager::GetVariable(std::int64_t scopeID, ObjectRef& ref) {
        ScopeInstance* inst = this->scopeInstances.at(scopeID);
        // std::cout<<*inst<<std::endl;

        while(true) {
            auto it = inst->t_Objects.find(ref.name);
            if(it == inst->t_Objects.end()) {
                if(inst->parentScopeInstance == nullptr) {
                    return nullptr;
                }
                inst = inst->parentScopeInstance;
            }
            else {
                return it->second;
            }
        }
    }

    void StateManager::G_CreateFunction(Node* EAST, FunctionSignature signature) {
        if(EAST == nullptr) { return; }
        Function* func = new Function();
        func->scopeEAST = EAST;
        func->signature = signature;
        this->g_Functions[signature.name] = func;
        return;
    }

    void StateManager::G_CreateVariable(VariableSignature signature) {
        std::cout<<"Remember to remake G_CreateVariable at a later date!"<<std::endl;
        Object* obj = new Object();
        this->g_Objects[signature.name] = obj;
    }

    Function* StateManager::G_GetFunction(std::string name) {
        return this->g_Functions.at(name);
    }

    StateManager::~StateManager() {
        for(auto k : this->g_Functions) {
            delete k.second;
        }
        for(auto k : this->g_Objects) {
            delete k.second;
        }
        for(auto k : this->scopeInstances) {
            ScopeInstance* inst = k.second;
            ScopeInstance* parent = inst->parentScopeInstance;
            delete inst;
            while(parent != nullptr) {
                inst = parent;
                parent = inst->parentScopeInstance;
                delete inst;
            }
        }
    }
    std::ostream& operator<<(std::ostream& os, const StateManager& inst)
    {
        bool printed=false;
        for(auto& k : inst.scopeInstances)
        {
            if(printed) {os<<'\n';}
            os<<"["<<k.first<<"]="<<*k.second;
            printed=true;
        }
        return os;
    }
}