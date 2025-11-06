#include "core/core.hpp"

std::string AST::print(std::shared_ptr < Expression > expression)
{
    return std::any_cast < std::string > (expression -> accept(*this));
}

std::any AST::visit_binary_expression(std::shared_ptr < Binary > expression)
{
    return parenthesize(expression -> operator_token.lexeme,
        expression -> left, expression -> right);
}

std::any AST::visit_grouping_expression(std::shared_ptr < Grouping > expression) 
{
      return parenthesize("group", expression -> expression);
}

std::any AST::visit_literal_expression(std::shared_ptr < Literal > expression)
{
    auto & value_type = expression -> value.type();

    if (value_type == typeid(nullptr))
    {
        return "null";
    }
    else if (value_type == typeid(std::string))
    {
        return std::any_cast < std::string > (expression -> value);
    }
    else if (value_type == typeid(double))
    {
        return std::to_string(std::any_cast < double > (expression -> value));
    }
    else if (value_type == typeid(bool))
    {
        return std::any_cast < bool > (expression -> value) ? "true" : "false";
    }

    return "Error in Literal Expression: literal type is not recognized.";
}

std::any AST::visit_unary_expression(std::shared_ptr < Unary > expression)
{
    return parenthesize(expression -> operator_token.lexeme, expression -> right);
}

template<class...E>
std::string AST::parenthesize(std::string_view name, E...expression)
{
    assert((... && std::is_same_v < E, std::shared_ptr < Expression>> ));

    std::ostringstream builder;

    builder << "(" << name;
    ((builder << " " << print(expression)), ...);
    builder << ")";

    return builder.str();
}