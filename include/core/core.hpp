#ifndef CORE_HPP
#define CORE_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <stdexcept>
#include <memory>
#include <string>
#include <cstring>
#include <cassert>
#include <vector>
#include <utility>
#include <map>
#include <any>
#include <cerrno>
#include <type_traits>

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

inline bool error_trigger = false;

struct Binary;
struct Grouping;
struct Literal;
struct Unary;

class Token;

void report(int line, std::string where, std::string message);
void error(int line, std::string message);
void error(const Token& token, std::string message);
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

struct ExpressionVisitor
{
    virtual std::any visit_binary_expression(std::shared_ptr<Binary> expr) = 0;
    virtual std::any visit_grouping_expression(std::shared_ptr<Grouping> expr) = 0;
    virtual std::any visit_literal_expression(std::shared_ptr<Literal> expr) = 0;
    virtual std::any visit_unary_expression(std::shared_ptr<Unary> expr) = 0;
    virtual ~ExpressionVisitor() = default;
};

struct Expression
{
    virtual std::any accept(ExpressionVisitor& visitor) = 0;
};

class AST : public ExpressionVisitor
{
    public:
        std::string print(std::shared_ptr <Expression> expression);
        std::any visit_binary_expression(std::shared_ptr<Binary> expression) override;
        std::any visit_grouping_expression(std::shared_ptr<Grouping> expression) override; 
        std::any visit_literal_expression(std::shared_ptr<Literal> expression) override;
        std::any visit_unary_expression(std::shared_ptr<Unary> expression) override;
        
    private:
        template<class...E>
        std::string parenthesize(std::string_view name, E...expression);
};

struct Binary : Expression, public std::enable_shared_from_this<Binary>
{
    Binary(std::shared_ptr<Expression> left, Token operator_token, std::shared_ptr<Expression> right) : left{std::move(left)}, operator_token{std::move(operator_token)}, right{std::move(right)}
    {}

    std::any accept(ExpressionVisitor& visitor) override
    {
        return visitor.visit_binary_expression(shared_from_this());
    }

    const std::shared_ptr<Expression> left;
    const Token operator_token;
    const std::shared_ptr<Expression> right;
};

struct Grouping : Expression, public std::enable_shared_from_this<Grouping>
{
    Grouping(std::shared_ptr<Expression> expression) : expression{std::move(expression)}
    {}

    std::any accept(ExpressionVisitor& visitor) override
    {
        return visitor.visit_grouping_expression(shared_from_this());
    }

    const std::shared_ptr<Expression> expression;
};

struct Literal : Expression, public std::enable_shared_from_this<Literal>
{
    Literal(std::any value) : value{std::move(value)}
    {}

    std::any accept(ExpressionVisitor& visitor) override
    {
        return visitor.visit_literal_expression(shared_from_this());
    }

  const std::any value;
};

struct Unary : Expression, public std::enable_shared_from_this<Unary>
{
    Unary(Token operator_token, std::shared_ptr<Expression> right) : operator_token{std::move(operator_token)}, right{std::move(right)}
    {}

    std::any accept(ExpressionVisitor& visitor) override
    {
        return visitor.visit_unary_expression(shared_from_this());
    }

    const Token operator_token;
    const std::shared_ptr<Expression> right;
};

class Parser
{
    struct ParseError : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    const std::vector<Token> &tokens;
    int current = 0;

    public:
        Parser(const std::vector<Token> &tokens);

        std::shared_ptr<Expression> parse();

    private:
        std::shared_ptr<Expression> expression();
        std::shared_ptr<Expression> equality();
        std::shared_ptr<Expression> comparison();
        std::shared_ptr<Expression> term();
        std::shared_ptr<Expression> factor();
        std::shared_ptr<Expression> unary();
        std::shared_ptr<Expression> primary();

        template<class...T>
        bool match(T...type);

        Token consume(TokenType type, std::string message);
        bool check(TokenType type);
        Token advance();
        bool at_end();
        Token peek();
        Token previous();
        ParseError error(const Token &token, std::string message);
        void synchronize();
};


#endif