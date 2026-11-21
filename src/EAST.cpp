#include "Xenoxy/impl/EAST.hpp"
#include <iostream>

namespace Xenoxy
{
    Node::~Node()
    {
        for(auto k : this->subNodes)
        {
            delete k;
        }
    }

    std::ostream& operator<<(std::ostream& os, const NodeType& type)
    {
        switch (type)
        {
        case NodeType::Stmt_Assignment:
            os<<"NodeType::Stmt_Assignment";
            break;
        case NodeType::Stmt_Declaration:
            os<<"NodeType::Stmt_Declaration";
            break;
        case NodeType::Stmt_Expression:
            os<<"NodeType::Stmt_Expression";
            break;
        case NodeType::Stmt_Function:
            os<<"NodeType::Stmt_Function";
            break;
        case NodeType::Stmt_If:
            os<<"NodeType::Stmt_If";
            break;
        case NodeType::Stmt_Unless:
            os<<"NodeType::Stmt_Unless";
            break;
        case NodeType::Stmt_While:
            os<<"NodeType::Stmt_While";
            break;
        case NodeType::Stmt_Until:
            os<<"NodeType::Stmt_Until";
            break;
        case NodeType::Stmt_Return:
            os<<"NodeType::Stmt_Return";
            break;
        case NodeType::Expr_Add:
            os<<"NodeType::Expr_Add";
            break;
        case NodeType::Expr_EqTest:
            os<<"NodeType::Expr_EqTest";
            break;
        case NodeType::Expr_NeqTest:
            os<<"NodeType::Expr_NeqTest";
            break;
        case NodeType::Expr_GeqTest:
            os<<"NodeType::Expr_GeqTest";
            break;
        case NodeType::Expr_LeqTest:
            os<<"NodeType::Expr_LeqTest";
            break;
        case NodeType::Expr_GtTest:
            os<<"NodeType::Expr_GtTest";
            break;
        case NodeType::Expr_LtTest:
            os<<"NodeType::Expr_LtTest";
            break;
        case NodeType::Expr_FloatLiteral:
            os<<"NodeType::Expr_FloatLiteral";
            break;
        case NodeType::Expr_FuncCall:
            os<<"NodeType::Expr_FuncCall";
            break;
        case NodeType::Expr_IntLiteral:
            os<<"NodeType::Expr_IntLiteral";
            break;
        case NodeType::Expr_Mult:
            os<<"NodeType::Expr_Mult";
            break;
        case NodeType::Expr_ObjAccess:
            os<<"NodeType::Expr_ObjAccess";
            break;
        case NodeType::Expr_Sub:
            os<<"NodeType::Expr_Sub";
            break;
        case NodeType::Expr_Assign:
            os<<"NodeType::Expr_Assign";
            break;
        case NodeType::Expr_Mod:
            os<<"NodeType::Expr_Mod";
            break;
        case NodeType::Expr_Div:
            os<<"NodeType::Expr_Div";
            break;
        case NodeType::Expr_Index:
            os<<"NodeType::Expr_Div";
            break;
        default:
            break;
        }
        return os;
    }
}
