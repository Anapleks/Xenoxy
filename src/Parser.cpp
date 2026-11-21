#include "Xenoxy/impl/Parser.hpp"
#include <iostream>

namespace Xenoxy {
    Token Parser::Peek(std::int64_t offset)
    {
        if(offset + this->m_tokenIdx >= this->m_tokens.size())
        {
            return Token{TokenType::Eof, std::monostate()};
        }

        return this->m_tokens[this->m_tokenIdx + offset];
    }

    void Parser::Consume(std::int64_t count)
    {
        this->m_tokenIdx+=count;
    }

    void Parser::ExpectAndConsume(TokenType type)
    {
        if(this->m_tokens[this->m_tokenIdx].type != type)
        {
            std::cout<<this->m_tokens[this->m_tokenIdx]<<std::endl;
            throw ParserError("Expected different symbol");
        }
        this->m_tokenIdx++;
    }

    bool Parser::HasTokens()
    {
        return this->m_tokenIdx < this->m_tokens.size();
    }

    Parser::ParserRange Parser::findRange(TokenType eol)
    {
        ParserRange pr{this->m_tokenIdx};

        while(this->m_tokens[pr.endIdx].type != eol && this->m_tokens[pr.endIdx].type != TokenType::Eof)
        {
            pr.endIdx++;
        }
        return pr;
    }

    std::vector<Parser::ParserRange> Parser::findListRanges(TokenType open, TokenType close, TokenType sep)
    {
        std::vector<ParserRange> res;

        std::int64_t currIdx=this->m_tokenIdx;
        std::int64_t openParen = 0;
        while(true)
        {
            if(this->m_tokens[currIdx].type == TokenType::Eof)
            {
                throw ParserError("List not closed");
            }
            if(this->m_tokens[currIdx].type == sep)
            {
                res.push_back(ParserRange{currIdx});
            }
            if(this->m_tokens[currIdx].type == close)
            {
                openParen--;
                if(openParen == 0)
                {
                    if(this->m_tokenIdx+1 != currIdx)
                    {
                        res.push_back(ParserRange{currIdx});
                    }
                    break;
                }
            }
            if(this->m_tokens[currIdx].type == open)
            {
                openParen++;
            }
            currIdx++;
        }
        return res;
    }

    VariableSignature Parser::ParseType()
    {
        VariableSignature VS;
        Token type = this->Peek(0);
        while(type.type == TokenType::Identifier)
        {
            if(std::get<std::string>(type.data) == "const")
            {
                if(VS.flags.Const) { throw ParserError("Double Const flag"); }
                VS.flags.Const = true;
            }
            else if(std::get<std::string>(type.data) == "imut")
            {
                if(VS.flags.Imut) { throw ParserError("Double Imut flag"); }
                VS.flags.Imut = true;
            }
            else if(std::get<std::string>(type.data) == "untracked")
            {
                if(VS.flags.Untracked) { throw ParserError("Double Untracked flag"); }
                VS.flags.Untracked = true;
            }
            else if(std::get<std::string>(type.data) == "static")
            {
                if(VS.flags.Static) { throw ParserError("Double Static flag"); }
                VS.flags.Static = true;
            }
            else
            {
                VS.type = std::get<std::string>(type.data);
                this->Consume();
                break;
            }
            this->Consume();
            type = this->Peek(0);
        }
        return VS;
    }

    VariableSignature Parser::ParseVariableSignature()
    {
        Token identifier = this->Peek(0);
        this->ExpectAndConsume(TokenType::Identifier);
        this->ExpectAndConsume(TokenType::Colon);
        VariableSignature VS = this->ParseType();
        VS.name = std::get<std::string>(identifier.data);
        return VS;
    }

