#include "Xenoxy/impl/Interpreter.hpp"
#include <iostream>

namespace Xenoxy
{
    void Interpreter::PopStacks()
    {
        if(this->nodeFrameStack.top().popSID)
        {
            this->stateMgr->DeleteScope(this->scopeIdStack.top());
            this->scopeIdStack.pop();
        }
        if(this->nodeFrameStack.top().exitSubscope) { this->stateMgr->LeaveSubscope(this->scopeIdStack.top()); }
        this->nodeFrameStack.pop();
    }

    void Interpreter::ForcePopStacks()
    {
        while(!this->nodeFrameStack.top().popSID)
        {
            if(this->nodeFrameStack.top().exitSubscope) { this->stateMgr->LeaveSubscope(this->scopeIdStack.top()); }
            this->nodeFrameStack.pop();
        }
        this->PopStacks();
    }

    bool Interpreter::ExecutionStackTest()
    {
        return (this->executionStack.size() == 0
                || (this->executionStack.size() > 0
                && this->executionStack.top().filled != this->executionStack.top().stackIdxs.size()));
    }

    void Interpreter::AddResult(Value val)
    {
        for(size_t i = 0; i < this->executionStack.top().stackIdxs.size(); i++)
        {
            if(this->executionStack.top().stackIdxs[i] == this->nodeFrameStack.size())
            {
                this->executionStack.top().results.push_back(val);
                this->executionStack.top().filled++;
                break;
            }
        }
    }

    void Interpreter::CreateExecutionRequest(std::uint64_t resultCount, bool createSubscope)
    {
        std::int64_t idx = this->nodeFrameStack.top().idx;
        Node* rootNode = this->nodeFrameStack.top().node;
        std::vector<size_t> idxs = {};
        for(size_t i = 0; i < resultCount; i++)
        {
            idxs.push_back(this->nodeFrameStack.size()+1);
        }
        this->executionStack.push(ExecutionFrame{
            idxs,
            {},
            this->nodeFrameStack.size()
        });

        if(rootNode->subNodes[idx]->subNodes.size() < resultCount)
        {
            // std::cout<<rootNode->subNodes[idx]->subNodes.size()<<std::endl;
            throw std::runtime_error("Invalid createExecutionRequest subNodes size");
        }

        this->nodeFrameStack.push(NodeFrame{
            rootNode->subNodes[idx],
            0,
            resultCount,
            false,
            createSubscope
        });

    }

