#include "core/core.hpp"

Chunk Compiler::compile(const std::vector<std::shared_ptr<Stmt>> &statements)
{
    for (auto &stmt : statements)
    {
        stmt->accept(*this);
    }

    emit(OpCode::RETURN);
    return chunk;
}

void Compiler::begin_scope()
{
    scope_depth++;
}

void Compiler::end_scope()
{
    scope_depth--;

    while (!locals.empty() && locals.back().depth > scope_depth)
    {
        emit(OpCode::POP);
        locals.pop_back();
    }
}

int Compiler::resolve_local(const std::string &name)
{
    for (int i = locals.size() - 1; i >= 0; i--)
    {
        if (locals[i].name == name)
        {

            return i;
        }
    }
    return -1;
}

void Compiler::emit(OpCode op)
{
    chunk.write(static_cast<uint8_t>(op));
}

void Compiler::emit_byte(uint8_t byte)
{
    chunk.write(byte);
}

void Compiler::emit_constant(Value value)
{
    emit(OpCode::CONSTANT);
    emit_byte(chunk.add_constant(std::move(value)));
}

int Compiler::emit_jump(OpCode op)
{
    emit(op);
    emit_byte(0xff);
    emit_byte(0xff);
    int offset_index = chunk.code.size() - 2;
    return offset_index;
}

std::optional<std::string> Compiler::infer_expression_type(std::shared_ptr<Expression> expr)
{
    if (auto lit = std::dynamic_pointer_cast<Literal>(expr))
    {
        const auto &v = lit->value;

        if (std::holds_alternative<double>(v))
            return "number";

        if (std::holds_alternative<std::string>(v))
            return "string";

        if (std::holds_alternative<bool>(v))
            return "bool";

        if (std::holds_alternative<std::nullptr_t>(v))
            return "null";

        if (std::holds_alternative<std::shared_ptr<Function>>(v))
            return "function";

        if (std::holds_alternative<std::shared_ptr<Class>>(v))
            return "class";

        if (std::holds_alternative<std::shared_ptr<Instance>>(v))
            return "instance";

        if (std::holds_alternative<std::shared_ptr<UserBoundMethod>>(v))
            return "bound_method";

        if (std::holds_alternative<std::shared_ptr<BoundMethod>>(v))
            return "native_method";

        if (std::holds_alternative<std::shared_ptr<Tag>>(v))
            return "tag";

        if (std::holds_alternative<std::shared_ptr<Texture2D>>(v))
            return "texture2d";

        if (std::holds_alternative<std::shared_ptr<TextElement>>(v))
            return "text_element";

        if (std::holds_alternative<std::shared_ptr<Dict>>(v))
            return "dict";

        if (std::holds_alternative<std::shared_ptr<Array>>(v))
            return "array";
    }

    if (auto call = std::dynamic_pointer_cast<Call>(expr))
    {
        if (auto get =
                std::dynamic_pointer_cast<Get>(call->callee))
        {
            // I will fix this later
        }
    }

    if (std::dynamic_pointer_cast<ArrayExpr>(expr))
        return "array";

    if (std::dynamic_pointer_cast<DictExpr>(expr))
        return "dict";

    if (auto var = std::dynamic_pointer_cast<Variable>(expr))
    {
        int slot = resolve_local(var->name.lexeme);

        if (slot != -1)
        {
            auto &local = locals[slot];

            if (local.type_name)
                return local.type_name;
        }

        auto it = global_types.find(var->name.lexeme);

        if (it != global_types.end())
            return it->second;
    }

    if (auto unary = std::dynamic_pointer_cast<Unary>(expr))
    {
        switch (unary->operator_token.type)
        {
        case MINUS:
            return "number";

        case BANG:
            return "bool";

        default:
            break;
        }
    }

    if (auto binary = std::dynamic_pointer_cast<Binary>(expr))
    {
        auto left = infer_expression_type(binary->left);
        auto right = infer_expression_type(binary->right);

        switch (binary->operator_token.type)
        {
        case PLUS:
            if (!left || !right)
                return std::nullopt;

            if (*left == "string" && *right == "string")
                return "string";
            if (*left == "array" && *right == "array")
                return "array";
            if (*left == "number" && *right == "number")
                return "number";

            return std::nullopt;

        case MINUS:
        case STAR:
        case SLASH:
        case PERCENT:
        case STAR_STAR:
            return "number";

        case GREATER:
        case GREATER_EQUAL:
        case LESS:
        case LESS_EQUAL:
        case EQUAL_EQUAL:
        case BANG_EQUAL:
            return "bool";

        default:
            break;
        }
    }

    if (auto call =
            std::dynamic_pointer_cast<Call>(expr))
    {
        if (auto var =
                std::dynamic_pointer_cast<Variable>(
                    call->callee))
        {
            auto it =
                function_signatures.find(
                    var->name.lexeme);

            if (it != function_signatures.end())
            {
                return it->second->return_type;
            }
        }
    }

    return std::nullopt;
}