    FunctionSignature Parser::ParseFunctionSignature()
    {
        this->ExpectAndConsume(TokenType::Fn);
        Token identifier = this->Peek();
        this->ExpectAndConsume(TokenType::Identifier);
        this->ExpectAndConsume(TokenType::LParen);
        std::vector<VariableSignature> argSignatures;
        while(this->Peek().type != TokenType::RParen && this->HasTokens())
        {
            argSignatures.push_back(this->ParseVariableSignature());
            if(this->Peek().type != TokenType::Comma && this->Peek().type != TokenType::RParen)
            {
                throw ParserError("Expected Comma or RParen");
            }
            if(this->Peek().type == TokenType::Comma)
            {
                this->Consume();
                if(this->Peek().type == TokenType::RParen)
                {
                    throw ParserError("Expected next argument signature");
                }
            }
        }
        this->ExpectAndConsume(TokenType::RParen);
        this->ExpectAndConsume(TokenType::Arrow);
        VariableSignature type = this->ParseType();
        FunctionSignature FS;
        FS.name = std::get<std::string>(identifier.data);
        FS.parameters = argSignatures;
        FS.type = type.type;
        FS.flags = type.flags;
        return FS;
    }

    bool Parser::IsPostfix(const TokenType& c)
    {
        switch (c)
        {
            case TokenType::LParen: return true;
            case TokenType::LSquare: return true;
            default: return false;
        }
    }

    bool Parser::IsPrefix(const TokenType& c)
    {
        switch (c)
        {
            default: return false;
        }
    }

    // if right == true -> right binding power, if right == false -> left binding power
    float Parser::ExpressionPrecedence(const TokenType& c, bool right)
    {
        switch(c)
        {
            case TokenType::Equals: return right ? 0.0 : 0.1;
            case TokenType::Geq: return right ? 1.1 : 1.0;
            case TokenType::Leq: return right ? 1.1 : 1.0;
            case TokenType::Gt: return right ? 1.1 : 1.0;
            case TokenType::Lt: return right ? 1.1 : 1.0;
            case TokenType::Eq: return right ? 1.1 : 1.0;
            case TokenType::Neq: return right ? 1.1 : 1.0;
            case TokenType::Plus: return right ? 2.1 : 2.0;
            case TokenType::Minus: return right ? 2.1 : 2.0;
            case TokenType::Asterisk: return right ? 3.1 : 3.0;
            case TokenType::Mod: return right ? 3.1 : 3.0;
            case TokenType::Div: return right ? 3.1 : 3.0;
            default:
                std::cout<<c<<std::endl;
                throw ParserError("Unexpected op type");
        }
    }

    Node* Parser::GenerateLHSNode(ParserRange pr, Token tokenL)
    {
        Node* lhs = new Node();
        switch (tokenL.type)
        {
        case TokenType::Identifier:
            lhs->data = ObjectRef{std::get<std::string>(tokenL.data)};
            lhs->type = NodeType::Expr_ObjAccess;
            return lhs;
        
        case TokenType::IntLiteral:
            lhs->data = std::get<std::int64_t>(tokenL.data);
            lhs->type = NodeType::Expr_IntLiteral;
            return lhs;

        case TokenType::FloatLiteral:
            lhs->data = std::get<double>(tokenL.data);
            lhs->type = NodeType::Expr_FloatLiteral;
            return lhs;
        
        case TokenType::LParen:
            delete lhs;
            this->Consume();
            lhs = this->ParseExpression(pr);
        default:
            delete lhs;
            std::cout<<"tokenL: "<<tokenL<<std::endl;
            throw ParserError("Invalid tokenL type");
        }
    }

