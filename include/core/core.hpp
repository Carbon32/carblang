#ifndef CORE_HPP
#define CORE_HPP

#include "deps.hpp"
#include "opcode.hpp"
#include "token_type.hpp"

inline bool error_trigger = false;
inline bool runtime_error_trigger = false;

#include "forward.hpp"
#include "native.hpp"
#include "define.hpp"
#include "array_methods.hpp"
#include "primitive_methods.hpp"
#include "globals_methods.hpp"

std::shared_ptr<BoundMethod> make_native_method(Value value, NativeMethod method);

#include "vm.hpp"
#include "error.hpp"
#include "token.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "scanner.hpp"
#include "parser.hpp"

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

class Compiler : public ExpressionVisitor, public StmtVisitor
{
    Chunk chunk;
    struct Local
    {
        std::string name;
        int depth;
    };

    std::vector<Local> locals;
    int scope_depth = 0;


    public:
        Chunk compile(const std::vector<std::shared_ptr<Stmt>>& statements);
        void begin_scope();
        void end_scope();
        int resolve_local(const std::string& name);

        Value visit_literal_expression(std::shared_ptr<Literal> expr) override;
        Value visit_grouping_expression(std::shared_ptr<Grouping> expr) override;
        Value visit_unary_expression(std::shared_ptr<Unary> expr) override;
        Value visit_binary_expression(std::shared_ptr<Binary> expr) override;
        Value visit_logical_expression(std::shared_ptr<Logical> expr) override;
        Value visit_assign_expression(std::shared_ptr<Assign> expr) override;
        Value visit_variable_expression(std::shared_ptr<Variable> expr) override;
        Value visit_call_expression(std::shared_ptr<Call> expr) override;
        Value visit_array_expression(std::shared_ptr<ArrayExpr> expr) override;
        Value visit_index_expression(std::shared_ptr<IndexExpr> expr) override;
        Value visit_index_assign_expression(std::shared_ptr<IndexAssign> expr) override;
        Value visit_get_expression(std::shared_ptr<Get> expr) override;

        Value visit_expression_stmt(std::shared_ptr<ExprStmt> stmt) override;
        Value visit_print_stmt(std::shared_ptr<PrintStmt> stmt) override;
        Value visit_println_stmt(std::shared_ptr<PrintLnStmt> stmt) override;
        Value visit_var_stmt(std::shared_ptr<VarStmt> stmt) override;
        Value visit_if_stmt(std::shared_ptr<IfStmt> stmt) override;
        Value visit_while_stmt(std::shared_ptr<WhileStmt> stmt) override;
        Value visit_block_stmt(std::shared_ptr<BlockStmt> stmt) override;
        Value visit_function_stmt(std::shared_ptr<FunctionStmt> stmt) override;
        Value visit_return_stmt(std::shared_ptr<ReturnStmt> stmt) override;
        Value visit_include_stmt(std::shared_ptr<IncludeStmt> stmt) override;

    private:
        void emit(OpCode op);
        void emit_byte(uint8_t byte);
        void emit_constant(Value value);
        int emit_jump(OpCode op);
        void patch_jump(int offset);
        void emit_loop(int loop_start);
};

#endif