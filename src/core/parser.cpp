#include "core/core.hpp"

Parser::Parser(const std::vector <Token>& tokens) : tokens{tokens} 
{}

std::vector<std::shared_ptr<Stmt>> Parser::parse()
{
    std::vector<std::shared_ptr<Stmt>> statements;
    while(!this->at_end())
    {
      statements.push_back(declaration());
    }
    return statements;
}

std::shared_ptr<Expression> Parser::expression()
{
    return assignment();
}

std::shared_ptr<Stmt> Parser::declaration()
{
    try
    {
        if(this->match(VAR)) return this->var_declaration();
        return this->statement();
    }
    catch(ParseError error)
    {
        this->synchronize();
        return nullptr;
    }
}

std::shared_ptr<Stmt> Parser::statement()
{
    if(this->match(PRINT)) return this->print_statement();
    if(this->match(PRINTLN)) return this->println_statement();
    if(this->match(IF)) return this->if_statement();
    if(this->match(WHILE)) return this->while_statement();
    if(this->match(FOR)) return this->for_statement();
    if(this->match(LEFT_BRACE)) return std::make_shared<BlockStmt>(this->block());
    return this->expression_statement();
}

std::shared_ptr<Stmt> Parser::print_statement()
{
    consume(LEFT_PAREN, "Expected '(' after 'print'.");
    std::shared_ptr<Expression> value = this->expression();
    consume(RIGHT_PAREN, "Expected ')' after value.");
    consume(SEMICOLON, "Expected ';' after value.");
    return std::make_shared<PrintStmt>(value);
}

std::shared_ptr<Stmt> Parser::println_statement()
{
    consume(LEFT_PAREN, "Expected '(' after 'println'.");
    std::shared_ptr<Expression> value = this->expression();
    consume(RIGHT_PAREN, "Expected ')' after value.");
    consume(SEMICOLON, "Expected ';' after value.");
    return std::make_shared<PrintLnStmt>(value);
}

std::shared_ptr<Stmt> Parser::for_statement()
{
    this->consume(LEFT_PAREN, "Expected '(' after 'for'.");

    std::shared_ptr<Stmt> initializer;
    if(this->match(SEMICOLON))
    {
        initializer = nullptr;
    }
    else if(this->match(VAR))
    {
        initializer = this->var_declaration();
    }
    else
    {
        initializer = this->expression_statement();
    }

    std::shared_ptr<Expression> condition = nullptr;
    if(!check(SEMICOLON))
    {
        condition = this->expression();
    }
    this->consume(SEMICOLON, "Expected ';' after loop condition.");

    std::shared_ptr<Expression> increment = nullptr;
    if(!check(RIGHT_PAREN))
    {
        increment = this->expression();
    }
    this->consume(RIGHT_PAREN, "Expected ')' after for clauses.");
    std::shared_ptr<Stmt> body = this->statement();

    if(increment != nullptr)
    {
        body = std::make_shared<BlockStmt>(std::vector<std::shared_ptr<Stmt>>{body, std::make_shared<ExprStmt>(increment)});
    }

    if(condition == nullptr)
    {
        condition = std::make_shared<Literal>(true);
    }
    body = std::make_shared<WhileStmt>(condition, body);

    if(initializer != nullptr)
    {
        body = std::make_shared<BlockStmt>(std::vector<std::shared_ptr<Stmt>>{initializer, body});
    }

    return body;
}

std::shared_ptr<Stmt> Parser::if_statement()
{
    this->consume(LEFT_PAREN, "Expected '(' after 'if'.");
    std::shared_ptr<Expression> condition = this->expression();
    this->consume(RIGHT_PAREN, "Expected ')' after if condition.");

    std::shared_ptr<Stmt> then_branch = this->statement();
    std::shared_ptr<Stmt> else_branch = nullptr;
    if(this->match(ELSE))
    {
        else_branch = statement();
    }

    return std::make_shared<IfStmt>(condition, then_branch, else_branch);
}

std::shared_ptr<Stmt> Parser::while_statement()
{
    this->consume(LEFT_PAREN, "Expected '(' after 'while'.");
    std::shared_ptr<Expression> condition = this->expression();
    this->consume(RIGHT_PAREN, "Expected ')' after condition.");
    std::shared_ptr<Stmt> body = this->statement();

    return std::make_shared<WhileStmt>(condition, body);
}

std::shared_ptr<Stmt> Parser::var_declaration()
{
    Token name = this->consume(IDENTIFIER, "Expected a variable name");

    std::shared_ptr<Expression> initializer = nullptr;
    if(match(EQUAL))
    {
        initializer = this->expression();
    }

    this->consume(SEMICOLON, "Expected ';' after variable declaration.");
    return std::make_shared<VarStmt>(std::move(name), initializer);
}

std::shared_ptr<Stmt> Parser::expression_statement()
{
    std::shared_ptr<Expression> expr = this->expression();
    this->consume(SEMICOLON, "Expected ';' after expression.");
    return std::make_shared<ExprStmt>(expr);
}

