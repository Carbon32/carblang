class VM
{
    Chunk* chunk = nullptr;
    uint8_t* ip = nullptr;
    std::vector<Value> stack;
    std::unordered_map<std::string, Value> globals;
    std::vector<CallFrame> frames;

public:
    void interpret(Chunk& new_chunk);
    void init_globals();

private:
    void run();
    Value pop();
    void push(Value value);
    uint16_t read_short();
    bool is_truthy(const Value& value);
    bool value_shallow_equal(const Value& x, const Value& y);
    void print_value(const Value& value);
    std::string stringify(const Value& value);
    void dump_state(const char* where);
    Value read_constant();
};