std::shared_ptr<Function> Compiler::compile_function(FunctionStmt &stmt, bool is_constructor, bool is_method)
{
    Compiler function_compiler;
    function_compiler.in_function = true;
    function_compiler.current_return_type = stmt.return_type;
    function_compiler.begin_scope();

    if (is_method)
        function_compiler.locals.push_back({"this", 0, true});

    std::vector<std::optional<std::string>> param_types;
    int slot = function_compiler.locals.size();
    for (auto &param : stmt.params)
    {
        function_compiler.locals.push_back({param.name.lexeme,
                                            0,
                                            false,
                                            param.type_name});
        param_types.push_back(param.type_name);
        slot++;
    }

    for (auto &s : stmt.body)
        s->accept(function_compiler);

    if (!is_constructor)
        function_compiler.emit(OpCode::NULL);
    else
        function_compiler.emit_constant(IgnoreReturnValue{});

    function_compiler.emit(OpCode::RETURN);

    return std::make_shared<Function>(
        stmt.name.lexeme,
        stmt.params.size(),
        function_compiler.chunk,
        param_types,
        stmt.return_type);
}

void Compiler::patch_jump(int offset)
{
    int jump = chunk.code.size() - offset - 2;
    if (jump > UINT16_MAX)
        throw std::runtime_error("Jump too large");

    chunk.code[offset] = (jump >> 8) & 0xff;
    chunk.code[offset + 1] = jump & 0xff;
}

void Compiler::emit_loop(int loop_start)
{
    int offset = chunk.code.size() - loop_start + 3;
    emit(OpCode::LOOP);
    if (offset > UINT16_MAX)
        throw std::runtime_error("Loop too large");
    emit_byte((offset >> 8) & 0xff);
    emit_byte(offset & 0xff);
}

Value Compiler::visit_literal_expression(std::shared_ptr<Literal> expr)
{
    if (std::holds_alternative<std::nullptr_t>(expr->value))
        emit(OpCode::NULL);
    else if (std::holds_alternative<bool>(expr->value))
        emit(std::get<bool>(expr->value) ? OpCode::TRUE : OpCode::FALSE);
    else
        emit_constant(expr->value);

    return {};
}

Value Compiler::visit_grouping_expression(std::shared_ptr<Grouping> expr)
{
    expr->expression->accept(*this);
    return {};
}

Value Compiler::visit_unary_expression(std::shared_ptr<Unary> expr)
{
    expr->right->accept(*this);
    switch (expr->operator_token.type)
    {
    case MINUS:
        emit(OpCode::NEGATE);
        break;
    case BANG:
        emit(OpCode::NOT);
        break;

    default:
        throw std::runtime_error("Invalid unary operator");
    }

    return {};
}

Value Compiler::visit_binary_expression(std::shared_ptr<Binary> expr)
{
    expr->left->accept(*this);
    expr->right->accept(*this);

    switch (expr->operator_token.type)
    {
    case PLUS:
        emit(OpCode::ADD);
        break;
    case MINUS:
        emit(OpCode::SUBTRACT);
        break;
    case STAR:
        emit(OpCode::MULTIPLY);
        break;
    case STAR_STAR:
        emit(OpCode::POWER);
        break;
    case SLASH:
        emit(OpCode::DIVIDE);
        break;
    case PERCENT:
        emit(OpCode::PERCENT);
        break;

    case EQUAL_EQUAL:
        emit(OpCode::EQUAL);
        break;
    case BANG_EQUAL:
        emit(OpCode::NOT_EQUAL);
        break;
    case GREATER:
        emit(OpCode::GREATER);
        break;
    case GREATER_EQUAL:
        emit(OpCode::GREATER_EQUAL);
        break;
    case LESS:
        emit(OpCode::LESS);
        break;
    case LESS_EQUAL:
        emit(OpCode::LESS_EQUAL);
        break;

    default:
        throw std::runtime_error("Unsupported binary operator");
    }

    return {};
}

