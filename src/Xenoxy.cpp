#include "Xenoxy/Xenoxy.hpp"
#include <iostream>
#include <vector>

#include "Xenoxy/impl/Lexer.hpp"
#include "Xenoxy/impl/Parser.hpp"
#include "Xenoxy/impl/StateManager.hpp"
#include "Xenoxy/impl/Interpreter.hpp"

namespace Xenoxy {
    class Runtime::RuntimeImpl
    {
        private:
        StateManager* stateMgr;
        Interpreter* interpreter;
        public:
        static constexpr std::string_view Version = "0.1.0";

        void Load(std::string code)
        {
            std::vector<Token> tokens = Lexer::Tokenize(code);
            // for(size_t i = 0; i < tokens.size(); i++) {
            //     std::cout<<tokens[i]<<std::endl;
            // }
            Parser parser(tokens);
            Node* programRoot = parser.Parse();

            stateMgr = new StateManager();
            for(auto& nde : programRoot->subNodes)
            {
                if(nde->type != NodeType::Stmt_Function) { continue; }
                stateMgr->G_CreateFunction(nde, std::get<FunctionSignature>(nde->identifier));
            }
            interpreter = new Interpreter();
        }

        void LoadFile(std::string path)
        {
        // std::cout<<path<<std::endl;
        // std::ifstream file;
        }

        void Run(std::string function)
        {
            interpreter->stateMgr = stateMgr;
            std::cout<<interpreter->Call(function)<<std::endl;
            std::cout<<*stateMgr<<std::endl;
        }

        ~RuntimeImpl()
        {
            delete stateMgr;
            delete interpreter;
        }
    };


    Runtime::Runtime()
    {
        this->pImpl = new RuntimeImpl();
    }

    Runtime::~Runtime()
    {
        delete this->pImpl;
    }

    void Runtime::Load(std::string code)
    {
        this->pImpl->Load(code);
    }

    void Runtime::LoadFile(std::string path)
    {
        this->pImpl->LoadFile(path);
    }

    void Runtime::Run(std::string function)
    {
        this->pImpl->Run(function);
    }
} // namespace Xenoxy

