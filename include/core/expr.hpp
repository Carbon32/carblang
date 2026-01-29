struct ExpressionVisitor
{
    virtual Value visit_binary_expression(std::shared_ptr<Binary> expr) = 0;
    virtual Value visit_grouping_expression(std::shared_ptr<Grouping> expr) = 0;
    virtual Value visit_literal_expression(std::shared_ptr<Literal> expr) = 0;
    virtual Value visit_unary_expression(std::shared_ptr<Unary> expr) = 0;
    virtual Value visit_logical_expression(std::shared_ptr<Logical> expr) = 0;
    virtual Value visit_assign_expression(std::shared_ptr<Assign> expr) = 0;
    virtual Value visit_variable_expression(std::shared_ptr<Variable> expr) = 0;
    virtual ~ExpressionVisitor() = default;
};

struct Expression
{
    virtual Value accept(ExpressionVisitor& visitor) = 0;
};

struct Binary : Expression, public std::enable_shared_from_this<Binary>
{
    Binary(std::shared_ptr<Expression> left, Token operator_token, std::shared_ptr<Expression> right) : left{std::move(left)}, operator_token{std::move(operator_token)}, right{std::move(right)}
    {}

    Value accept(ExpressionVisitor& visitor) override
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

    Value accept(ExpressionVisitor& visitor) override
    {
        return visitor.visit_grouping_expression(shared_from_this());
    }

    const std::shared_ptr<Expression> expression;
};

struct Literal : Expression, public std::enable_shared_from_this<Literal>
{
    Literal(Value value) : value{std::move(value)}
    {}

    Value accept(ExpressionVisitor& visitor) override
    {
        return visitor.visit_literal_expression(shared_from_this());
    }

  const Value value;
};

struct Logical : Expression, public std::enable_shared_from_this<Logical>
{
    Logical(std::shared_ptr<Expression> left, Token operator_token, std::shared_ptr<Expression> right) : left{std::move(left)}, operator_token{std::move(operator_token)}, right{std::move(right)}
    {}

    Value accept(ExpressionVisitor& visitor) override
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

    Value accept(ExpressionVisitor& visitor) override
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

    Value accept(ExpressionVisitor& visitor) override
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

    Value accept(ExpressionVisitor& visitor) override
    {
        return visitor.visit_variable_expression(shared_from_this());
    }

    const Token name;
};