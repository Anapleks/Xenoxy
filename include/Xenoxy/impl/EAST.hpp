#ifndef EAST_hpp
#define EAST_hpp
#include <vector>
#include <variant>
#include <cstdint>

#include "Xenoxy/impl/Types.hpp"
#include <string>

namespace Xenoxy {
    enum class NodeType {
        // Throw error
        None,
        // var identifier [=subNodes[0]];
        Stmt_Declaration,
        // identifier = subNodes[0];
        Stmt_Assignment,
        // subExpression;
        Stmt_Expression,
        // identifier { subNodes };
        Stmt_Function,
        // if (subNodes[0]) { subNodes[1::] };
        Stmt_If,
        // unless (subNodes[0]) { subNodes[1::] };
        Stmt_Unless,
        // while (subNodes[0]) { subNodes[1::] };
        Stmt_While,
        // until (subNodes[0]) { subNodes[1::] };
        Stmt_Until,
        // return [subNodes[0]]
        Stmt_Return,
        // data=monostate + subNodes [0] - lhs, [1] - rhs
        Expr_Add,
        // data=monostate + subNodes [0] - lhs, [1] - rhs
        Expr_Sub,
        // data=monostate + subNodes [0] - lhs, [1] - rhs
        Expr_Mult,
        // data=monostate + subNodes [0] - lhs, [1] - rhs
        Expr_Mod,
        // data=monostate + subNodes [0] - lhs, [1] - rhs
        Expr_Div,
        // data=FunctionRef + subNodes [0] - arg0, [1] - arg1, ..., [n] - argn
        Expr_FuncCall,
        // data=ObjAccess
        Expr_ObjAccess,
        // data=IntLiteral
        Expr_IntLiteral,
        // data=FloatLiteral
        Expr_FloatLiteral,
        // monostate + subNodes [0] - lhs, [1] - rhs
        Expr_EqTest,
        // monostate + subNodes [0] - lhs, [1] - rhs
        Expr_NeqTest,
        // monostate + subNodes [0] - lhs, [1] - rhs
        Expr_GeqTest,
        // monostate + subNodes [0] - lhs, [1] - rhs
        Expr_LeqTest,
        // monostate + subNodes [0] - lhs, [1] - rhs
        Expr_GtTest,
        // monostate + subNodes [0] - lhs, [1] - rhs
        Expr_LtTest,
    };

    class Node {
        public:
        // type of this Node
        NodeType type = NodeType::None;
        // contains all subNodes as specified in NodeType
        std::vector<Node*> subNodes = {};
        // expected to be used in a Node with type like Expr_*
        std::variant<
            std::monostate,
            ObjectRef,
            FunctionRef,
            std::int64_t,
            std::string,
            double> data = std::monostate();
        
        // expected to be used in a Node with type like Stmt_*
        std::variant<
            std::monostate,
            VariableSignature,
            ObjectRef,
            FunctionSignature> identifier = std::monostate();

        ~Node();
    };

    std::ostream& operator<<(std::ostream& os, const NodeType& type);
}

#endif