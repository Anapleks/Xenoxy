#include "Xenoxy/Xenoxy.hpp"
#include <iostream>
#include <vector>

#include "Xenoxy/impl/Lexer.hpp"
#include "Xenoxy/impl/Parser.hpp"
#include "Xenoxy/impl/StateManager.hpp"
#include "Xenoxy/impl/TypeManager.hpp"
#include "Xenoxy/impl/Types.hpp"
#include "Xenoxy/impl/Interpreter.hpp"

namespace Xenoxy {
    class Runtime::RuntimeImpl
    {
        private:
        StateManager* stateMgr;
        TypeManager* typeMgr;
        Interpreter* interpreter;
        public:
        static constexpr std::string_view Version = "0.1.0";

        void Load(std::string code)
        {
            std::vector<Token> tokens = Lexer::Tokenize(code);
            Parser parser(tokens);
            Node* programRoot = parser.Parse();

            this->stateMgr = new StateManager();
            for(auto& nde : programRoot->subNodes)
            {
                if(nde->type != NodeType::Stmt_Function) { continue; }
                this->stateMgr->G_CreateFunction(nde, std::get<FunctionSignature>(nde->identifier));
            }

            this->typeMgr = new TypeManager();

            TypeDef intTypeDef("int", TypeType::Primitive, 8);
            TypeDef floatTypeDef("float", TypeType::Primitive, 8);
            TypeDef boolTypeDef("bool", TypeType::Primitive, 1);

            this->typeMgr->AddTypeDef(intTypeDef);
            this->typeMgr->AddTypeDef(floatTypeDef);
            this->typeMgr->AddTypeDef(boolTypeDef);

            this->interpreter = new Interpreter();
        }

        void LoadFile(std::string path)
        {
        // std::cout<<path<<std::endl;
        // std::ifstream file;
        }

        ObjectInst Run(std::string function)
        {
            this->interpreter->stateMgr = this->stateMgr;
            this->interpreter->typeMgr = this->typeMgr;
            return this->interpreter->Call(function);
            // std::cout<<*this->stateMgr<<std::endl;
        }

        ~RuntimeImpl()
        {
            delete this->stateMgr;
            delete this->typeMgr;
            delete this->interpreter;
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

    ObjectInst Runtime::Run(std::string function)
    {
        return this->pImpl->Run(function);
    }
} // namespace Xenoxy

