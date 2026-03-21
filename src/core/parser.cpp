#include "core/core.hpp"

Parser::Parser(const std::vector<Token> &tokens) : tokens{tokens} {}

std::vector<std::shared_ptr<Stmt>> Parser::parse()
{
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!this->at_end())
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
        if (this->match(FUNCTION))
            return function_declaration();
        if (this->match(VAR))
            return this->var_declaration();
        if (this->match(CONST))
            return this->const_declaration();
        if (this->match(INCLUDE))
            return include_statement();
        if (this->match(CLASS))
            return class_declaration();
        return this->statement();
    }
    catch (ParseError error)
    {
        this->synchronize();
        return nullptr;
    }
}

std::shared_ptr<Stmt> Parser::statement()
{
    if (this->match(IF))
        return if_statement();
    if (this->match(WHILE))
        return while_statement();
    if (this->match(FOR))
        return for_statement();
    if (this->match(RETURN))
        return return_statement();
    if (this->match(LEFT_BRACE))
        return ANG<BlockStmt>(block());
    return expression_statement();
}

std::shared_ptr<Stmt> Parser::class_declaration()
{
    Token name = consume(IDENTIFIER, "Expected a class name");

    std::shared_ptr<Variable> super_class = nullptr;
    if (match(COLON))
    {
        consume(IDENTIFIER, "Expected a super class name");
        super_class = ANG<Variable>(previous());
    }

    consume(LEFT_BRACE, "Expected \"{\" before class body");

    std::vector<std::shared_ptr<FunctionStmt>> methods;
    while (!check(RIGHT_BRACE) && !at_end())
    {
        consume(FUNCTION, "Expected \"function\" before method");
        auto method = std::dynamic_pointer_cast<FunctionStmt>(function_declaration());
        method->is_method = true;
        methods.push_back(method);
    }

    consume(RIGHT_BRACE, "Expected \"}\" after class body");

    return ANG<ClassStmt>(name, super_class, methods);
}

std::shared_ptr<Stmt> Parser::include_statement()
{
    Token file_token = consume(STRING, "Expected a file name after #include");
    if (match(SEMICOLON))
    {
    }
    return ANG<IncludeStmt>(std::get<std::string>(file_token.literal));
}

std::shared_ptr<Stmt> Parser::return_statement()
{
    Token keyword = previous();
    std::shared_ptr<Expression> value = nullptr;

    if (!check(SEMICOLON))
        value = expression();

    consume(SEMICOLON, "Expected \";\" after return value");
    return ANG<ReturnStmt>(keyword, value);
}

std::shared_ptr<Stmt> Parser::for_statement()
{
    this->consume(LEFT_PAREN, "Expected \"(\" after \"for\"");

    std::shared_ptr<Stmt> initializer;
    if (this->match(SEMICOLON))
    {
        initializer = nullptr;
    }
    else if (this->match(VAR))
    {
        initializer = var_declaration();
    }
    else
    {
        initializer = expression_statement();
    }

    std::shared_ptr<Expression> condition = nullptr;
    if (!check(SEMICOLON))
        condition = expression();
    this->consume(SEMICOLON, "Expected \";\" after loop condition");

    std::shared_ptr<Expression> increment = nullptr;
    if (!check(RIGHT_PAREN))
        increment = expression();
    this->consume(RIGHT_PAREN, "Expected \")\" after for clauses");

    std::shared_ptr<Stmt> body = statement();

    if (increment != nullptr)
    {
        body = ANG<BlockStmt>(std::vector<std::shared_ptr<Stmt>>{body, ANG<ExprStmt>(increment)});
    }

    if (condition == nullptr)
        condition = ANG<Literal>(true);
    body = ANG<WhileStmt>(condition, body);

    if (initializer != nullptr)
    {
        body = ANG<BlockStmt>(std::vector<std::shared_ptr<Stmt>>{initializer, body});
    }

    return body;
}

std::shared_ptr<Stmt> Parser::if_statement()
{
    this->consume(LEFT_PAREN, "Expected \"(\" after \"if\"");
    std::shared_ptr<Expression> condition = this->expression();
    this->consume(RIGHT_PAREN, "Expected \")\" after if condition");

    std::shared_ptr<Stmt> then_branch = statement();
    std::shared_ptr<Stmt> else_branch = nullptr;
    if (this->match(ELSE))
    {
        else_branch = statement();
    }

    return ANG<IfStmt>(condition, then_branch, else_branch);
}

std::shared_ptr<Stmt> Parser::while_statement()
{
    this->consume(LEFT_PAREN, "Expected \"(\" after \"while\"");
    std::shared_ptr<Expression> condition = this->expression();
    this->consume(RIGHT_PAREN, "Expected \")\" after condition");
    std::shared_ptr<Stmt> body = statement();

    return ANG<WhileStmt>(condition, body);
}