std::vector<std::shared_ptr<Stmt>> Parser::block()
{
    std::vector<std::shared_ptr<Stmt>> statements;

    while(!this->check(RIGHT_BRACE) && !this->at_end())
    {
        statements.push_back(declaration());
    }

    this->consume(RIGHT_BRACE, "Expected '}' after block.");
    return statements;
}

std::shared_ptr<Expression> Parser::assignment()
{
    std::shared_ptr<Expression> expr = this->or_expression();

    if(this->match(EQUAL))
    {
        Token equals = this->previous();
        std::shared_ptr<Expression> value = this->assignment();

        if(Variable* e = dynamic_cast<Variable*>(expr.get()))
        {
            Token name = e->name;
            return std::make_shared<Assign>(std::move(name), value);
        }

        error(std::move(equals), "Invalid assignment target.");
    }
    return expr;
}

std::shared_ptr<Expression> Parser::or_expression()
{
    std::shared_ptr<Expression> expr = this->and_expression();

    while(this->match(OR))
    {
        Token op = this->previous();
        std::shared_ptr<Expression> right = this->and_expression();
        expr = std::make_shared<Logical>(expr, std::move(op), right);
    }

    return expr;
}

std::shared_ptr<Expression> Parser::and_expression()
{
    std::shared_ptr<Expression> expr = this->equality();

    while(this->match(AND))
    {
        Token op = this->previous();
        std::shared_ptr<Expression> right = this->equality();
        expr = std::make_shared<Logical>(expr, std::move(op), right);
    }

    return expr;
}

std::shared_ptr<Expression> Parser::equality()
{
    std::shared_ptr<Expression> expr = comparison();

    while(this->match(BANG_EQUAL, EQUAL_EQUAL))
    {
        Token token_operator = this->previous();
        std::shared_ptr<Expression> right = comparison();
        expr = std::make_shared <Binary> (expr, std::move(token_operator), right);
    }

    return expr;
}

std::shared_ptr<Expression> Parser::comparison()
{
    std::shared_ptr<Expression> expr = term();

    while(this->match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL))
    {
        Token token_operator = this->previous();
        std::shared_ptr<Expression> right = term();
        expr = std::make_shared<Binary> (expr, std::move(token_operator), right);
    }

    return expr;
}

std::shared_ptr<Expression> Parser::term()
{
    std::shared_ptr<Expression> expr = factor();

    while(this->match(MINUS, PLUS))
    {
        Token token_operator = this->previous();
        std::shared_ptr<Expression> right = factor();
        expr = std::make_shared<Binary> (expr, std::move(token_operator), right);
    }

    return expr;
}

std::shared_ptr<Expression> Parser::factor()
{
    std::shared_ptr<Expression> expr = unary();

    while(this->match(SLASH, STAR))
    {
        Token token_operator = this->previous();
        std::shared_ptr<Expression> right = unary();
        expr = std::make_shared<Binary> (expr, std::move(token_operator), right);
    }

    return expr;
}

std::shared_ptr<Expression> Parser::unary()
{
    if(this->match(BANG, MINUS))
    {
        Token token_operator = this->previous();
        std::shared_ptr<Expression> right = unary();
        return std::make_shared<Unary> (std::move(token_operator), right);
    }

    return this->primary();
}

std::shared_ptr<Expression> Parser::primary()
{
    if(this->match(FALSE)) return std::make_shared < Literal > (false);
    if(this->match(TRUE)) return std::make_shared < Literal > (true);
    if(this->match(NULL)) return std::make_shared < Literal > (nullptr);

    if(this->match(NUMBER, STRING))
    {
        return std::make_shared<Literal> (this->previous().literal);
    }

    if(this->match(IDENTIFIER))
    {
        return std::make_shared<Variable>(previous());
    }

    if(this->match(LEFT_PAREN))
    {
        std::shared_ptr<Expression> expr = expression();
        consume(RIGHT_PAREN, "Expected a \")\" after expression");
        return std::make_shared < Grouping > (expr);
    }

    throw this->error(this->peek(), "Expected expression");
}

template<class...T>
bool Parser::match(T...type)
{
    assert((... && std::is_same_v<T, TokenType> ));

    if((... || this->check(type)))
    {
        this->advance();
        return true;
    }

    return false;
}

Token Parser::consume(TokenType type, std::string message)
{
    if(this->check(type)) return this->advance();
    throw this->error(this->peek(), message);
}

bool Parser::check(TokenType type)
{
    if(this->at_end()) return false;
    return this->peek().type == type;
}

Token Parser::advance()
{
    if(!this->at_end()) ++current;
    return this->previous();
}

bool Parser::at_end()
{
    return this->peek().type == END_OF_FILE;
}

Token Parser::peek()
{
    return tokens.at(current);
}

Token Parser::previous()
{
    return tokens.at(current - 1);
}

Parser::ParseError Parser::error(const Token &token, std::string message)
{
    ::error(token, message);
    return ParseError("");
}

void Parser::synchronize()
{
    this->advance();

    while(!this->at_end())
    {
        if(this->previous().type == SEMICOLON) return;

        switch(this->peek().type)
        {
            case CLASS:
            case FUNCTION:
            case VAR:
            case FOR:
            case IF:
            case WHILE:
            case PRINT:
            case PRINTLN:
            case RETURN:
                return;
        }
        this->advance();
    }
}