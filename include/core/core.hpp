#ifndef CORE_HPP
#define CORE_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <stdexcept>
#include <cmath>
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
    PRINT, PRINTLN, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    END_OF_FILE
};

inline bool error_trigger = false;
inline bool runtime_error_trigger = false;

struct Binary;
struct Grouping;
struct Literal;
struct Unary;
struct Assign;
struct Variable;
struct Logical;

struct Expression;
struct ExpressionVisitor;

struct Stmt;
struct StmtVisitor;

struct BlockStmt;
struct ExprStmt;
struct PrintStmt;
struct PrintLnStmt;
struct IfStmt;
struct WhileStmt;
struct VarStmt;

class Environment;
class Token;
class RuntimeError;
class Interpreter;

void report(int line, std::string where, std::string message);
void error(int line, std::string message);
void error(const Token& token, std::string message);
void runtime_error(const RuntimeError &error);
std::string to_string(TokenType type);

class Carblang
{
    public:
        Interpreter* interpreter;

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

class RuntimeError : public std::runtime_error
{
    public:
        const Token& token;
        RuntimeError(const Token& token, std::string message) : std::runtime_error{message.data()}, token{token}
        {}
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
    virtual std::any visit_logical_expression(std::shared_ptr<Logical> expr) = 0;
    virtual std::any visit_assign_expression(std::shared_ptr<Assign> expr) = 0;
    virtual std::any visit_variable_expression(std::shared_ptr<Variable> expr) = 0;
    virtual ~ExpressionVisitor() = default;
};

struct Expression
{
    virtual std::any accept(ExpressionVisitor& visitor) = 0;
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

struct Logical : Expression, public std::enable_shared_from_this<Logical>
{
    Logical(std::shared_ptr<Expression> left, Token operator_token, std::shared_ptr<Expression> right) : left{std::move(left)}, operator_token{std::move(operator_token)}, right{std::move(right)}
    {}

    std::any accept(ExpressionVisitor& visitor) override
    {
        return visitor.visit_logical_expression(shared_from_this());
    }

    const std::shared_ptr<Expression> left;
    const Token operator_token;
    const std::shared_ptr<Expression> right;
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

struct Assign : Expression, public std::enable_shared_from_this<Assign>
{
    Assign(Token name, std::shared_ptr<Expression> value) : name{std::move(name)}, value{std::move(value)}
    {}

    std::any accept(ExpressionVisitor& visitor) override
    {
        return visitor.visit_assign_expression(shared_from_this());
    }

    const Token name;
    const std::shared_ptr<Expression> value;
};

struct Variable : Expression, public std::enable_shared_from_this<Variable>
{
    Variable(Token name) : name{std::move(name)}
    {}

    std::any accept(ExpressionVisitor& visitor) override
    {
        return visitor.visit_variable_expression(shared_from_this());
    }

    const Token name;
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

        std::vector<std::shared_ptr<Stmt>> parse();

    private:
        std::shared_ptr<Expression> expression();
        std::shared_ptr<Stmt> declaration();
        std::shared_ptr<Stmt> statement();
        std::shared_ptr<Stmt> print_statement();
        std::shared_ptr<Stmt> println_statement();
        std::shared_ptr<Stmt> for_statement();
        std::shared_ptr<Stmt> if_statement();
        std::shared_ptr<Stmt> while_statement();
        std::shared_ptr<Stmt> var_declaration();
        std::shared_ptr<Stmt> expression_statement();
        std::vector<std::shared_ptr<Stmt>> block();
        std::shared_ptr<Expression> assignment();
        std::shared_ptr<Expression> and_expression();
        std::shared_ptr<Expression> or_expression();
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

struct StmtVisitor
{
    virtual std::any visit_block_stmt(std::shared_ptr<BlockStmt> stmt) = 0;
    virtual std::any visit_expression_stmt(std::shared_ptr<ExprStmt> stmt) = 0;
    virtual std::any visit_print_stmt(std::shared_ptr<PrintStmt> stmt) = 0;
    virtual std::any visit_println_stmt(std::shared_ptr<PrintLnStmt> stmt) = 0;
    virtual std::any visit_if_stmt(std::shared_ptr<IfStmt> stmt) = 0;
    virtual std::any visit_while_stmt(std::shared_ptr<WhileStmt> stmt) = 0;
    virtual std::any visit_var_stmt(std::shared_ptr<VarStmt> stmt) = 0;
    virtual ~StmtVisitor() = default;
};

struct Stmt
{
    virtual std::any accept(StmtVisitor& visitor) = 0;
};

struct BlockStmt : Stmt, public std::enable_shared_from_this<BlockStmt>
{
    BlockStmt(std::vector<std::shared_ptr<Stmt>> statements) : statements{std::move(statements)}
    {}