Value Compiler::visit_expression_stmt(std::shared_ptr<ExprStmt> stmt)
{
    stmt->expression->accept(*this);
    if (!std::dynamic_pointer_cast<Assign>(stmt->expression))
        emit(OpCode::POP);

    return {};
}

Value Compiler::visit_var_stmt(std::shared_ptr<VarStmt> stmt)
{
    auto initializer_type = stmt->initializer ? infer_expression_type(stmt->initializer) : std::nullopt;

    if (scope_depth > 0 && in_function)
    {
        int slot_index = locals.size();
        locals.push_back({stmt->name.lexeme,
                          scope_depth,
                          stmt->is_const,
                          stmt->type_name});

        if (stmt->type_name && stmt->initializer)
        {
            if (!initializer_type)
            {
                throw std::runtime_error(
                    "Cannot infer type for initializer of \"" +
                    stmt->name.lexeme + "\"");
            }

            if (*initializer_type != *stmt->type_name)
            {
                throw std::runtime_error(
                    "Type mismatch for variable '" + stmt->name.lexeme +
                    "': declared '" + *stmt->type_name +
                    "', but initializer is '" + *initializer_type + "'");
            }
        }

        if (stmt->initializer)
            stmt->initializer->accept(*this);
        else
            emit(OpCode::NULL);

        emit(OpCode::SET_LOCAL);
        emit_byte(slot_index);
    }
    else
    {
        if (stmt->initializer)
        {
            if (stmt->type_name)
            {
                if (!initializer_type)
                    throw std::runtime_error(
                        "Cannot infer type for initializer of global \"" +
                        stmt->name.lexeme + "\"");

                if (*initializer_type != *stmt->type_name)
                    throw std::runtime_error(
                        "Type mismatch for global variable '" + stmt->name.lexeme +
                        "': declared '" + *stmt->type_name +
                        "', but initializer is '" + *initializer_type + "'");
            }

            stmt->initializer->accept(*this);
        }
        else
        {
            emit(OpCode::NULL);
        }

        uint8_t name = chunk.add_constant(stmt->name.lexeme);

        if (stmt->is_const)
            emit(OpCode::DEFINE_CONST);
        else
        {
            if (stmt->type_name)
                global_types[stmt->name.lexeme] = *stmt->type_name;
            emit(OpCode::DEFINE_GLOBAL);
        }
        emit_byte(name);
    }

    return {};
}

Value Compiler::visit_if_stmt(std::shared_ptr<IfStmt> stmt)
{
    stmt->condition->accept(*this);
    int else_jump = emit_jump(OpCode::JUMP_IF_FALSE);
    stmt->then_branch->accept(*this);

    int end_jump = 0;
    if (stmt->else_branch)
    {
        end_jump = emit_jump(OpCode::JUMP);
    }

    patch_jump(else_jump);

    if (stmt->else_branch)
    {
        stmt->else_branch->accept(*this);
        patch_jump(end_jump);
    }

    return {};
}

Value Compiler::visit_while_stmt(std::shared_ptr<WhileStmt> stmt)
{
    int loop_start = chunk.code.size();
    stmt->condition->accept(*this);
    int exit_jump = emit_jump(OpCode::JUMP_IF_FALSE);
    stmt->body->accept(*this);
    emit_loop(loop_start);
    patch_jump(exit_jump);

    return {};
}

Value Compiler::visit_logical_expression(std::shared_ptr<Logical> expr)
{
    expr->left->accept(*this);

    if (expr->operator_token.type == OR)
    {
        int jump = emit_jump(OpCode::JUMP_IF_TRUE);
        expr->right->accept(*this);

        int end_jump = emit_jump(OpCode::JUMP);

        patch_jump(jump);
        emit(OpCode::TRUE);

        patch_jump(end_jump);
    }
    else
    {
        int jump = emit_jump(OpCode::JUMP_IF_FALSE);
        expr->right->accept(*this);

        int end_jump = emit_jump(OpCode::JUMP);

        patch_jump(jump);
        emit(OpCode::FALSE);

        patch_jump(end_jump);
    }

    return {};
}