    Node* Parser::GenerateParentNode(Node* lhs, Token op, Node* rhs)
    {
        Node* parent = new Node();
        parent->subNodes.push_back(lhs);
        parent->subNodes.push_back(rhs);

        switch(op.type)
        {
            case TokenType::Plus:
                parent->type = NodeType::Expr_Add;
                break;
            case TokenType::Minus:
                parent->type = NodeType::Expr_Sub;
                break;
            case TokenType::Asterisk:
                parent->type = NodeType::Expr_Mult;
                break;
            case TokenType::Div:
                parent->type = NodeType::Expr_Div;
                break;
            case TokenType::Mod:
                parent->type = NodeType::Expr_Mod;
                break;
            case TokenType::Eq:
                parent->type = NodeType::Expr_EqTest;
                break;
            case TokenType::Neq:
                parent->type = NodeType::Expr_NeqTest;
                break;
            case TokenType::Gt:
                parent->type = NodeType::Expr_GtTest;
                break;
            case TokenType::Geq:
                parent->type = NodeType::Expr_GeqTest;
                break;
            case TokenType::Lt:
                parent->type = NodeType::Expr_LtTest;
                break;
            case TokenType::Leq:
                parent->type = NodeType::Expr_LeqTest;
                break;
            case TokenType::Equals:
                parent->type = NodeType::Expr_Assign;
                break;
            default:
                std::cout<<op.type<<std::endl;
                throw ParserError("Unexpected op TokenType");
        }
        return parent;
    }

    Node* Parser::ParsePrefixExpression(ParserRange pr, Node* lhs)
    {
        
    }

    Node* Parser::ParsePostfixExpression(ParserRange pr, Node* lhs)
    {
        if(this->Peek().type == TokenType::LParen)
        {
            std::vector<ParserRange> ranges = this->findListRanges(TokenType::LParen, TokenType::RParen);
            lhs->type = NodeType::Expr_FuncCall;
            this->ExpectAndConsume(TokenType::LParen);
            for(size_t i=0; i<ranges.size(); i++)
            {
                if(ranges[i].endIdx - this->m_tokenIdx <= 0)
                {
                    throw std::runtime_error("Invalid size of argument");
                }

                lhs->subNodes.push_back(this->ParseExpression(ranges[i]));
                if(this->Peek().type == TokenType::Comma)
                {
                    this->ExpectAndConsume(TokenType::Comma);
                }
            }
            this->ExpectAndConsume(TokenType::RParen);
            ObjectRef OR = std::get<ObjectRef>(lhs->data);
            FunctionRef FR{OR.name};
            lhs->data = FR;
            return lhs;
        }
    }

    Node* Parser::ParseExpression(ParserRange pr, float minBp)
    {
        Token tokenL = this->Peek();
        Node* lhs = this->GenerateLHSNode(pr, tokenL);
        // if(this->IsPrefix(tokenL.type)) { this->ParsePrefixExpression(pr, lhs); }
        this->Consume();
        while(true)
        {
            if(!this->HasTokens() || this->m_tokenIdx >= pr.endIdx)
            {
                return lhs;
            }
            if(this->Peek().type == TokenType::RParen)
            {
                return lhs;
            }

            Token op = this->Peek();
            if(this->IsPostfix(op.type))
            {
                this->ParsePostfixExpression(pr, lhs);
                continue;
            }
            
            float leftBp = this->ExpressionPrecedence(op.type, false);
            float rightBp = this->ExpressionPrecedence(op.type, true);

            if(leftBp < minBp)
            {
                return lhs;
            }

            this->Consume();

            Node* rhs = this->ParseExpression(pr, rightBp);

            Node* parent = this->GenerateParentNode(lhs, op, rhs);

            lhs = parent;
        }
    }