    std::any accept(StmtVisitor& visitor) override
    {
        return visitor.visit_block_stmt(shared_from_this());
    }

    const std::vector<std::shared_ptr<Stmt>> statements;
};

struct ExprStmt : Stmt, public std::enable_shared_from_this<ExprStmt>
{
    ExprStmt(std::shared_ptr<Expression> expression) : expression{std::move(expression)}
    {}

    std::any accept(StmtVisitor& visitor) override
    {
        return visitor.visit_expression_stmt(shared_from_this());
    }
    const std::shared_ptr<Expression> expression;
};

struct PrintStmt : Stmt, public std::enable_shared_from_this<PrintStmt>
{
    PrintStmt(std::shared_ptr<Expression> expression) : expression{std::move(expression)}
    {}

    std::any accept(StmtVisitor& visitor) override
    {
        return visitor.visit_print_stmt(shared_from_this());
    }

    const std::shared_ptr<Expression> expression;
};

struct PrintLnStmt : Stmt, public std::enable_shared_from_this<PrintLnStmt>
{
    PrintLnStmt(std::shared_ptr<Expression> expression) : expression{std::move(expression)}
    {}

    std::any accept(StmtVisitor& visitor) override
    {
        return visitor.visit_println_stmt(shared_from_this());
    }

    const std::shared_ptr<Expression> expression;
};

struct IfStmt : Stmt, public std::enable_shared_from_this<IfStmt>
{
    IfStmt(std::shared_ptr<Expression> condition, std::shared_ptr<Stmt> then_branch, std::shared_ptr<Stmt> else_branch)
    : condition{std::move(condition)}, then_branch{std::move(then_branch)}, else_branch{std::move(else_branch)}
    {}

    std::any accept(StmtVisitor& visitor) override
    {
        return visitor.visit_if_stmt(shared_from_this());
    }

    const std::shared_ptr<Expression> condition;
    const std::shared_ptr<Stmt> then_branch;
    const std::shared_ptr<Stmt> else_branch;
};

struct WhileStmt: Stmt, public std::enable_shared_from_this<WhileStmt>
{
    WhileStmt(std::shared_ptr<Expression> condition, std::shared_ptr<Stmt> body)
    : condition{std::move(condition)}, body{std::move(body)}
    {}

    std::any accept(StmtVisitor& visitor) override
    {
        return visitor.visit_while_stmt(shared_from_this());
    }

    const std::shared_ptr<Expression> condition;
    const std::shared_ptr<Stmt> body;
};

struct VarStmt : Stmt, public std::enable_shared_from_this<VarStmt>
{
    VarStmt(Token name, std::shared_ptr<Expression> initializer) : name{std::move(name)}, initializer{std::move(initializer)}
    {}

    std::any accept(StmtVisitor& visitor) override
    {
        return visitor.visit_var_stmt(shared_from_this());
    }

    const Token name;
    const std::shared_ptr<Expression> initializer;
};

class Environment : public std::enable_shared_from_this<Environment>
{
    std::shared_ptr<Environment> enclosing;
    std::map<std::string, std::any> values;

    public:
        Environment() : enclosing{nullptr}
        {}

        Environment(std::shared_ptr<Environment> enclosing) : enclosing{std::move(enclosing)}
        {}

        std::any get(const Token& name)
        {
            auto element = values.find(name.lexeme);
            if(element != values.end())
            {
                return element->second;
            }

            if(enclosing != nullptr) return enclosing->get(name);
            throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'");
        }

        void assign(const Token& name, std::any value)
        {
            auto element = values.find(name.lexeme);
            if(element != values.end())
            {
                element->second = std::move(value);
                return;
            }

            if(enclosing != nullptr)
            {
                enclosing->assign(name, std::move(value));
                return;
            }

            throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'");
        }

