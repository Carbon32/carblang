#include "core/html/tag.hpp"

struct IgnoreReturnValue
{
    bool operator==(const IgnoreReturnValue &) const noexcept { return true; }
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
    std::shared_ptr<Tag>,
    std::shared_ptr<Texture2D>,
    std::shared_ptr<TextElement>,
    IgnoreReturnValue>;

struct Chunk
{
    std::vector<uint8_t> code;
    std::vector<Value> constants;
    std::unordered_map<std::string, uint16_t> string_constant_indices;
    uint16_t add_constant(Value value)
    {
        if (const auto *text = std::get_if<std::string>(&value))
        {
            const auto existing = string_constant_indices.find(*text);
            if (existing != string_constant_indices.end())
                return existing->second;
        }

        if (constants.size() > std::numeric_limits<uint16_t>::max())
            throw std::runtime_error("A chunk cannot contain more than 65536 constants");

        const uint16_t index = static_cast<uint16_t>(constants.size());
        if (const auto *text = std::get_if<std::string>(&value))
            string_constant_indices.emplace(*text, index);
        constants.push_back(std::move(value));
        return index;
    }

    void write(uint8_t byte)
    {
        code.push_back(byte);
    }
};

struct CallFrame
{
    Chunk *chunk;
    const uint8_t *ip;
    size_t stack_start;
    bool is_constructor = false;
    std::shared_ptr<Function> function;
};

struct PropertyInfo
{
    std::string owner_class;
    bool is_public = true;
    std::optional<std::string> type_name;
};

struct Function
{
    std::string name;
    size_t arity;
    Chunk chunk;
    std::vector<std::optional<std::string>> param_types;
    std::optional<std::string> return_type;
    std::shared_ptr<Class> super_class;
    std::string owner_class;
    bool is_public = true;
    std::unordered_map<std::string, PropertyInfo> declared_properties;

    Function(std::string name,
             size_t arity,
             Chunk chunk,
             std::vector<std::optional<std::string>> param_types,
             std::optional<std::string> return_type = std::nullopt)
        : name(std::move(name)),
          arity(arity),
          chunk(std::move(chunk)),
          param_types(std::move(param_types)),
          return_type(std::move(return_type))
    {
    }
};

struct BoundMethod
{
    Value receiver;
    NativeMethod method;

    BoundMethod(Value receiver, NativeMethod method)
        : receiver(std::move(receiver)), method(method) {}
};

struct UserBoundMethod
{
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

    void set(const std::string &key, Value val)
    {
        entries[key] = std::move(val);
    }

    Value get(const std::string &key) const
    {
        auto it = entries.find(key);
        if (it != entries.end())
            return it->second;
        return nullptr;
    }

    bool contains(const std::string &key) const
    {
        return entries.find(key) != entries.end();
    }

    bool remove(const std::string &key)
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
    std::unordered_map<std::string, PropertyInfo> properties;
    std::shared_ptr<Class> super_class;
    bool is_user_defined = false;

    Class(std::string name, std::unordered_map<std::string, std::shared_ptr<Function>> methods,
          std::shared_ptr<Class> super_class = nullptr)
        : name(std::move(name)), methods(std::move(methods)), super_class(std::move(super_class)) {}

    std::shared_ptr<Function> find_method(const std::string &name) const
    {
        auto it = methods.find(name);
        if (it != methods.end())
            return it->second;
        if (super_class)
            return super_class->find_method(name);
        return nullptr;
    }

    const PropertyInfo *find_property(const std::string &name) const
    {
        auto it = properties.find(name);
        if (it != properties.end())
            return &it->second;
        return super_class ? super_class->find_property(name) : nullptr;
    }
};