    Node* Parser::Parse()
    {
        Node* programRoot = new Node();
        this->m_parent.push(programRoot);
        while(this->HasTokens())
        {
            Token next = this->Peek();
            if(next.type == TokenType::Fn)
            {
                FunctionSignature FS = this->ParseFunctionSignature();
                Node* funcNode = new Node();
                funcNode->type = NodeType::Stmt_Function;
                funcNode->identifier = FS;
                this->m_parent.top()->subNodes.push_back(funcNode);
                this->m_parent.push(funcNode);
                this->ExpectAndConsume(TokenType::LBrace);
                continue;
            }
            else if(next.type == TokenType::Var)
            {
                this->ExpectAndConsume(TokenType::Var);
                VariableSignature VS = this->ParseVariableSignature();
                Node* declNode = new Node();
                declNode->type = NodeType::Stmt_Declaration;
                declNode->identifier = VS;
                if(this->Peek().type == TokenType::Semi)
                {
                    this->Consume();
                    this->m_parent.top()->subNodes.push_back(declNode);
                    continue;
                }
                if(this->Peek().type != TokenType::Equals)
                {
                    throw ParserError("Expected Semi or Equals");
                }
                this->Consume();

                ParserRange PR = this->findRange();
                Node* exprRoot = this->ParseExpression(PR);
                declNode->subNodes.push_back(exprRoot);
                this->m_parent.top()->subNodes.push_back(declNode);
            }
            else if(next.type == TokenType::Return)
            {
                this->ExpectAndConsume(TokenType::Return);
                ParserRange pr = this->findRange();
                Node* expr = this->ParseExpression(pr);
                Node* returnNode = new Node();
                returnNode->type = NodeType::Stmt_Return;
                returnNode->subNodes.push_back(expr);
                this->m_parent.top()->subNodes.push_back(returnNode);
            }
            else if(next.type == TokenType::If)
            {
                this->ExpectAndConsume(TokenType::If);
                this->ExpectAndConsume(TokenType::LParen);
                ParserRange pr = this->findRange(TokenType::RParen);
                Node* expr = this->ParseExpression(pr);
                Node* ifNode = new Node();
                ifNode->type = NodeType::Stmt_If;
                ifNode->subNodes.push_back(expr);
                this->m_parent.top()->subNodes.push_back(ifNode);
                this->m_parent.push(ifNode);
            }
            else if(next.type == TokenType::Unless)
            {
                this->ExpectAndConsume(TokenType::Unless);
                this->ExpectAndConsume(TokenType::LParen);
                ParserRange pr = this->findRange(TokenType::RParen);
                Node* expr = this->ParseExpression(pr);
                Node* ifNode = new Node();
                ifNode->type = NodeType::Stmt_Unless;
                ifNode->subNodes.push_back(expr);
                this->m_parent.top()->subNodes.push_back(ifNode);
                this->m_parent.push(ifNode);
            }
            else if(next.type == TokenType::While)
            {
                this->ExpectAndConsume(TokenType::While);
                this->ExpectAndConsume(TokenType::LParen);
                ParserRange pr = this->findRange(TokenType::RParen);
                Node* expr = this->ParseExpression(pr);
                Node* ifNode = new Node();
                ifNode->type = NodeType::Stmt_While;
                ifNode->subNodes.push_back(expr);
                this->m_parent.top()->subNodes.push_back(ifNode);
                this->m_parent.push(ifNode);
            }
            else if(next.type == TokenType::Until)
            {
                this->ExpectAndConsume(TokenType::Until);
                this->ExpectAndConsume(TokenType::LParen);
                ParserRange pr = this->findRange(TokenType::RParen);
                Node* expr = this->ParseExpression(pr);
                Node* ifNode = new Node();
                ifNode->type = NodeType::Stmt_Until;
                ifNode->subNodes.push_back(expr);
                this->m_parent.top()->subNodes.push_back(ifNode);
                this->m_parent.push(ifNode);
            }
            else if(next.type == TokenType::RBrace)
            {
                if(this->m_parent.size() == 1) 
                {
                    throw ParserError("Too many closing Braces '}'");
                }
                this->m_parent.pop();
            }
            else if(next.type == TokenType::Identifier)
            {
                ParserRange pr = this->findRange();
                Node* expr = this->ParseExpression(pr);
                this->m_parent.top()->subNodes.push_back(expr);
            }
            
            this->Consume();
        }
        return programRoot;
    }
}
