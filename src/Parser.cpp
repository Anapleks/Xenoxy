#include "Xenoxy/impl/Parser.hpp"
#include <iostream>

namespace Xenoxy {
    Token& Parser::Peek(std::int64_t offset)
    {
        if(offset + this->m_tokenIdx >= this->m_tokens.size())
        {
            throw ParserError("Unable to peek");
        }

        return this->m_tokens[this->m_tokenIdx + offset];
    }

    Token& Parser::ExpectAndPeek(TokenType type, std::int64_t offset)
    {
        if(offset + this->m_tokenIdx >= this->m_tokens.size())
        {
            throw ParserError("Unable to peek");
        }
        if(this->m_tokens[this->m_tokenIdx + offset].type != type)
        {
            throw ParserError("Expected different symbol");
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
            throw ParserError("Expected different symbol");
        }
        this->m_tokenIdx++;
    }

    VariableSignature Parser::ParseVariableSignature()
    {
        Token& identifier = this->ExpectAndPeek(TokenType::Identifier, 0);
        this->Consume();
        this->ExpectAndConsume(TokenType::Colon);
        VariableSignature VS = this->ParseType();
        VS.name = std::get<std::string>(identifier.data);
        return VS;
    }

    VariableSignature Parser::ParseType()
    {
        VariableSignature VS;
        Token& type = this->Peek(0);
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

    bool IsPostfix(const TokenType& c)
    {
        switch (c)
        {
            case TokenType::LParen: return true;
            default: return false;
        }
    }

    // if right == true -> right binding power, if right == false -> left binding power
    float ExpressionPrecedence(const TokenType& c, bool right)
    {
        switch(c)
        {
            case TokenType::Geq: return right ? 0.1 : 0.0;
            case TokenType::Leq: return right ? 0.1 : 0.0;
            case TokenType::Gt: return right ? 0.1 : 0.0;
            case TokenType::Lt: return right ? 0.1 : 0.0;
            case TokenType::Eq: return right ? 0.1 : 0.0;
            case TokenType::Neq: return right ? 0.1 : 0.0;
            case TokenType::Plus: return right ? 1.1 : 1.0;
            case TokenType::Minus: return right ? 1.1 : 1.0;
            case TokenType::Asterisk: return right ? 2.1 : 2.0;
            case TokenType::Mod: return right ? 2.1 : 2.0;
            case TokenType::Div: return right ? 2.1 : 2.0;
            default:
                throw ParserError("Unexpected op type");
        }
    }

    Node* Parser::ParsePostfixExpression(std::vector<Token>& tokens, Node* lhs)
    {
        Token op = Token(tokens[0]);

        // parse func call
        if(op.type == TokenType::LParen)
        {
            size_t parenCount=1;
            tokens.erase(tokens.begin());

            Node* funcCall = lhs;
            funcCall->type = NodeType::Expr_FuncCall;
            FunctionRef FR{std::get<ObjectRef>(lhs->data).name};
            funcCall->data = FR;

            std::vector<Token> argTokens;

            while(true)
            {
                if(tokens[0].type == TokenType::Eof) { throw ParserError("No closing parenthesis"); }
                else if(parenCount == 0) { throw ParserError("Too Many closing parenthesis"); }
                else if(parenCount == 1 && tokens[0].type == TokenType::RParen)
                {
                    // closing paren, no more args

                    if(argTokens.size() == 0)
                    {
                        if(lhs->subNodes.size() > 0)
                        {
                            throw ParserError("Expected another argument");
                        }
                        break;
                    }
                    argTokens.push_back(Token{TokenType::Eof});
                    lhs->subNodes.push_back(this->ParseExpression(argTokens));
                    argTokens.clear();
                    tokens.erase(tokens.begin());
                    break;
                }
                else if(parenCount == 1 && tokens[0].type == TokenType::Comma)
                {
                    // make new subnode, more than 1 argument passed
                    if(argTokens.size() == 0) { throw ParserError("Expected an argument"); }
                    argTokens.push_back(Token{TokenType::Eof});
                    lhs->subNodes.push_back(this->ParseExpression(argTokens));
                    argTokens.clear();
                } 
                else { argTokens.push_back(tokens[0]); }
                if(tokens[0].type == TokenType::LParen) { parenCount++; }
                if(tokens[0].type == TokenType::RParen) { parenCount--; }
                tokens.erase(tokens.begin());
            }
        }
        return lhs;
    }

    Node* Parser::ParseExpression(std::vector<Token>& tokens, float minBp)
    {
        Node* lhs = new Node();
        switch(tokens[0].type)
        {
            case TokenType::Identifier:
                lhs->data = ObjectRef{std::get<std::string>(tokens[0].data)};
                lhs->type = NodeType::Expr_ObjAccess;
                break;
            case TokenType::IntLiteral:
                lhs->data = std::get<std::int64_t>(tokens[0].data);
                lhs->type = NodeType::Expr_IntLiteral;
                break;
            case TokenType::FloatLiteral:
                lhs->data = std::get<double>(tokens[0].data);
                lhs->type = NodeType::Expr_FloatLiteral;
                break;
            case TokenType::LParen:
                delete lhs;
                tokens.erase(tokens.begin());
                lhs = ParseExpression(tokens);
                break;
            default:
                delete lhs;
                throw ParserError("Unexpected TokenL type");
        }

        tokens.erase(tokens.begin());
        while(true)
        {
            if(tokens[0].type == TokenType::Eof)
            {
                return lhs;
            }
            if(tokens[0].type == TokenType::RParen)
            {
                return lhs;
            }
            
            Token op = Token(tokens[0]);
            if(IsPostfix(op.type)) { lhs = this->ParsePostfixExpression(tokens, lhs); continue; }
            float leftBp = ExpressionPrecedence(op.type, false);
            float rightBp = ExpressionPrecedence(op.type, true);

            if(leftBp < minBp)
            {
                return lhs;
            }

            tokens.erase(tokens.begin());
            Node* rhs = this->ParseExpression(tokens, rightBp);

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
                case TokenType::Eq:
                    parent->type = NodeType::Expr_EqTest;
                    break;
                case TokenType::Neq:
                    parent->type = NodeType::Expr_NeqTest;
                    break;
                case TokenType::Geq:
                    parent->type = NodeType::Expr_GeqTest;
                    break;
                case TokenType::Leq:
                    parent->type = NodeType::Expr_LeqTest;
                    break;
                case TokenType::Gt:
                    parent->type = NodeType::Expr_GtTest;
                    break;
                case TokenType::Lt:
                    parent->type = NodeType::Expr_LtTest;
                    break;
                case TokenType::Mod:
                    parent->type = NodeType::Expr_Mod;
                    break;
                case TokenType::Div:
                    parent->type = NodeType::Expr_Div;
                    break;
            }
            lhs = parent;
        }
    }

