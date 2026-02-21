struct IgnoreReturnValue
{
    bool operator==(const IgnoreReturnValue&) const noexcept { return true; }
};

using Value = std::variant<
    std::nullptr_t,
    double,
    bool,
    std::string,
    std::shared_ptr<Function>,
    std::shared_ptr<Array>,
    std::shared_ptr<BoundMethod>,
    std::shared_ptr<Instance>,
    std::shared_ptr<Class>,
    std::shared_ptr<UserBoundMethod>,
    std::shared_ptr<Dict>,
    IgnoreReturnValue
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
    bool is_constructor = false;
};

struct Function
{
    std::string name;
    size_t arity;
    Chunk chunk;

    Function(std::string name, size_t arity, Chunk chunk) : name(std::move(name)), arity(arity), chunk(std::move(chunk)) {}
};

struct BoundMethod
{
    Value receiver;
    NativeMethod method;

    BoundMethod(Value receiver, NativeMethod method)
        : receiver(std::move(receiver)), method(method) {}
};

struct UserBoundMethod {
    std::shared_ptr<Instance> receiver;
    std::shared_ptr<Function> method;

    UserBoundMethod(std::shared_ptr<Instance> receiver, std::shared_ptr<Function> method)
        : receiver(receiver), method(method) {}
};

struct Array
{
    std::vector<Value> elements;
};

struct Dict
{
    std::unordered_map<std::string, Value> entries;

    void set(const std::string& key, Value val)
    {
        entries[key] = std::move(val);
    }

    Value get(const std::string& key) const
    {
        auto it = entries.find(key);
        if(it != entries.end()) return it->second;
        return nullptr;
    }

    bool contains(const std::string& key) const
    {
        return entries.find(key) != entries.end();
    }

    bool remove(const std::string& key)
    {
        return entries.erase(key) > 0;
    }

    size_t length() const
    {
        return entries.size();
    }

    void clear()
    {
        entries.clear();
    }

    std::shared_ptr<Dict> copy() const
    {
        auto d = std::make_shared<Dict>();
        d->entries = entries;
        return d;
    }
};

struct Instance
{
    std::shared_ptr<Class> klass;
    std::unordered_map<std::string, Value> fields;

    Instance(std::shared_ptr<Class> klass) : klass(std::move(klass)) {}
};

struct Class
{
    std::string name;
    std::unordered_map<std::string, std::shared_ptr<Function>> methods;
    std::shared_ptr<Class> super_class;

    Class(std::string name, std::unordered_map<std::string, std::shared_ptr<Function>> methods,
          std::shared_ptr<Class> super_class = nullptr)
        : name(std::move(name)), methods(std::move(methods)), super_class(std::move(super_class)) {}

    std::shared_ptr<Function> find_method(const std::string& name)
    {
        auto it = methods.find(name);
        if(it != methods.end()) return it->second;
        if(super_class) return super_class->find_method(name);
        return nullptr;
    }
};