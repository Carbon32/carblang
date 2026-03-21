class Compiler : public ExpressionVisitor, public StmtVisitor
{
    Chunk chunk;
    struct Local
    {
        std::string name;
        int depth;
        bool is_const = false;
    };

    std::vector<Local> locals;
    std::set<std::string> included_files;
    int scope_depth = 0;

public:
    Chunk compile(const std::vector<std::shared_ptr<Stmt>> &statements);
    void begin_scope();
    void end_scope();
    int resolve_local(const std::string &name);

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
    Value visit_set_expression(std::shared_ptr<Set> expr) override;
    Value visit_super_expression(std::shared_ptr<Super> expr) override;
    Value visit_this_expression(std::shared_ptr<This> expr) override;
    Value visit_dict_expression(std::shared_ptr<DictExpr> expr) override;

    Value visit_expression_stmt(std::shared_ptr<ExprStmt> stmt) override;
    Value visit_var_stmt(std::shared_ptr<VarStmt> stmt) override;
    Value visit_if_stmt(std::shared_ptr<IfStmt> stmt) override;
    Value visit_while_stmt(std::shared_ptr<WhileStmt> stmt) override;
    Value visit_block_stmt(std::shared_ptr<BlockStmt> stmt) override;
    Value visit_function_stmt(std::shared_ptr<FunctionStmt> stmt) override;
    Value visit_return_stmt(std::shared_ptr<ReturnStmt> stmt) override;
    Value visit_include_stmt(std::shared_ptr<IncludeStmt> stmt) override;
    Value visit_class_stmt(std::shared_ptr<ClassStmt> stmt) override;

private:
    void emit(OpCode op);
    void emit_byte(uint8_t byte);
    void emit_constant(Value value);
    std::shared_ptr<Function> compile_function(FunctionStmt &stmt, bool is_constructor, bool is_method);
    int emit_jump(OpCode op);
    void patch_jump(int offset);
    void emit_loop(int loop_start);
};