std::shared_ptr<Stmt> Parser::var_declaration()
{
    Token name = this->consume(IDENTIFIER, "Expected a variable name");

    std::shared_ptr<Expression> initializer = nullptr;
    if (match(EQUAL))
        initializer = expression();

    this->consume(SEMICOLON, "Expected \";\" after variable declaration");
    return ANG<VarStmt>(std::move(name), initializer);
}

std::shared_ptr<Stmt> Parser::const_declaration()
{
    Token name = consume(IDENTIFIER, "Expected a variable name");

    consume(EQUAL, "Const variables must be initialized");

    std::shared_ptr<Expression> initializer = expression();

    consume(SEMICOLON, "Expected \";\" after const declaration");

    return ANG<VarStmt>(std::move(name), initializer, true);
}

std::shared_ptr<Stmt> Parser::function_declaration()
{
    Token name = consume(IDENTIFIER, "Expected function name");
    consume(LEFT_PAREN, "Expected \"(\" after function name");

    std::vector<Token> parameters;
    if (!check(RIGHT_PAREN))
    {
        do
        {
            if (parameters.size() >= 255)
                error(peek(), "Can\"t have more than 255 parameters");
            parameters.push_back(consume(IDENTIFIER, "Expected parameter name"));
        } while (match(COMMA));
    }

    consume(RIGHT_PAREN, "Expected \")\" after parameters");
    consume(LEFT_BRACE, "Expected \"{\" before function body");

    std::vector<std::shared_ptr<Stmt>> body = block();

    return ANG<FunctionStmt>(std::move(name), std::move(parameters), std::move(body));
}

std::shared_ptr<Stmt> Parser::expression_statement()
{
    std::shared_ptr<Expression> expr = this->expression();
    this->consume(SEMICOLON, "Expected \";\" after expression");
    return ANG<ExprStmt>(expr);
}

std::vector<std::shared_ptr<Stmt>> Parser::block()
{
    std::vector<std::shared_ptr<Stmt>> statements;

    while (!this->check(RIGHT_BRACE) && !this->at_end())
    {
        statements.push_back(declaration());
    }

    this->consume(RIGHT_BRACE, "Expected \"}\" after block");
    return statements;
}

std::shared_ptr<Expression> Parser::assignment()
{
    std::shared_ptr<Expression> expr = or_expression();

    if (this->match(EQUAL))
    {
        Token equals = this->previous();
        std::shared_ptr<Expression> value = assignment();

        if (auto var = dynamic_cast<Variable *>(expr.get()))
        {
            Token name = var->name;
            return ANG<Assign>(std::move(name), value);
        }

        if (auto idx = dynamic_cast<IndexExpr *>(expr.get()))
        {
            return ANG<IndexAssign>(idx->array, idx->index, value);
        }

        if (auto get = dynamic_cast<Get *>(expr.get()))
        {
            return ANG<Set>(get->object, get->name, value);
        }

        error(std::move(equals), "Invalid assignment target");
    }

    return expr;
}

std::shared_ptr<Expression> Parser::or_expression()
{
    std::shared_ptr<Expression> expr = and_expression();
    while (this->match(OR))
    {
        Token op = this->previous();
        std::shared_ptr<Expression> right = and_expression();
        expr = ANG<Logical>(expr, std::move(op), right);
    }
    return expr;
}

std::shared_ptr<Expression> Parser::and_expression()
{
    std::shared_ptr<Expression> expr = equality();
    while (this->match(AND))
    {
        Token op = this->previous();
        std::shared_ptr<Expression> right = equality();
        expr = ANG<Logical>(expr, std::move(op), right);
    }
    return expr;
}

std::shared_ptr<Expression> Parser::equality()
{
    std::shared_ptr<Expression> expr = comparison();
    while (this->match(BANG_EQUAL, EQUAL_EQUAL))
    {
        Token op = previous();
        std::shared_ptr<Expression> right = comparison();
        expr = ANG<Binary>(expr, std::move(op), right);
    }
    return expr;
}

std::shared_ptr<Expression> Parser::comparison()
{
    std::shared_ptr<Expression> expr = term();
    while (this->match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL))
    {
        Token op = previous();
        std::shared_ptr<Expression> right = term();
        expr = ANG<Binary>(expr, std::move(op), right);
    }
    return expr;
}

std::shared_ptr<Expression> Parser::term()
{
    std::shared_ptr<Expression> expr = factor();
    while (this->match(MINUS, PLUS))
    {
        Token op = previous();
        std::shared_ptr<Expression> right = factor();
        expr = ANG<Binary>(expr, std::move(op), right);
    }
    return expr;
}