      void define(const std::string& name, std::any value)
      {
            values[name] = std::move(value);
      }
};

class Interpreter : public ExpressionVisitor, public StmtVisitor
{
    std::shared_ptr<Environment> environment = std::make_shared<Environment>();

    public:
        void interpret(std::vector<std::shared_ptr<Stmt>>& statements)
        {
            try
            {
                for(const std::shared_ptr<Stmt>& statement : statements)
                {
                    execute(statement);
                }
            }
            catch (RuntimeError error)
            {
                runtime_error(error);
            }
        }

    private:
        std::any evaluate(std::shared_ptr<Expression> expression)
        {
            return expression->accept(*this);
        }

        void execute(std::shared_ptr<Stmt> stmt)
        {
            stmt->accept(*this);
        }

        void execute_block(const std::vector<std::shared_ptr<Stmt>>& statements, std::shared_ptr<Environment> environment)
        {
            std::shared_ptr<Environment> previous = this->environment;
            try
            {
                this->environment = environment;

                for(const std::shared_ptr<Stmt>& statement : statements)
                {
                    execute(statement);
                }
            }
            catch(...)
            {
                this->environment = previous;
                throw;
            }

            this->environment = previous;
        }

    public:

        std::any visit_block_stmt(std::shared_ptr<BlockStmt> stmt) override
        {
            execute_block(stmt->statements, std::make_shared<Environment>(environment));
            return {};
        }

        std::any visit_expression_stmt(std::shared_ptr<ExprStmt> stmt) override
        {
            evaluate(stmt->expression);
            return {};
        }

        std::any visit_print_stmt(std::shared_ptr<PrintStmt> stmt) override
        {
            std::any value = evaluate(stmt->expression);
            std::cout << stringify(value);
            return {};
        }

        std::any visit_println_stmt(std::shared_ptr<PrintLnStmt> stmt) override
        {
            std::any value = evaluate(stmt->expression);
            std::cout << stringify(value) << "\n";
            return {};
        }

        std::any visit_if_stmt(std::shared_ptr<IfStmt> stmt) override
        {
            if(this->is_truthy(this->evaluate(stmt->condition)))
            {
                this->execute(stmt->then_branch);
            }
            else if(stmt->else_branch != nullptr)
            {
                this->execute(stmt->else_branch);
            }
            return {};
        }

        std::any visit_while_stmt(std::shared_ptr<WhileStmt> stmt) override
        {
            while(this->is_truthy(this->evaluate(stmt->condition)))
            {
                this->execute(stmt->body);
            }
            return {};
        }

        std::any visit_var_stmt(std::shared_ptr<VarStmt> stmt) override
        {
            std::any value = nullptr;
            if(stmt->initializer != nullptr)
            {
                value = evaluate(stmt->initializer);
            }

            environment->define(stmt->name.lexeme, std::move(value));
            return {};
        }

        std::any visit_assign_expression(std::shared_ptr<Assign> expr) override
        {
            std::any value = evaluate(expr->value);
            environment->assign(expr->name, value);
            return value;
        }

        std::any visit_variable_expression(std::shared_ptr<Variable> expr) override
        {
            return environment->get(expr->name);
        }

        std::any visit_logical_expression(std::shared_ptr<Logical> expr) override
        {
            std::any left = this->evaluate(expr->left);

            if(expr->operator_token.type == TokenType::OR)
            {
                if(this->is_truthy(left)) return left;
            }
            else if(expr->operator_token.type == TokenType::AND)
            {
                if(!this->is_truthy(left)) return left;
            }

            return this->evaluate(expr->right);
        }

