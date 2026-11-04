#ifndef Parser_hpp
#define Parser_hpp
#include "Xenoxy/impl/Lexer.hpp"
#include "Xenoxy/impl/EAST.hpp"
#include <vector>
#include <stack>

namespace Xenoxy {
    class ParserError: public std::runtime_error {
        public:
        explicit ParserError(std::string message)
        : std::runtime_error(message) {}
    };

    class Parser {
        private:
        std::int64_t m_tokenIdx = 0;
        std::vector<Token>& m_tokens;
        std::stack<Node*> m_parent;
        Token& Peek(std::int64_t offset=0);
        Token& ExpectAndPeek(TokenType type, std::int64_t offset);
        void Consume(std::int64_t count=1);
        void ExpectAndConsume(TokenType type);
        VariableSignature ParseVariableSignature();
        VariableSignature ParseType();
        Node* ParsePostfixExpression(std::vector<Token>& tokens, Node* lhs);
        Node* ParseExpression(std::vector<Token>& tokens, float minBp=0.0);
        public:
        Parser(std::vector<Token>& tokens) : m_tokens(tokens) {};
        Node* Parse();
    };
}

#endif