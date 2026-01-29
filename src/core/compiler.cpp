#include "core/core.hpp"

Chunk Compiler::compile(const std::vector<std::shared_ptr<Stmt>> &statements)
{
    for(auto &stmt : statements)
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

    while(!locals.empty() && locals.back().depth > scope_depth)
    {
        emit(OpCode::POP);
        locals.pop_back();
    }
}

int Compiler::resolve_local(const std::string &name)
{
    for(int i = locals.size() - 1; i >= 0; i--)
    {
        if (locals[i].name == name)
            return i;
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

void Compiler::patch_jump(int offset)
{
    int jump = chunk.code.size() - offset - 2;
    if(jump > UINT16_MAX)
        throw std::runtime_error("Jump too large");

    chunk.code[offset] = (jump >> 8) & 0xff;
    chunk.code[offset + 1] = jump & 0xff;
}

void Compiler::emit_loop(int loop_start)
{
    int offset = chunk.code.size() - loop_start + 3;
    emit(OpCode::LOOP);
    if(offset > UINT16_MAX)
        throw std::runtime_error("Loop too large");
    emit_byte((offset >> 8) & 0xff);
    emit_byte(offset & 0xff);
}

Value Compiler::visit_literal_expression(std::shared_ptr<Literal> expr)
{
    if(std::holds_alternative<std::nullptr_t>(expr->value)) emit(OpCode::NULL);
    else if(std::holds_alternative<bool>(expr->value)) emit(std::get<bool>(expr->value) ? OpCode::TRUE : OpCode::FALSE);
    else emit_constant(expr->value);

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

    switch(expr->operator_token.type)
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

    switch(expr->operator_token.type)
    {
        case PLUS: emit(OpCode::ADD); break;
        case MINUS: emit(OpCode::SUBTRACT); break;
        case STAR: emit(OpCode::MULTIPLY); break;
        case SLASH: emit(OpCode::DIVIDE); break;

        case EQUAL_EQUAL: emit(OpCode::EQUAL); break;
        case GREATER: emit(OpCode::GREATER); break;
        case LESS: emit(OpCode::LESS); break;

        default:
            throw std::runtime_error("Unsupported binary operator");
    }

    return {};
}

Value Compiler::visit_expression_stmt(std::shared_ptr<ExprStmt> stmt)
{
    stmt->expression->accept(*this);
    emit(OpCode::POP);
    return {};
}

Value Compiler::visit_print_stmt(std::shared_ptr<PrintStmt> stmt)
{
    stmt->expression->accept(*this);
    emit(OpCode::PRINT);
    return {};
}

Value Compiler::visit_println_stmt(std::shared_ptr<PrintLnStmt> stmt)
{
    stmt->expression->accept(*this);
    emit(OpCode::PRINTLN);
    return {};
}

Value Compiler::visit_var_stmt(std::shared_ptr<VarStmt> stmt)
{
    if(scope_depth > 0)
    {
        if(stmt->initializer) stmt->initializer->accept(*this);
        else emit(OpCode::NULL);

        locals.push_back({ stmt->name.lexeme, scope_depth });
    }
    else
    {
        if(stmt->initializer) stmt->initializer->accept(*this);
        else emit(OpCode::NULL);

        uint8_t name = chunk.add_constant(stmt->name.lexeme);
        emit(OpCode::DEFINE_GLOBAL);
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
    if(stmt->else_branch)
    {
        end_jump = emit_jump(OpCode::JUMP);
    }

    patch_jump(else_jump);

    if(stmt->else_branch)
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
    emit(OpCode::POP); 
    stmt->body->accept(*this);
    emit_loop(loop_start);
    patch_jump(exit_jump);
    emit(OpCode::POP);

    return {};
}


Value Compiler::visit_logical_expression(std::shared_ptr<Logical> expr)
{
    expr->left->accept(*this);
    int short_circuit_jump = emit_jump(expr->operator_token.type == OR ? OpCode::JUMP_IF_TRUE : OpCode::JUMP_IF_FALSE);
    expr->right->accept(*this);
    patch_jump(short_circuit_jump);

    return {};
}


Value Compiler::visit_assign_expression(std::shared_ptr<Assign> expr)
{
    expr->value->accept(*this);
    int slot = resolve_local(expr->name.lexeme);
    if (slot != -1)
    {
        emit(OpCode::SET_LOCAL);
        emit_byte(slot);
    }
    else
    {
        uint8_t name = chunk.add_constant(expr->name.lexeme);
        emit(OpCode::SET_GLOBAL);
        emit_byte(name);
    }
    return {};
}


Value Compiler::visit_variable_expression(std::shared_ptr<Variable> expr)
{
    int slot = resolve_local(expr->name.lexeme);
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
    for(auto& s : stmt->statements) s->accept(*this);
    end_scope();
    return {};
}