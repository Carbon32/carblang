#ifndef CORE_HPP
#define CORE_HPP

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cassert>
#include <cstring>
#include <vector>
#include <utility>
#include <map>
#include <any>
#include <cerrno>

// This macro is defined in one of the includes above, it took me a few hours to find it
#undef NULL

enum TokenType
{
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    IDENTIFIER, STRING, NUMBER,

    AND, CLASS, ELSE, FALSE, FUNCTION, FOR, IF, NULL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    END_OF_FILE
};

extern bool error_trigger;

void report(int line, std::string where, std::string message);
void error(int line, std::string message);
std::string to_string(TokenType type);

class Carblang
{
    public:
        Carblang();
        void start(int argc, char **argv);
        void run(std::string code);

        std::string read_file(const char *file);

        void handle_file(const char *file);
        void run_instructions();
};

class Token
{
    public:
        const TokenType type;
        const std::string lexeme;
        const std::any literal;
        const int line;

        Token(TokenType type, std::string lexeme, std::any literal, int line);
        std::string stringify() const;
};

class Scanner
{
    public:
        Scanner(std::string src);
        std::vector<Token> scan_tokens();
    private:
      static const std::map<std::string, TokenType> keywords;
      std::string src;
      std::vector<Token> tokens;
      int start = 0;
      int current = 0;
      int line = 1;

      void scan_single_token();
      void identifier();
      void number();
      void string();
      bool match(char expected);
      char peek();
      char peek_next();
      bool is_alpha(char c);
      bool is_alpha_numeric(char c);
      bool is_digit(char c);
      bool at_end();
      char advance();
      void add_token(TokenType type);
      void add_token(TokenType type, std::any literal);
};

#endif