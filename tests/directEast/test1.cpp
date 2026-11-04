#include <Xenoxy/Xenoxy.hpp>
#include <Xenoxy/impl/EAST.hpp>
#include <Xenoxy/impl/Types.hpp>
#include <Xenoxy/impl/StateManager.hpp>
#include <Xenoxy/impl/Interpreter.hpp>
#include <iostream>

int main() {
//     Xenoxy::Runtime runtime;
//     runtime.load(R"(fn main() -> int {
//     var a: int = 32 * 64;
//     var b: int = 13 * a + 5;
//     var c: int = 5 * (a + b) - a;
// })"
//     );
//     runtime.run();
/*
    fn main() -> int {
        var a: int = 1;
        var b: int = a + 3;
        return b;
    }
*/
    Xenoxy::Node* root = new Xenoxy::Node();
    root->type = Xenoxy::NodeType::Stmt_Function;
    root->identifier = Xenoxy::FunctionSignature{"main", "int", {false, false, false, false}, {}};
    root->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[0]->type = Xenoxy::NodeType::Stmt_Declaration;
    root->subNodes[0]->identifier =Xenoxy::VariableSignature{"a", "int", {false, false, false, false}};
    root->subNodes[0]->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[0]->subNodes[0]->type = Xenoxy::NodeType::Expr_IntLiteral;
    root->subNodes[0]->subNodes[0]->data = 1;
    root->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[1]->type = Xenoxy::NodeType::Stmt_Declaration;
    root->subNodes[1]->identifier =Xenoxy::VariableSignature{"b", "int", {false, false, false, false}};
    root->subNodes[1]->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[1]->subNodes[0]->type = Xenoxy::NodeType::Expr_Add;
    root->subNodes[1]->subNodes[0]->data = std::monostate();
    root->subNodes[1]->subNodes[0]->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[1]->subNodes[0]->subNodes[0]->type = Xenoxy::NodeType::Expr_ObjAccess;
    root->subNodes[1]->subNodes[0]->subNodes[0]->data = Xenoxy::ObjectRef{"a"};
    root->subNodes[1]->subNodes[0]->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[1]->subNodes[0]->subNodes[1]->type = Xenoxy::NodeType::Expr_IntLiteral;
    root->subNodes[1]->subNodes[0]->subNodes[1]->data = 3;
    root->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[2]->type = Xenoxy::NodeType::Stmt_Return;
    root->subNodes[2]->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[2]->subNodes[0]->type = Xenoxy::NodeType::Expr_ObjAccess;
    root->subNodes[2]->subNodes[0]->data = Xenoxy::ObjectRef{"b"};
    Xenoxy::StateManager* mgr = new Xenoxy::StateManager();
    
    Xenoxy::FunctionSignature mainSig;
    mainSig.flags = {false, false, false, false};
    mainSig.name = "main";
    mainSig.parameters.clear();
    mainSig.type = "int";

    mgr->G_CreateFunction(root, mainSig);
    root = nullptr;
    
    Xenoxy::Interpreter* interpreter = new Xenoxy::Interpreter();
    interpreter->stateMgr = mgr;
    Xenoxy::Value retVal = interpreter->Call("main");
    std::cout<<retVal<<std::endl;
    std::cout<<*mgr<<std::endl;
    delete mgr;
    delete interpreter;
}