using Value = std::variant<
    std::nullptr_t,
    double,
    bool,
    std::string,
    std::shared_ptr<Function>,
    std::shared_ptr<Array>,
    std::shared_ptr<BoundMethod>
>;

struct Chunk
{
    std::vector<uint8_t> code;
    std::vector<Value> constants;
    uint8_t add_constant(Value value)
    {
        constants.push_back(std::move(value));
        return static_cast<uint8_t>(constants.size() - 1);
    }

    void write(uint8_t byte)
    {
        code.push_back(byte);
    }
};

struct CallFrame
{
    Chunk* chunk;
    uint8_t* ip;
    size_t stack_start;
};

struct Function
{
    std::string name;
    int arity;
    Chunk chunk;

    Function(std::string name, int arity, Chunk chunk) : name(std::move(name)), arity(arity), chunk(std::move(chunk)) {}
};

struct BoundMethod
{
    std::shared_ptr<Array> receiver;
    NativeMethod method;

    BoundMethod(std::shared_ptr<Array> receiver, NativeMethod method) : receiver(std::move(receiver)), method(method) {}
};

struct Array
{
    std::vector<Value> elements;
};