    Node* Parser::Parse()
    {
        Node* programRoot = new Node();
        this->m_parent.push(programRoot);
        while(this->m_tokenIdx < this->m_tokens.size())
        {
            Token& next = this->Peek(0);
            if(next.type == TokenType::Fn)
            {
                this->Consume();
                Token& identifier = this->ExpectAndPeek(TokenType::Identifier, 0);
                this->Consume();
                FunctionSignature FS;
                FS.name = std::get<std::string>(identifier.data);
                this->ExpectAndConsume(TokenType::LParen);
                
                while(this->Peek(0).type != TokenType::RParen)
                {
                    VariableSignature VS = this->ParseVariableSignature();
                    FS.parameters.push_back(VS);
                    if(this->Peek(0).type != TokenType::RParen)
                    {
                        if(this->Peek(0).type != TokenType::Comma) { throw ParserError("Unexpected token"); }
                        this->Consume();
                    }
                }
                this->ExpectAndConsume(TokenType::RParen);
                this->ExpectAndConsume(TokenType::Arrow);
                VariableSignature VS = this->ParseType();
                FS.type = VS.type;
                FS.flags = VS.flags;
                Node* fnNode = new Node();
                fnNode->identifier = FS;
                fnNode->type = NodeType::Stmt_Function;
                this->m_parent.top()->subNodes.push_back(fnNode);
                this->m_parent.push(fnNode);
            }
            else if(next.type == TokenType::Var)
            {
                this->Consume();
                VariableSignature VS = this->ParseVariableSignature();
                if(this->Peek().type == TokenType::Semi)
                {
                    this->Consume();
                    continue;
                }
                else if(this->Peek().type != TokenType::Equals)
                {
                    throw ParserError("Expected semicolon or equals sign");
                }
                this->Consume();
                std::vector<Token> expression = {};
                while(this->Peek().type != TokenType::Semi && this->Peek().type != TokenType::Eof) {
                    expression.push_back(this->Peek());
                    this->Consume();
                }
                this->ExpectAndPeek(TokenType::Semi, 0);
                expression.push_back(Token{TokenType::Eof});
                Node* varNode = new Node();
                varNode->identifier = VS;
                varNode->type = NodeType::Stmt_Declaration;
                varNode->subNodes.push_back(this->ParseExpression(expression));

                this->m_parent.top()->subNodes.push_back(varNode);
            }
            else if(next.type == TokenType::Return)
            {
                this->Consume();
                
                std::vector<Token> expression = {};
                while(this->Peek().type != TokenType::Semi && this->Peek().type != TokenType::Eof) {
                    expression.push_back(this->Peek());
                    this->Consume();
                }
                this->ExpectAndPeek(TokenType::Semi, 0);
                expression.push_back(Token{TokenType::Eof});
                Node* returnNode = new Node();
                returnNode->type = NodeType::Stmt_Return;
                returnNode->subNodes.push_back(this->ParseExpression(expression));

                this->m_parent.top()->subNodes.push_back(returnNode);
            }
            else if(next.type == TokenType::Identifier)
            {
                if(this->Peek(1).type == TokenType::Equals)
                {
                    std::string ident = std::get<std::string>(next.data);
                    this->Consume(2);

                    std::vector<Token> expression = {};
                    while(this->Peek().type != TokenType::Semi && this->Peek().type != TokenType::Eof) {
                        expression.push_back(this->Peek());
                        this->Consume();
                    }
                    this->ExpectAndPeek(TokenType::Semi, 0);
                    expression.push_back(Token{TokenType::Eof});
                    Node* assignNode = new Node();
                    assignNode->type = NodeType::Stmt_Assignment;
                    assignNode->identifier = ObjectRef{ident};
                    assignNode->subNodes.push_back(this->ParseExpression(expression));

                    this->m_parent.top()->subNodes.push_back(assignNode);
                }
            }
            else if(next.type == TokenType::If)
            {
                this->Consume();
                this->ExpectAndConsume(TokenType::LParen);
                std::vector<Token> expression = {};
                while(this->Peek().type != TokenType::RParen && this->Peek().type != TokenType::Eof) {
                    expression.push_back(this->Peek());
                    this->Consume();
                }
                this->ExpectAndPeek(TokenType::RParen, 0);
                expression.push_back(Token{TokenType::Eof});
                Node* ifNode = new Node();
                ifNode->type = NodeType::Stmt_If;
                ifNode->subNodes.push_back(this->ParseExpression(expression));

                this->m_parent.top()->subNodes.push_back(ifNode);

                this->m_parent.push(ifNode);
            }
            else if(next.type == TokenType::Unless)
            {
                this->Consume();
                this->ExpectAndConsume(TokenType::LParen);
                std::vector<Token> expression = {};
                while(this->Peek().type != TokenType::RParen && this->Peek().type != TokenType::Eof) {
                    expression.push_back(this->Peek());
                    this->Consume();
                }
                this->ExpectAndPeek(TokenType::RParen, 0);
                expression.push_back(Token{TokenType::Eof});
                Node* unlessNode = new Node();
                unlessNode->type = NodeType::Stmt_Unless;
                unlessNode->subNodes.push_back(this->ParseExpression(expression));

                this->m_parent.top()->subNodes.push_back(unlessNode);

                this->m_parent.push(unlessNode);
            }
            else if(next.type == TokenType::While)
            {
                this->Consume();
                this->ExpectAndConsume(TokenType::LParen);
                std::vector<Token> expression = {};
                while(this->Peek().type != TokenType::RParen && this->Peek().type != TokenType::Eof) {
                    expression.push_back(this->Peek());
                    this->Consume();
                }
                this->ExpectAndPeek(TokenType::RParen, 0);
                expression.push_back(Token{TokenType::Eof});
                Node* whileNode = new Node();
                whileNode->type = NodeType::Stmt_While;
                whileNode->subNodes.push_back(this->ParseExpression(expression));

                this->m_parent.top()->subNodes.push_back(whileNode);

                this->m_parent.push(whileNode);
            }
            else if(next.type == TokenType::Until)
            {
                this->Consume();
                this->ExpectAndConsume(TokenType::LParen);
                std::vector<Token> expression = {};
                while(this->Peek().type != TokenType::RParen && this->Peek().type != TokenType::Eof) {
                    expression.push_back(this->Peek());
                    this->Consume();
                }
                this->ExpectAndPeek(TokenType::RParen, 0);
                expression.push_back(Token{TokenType::Eof});
                Node* untilNode = new Node();
                untilNode->type = NodeType::Stmt_Until;
                untilNode->subNodes.push_back(this->ParseExpression(expression));

                this->m_parent.top()->subNodes.push_back(untilNode);

                this->m_parent.push(untilNode);
            }
            else if(next.type == TokenType::RBrace)
            {
                this->m_parent.pop();
                if(this->m_parent.size() == 0)
                {
                    throw ParserError("Too many closing braces");
                }
            }
            this->Consume();
        }
        return programRoot;
    }
}
