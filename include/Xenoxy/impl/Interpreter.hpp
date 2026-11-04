#ifndef Interpreter_hpp
#define Interpreter_hpp
#include <string>
#include <stack>
#include <cstdint>
#include <bitset>
#include "Xenoxy/impl/Types.hpp"
#include "Xenoxy/impl/StateManager.hpp"

namespace Xenoxy {
    // struct StatementFrame {
    //     std::int64_t idx;
    //     Statement* stmt=nullptr;
    // };

    // struct ExpressionFrame {
    //     Expression* expr=nullptr;
    //     Value val;
    // };

    struct NodeFrame {
        Node* node=nullptr;
        std::uint64_t idx=0;
        std::uint64_t endIdx=-1;
        Value val=Value();
        bool popSID = false;
        bool exitSubscope = false;
    };

    struct ControlFrame {
        std::int64_t popMax=0;
        // scopeId -> node
        std::bitset<2> stacks{0};
    };

    struct ExecutionFrame {
        std::vector<size_t> stackIdxs;
        std::vector<Value> results;
        size_t rootIdx;
        size_t filled=0;
    };

    class Interpreter {
        private:
        // performs test for whether the executionStack is meant to receive or send a request
        bool ExecutionStackTest();
        // adds result of expression evaluation into execution stack
        void AddResult(Value val);
        // pops following the rules set by the controlStack
        void PopStacks();
        // pops control stack, while keeping other stacks synchronised
        void ForcePopStacks();
        // creates an execution stack request and required node stack entry
        void CreateExecutionRequest(std::uint64_t resultCount, bool createSubscope=false);
        public:
        StateManager* stateMgr=nullptr;
        // scope Ids, basically rip stack
        std::stack<std::int64_t> scopeIdStack;
        // node execution stack
        std::stack<NodeFrame> nodeFrameStack;
        // gathers results stack
        std::stack<ExecutionFrame> executionStack;
        Value Call(std::string functionName);
    };
}

#endif