        std::any visit_binary_expression(std::shared_ptr<Binary> expression) override
        {
            std::any left = evaluate(expression->left);
            std::any right = evaluate(expression->right);

            switch (expression->operator_token.type)
            {
                case BANG_EQUAL: return !is_equal(left, right);
                case EQUAL_EQUAL: return is_equal(left, right);
                case GREATER:
                    check_number_operands(expression->operator_token, left, right);
                    return std::any_cast<double>(left) > std::any_cast<double>(right);
                case GREATER_EQUAL:
                    check_number_operands(expression->operator_token, left, right);
                    return std::any_cast<double>(left) >= std::any_cast<double>(right);
                case LESS:
                    check_number_operands(expression->operator_token, left, right);
                    return std::any_cast<double>(left) < std::any_cast<double>(right);
                case LESS_EQUAL:
                    check_number_operands(expression->operator_token, left, right);
                    return std::any_cast<double>(left) <= std::any_cast<double>(right);
                case MINUS:
                    check_number_operands(expression->operator_token, left, right);
                    return std::any_cast<double>(left) - std::any_cast<double>(right);
                case PLUS:
                    if(left.type() == typeid(double) && right.type() == typeid(double))
                    {
                        return std::any_cast<double>(left) + std::any_cast<double>(right);
                    }

                    if(left.type() == typeid(std::string) && right.type() == typeid(std::string))
                    {
                      return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
                    }

                    throw RuntimeError{expression->operator_token, "Operands must be two numbers or two strings."};
              
                case SLASH:
                    {
                        check_number_operands(expression->operator_token, left, right);
                        double right_value = std::any_cast<double>(right);
                        if(right_value == 0.0)
                        {
                            throw RuntimeError{expression->operator_token, "Division by zero"};
                        }
                        return std::any_cast<double>(left) / std::any_cast<double>(right);
                    }
                case STAR:
                    check_number_operands(expression->operator_token, left, right);
                    return std::any_cast<double>(left) * std::any_cast<double>(right);
                default:
                    throw RuntimeError{expression->operator_token, "Unknown binary operator"};

            }
        }

        std::any visit_grouping_expression(std::shared_ptr<Grouping> expression) override
        {
            return evaluate(expression->expression);
        }

        std::any visit_literal_expression(std::shared_ptr<Literal> expression) override
        {
            return expression->value;
        }

        std::any visit_unary_expression(std::shared_ptr<Unary> expression) override
        {
            std::any right = evaluate(expression->right);
            switch (expression->operator_token.type)
            {
                case BANG:
                    return !is_truthy(right);
                case MINUS:
                    check_number_operand(expression->operator_token, right);
                    return -std::any_cast<double>(right);
                default:
                    throw RuntimeError{expression->operator_token, "Unknown unary operator"};
            }
        }

    private:
        void check_number_operand(const Token& operator_token, const std::any& operand)
        {
            if(operand.type() == typeid(double)) return;
            throw RuntimeError{operator_token, "Operand must be a number."};
        }

        void check_number_operands(const Token& operator_token, const std::any& left, const std::any& right)
        {
            if(left.type() == typeid(double) && right.type() == typeid(double))
            {
                return;
            }
            throw RuntimeError{operator_token, "Operands must be numbers."};
        }

        bool is_truthy(const std::any& object)
        {
            if(object.type() == typeid(nullptr)) return false;
            if(object.type() == typeid(bool))
            {
                return std::any_cast<bool>(object);
            }
            return true;
        }

        bool is_equal(const std::any& a, const std::any& b)
        {
            if(a.type() == typeid(nullptr) && b.type() == typeid(nullptr))
            {
                return true;
            }

            if(a.type() == typeid(nullptr)) return false;

            if (a.type() == typeid(std::string) && b.type() == typeid(std::string))
            {
                return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);
            }

            if(a.type() == typeid(double) && b.type() == typeid(double))
            {
                return std::any_cast<double>(a) == std::any_cast<double>(b);
            }

            if(a.type() == typeid(bool) && b.type() == typeid(bool))
            {
                return std::any_cast<bool>(a) == std::any_cast<bool>(b);
            }
            return false;
        }

        std::string stringify(const std::any& object)
        {
            if (!object.has_value()) return "null";

            if(object.type() == typeid(double))
            {
                double d = std::any_cast<double>(object);

                if (std::floor(d) == d)
                {
                    return std::to_string(static_cast<int>(d));
                }
                else
                {
                    std::ostringstream out;
                    out << std::setprecision(10) << d;
                    return out.str();
                }
            }

            if(object.type() == typeid(std::string))
            {
                return std::any_cast<std::string>(object);
            }

            if(object.type() == typeid(bool))
            {
                return std::any_cast<bool>(object) ? "true" : "false";
            }

            return "Error in stringify: object type not recognized";
        }
};

#endif