#include "Xenoxy/impl/Lexer.hpp"
#include <iostream>

namespace Xenoxy {
    std::ostream& operator<<(std::ostream& os, const TokenType& value) {
        switch (value)
        {
        case TokenType::Null:
            os<<"TokenType::Null";
            break;
        case TokenType::Fn:
            os<<"TokenType::Fn";
            break;
        case TokenType::LParen:
            os<<"TokenType::LParen";
            break;
        case TokenType::RParen:
            os<<"TokenType::RParen";
            break;
        case TokenType::LBrace:
            os<<"TokenType::LBrace";
            break;
        case TokenType::RBrace:
            os<<"TokenType::RBrace";
            break;
        case TokenType::Arrow:
            os<<"TokenType::Arrow";
            break;
        case TokenType::Var:
            os<<"TokenType::Var";
            break;
        case TokenType::Identifier:
            os<<"TokenType::Identifier";
            break;
        case TokenType::Equals:
            os<<"TokenType::Equals";
            break;
        case TokenType::Plus:
            os<<"TokenType::Plus";
            break;
        case TokenType::Minus:
            os<<"TokenType::Minus";
            break;
        case TokenType::Eq:
            os<<"TokenType::Eq";
            break;
        case TokenType::Neq:
            os<<"TokenType::Neq";
            break;
        case TokenType::IntLiteral:
            os<<"TokenType::IntLiteral";
            break;
        case TokenType::StringLiteral:
            os<<"TokenType::StringLiteral";
            break;
        case TokenType::FloatLiteral:
            os<<"TokenType::FloatLiteral";
            break;
        case TokenType::Semi:
            os<<"TokenType::Semi";
            break;
        case TokenType::Colon:
            os<<"TokenType::Colon";
            break;
        case TokenType::If:
            os<<"TokenType::If";
            break;
        case TokenType::Unless:
            os<<"TokenType::Unless";
            break;
        case TokenType::Elif:
            os<<"TokenType::Elif";
            break;
        case TokenType::Else:
            os<<"TokenType::Else";
            break;
        case TokenType::Return:
            os<<"TokenType::Return";
            break;
        case TokenType::Comma:
            os<<"TokenType::Comma";
            break;
        case TokenType::Asterisk:
            os<<"TokenType::Asterisk";
            break;
        case TokenType::Eof:
            os<<"TokenType::Eof";
            break;
        case TokenType::Geq:
            os<<"TokenType::Geq";
            break;
        case TokenType::Leq:
            os<<"TokenType::Leq";
            break;
        case TokenType::While:
            os<<"TokenType::While";
            break;
        case TokenType::Gt:
            os<<"TokenType::Gt";
            break;
        case TokenType::Lt:
            os<<"TokenType::Lt";
            break;
        case TokenType::Until:
            os<<"TokenType::Until";
            break;
        case TokenType::Mod:
            os<<"TokenType::Mod";
            break;
        case TokenType::Div:
            os<<"TokenType::Div";
            break;
        default:
            break;
        }
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const Token& value) {
        os<<value.type;
        const auto* string_val = std::get_if<std::string>(&value.data);
        if(string_val != nullptr) {
            os<<"(\""<<*string_val<<"\")";
        }
        const auto* int_val = std::get_if<std::int64_t>(&value.data);
        if(int_val != nullptr) {
            os<<"("<<*int_val<<")";
        }
        const auto* float_val = std::get_if<double>(&value.data);
        if(float_val != nullptr) {
            os<<"("<<*float_val<<")";
        }
        return os;
    }

