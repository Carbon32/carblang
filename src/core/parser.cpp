#include "core/core.hpp"

Parser::Parser(const std::vector <Token>& tokens) : tokens{tokens} 
{}

std::shared_ptr<Expression> Parser::parse()
{
    try
    {
        return expression();
    }
    catch (ParseError error)
    {
        return nullptr;
    }
}

std::shared_ptr<Expression> Parser::expression()
{
    return equality();
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
            case RETURN:
                return;
        }
        this->advance();
    }
}