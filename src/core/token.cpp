#include "core/core.hpp"

std::string to_string(TokenType type)
{
    static const std::string strings[] = {
        "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE",
        "COMMA", "DOT", "MINUS", "PLUS", "SEMICOLON", "SLASH", "STAR",
        "BANG", "BANG_EQUAL",
        "EQUAL", "EQUAL_EQUAL",
        "GREATER", "GREATER_EQUAL",
        "LESS", "LESS_EQUAL",
        "IDENTIFIER", "STRING", "NUMBER",
        "AND", "CLASS", "ELSE", "FALSE", "FUNCTION", "FOR", "IF", "NULL", "OR",
        "PRINT", "PRINTLN", "RETURN", "SUPER", "THIS", "TRUE", "VAR", "WHILE",
        "END_OF_FILE"
    };
    return strings[static_cast<int>(type)];
}

Token::Token(TokenType type, std::string lexeme, Value literal, int line)
: type(type), lexeme(std::move(lexeme)), literal(std::move(literal)), line(line) {}

std::string Token::stringify() const
{
    std::string literal_text;
    switch(this->type)
    {
        case IDENTIFIER:
            literal_text = this->lexeme;
            break;
        case STRING:
            literal_text = std::get<std::string>(this->literal);
            break;
        case NUMBER:
            literal_text = std::to_string(std::get<double>(this->literal));
            break;
        case TRUE:
            literal_text = "true";
            break;
        case FALSE:
            literal_text = "false";
            break;
        default:
            literal_text = "null";
    }
    return ::to_string(this->type) + " " + this->lexeme + " " + literal_text;
}