std::shared_ptr<Expression> Parser::factor()
{
    std::shared_ptr<Expression> expr = unary();
    while (this->match(SLASH, STAR))
    {
        Token op = previous();
        std::shared_ptr<Expression> right = unary();
        expr = ANG<Binary>(expr, std::move(op), right);
    }
    return expr;
}

std::shared_ptr<Expression> Parser::unary()
{
    if (this->match(BANG, MINUS))
    {
        Token op = previous();
        std::shared_ptr<Expression> right = unary();
        return ANG<Unary>(std::move(op), right);
    }
    return call();
}

std::shared_ptr<Expression> Parser::call()
{
    std::shared_ptr<Expression> expr = primary();
    while (true)
    {
        if (match(LEFT_PAREN))
            expr = finish_call(expr);
        else if (match(LEFT_BRACKET))
        {
            auto index = expression();
            consume(RIGHT_BRACKET, "Expected \"]\" after index");
            expr = ANG<IndexExpr>(expr, index);
        }
        else if (match(DOT))
        {
            Token name = consume(IDENTIFIER, "Expected property name after \".\"");
            expr = ANG<Get>(expr, name);
        }
        else
            break;
    }
    return expr;
}

std::shared_ptr<Expression> Parser::finish_call(std::shared_ptr<Expression> callee)
{
    std::vector<std::shared_ptr<Expression>> arguments;
    if (!check(RIGHT_PAREN))
    {
        do
        {
            if (arguments.size() >= 255)
                error(peek(), "Can\"t have more than 255 arguments");
            arguments.push_back(expression());
        } while (match(COMMA));
    }

    Token paren = consume(RIGHT_PAREN, "Expected \")\" after arguments");
    return ANG<Call>(std::move(callee), std::move(paren), std::move(arguments));
}

std::shared_ptr<Expression> Parser::primary()
{
    if (match(FALSE))
        return ANG<Literal>(false);
    if (match(TRUE))
        return ANG<Literal>(true);
    if (match(NULL))
        return ANG<Literal>(nullptr);

    if (match(NUMBER, STRING))
        return ANG<Literal>(previous().literal);
    if (match(IDENTIFIER))
        return ANG<Variable>(previous());

    if (match(LEFT_BRACKET))
    {
        std::vector<std::shared_ptr<Expression>> elements;
        if (!check(RIGHT_BRACKET))
        {
            do
                elements.push_back(expression());
            while (match(COMMA));
        }
        consume(RIGHT_BRACKET, "Expected \"]\" after array literal");
        return ANG<ArrayExpr>(std::move(elements));
    }

    if (match(LEFT_PAREN))
    {
        auto expr = expression();
        consume(RIGHT_PAREN, "Expected \")\" after expression");
        return ANG<Grouping>(expr);
    }

    if (match(TokenType::THIS))
        return ANG<This>(previous());

    if (match(LEFT_BRACE))
    {
        std::vector<std::pair<std::shared_ptr<Expression>, std::shared_ptr<Expression>>> entries;
        if (!check(RIGHT_BRACE))
        {
            do
            {
                std::shared_ptr<Expression> key;
                if (match(STRING))
                    key = ANG<Literal>(previous().literal);
                else if (match(IDENTIFIER))
                    key = ANG<Literal>(previous().lexeme);
                else
                    throw error(peek(), "Expected string or identifier as dictionary key");

                consume(COLON, "Expected \":\" after dictionary key");
                auto value = expression();
                entries.push_back({key, value});
            } while (match(COMMA));
        }
        consume(RIGHT_BRACE, "Expected \"}\" after dictionary literal");
        return ANG<DictExpr>(std::move(entries));
    }

    throw error(peek(), "Expected expression");
}

template <class... T>
bool Parser::match(T... type)
{
    assert((... && std::is_same_v<T, TokenType>));

    if ((... || this->check(type)))
    {
        this->advance();
        return true;
    }

    return false;
}

Token Parser::consume(TokenType type, std::string message)
{
    if (this->check(type))
        return this->advance();
    throw this->error(this->peek(), message);
}

bool Parser::check(TokenType type)
{
    if (this->at_end())
        return false;
    return this->peek().type == type;
}

Token Parser::advance()
{
    if (!this->at_end())
        ++current;
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

    while (!this->at_end())
    {
        if (this->previous().type == SEMICOLON)
            return;

        switch (this->peek().type)
        {
        case CLASS:
        case FUNCTION:
        case VAR:
        case FOR:
        case IF:
        case WHILE:
        case RETURN:
            return;
        }
        this->advance();
    }
}