    Value Interpreter::Call(std::string functionName)
    {
        std::cout<<functionName<<std::endl;

        if(this->stateMgr == nullptr) { throw std::runtime_error("Interpreter does not have access to State Manager"); }
        Function* rootFunc = this->stateMgr->G_GetFunction(functionName);
        if(rootFunc == nullptr) { throw std::runtime_error("Called function does not exist"); }

        FunctionRef ref;
        ref.name = rootFunc->signature.name;

        std::int64_t sid = this->stateMgr->CreateScope(ref);
        this->scopeIdStack.push(sid);
        this->nodeFrameStack.push(NodeFrame{rootFunc->scopeEAST, 0, -1ULL, Value(), true, false});
        Value lastReturn = Value();
        bool returning = false;

        // execute until out of all possible scopes/statements/expression
        while(this->nodeFrameStack.size() > 0)
        {
            if(this->executionStack.size() > 100)
            {
                throw std::runtime_error("Too deep bough");
            }
            if(this->nodeFrameStack.top().idx >= this->nodeFrameStack.top().node->subNodes.size()
               || this->nodeFrameStack.top().idx >= this->nodeFrameStack.top().endIdx)
            {
                this->PopStacks();
                continue;
            }
            // std::cout<<"nodeFrameSize="<<this->nodeFrameStack.size()
            //          <<", executionSize="<<this->executionStack.size()
            //          <<", subSize="<<this->nodeFrameStack.top().node->subNodes.size()
            //          <<", popSID="<<this->nodeFrameStack.top().popSID
            //          <<", idx="<<this->nodeFrameStack.top().idx<<std::endl;
            // std::cout<<*this->stateMgr<<std::endl;
            std::int64_t idx = this->nodeFrameStack.top().idx;
            Node* rootNode = this->nodeFrameStack.top().node;
            if(rootNode->subNodes[idx]->type == NodeType::Expr_IntLiteral)
            {
                this->AddResult(Value{std::get<std::int64_t>(rootNode->subNodes[idx]->data)});
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Expr_ObjAccess)
            {
                ObjectRef& OR = std::get<ObjectRef>(rootNode->subNodes[idx]->data);
                Object* obj = this->stateMgr->GetVariable(this->scopeIdStack.top(), OR);
                this->AddResult(Value(*obj->val));
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Expr_Add)
            {
                if(this->ExecutionStackTest())
                {
                    this->CreateExecutionRequest(2);
                    continue;
                }
                Value res = this->executionStack.top().results[0] + this->executionStack.top().results[1];
                this->executionStack.pop();
                this->AddResult(res);
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Expr_Sub)
            {
                if(this->ExecutionStackTest())
                {
                    this->CreateExecutionRequest(2);
                    continue;
                }
                Value res = this->executionStack.top().results[0] - this->executionStack.top().results[1];
                this->executionStack.pop();
                this->AddResult(res);
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Expr_Mult)
            {
                if(this->ExecutionStackTest())
                {
                    this->CreateExecutionRequest(2);
                    continue;
                }
                Value res = this->executionStack.top().results[0] * this->executionStack.top().results[1];
                this->executionStack.pop();
                this->AddResult(res);
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Expr_Mod)
            {
                if(this->ExecutionStackTest())
                {
                    this->CreateExecutionRequest(2);
                    continue;
                }
                Value res = this->executionStack.top().results[0] % this->executionStack.top().results[1];
                this->executionStack.pop();
                this->AddResult(res);
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Expr_Div)
            {
                if(this->ExecutionStackTest())
                {
                    this->CreateExecutionRequest(2);
                    continue;
                }
                Value res = this->executionStack.top().results[0] / this->executionStack.top().results[1];
                this->executionStack.pop();
                this->AddResult(res);
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Expr_EqTest)
            {
                if(this->ExecutionStackTest())
                {
                    this->CreateExecutionRequest(2);
                    continue;
                }
                Value res = this->executionStack.top().results[0] == this->executionStack.top().results[1];
                this->executionStack.pop();
                this->AddResult(res);
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Expr_NeqTest)
            {
                if(this->ExecutionStackTest())
                {
                    this->CreateExecutionRequest(2);
                    continue;
                }
                Value res = this->executionStack.top().results[0] != this->executionStack.top().results[1];
                this->executionStack.pop();
                this->AddResult(res);
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Expr_GeqTest)
            {
                if(this->ExecutionStackTest())
                {
                    this->CreateExecutionRequest(2);
                    continue;
                }
                Value res = this->executionStack.top().results[0] >= this->executionStack.top().results[1];
                this->executionStack.pop();
                this->AddResult(res);
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Expr_LeqTest)
            {
                if(this->ExecutionStackTest())
                {
                    this->CreateExecutionRequest(2);
                    continue;
                }
                Value res = this->executionStack.top().results[0] <= this->executionStack.top().results[1];
                this->executionStack.pop();
                this->AddResult(res);
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Expr_GtTest)
            {
                if(this->ExecutionStackTest())
                {
                    this->CreateExecutionRequest(2);
                    continue;
                }
                Value res = this->executionStack.top().results[0] > this->executionStack.top().results[1];
                this->executionStack.pop();
                this->AddResult(res);
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Expr_LtTest)
            {
                if(this->ExecutionStackTest())
                {
                    this->CreateExecutionRequest(2);
                    continue;
                }
                Value res = this->executionStack.top().results[0] < this->executionStack.top().results[1];
                this->executionStack.pop();
                this->AddResult(res);
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Expr_FuncCall)
            {
                Node* thisNode = rootNode->subNodes[idx];
                if(returning)
                {
                    returning = false;
                    this->nodeFrameStack.top().idx++;
                    this->executionStack.pop();
                    this->AddResult(lastReturn);
                    continue;
                }

                if(this->ExecutionStackTest())
                {
                    returning = false;
                    this->CreateExecutionRequest(thisNode->subNodes.size());
                    continue;
                }

                FunctionRef FR = std::get<FunctionRef>(thisNode->data);
                Function* calledFunction = this->stateMgr->G_GetFunction(FR.name);
                
                if(calledFunction->signature.parameters.size() != this->executionStack.top().filled)
                {
                    throw std::runtime_error("Invalid Arg count");
                }

                std::int64_t SID = this->stateMgr->CreateScope(FR);
                this->scopeIdStack.push(SID);
                this->nodeFrameStack.push(NodeFrame{
                    calledFunction->scopeEAST,
                    0,
                    -1ULL,
                    Value(),
                    true,
                    false
                });

                for(size_t i = 0; i < this->executionStack.top().results.size(); i++)
                {
                    Object* obj = this->stateMgr->CreateVariable(SID, calledFunction->signature.parameters[i]);
                    obj->val = new Value(this->executionStack.top().results[i]);
                }

                this->executionStack.pop();
                lastReturn = Value();
                continue;
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Stmt_Declaration)
            {
                if(this->ExecutionStackTest())
                {
                    this->CreateExecutionRequest(1);
                    continue;
                }
                VariableSignature VS = std::get<VariableSignature>(rootNode->subNodes[idx]->identifier);
                Object* obj = this->stateMgr->CreateVariable(this->scopeIdStack.top(), VS);
                obj->val = new Value(this->executionStack.top().results[0]);
                this->executionStack.pop();
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Stmt_Assignment)
            {
                if(this->ExecutionStackTest())
                {
                    this->CreateExecutionRequest(1);
                    continue;
                }
                ObjectRef OR = std::get<ObjectRef>(rootNode->subNodes[idx]->identifier);
                Object* obj = this->stateMgr->GetVariable(this->scopeIdStack.top(), OR);
                delete obj->val;
                obj->val = new Value(this->executionStack.top().results[0]);
                this->executionStack.pop();
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Stmt_If)
            {
                if(this->ExecutionStackTest())
                {
                    this->CreateExecutionRequest(1);
                    continue;
                }
                if(std::get<bool>(this->executionStack.top().results[0].data))
                {
                    this->nodeFrameStack.top().idx++;
                    this->nodeFrameStack.push(NodeFrame{
                        rootNode->subNodes[idx],
                        1,
                        -1ULL,
                        Value(),
                        false,
                        true
                    });
                    this->executionStack.pop();
                    this->stateMgr->EnterSubscope(this->scopeIdStack.top());
                    continue;
                }
                this->executionStack.pop();
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Stmt_Unless)
            {
                if(this->ExecutionStackTest())
                {
                    this->CreateExecutionRequest(1);
                    continue;
                }
                if(!std::get<bool>(this->executionStack.top().results[0].data))
                {
                    this->nodeFrameStack.top().idx++;
                    this->nodeFrameStack.push(NodeFrame{
                        rootNode->subNodes[idx],
                        1,
                        -1ULL,
                        Value(),
                        false,
                        true
                    });
                    this->executionStack.pop();
                    this->stateMgr->EnterSubscope(this->scopeIdStack.top());
                    continue;
                }
                this->executionStack.pop();
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Stmt_While)
            {
                if(this->ExecutionStackTest())
                {
                    this->CreateExecutionRequest(1);
                    continue;
                }
                if(std::get<bool>(this->executionStack.top().results[0].data))
                {
                    this->nodeFrameStack.push(NodeFrame{
                        rootNode->subNodes[idx],
                        1,
                        -1ULL,
                        Value(),
                        false,
                        true
                    });
                    this->executionStack.pop();
                    this->stateMgr->EnterSubscope(this->scopeIdStack.top());
                    continue;
                }
                this->executionStack.pop();
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Stmt_Until)
            {
                if(this->ExecutionStackTest())
                {
                    this->CreateExecutionRequest(1);
                    continue;
                }
                if(!std::get<bool>(this->executionStack.top().results[0].data))
                {
                    this->nodeFrameStack.push(NodeFrame{
                        rootNode->subNodes[idx],
                        1,
                        -1ULL,
                        Value(),
                        false,
                        true
                    });
                    this->executionStack.pop();
                    this->stateMgr->EnterSubscope(this->scopeIdStack.top());
                    continue;
                }
                this->executionStack.pop();
            }
            else if(rootNode->subNodes[idx]->type == NodeType::Stmt_Return)
            {
                if(this->ExecutionStackTest())
                {
                    this->CreateExecutionRequest(1);
                    continue;
                }
                lastReturn = Value(this->executionStack.top().results[0]);
                this->ForcePopStacks();
                returning = true;
                continue;
            }
            else
            {
                std::cout<<"Unknown Type: "<< rootNode->subNodes[idx]->type<<std::endl;
                throw std::runtime_error("Interpreter Exception");
            }
            this->nodeFrameStack.top().idx++;
        }
        return Value(lastReturn);
    }
}