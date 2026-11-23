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

    // Rewrite on ranges -> remove creating and destroying so many vectors
    // - removes tech debt
    // - simplifies code (no sub-vector generation)
    class Parser {
        private:
        struct ParserRange
        {
            std::int64_t endIdx;
        };
        std::int64_t m_tokenIdx = 0;
        std::vector<Token>& m_tokens;
        std::stack<Node*> m_parent;
        Token Peek(std::int64_t offset=0);
        void Consume(std::int64_t count=1);
        void ExpectAndConsume(TokenType type);
        bool IsPostfix(const TokenType& c);
        bool IsPrefix(const TokenType& c);
        float ExpressionPrecedence(const TokenType& c, bool right);
        bool HasTokens();
        // finds range to eol (end of line), default = TokenType::Semi
        ParserRange findRange(TokenType eol = TokenType::Semi);
        std::vector<ParserRange> findListRanges(TokenType open = TokenType::LSquare, TokenType close = TokenType::RSquare, TokenType sep = TokenType::Comma);
        VariableSignature ParseType();
        VariableSignature ParseVariableSignature();
        FunctionSignature ParseFunctionSignature();
        Node* GenerateLHSNode(ParserRange pr, Token tokenL);
        Node* GenerateParentNode(Node* lhs, Token op, Node* rhs);
        Node* ParseArray(Node* arrayParent);
        Node* ParsePrefixExpression(ParserRange pr, Node* lhs);
        Node* ParsePostfixExpression(ParserRange pr, Node* lhs);
        Node* ParseExpression(ParserRange pr, float minBp=0.0);
        public:
        Parser(std::vector<Token>& tokens) : m_tokens(tokens) {};
        Node* Parse();
    };
}

#endif