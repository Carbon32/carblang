class VM
{
    Chunk* chunk = nullptr;
    uint8_t* ip = nullptr;
    std::vector<Value> stack;
    std::unordered_map<std::string, Value> globals;

public:
    void interpret(Chunk& new_chunk);

private:
    void run();
    Value pop();
    uint16_t read_short();
    bool is_truthy(const Value& value);
    void print_value(const Value& value);
};