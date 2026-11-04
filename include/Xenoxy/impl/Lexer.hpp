#ifndef Lexer_hpp
#define Lexer_hpp
#include <iostream>
#include <vector>
#include <variant>
#include <cstdint>
#include <string>

namespace Xenoxy {

    enum class TokenType {
    //  null  fn   (        )       {       }       ->   var                =      +     -     ==   !=
        Null, Fn, LParen, RParen, LBrace, RBrace, Arrow, Var, Identifier, Equals, Plus, Minus, Eq, Neq, 
        IntLiteral, StringLiteral, FloatLiteral, Semi, Colon, If, Unless, Elif, Else, Return, Comma, Eof,
    //     *       >=   <=          >  <           %    /
        Asterisk, Geq, Leq, While, Gt, Lt, Until, Mod, Div
    };

    std::ostream& operator<<(std::ostream& os, const TokenType& value);

    struct Token {
        TokenType type;
        std::variant<std::monostate, std::string, std::int64_t, double> data;
    };

    std::ostream& operator<<(std::ostream& os, const Token& value);    

    class LexerError: public std::runtime_error {
        public:
        explicit LexerError(std::string message)
        : std::runtime_error(message) {}
    };

    class Lexer {
        public:
        static std::vector<Token> Tokenize(std::string& code);
    };
}

#endif