    bool isnum(char& c) {
        return c >= '0' && c <= '9';
    }
    std::vector<Token> Lexer::Tokenize(std::string& code) {
        std::vector<Token> tokens;
        size_t i = 0;
        size_t column = 1;
        size_t row = 1;
        while(i<code.size()) {
            if(code[i] == '\n') { row=1; column++; }
            if(i + 1 < code.size()) {
                if(code[i] == '-' && code[i+1] == '>') {
                    tokens.push_back({TokenType::Arrow, std::monostate()});
                    i+=2;
                    row+=2;
                    continue;
                }
                if(code[i] == '=' && code[i+1] == '=') {
                    tokens.push_back({TokenType::Eq, std::monostate()});
                    i+=2;
                    row+=2;
                    continue;
                }
                if(code[i] == '>' && code[i+1] == '=') {
                    tokens.push_back({TokenType::Geq, std::monostate()});
                    i+=2;
                    row+=2;
                    continue;
                }
                if(code[i] == '<' && code[i+1] == '=') {
                    tokens.push_back({TokenType::Leq, std::monostate()});
                    i+=2;
                    row+=2;
                    continue;
                }
                if(code[i] == '!' && code[i+1] == '=') {
                    tokens.push_back({TokenType::Neq, std::monostate()});
                    i+=2;
                    row+=2;
                    continue;
                }
            }
            if(i < code.size()) {
                if(code[i] == '(') {
                    tokens.push_back({TokenType::LParen, std::monostate()});
                    i++;
                    row++;
                    continue;
                }
                if(code[i] == ')') {
                    tokens.push_back({TokenType::RParen, std::monostate()});
                    i++;
                    row++;
                    continue;
                }
                if(code[i] == '{') {
                    tokens.push_back({TokenType::LBrace, std::monostate()});
                    i++;
                    row++;
                    continue;
                }
                if(code[i] == '}') {
                    tokens.push_back({TokenType::RBrace, std::monostate()});
                    i++;
                    row++;
                    continue;
                }
                if(code[i] == ';') {
                    tokens.push_back({TokenType::Semi, std::monostate()});
                    i++;
                    row++;
                    continue;
                }
                if(code[i] == ':') {
                    tokens.push_back({TokenType::Colon, std::monostate()});
                    i++;
                    row++;
                    continue;
                }
                if(code[i] == '=') {
                    tokens.push_back({TokenType::Equals, std::monostate()});
                    i++;
                    row++;
                    continue;
                }
                if(code[i] == '+') {
                    tokens.push_back({TokenType::Plus, std::monostate()});
                    i++;
                    row++;
                    continue;
                }
                if(code[i] == '-') {
                    tokens.push_back({TokenType::Minus, std::monostate()});
                    i++;
                    row++;
                    continue;
                }
                if(code[i] == ',') {
                    tokens.push_back({TokenType::Comma, std::monostate()});
                    i++;
                    row++;
                    continue;
                }
                if(code[i] == '*') {
                    tokens.push_back({TokenType::Asterisk, std::monostate()});
                    i++;
                    row++;
                    continue;
                }
                if(code[i] == '<') {
                    tokens.push_back({TokenType::Lt, std::monostate()});
                    i++;
                    row++;
                    continue;
                }
                if(code[i] == '>') {
                    tokens.push_back({TokenType::Gt, std::monostate()});
                    i++;
                    row++;
                    continue;
                }
                if(code[i] == '%') {
                    tokens.push_back({TokenType::Mod, std::monostate()});
                    i++;
                    row++;
                    continue;
                }
                if(code[i] == '/') {
                    tokens.push_back({TokenType::Div, std::monostate()});
                    i++;
                    row++;
                    continue;
                }
            }
            // text lexer (keywords, identifiers)
            if(std::isalpha(static_cast<unsigned char>(code[i])) || code[i] == '_') {
                std::string concat = "";
                while(i < code.size() && (std::isalnum(static_cast<unsigned char>(code[i])) || code[i] == '_')) {
                    concat += code[i];
                    i++;
                    row++;
                }
                if(concat == "fn") { tokens.push_back({TokenType::Fn, std::monostate()}); continue; }
                if(concat == "var") { tokens.push_back({TokenType::Var, std::monostate()}); continue; }
                if(concat == "if") { tokens.push_back({TokenType::If, std::monostate()}); continue; }
                if(concat == "unless") { tokens.push_back({TokenType::Unless, std::monostate()}); continue; }
                if(concat == "while") { tokens.push_back({TokenType::While, std::monostate()}); continue; }
                if(concat == "until") { tokens.push_back({TokenType::Until, std::monostate()}); continue; }
                if(concat == "return") { tokens.push_back({TokenType::Return, std::monostate()}); continue; }
                tokens.push_back({TokenType::Identifier, concat});
                continue;
            }
            if(isnum(code[i])) {
                std::int64_t val = 0;
                while(i < code.size() && isnum(code[i])) {
                    val *= 10;
                    val += code[i] - '0';
                    i++;
                    row++;
                }
                if(i < code.size() && code[i] == '.') {
                    double float_val = static_cast<double>(val);
                    i++;
                    row++;
                    double div = 1;
                    while(i < code.size() && isnum(code[i])) {
                        div *= 10;
                        float_val += static_cast<double>(code[i] - '0')/div;
                        i++;
                    row++;
                    }
                    tokens.push_back({TokenType::FloatLiteral, float_val});
                    continue;
                }
                tokens.push_back({TokenType::IntLiteral, val});
                continue;
            }
            if(code[i] != ' ' && code[i] != '\t' && code[i] != '\n' && code[i] != '\r') {
                std::string error_chr = "";
                error_chr += code[i];
                std::string error_msg = "invalid syntax: '" + error_chr + "'@" + std::to_string(row-1) + "." + std::to_string(column);
                throw LexerError(error_msg);
            }
            i++;
            row++;
        }
        tokens.push_back(Token{TokenType::Eof});
        return tokens;
    }
}