Value Compiler::visit_assign_expression(std::shared_ptr<Assign> expr)
{
    auto inferred = infer_expression_type(expr->value);

    int slot = in_function ? resolve_local(expr->name.lexeme) : -1;

    if (slot != -1)
    {
        auto &local = locals[slot];

        if (local.is_const)
        {
            throw std::runtime_error(
                "Cannot reassign const variable \"" +
                expr->name.lexeme + "\"");
        }

        if (local.type_name && inferred)
        {
            if (*local.type_name != *inferred)
            {
                throw std::runtime_error(
                    "Cannot assign " +
                    *inferred +
                    " to variable \"" +
                    expr->name.lexeme +
                    "\" of type " +
                    *local.type_name);
            }
        }

        expr->value->accept(*this);

        emit(OpCode::SET_LOCAL);
        emit_byte(slot);

        return {};
    }

    auto it = global_types.find(expr->name.lexeme);

    if (it != global_types.end() && inferred)
    {
        if (it->second != *inferred)
        {
            throw std::runtime_error(
                "Cannot assign " +
                *inferred +
                " to variable \"" +
                expr->name.lexeme +
                "\" of type " +
                it->second);
        }
    }

    expr->value->accept(*this);

    uint8_t name = chunk.add_constant(expr->name.lexeme);

    emit(OpCode::SET_GLOBAL);
    emit_byte(name);

    return {};
}

Value Compiler::visit_variable_expression(std::shared_ptr<Variable> expr)
{
    int slot = in_function ? resolve_local(expr->name.lexeme) : -1;
    if (slot != -1)
    {
        emit(OpCode::GET_LOCAL);
        emit_byte(slot);
    }
    else
    {
        uint8_t name = chunk.add_constant(expr->name.lexeme);
        emit(OpCode::GET_GLOBAL);
        emit_byte(name);
    }
    return {};
}

Value Compiler::visit_block_stmt(std::shared_ptr<BlockStmt> stmt)
{
    begin_scope();
    for (auto &s : stmt->statements)
        s->accept(*this);
    end_scope();
    return {};
}

Value Compiler::visit_function_stmt(std::shared_ptr<FunctionStmt> stmt)
{
    auto fn = compile_function(*stmt, false, false);

    uint8_t fn_index = chunk.add_constant(fn);
    emit(OpCode::CLOSURE);
    emit_byte(fn_index);

    uint8_t name_index = chunk.add_constant(stmt->name.lexeme);
    emit(OpCode::DEFINE_GLOBAL);
    emit_byte(name_index);

    fn->param_types.clear();
    for (auto &param : stmt->params)
        fn->param_types.push_back(param.type_name ? *param.type_name : "any");

    fn->return_type = stmt->return_type;
    function_signatures[stmt->name.lexeme] = fn;

    return {};
}

Value Compiler::visit_call_expression(std::shared_ptr<Call> expr)
{
    expr->callee->accept(*this);

    std::shared_ptr<Function> callee_fn = nullptr;

    if (auto var = std::dynamic_pointer_cast<Variable>(expr->callee))
    {
        auto it = function_signatures.find(var->name.lexeme);
        if (it != function_signatures.end())
            callee_fn = it->second;
    }

    if (callee_fn)
    {
        if (expr->arguments.size() != callee_fn->arity)
        {
            throw std::runtime_error(
                "Expected " + std::to_string(callee_fn->arity) +
                " arguments but got " + std::to_string(expr->arguments.size()));
        }

        for (size_t i = 0; i < expr->arguments.size(); i++)
        {
            auto arg_type = infer_expression_type(expr->arguments[i]);
            const auto &param_type = callee_fn->param_types[i];

            if (param_type != "any" && arg_type && *arg_type != param_type)
            {
                throw std::runtime_error(
                    "Argument " + std::to_string(i + 1) +
                    " expected type \"" + (param_type ? *param_type : "any") + "\"" +
                    ", got \"" + (arg_type ? *arg_type : "unknown") + "\"");
            }
        }
    }

    for (auto &arg : expr->arguments)
        arg->accept(*this);

    emit(OpCode::CALL);
    emit_byte(static_cast<uint8_t>(expr->arguments.size()));

    return {};
}

