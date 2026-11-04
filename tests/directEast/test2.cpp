#include <Xenoxy/Xenoxy.hpp>
#include <Xenoxy/impl/StateManager.hpp>
#include <Xenoxy/impl/EAST.hpp>
#include <Xenoxy/impl/Interpreter.hpp>

int main() {
/*
    fn main() -> int {
        var a: int = 3;
        if(a==3) {
            a = a + 3;
            var b: int = a + 3;
        }
        return a;
    }
*/
    // fn main() -> int {...}
    Xenoxy::Node* root = new Xenoxy::Node;
    root->type = Xenoxy::NodeType::Stmt_Function;
    root->identifier = Xenoxy::FunctionSignature{"main", "int", {false, false, false, false}, {}};
    // var a: int = 3;
    root->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[0]->type = Xenoxy::NodeType::Stmt_Declaration;
    root->subNodes[0]->identifier = Xenoxy::VariableSignature{"a", "int", {false, false, false, false}};
    root->subNodes[0]->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[0]->subNodes[0]->type = Xenoxy::NodeType::Expr_IntLiteral;
    root->subNodes[0]->subNodes[0]->data = 3;
    // // if(a==3) { .. }
    root->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[1]->type = Xenoxy::NodeType::Stmt_If;
    root->subNodes[1]->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[1]->subNodes[0]->type = Xenoxy::NodeType::Expr_EqTest;
    root->subNodes[1]->subNodes[0]->data = std::monostate();
    root->subNodes[1]->subNodes[0]->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[1]->subNodes[0]->subNodes[0]->type = Xenoxy::NodeType::Expr_IntLiteral;
    root->subNodes[1]->subNodes[0]->subNodes[0]->data = 3;
    root->subNodes[1]->subNodes[0]->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[1]->subNodes[0]->subNodes[1]->type = Xenoxy::NodeType::Expr_ObjAccess;
    root->subNodes[1]->subNodes[0]->subNodes[1]->data = Xenoxy::ObjectRef{"a"};
    // a = a + 3;
    root->subNodes[1]->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[1]->subNodes[1]->type = Xenoxy::NodeType::Stmt_Assignment;
    root->subNodes[1]->subNodes[1]->identifier = Xenoxy::ObjectRef{"a"};
    root->subNodes[1]->subNodes[1]->subNodes.push_back(new Xenoxy::Node);
    root->subNodes[1]->subNodes[1]->subNodes[0]->type = Xenoxy::NodeType::Expr_Add;
    root->subNodes[1]->subNodes[1]->subNodes[0]->data = std::monostate();
    root->subNodes[1]->subNodes[1]->subNodes[0]->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[1]->subNodes[1]->subNodes[0]->subNodes[0]->type = Xenoxy::NodeType::Expr_IntLiteral;
    root->subNodes[1]->subNodes[1]->subNodes[0]->subNodes[0]->data = 3;
    root->subNodes[1]->subNodes[1]->subNodes[0]->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[1]->subNodes[1]->subNodes[0]->subNodes[1]->type = Xenoxy::NodeType::Expr_ObjAccess;
    root->subNodes[1]->subNodes[1]->subNodes[0]->subNodes[1]->data = Xenoxy::ObjectRef{"a"};

    root->subNodes[1]->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[1]->subNodes[2]->type = Xenoxy::NodeType::Stmt_Declaration;
    root->subNodes[1]->subNodes[2]->identifier = Xenoxy::VariableSignature{"b", "int", {false, false, false, false}};
    root->subNodes[1]->subNodes[2]->subNodes.push_back(new Xenoxy::Node);
    root->subNodes[1]->subNodes[2]->subNodes[0]->type = Xenoxy::NodeType::Expr_Add;
    root->subNodes[1]->subNodes[2]->subNodes[0]->data = std::monostate();
    root->subNodes[1]->subNodes[2]->subNodes[0]->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[1]->subNodes[2]->subNodes[0]->subNodes[0]->type = Xenoxy::NodeType::Expr_IntLiteral;
    root->subNodes[1]->subNodes[2]->subNodes[0]->subNodes[0]->data = 3;
    root->subNodes[1]->subNodes[2]->subNodes[0]->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[1]->subNodes[2]->subNodes[0]->subNodes[1]->type = Xenoxy::NodeType::Expr_ObjAccess;
    root->subNodes[1]->subNodes[2]->subNodes[0]->subNodes[1]->data = Xenoxy::ObjectRef{"a"};
    // return a
    root->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[2]->type = Xenoxy::NodeType::Stmt_Return;
    root->subNodes[2]->subNodes.push_back(new Xenoxy::Node());
    root->subNodes[2]->subNodes[0]->type = Xenoxy::NodeType::Expr_ObjAccess;
    root->subNodes[2]->subNodes[0]->data = Xenoxy::ObjectRef{"a"};
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