Value Compiler::visit_return_stmt(std::shared_ptr<ReturnStmt> stmt)
{
    std::optional<std::string> actual_type;

    if (stmt->value)
    {
        stmt->value->accept(*this);
        actual_type = infer_expression_type(stmt->value);
    }
    else
    {
        emit(OpCode::NULL);
        actual_type = "nothing";
    }

    if (current_return_type)
    {
        if (!actual_type)
        {
            throw std::runtime_error("Cannot infer return type");
        }

        if (*current_return_type != "any" &&
            *actual_type != *current_return_type)
        {
            throw std::runtime_error(
                "Expected return type \"" +
                *current_return_type +
                "\", got \"" +
                *actual_type + "\"");
        }
    }

    emit(OpCode::RETURN);

    return {};
}

Value Compiler::visit_array_expression(std::shared_ptr<ArrayExpr> expr)
{
    for (auto &element : expr->elements)
        element->accept(*this);

    emit(OpCode::ARRAY);
    emit_byte(static_cast<uint8_t>(expr->elements.size()));

    return {};
}

Value Compiler::visit_index_expression(std::shared_ptr<IndexExpr> expr)
{
    expr->array->accept(*this);
    expr->index->accept(*this);

    emit(OpCode::GET_INDEX);
    return {};
}

Value Compiler::visit_index_assign_expression(std::shared_ptr<IndexAssign> expr)
{
    expr->array->accept(*this);
    expr->index->accept(*this);
    expr->value->accept(*this);

    emit(OpCode::SET_INDEX);
    return {};
}

Value Compiler::visit_get_expression(std::shared_ptr<Get> expr)
{
    expr->object->accept(*this);
    uint8_t name = chunk.add_constant(expr->name.lexeme);
    emit(OpCode::GET_PROPERTY);
    emit_byte(name);
    return {};
}

Value Compiler::visit_set_expression(std::shared_ptr<Set> expr)
{
    expr->object->accept(*this);
    expr->value->accept(*this);
    uint8_t name_index = chunk.add_constant(expr->name.lexeme);
    emit(OpCode::SET_PROPERTY);
    emit_byte(name_index);
    return {};
}

Value Compiler::visit_include_stmt(std::shared_ptr<IncludeStmt> stmt)
{
#ifndef IGNORE_INCLUDE
    if (included_files.find(stmt->file_name) != included_files.end())
    {
        throw std::runtime_error("File already included: " + stmt->file_name);
    }

    included_files.insert(stmt->file_name);

    std::ifstream file(stmt->file_name);
    if (!file.is_open())
        throw std::runtime_error("Failed to open include file: " + stmt->file_name);

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scan_tokens();
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> stmts = parser.parse();

    for (auto &s : stmts)
    {
        s->accept(*this);
    }
#endif

#ifdef IGNORE_INCLUDE
    throw std::runtime_error("#include is disabled in this mode");
#endif

    return {};
}

Value Compiler::visit_class_stmt(std::shared_ptr<ClassStmt> stmt)
{
    bool has_super_class = stmt->super_class != nullptr;

    if (has_super_class)
    {
        stmt->super_class->accept(*this);
    }

    emit(OpCode::CLASS);
    uint8_t name_index = chunk.add_constant(stmt->name.lexeme);
    emit_byte(name_index);

    for (auto &method : stmt->methods)
    {
        bool is_constructor = method->name.lexeme == "init";
        auto fn = compile_function(*method, is_constructor, true);

        uint8_t fn_index = chunk.add_constant(fn);
        emit(OpCode::CLOSURE);
        emit_byte(fn_index);

        uint8_t method_index = chunk.add_constant(method->name.lexeme);
        emit(OpCode::METHOD);
        emit_byte(method_index);
    }

    if (has_super_class)
    {
        emit(OpCode::INHERIT);
    }

    emit(OpCode::DEFINE_GLOBAL);
    emit_byte(name_index);

    return {};
}

Value Compiler::visit_super_expression(std::shared_ptr<Super> expr)
{
    emit(OpCode::GET_SUPER);
    uint8_t name_index = chunk.add_constant(expr->method.lexeme);
    emit_byte(name_index);
    return {};
}

Value Compiler::visit_this_expression(std::shared_ptr<This> expr)
{
    emit(OpCode::GET_LOCAL);
    emit_byte(0);
    return {};
}

Value Compiler::visit_dict_expression(std::shared_ptr<DictExpr> expr)
{
    emit(OpCode::DICT);

    for (auto &[key, value] : expr->entries)
    {
        key->accept(*this);
        value->accept(*this);
        emit(OpCode::SET_ITEM);
    }

    return {};
}