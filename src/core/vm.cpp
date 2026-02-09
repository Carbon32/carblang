#include "core/core.hpp"

void VM::init_globals()
{
    globals["input"] = make_native_method(nullptr, NativeMethod::INPUT);
    globals["fill"] = make_native_method(nullptr, NativeMethod::FILL);
    globals["init"] = make_native_method(nullptr, NativeMethod::INIT);
    globals["array_input"] = make_native_method(nullptr, NativeMethod::ARRAY_INPUT);
    globals["random"] = make_native_method(nullptr, NativeMethod::RAND);
    globals["random_integer"] = make_native_method(nullptr, NativeMethod::RANDINT);
}

void VM::interpret(Chunk& new_chunk)
{
    chunk = &new_chunk;
    ip = chunk->code.data();
    run();
}

void VM::run()
{

    for(;;)
    {
        OpCode opcode = static_cast<OpCode>(*ip++);
        switch(opcode)
        {
            case OpCode::CONSTANT:
            {
                uint8_t index = *ip++;
                stack.push_back(chunk->constants[index]);
                break;
            }

            case OpCode::ADD:
            case OpCode::SUBTRACT:
            case OpCode::MULTIPLY:
            case OpCode::DIVIDE:
            {
                Value b = pop();
                Value a = pop();

                if(opcode == OpCode::ADD &&
                    (std::holds_alternative<std::string>(a) ||
                     std::holds_alternative<std::string>(b)))
                {
                    stack.push_back(stringify(a) + stringify(b));
                    break;
                }

                if(opcode == OpCode::ADD &&
                    std::holds_alternative<std::shared_ptr<Array>>(a) &&
                    std::holds_alternative<std::shared_ptr<Array>>(b))
                {
                    auto left  = std::get<std::shared_ptr<Array>>(a);
                    auto right = std::get<std::shared_ptr<Array>>(b);

                    auto result = std::make_shared<Array>();
                    result->elements.reserve(
                        left->elements.size() + right->elements.size()
                    );

                    result->elements.insert(
                        result->elements.end(),
                        left->elements.begin(),
                        left->elements.end()
                    );

                    result->elements.insert(
                        result->elements.end(),
                        right->elements.begin(),
                        right->elements.end()
                    );

                    stack.push_back(result);
                    break;
                }

                if(std::holds_alternative<std::string>(a) && std::holds_alternative<double>(b))
                {
                    if(opcode == OpCode::MULTIPLY)
                    {
                        auto s = std::get<std::string>(a);
                        double n = std::get<double>(b);

                        if(std::floor(n) != n)
                            throw std::runtime_error("Cannot multiply string by non-integer number");

                        if(n < 0) n = 0;
                        std::string result;
                        result.reserve(s.size() * static_cast<size_t>(n));
                        for(int i = 0; i < (int)n; ++i)
                            result += s;

                        stack.push_back(result);
                        break;
                    }
                }

                if(std::holds_alternative<double>(a) && std::holds_alternative<std::string>(b))
                {
                    if(opcode == OpCode::MULTIPLY)
                    {
                        double n = std::get<double>(a);
                        auto s = std::get<std::string>(b);

                        if(std::floor(n) != n)
                            throw std::runtime_error("Cannot multiply string by non-integer number");

                        if(n < 0) n = 0;
                        std::string result;
                        result.reserve(s.size() * static_cast<size_t>(n));
                        for(int i = 0; i < (int) n; ++i)
                            result += s;

                        stack.push_back(result);
                        break;
                    }
                }

                if(std::holds_alternative<std::shared_ptr<Array>>(a) &&
                   std::holds_alternative<double>(b))
                {
                    auto arr = std::get<std::shared_ptr<Array>>(a);
                    double scalar = std::get<double>(b);

                    auto result = std::make_shared<Array>();
                    result->elements.reserve(arr->elements.size());

                    for(const auto& v : arr->elements)
                    {
                        if(!std::holds_alternative<double>(v))
                            throw std::runtime_error("Array must contain only numbers");

                        double x = std::get<double>(v);
                        double r;

                        switch(opcode)
                        {
                            case OpCode::ADD: r = x + scalar; break;
                            case OpCode::SUBTRACT: r = x - scalar; break;
                            case OpCode::MULTIPLY: r = x * scalar; break;
                            case OpCode::DIVIDE:
                                if(scalar == 0)
                                    throw std::runtime_error("Division by zero");
                                r = x / scalar;
                                break;
                            default: r = 0;
                        }

                        result->elements.push_back(r);
                    }

                    stack.push_back(result);
                    break;
                }

                if(!std::holds_alternative<double>(a) ||
                    !std::holds_alternative<double>(b))
                {
                    throw std::runtime_error("Operands must be numbers");
                }

                double x = std::get<double>(a);
                double y = std::get<double>(b);

                switch (opcode)
                {
                    case OpCode::ADD:
                        stack.push_back(x + y);
                        break;

                    case OpCode::SUBTRACT:
                        stack.push_back(x - y);
                        break;

                    case OpCode::MULTIPLY:
                        stack.push_back(x * y);
                        break;

                    case OpCode::DIVIDE:
                        if (y == 0)
                            throw std::runtime_error("Division by zero");
                        stack.push_back(x / y);
                        break;

                    default:
                        break;
                }

                break;
            }

            case OpCode::NOT:
            {
                Value v = pop();
                stack.push_back(!is_truthy(v));
                break;
            }

            case OpCode::NEGATE:
            {
                Value v = pop();
                if(!std::holds_alternative<double>(v))
                    throw std::runtime_error("Operand must be a number");
                stack.push_back(-std::get<double>(v));
                break;
            }

            case OpCode::EQUAL:
            {
                Value b = pop();
                Value a = pop();
                stack.push_back(a == b);
                break;
            }

            case OpCode::GREATER:
            case OpCode::LESS:
            {
                Value b = pop();
                Value a = pop();
                if(!std::holds_alternative<double>(a) || !std::holds_alternative<double>(b))
                    throw std::runtime_error("Operands must be numbers");

                double x = std::get<double>(a);
                double y = std::get<double>(b);

                stack.push_back(opcode == OpCode::GREATER ? x > y : x < y);
                break;
            }

            case OpCode::PRINT:
            case OpCode::PRINTLN:
            {
                Value v = pop();
                print_value(v);
                if(opcode == OpCode::PRINTLN) std::cout << std::endl;
                break;
            }

            case OpCode::POP:
                pop();
                break;

            case OpCode::DEFINE_GLOBAL:
            {
                uint8_t nameIndex = *ip++;
                const std::string& name = std::get<std::string>(chunk->constants[nameIndex]);
                globals[name] = pop();
                break;
            }

            case OpCode::GET_GLOBAL:
            {
                uint8_t nameIndex = *ip++;
                const std::string& name = std::get<std::string>(chunk->constants[nameIndex]);
                auto it = globals.find(name);
                if(it == globals.end()) throw std::runtime_error("Undefined variable \"" + name + "\"");
                stack.push_back(it->second);
                break;
            }

            case OpCode::SET_GLOBAL:
            {
                uint8_t nameIndex = *ip++;
                const std::string& name = std::get<std::string>(chunk->constants[nameIndex]);
                auto it = globals.find(name);
                if(it == globals.end()) throw std::runtime_error("Undefined variable \"" + name + "\"");
                globals[name] = stack.back();
                break;
            }

            case OpCode::GET_LOCAL:
            {
                uint8_t slot = *ip++;
                if(frames.empty())
                    throw std::runtime_error("GET_LOCAL called with no active frame");

                size_t idx = frames.back().stack_start + slot;
                if(idx >= stack.size())
                    throw std::runtime_error("GET_LOCAL out of bounds");

                stack.push_back(stack[idx]);
                break;
            }

            case OpCode::SET_LOCAL:
            {
                uint8_t slot = *ip++;
                if(frames.empty())
                    throw std::runtime_error("SET_LOCAL called with no active frame");

                size_t idx = frames.back().stack_start + slot;
                if(idx >= stack.size())
                    throw std::runtime_error("SET_LOCAL out of bounds");

                stack[idx] = stack.back();
                break;
            }

            case OpCode::JUMP:
            {
                uint16_t offset = read_short();
                ip += offset;
                break;
            }

            case OpCode::JUMP_IF_TRUE:
            {
                uint16_t offset = read_short();
                Value condition = pop();
                if(is_truthy(condition)) ip += offset;
                break;
            }

            case OpCode::JUMP_IF_FALSE:
            {
                uint16_t offset = read_short();
                Value condition = stack.back();
                if(!is_truthy(condition)) ip += offset;
                break;
            }

            case OpCode::LOOP:
            {
                uint16_t offset = read_short();
                ip -= offset;
                break;
            }

            case OpCode::CLOSURE:
            {
                uint8_t constant_index = *ip++;
                Value constant = chunk->constants[constant_index];

                if (!std::holds_alternative<std::shared_ptr<Function>>(constant))
                    throw std::runtime_error("CLOSURE constant is not a function");

                stack.push_back(constant);
                break;
            }

            case OpCode::CALL:
            {
                uint8_t arg_count = *ip++;

                if(stack.size() < arg_count + 1)
                    throw std::runtime_error("Not enough values on the stack for CALL");

                Value callee = stack[stack.size() - 1 - arg_count];
                size_t callee_index = stack.size() - 1 - arg_count;
                if(std::holds_alternative<std::shared_ptr<BoundMethod>>(callee))
                {
                    size_t local_index = stack.size() - 1 - arg_count;
                    auto method = std::get<std::shared_ptr<BoundMethod>>(stack[local_index]);
                    std::vector<Value> args(stack.begin() + local_index + 1, stack.begin() + local_index + 1 + arg_count);
                    stack.erase(stack.begin() + local_index, stack.begin() + local_index + 1 + arg_count);

                    switch(method->method)
                    {
                        NATIVE_GLOBALS_INPUT
                        NATIVE_GLOBALS_RAND
                        NATIVE_GLOBALS_RANDINT
                        NATIVE_GLOBALS_FILL
                        NATIVE_GLOBALS_INIT
                        NATIVE_GLOBALS_ARRAY_INPUT

                        NATIVE_ARRAY_PUSH
                        NATIVE_ARRAY_POP
                        NATIVE_ARRAY_LENGTH
                        NATIVE_ARRAY_IS_EMPTY
                        NATIVE_ARRAY_CLEAR
                        NATIVE_ARRAY_CONTAINS
                        NATIVE_ARRAY_INDEX_OF
                        NATIVE_ARRAY_JOIN
                        NATIVE_ARRAY_INSERT
                        NATIVE_ARRAY_REMOVE_AT
                        NATIVE_ARRAY_REVERSE
                        NATIVE_ARRAY_FIRST
                        NATIVE_ARRAY_LAST
                        NATIVE_ARRAY_SLICE
                        NATIVE_ARRAY_COPY
                        NATIVE_ARRAY_CONCAT
                        NATIVE_ARRAY_SWAP
                        NATIVE_ARRAY_COUNT
                        NATIVE_ARRAY_EQUALS
                        NATIVE_ARRAY_LAST_INDEX_OF
                        NATIVE_ARRAY_SUM
                        NATIVE_ARRAY_MIN
                        NATIVE_ARRAY_MAX
                        NATIVE_ARRAY_AVERAGE
                        NATIVE_ARRAY_TRIM

                        NATIVE_PRIMITIVE_TYPE
                        NATIVE_PRIMITIVE_TO_STRING
                        NATIVE_PRIMITIVE_POW
                        NATIVE_PRIMITIVE_SQRT
                        NATIVE_PRIMITIVE_FACT
                        NATIVE_PRIMITIVE_TO_INT
                        NATIVE_PRIMITIVE_BOOL_TO_INT
                        NATIVE_PRIMITIVE_FLOOR
                        NATIVE_PRIMITIVE_CEIL
                        NATIVE_PRIMITIVE_UPPER
                        NATIVE_PRIMITIVE_LOWER
                        NATIVE_PRIMITIVE_CAPITALIZE
                        NATIVE_PRIMITIVE_SWAPCASE
                        NATIVE_PRIMITIVE_FIND
                        NATIVE_PRIMITIVE_FIND_LAST
                        NATIVE_PRIMITIVE_STR_FIRST
                        NATIVE_PRIMITIVE_STR_LAST
                        NATIVE_PRIMITIVE_STARTS_WITH
                        NATIVE_PRIMITIVE_ENDS_WITH
                        NATIVE_PRIMITIVE_IS_NUMBER
                        NATIVE_PRIMITIVE_STR_TRIM
                        NATIVE_PRIMITIVE_TO_ARRAY
                        NATIVE_PRIMITIVE_REPLACE
                        NATIVE_PRIMITIVE_IS_SPACE
                        NATIVE_PRIMITIVE_IS_ALL_SPACES
                        NATIVE_PRIMITIVE_STR_IS_EMPTY
                        NATIVE_PRIMITIVE_STR_LENGTH
                        NATIVE_PRIMITIVE_STR_SLICE
                        NATIVE_PRIMITIVE_STR_COUNT
                        NATIVE_PRIMITIVE_TO_NUMBER

                        default:
                            throw std::runtime_error("Unrecognized method");
                    }

                    continue;
                }

                if(!std::holds_alternative<std::shared_ptr<Function>>(callee))
                    throw std::runtime_error("Can only call functions");

                auto function = std::get<std::shared_ptr<Function>>(callee);

                frames.push_back({
                    chunk,
                    ip,
                    callee_index
                });

                stack.erase(stack.begin() + callee_index);
                chunk = &function->chunk;
                ip = chunk->code.data();
                break;
            }

            case OpCode::RETURN:
            {
                Value result = pop();

                if(frames.empty()) {
                    stack.push_back(result);
                    return;
                }

                CallFrame frame = frames.back();
                frames.pop_back();

                stack.resize(frame.stack_start);
                stack.push_back(result);

                chunk = frame.chunk;
                ip = frame.ip;

                break;
            }

            case OpCode::ARRAY:
            {
                uint8_t count = *ip++;

                auto array = std::make_shared<Array>();
                array->elements.resize(count);
                for(int i = count - 1; i >= 0; --i)
                    array->elements[i] = pop();

                stack.push_back(array);
                break;
            }

            case OpCode::GET_INDEX:
            {
                Value index_val = pop();
                Value container_val = pop();

                if(!std::holds_alternative<double>(index_val))
                    throw std::runtime_error("Index must be a number");

                int index = static_cast<int>(std::get<double>(index_val));

                if(std::holds_alternative<std::shared_ptr<Array>>(container_val)) {
                    auto array = std::get<std::shared_ptr<Array>>(container_val);

                    if(index < 0 || index >= (int)array->elements.size())
                        throw std::runtime_error("Array index out of bounds");

                    push(array->elements[index]);
                }
                else if(std::holds_alternative<std::string>(container_val)) {
                    const auto& str = std::get<std::string>(container_val);

                    if(index < 0 || index >= (int)str.size())
                        throw std::runtime_error("String index out of bounds");

                    std::string ch(1, str[index]);
                    push(ch);
                }
                else
                {
                    throw std::runtime_error("Can only index arrays or strings");
                }

                break;
            }

            case OpCode::SET_INDEX:
            {
                Value value = pop();
                Value index_val = pop();
                Value array_val = pop();

                if(!std::holds_alternative<std::shared_ptr<Array>>(array_val))
                    throw std::runtime_error("Can only index arrays");

                if(!std::holds_alternative<double>(index_val))
                    throw std::runtime_error("Array index must be a number");

                auto array = std::get<std::shared_ptr<Array>>(array_val);
                int index = static_cast<int>(std::get<double>(index_val));

                if(index < 0 || index >= (int)array->elements.size())
                    throw std::runtime_error("Array index out of bounds");

                array->elements[index] = value;
                push(value);
                break;
            }

            case OpCode::GET_PROPERTY:
            {
                std::string name = std::get<std::string>(read_constant());
                Value object = pop();

                if(name == "type")
                {
                    push(make_native_method(object, NativeMethod::TYPE));
                    break;
                }

                if(std::holds_alternative<bool>(object))
                {
                    if(name == "to_int") { push(make_native_method(object, NativeMethod::BOOL_TO_INT)); break; }
                    if(name == "to_string") { push(make_native_method(object, NativeMethod::TO_STRING)); break; }
                }

                if(std::holds_alternative<double>(object))
                {
                    if(name == "to_string") { push(make_native_method(object, NativeMethod::TO_STRING)); break; }
                    if(name == "pow") { push(make_native_method(object, NativeMethod::POW)); break; }
                    if(name == "sqrt") { push(make_native_method(object, NativeMethod::SQRT)); break; }
                    if(name == "fact") { push(make_native_method(object, NativeMethod::FACT)); break;}
                    if(name == "to_int") { push(make_native_method(object, NativeMethod::TO_INT)); break; }
                    if(name == "floor") { push(make_native_method(object, NativeMethod::FLOOR)); break; }
                    if(name == "ceil") { push(make_native_method(object, NativeMethod::CEIL)); break; }
                }

                if(std::holds_alternative<std::string>(object))
                {
                    if(name == "to_number") { push(make_native_method(object, NativeMethod::TO_NUMBER)); break; }
                    if(name == "upper") { push(make_native_method(object, NativeMethod::UPPER)); break; }
                    if(name == "lower") { push(make_native_method(object, NativeMethod::LOWER)); break; }
                    if(name == "capitalize") { push(make_native_method(object, NativeMethod::CAPITALIZE)); break; }
                    if(name == "swap") { push(make_native_method(object, NativeMethod::SWAPCASE)); break; }
                    if(name == "find") { push(make_native_method(object, NativeMethod::FIND)); break; }
                    if(name == "find_last") { push(make_native_method(object, NativeMethod::FIND_LAST)); break; }
                    if(name == "first") { push(make_native_method(object, NativeMethod::STR_FIRST)); break; }
                    if(name == "last") { push(make_native_method(object, NativeMethod::STR_LAST)); break; }
                    if(name == "starts_with") { push(make_native_method(object, NativeMethod::STARTS_WITH)); break; }
                    if(name == "ends_with") { push(make_native_method(object, NativeMethod::ENDS_WITH)); break; }
                    if(name == "is_number") { push(make_native_method(object, NativeMethod::IS_NUMBER)); break; }
                    if(name == "is_space") { push(make_native_method(object, NativeMethod::IS_SPACE)); break; }
                    if(name == "is_all_spaces") { push(make_native_method(object, NativeMethod::IS_ALL_SPACES)); break; }
                    if(name == "replace") { push(make_native_method(object, NativeMethod::REPLACE)); break; }
                    if(name == "trim") { push(make_native_method(object, NativeMethod::STR_TRIM)); break; }
                    if(name == "to_array") { push(make_native_method(object, NativeMethod::TO_ARRAY)); break; }
                    if(name == "is_empty") { push(make_native_method(object, NativeMethod::STR_IS_EMPTY)); break; }
                    if(name == "length") { push(make_native_method(object, NativeMethod::STR_LENGTH)); break; }
                    if(name == "count") { push(make_native_method(object, NativeMethod::STR_COUNT)); break; }
                    if(name == "slice") { push(make_native_method(object, NativeMethod::STR_SLICE)); break; }
                }

                // Eh, make this a little better later on
                // I will make this one-liners later
                if(std::holds_alternative<std::shared_ptr<Array>>(object))
                {
                    auto array = std::get<std::shared_ptr<Array>>(object);

                    if(name == "length")
                    {
                        push(make_native_method(array, NativeMethod::LENGTH));
                        break;
                    }

                    else if(name == "push")
                    {
                        push(make_native_method(array, NativeMethod::PUSH));
                        break;
                    }

                    else if(name == "pop")
                    {
                        push(make_native_method(array, NativeMethod::POP));
                        break;
                    }

                    else if(name == "is_empty")
                    {
                        push(make_native_method(array, NativeMethod::IS_EMPTY));
                        break;
                    }

                    else if(name == "clear")
                    {
                        push(make_native_method(array, NativeMethod::CLEAR));
                        break;
                    }

                    else if(name == "contains")
                    {
                        push(make_native_method(array, NativeMethod::CONTAINS));
                        break;
                    }

                    else if(name == "first_index")
                    {
                        push(make_native_method(array, NativeMethod::INDEX_OF));
                        break;
                    }

                    else if(name == "insert")
                    {
                        push(make_native_method(array, NativeMethod::INSERT));
                        break;
                    }

                    else if(name == "remove")
                    {
                        push(make_native_method(array, NativeMethod::REMOVE_AT));
                        break;
                    }

                    else if(name == "join")
                    {
                        push(make_native_method(array, NativeMethod::JOIN));
                        break;
                    }

                    else if (name == "last_index")
                    {
                        push(make_native_method(array, NativeMethod::LAST_INDEX_OF));
                        break;
                    }

                    else if (name == "equals")
                    {
                        push(make_native_method(array, NativeMethod::EQUALS));
                        break;
                    }

                    else if (name == "count")
                    {
                        push(make_native_method(array, NativeMethod::COUNT));
                        break;
                    }

                    else if (name == "swap")
                    {
                        push(make_native_method(array, NativeMethod::SWAP));
                        break;
                    }

                    else if (name == "concat")
                    {
                        push(make_native_method(array, NativeMethod::CONCAT));
                        break;
                    }

                    else if (name == "copy")
                    {
                        push(make_native_method(array, NativeMethod::COPY));
                        break;
                    }

                    else if (name == "slice")
                    {
                        push(make_native_method(array, NativeMethod::SLICE));
                        break;
                    }

                    else if (name == "last")
                    {
                        push(make_native_method(array, NativeMethod::LAST));
                        break;
                    }

                    else if (name == "first")
                    {
                        push(make_native_method(array, NativeMethod::FIRST));
                        break;
                    }

                    else if (name == "reverse")
                    {
                        push(make_native_method(array, NativeMethod::REVERSE));
                        break;
                    }

                    else if (name == "trim")
                    {
                        push(make_native_method(array, NativeMethod::TRIM));
                        break;
                    }

                    if(name == "sum") { push(make_native_method(array, NativeMethod::SUM)); break; }
                    if(name == "min") { push(make_native_method(array, NativeMethod::MIN)); break; }
                    if(name == "max") { push(make_native_method(array, NativeMethod::MAX)); break; }
                    if(name == "average") { push(make_native_method(array, NativeMethod::AVERAGE)); break; }
                }

                throw std::runtime_error("Undefined method \"" + name + "\"");
            }

            case OpCode::NULL:
                stack.push_back(nullptr);
                break;

            case OpCode::TRUE:
                stack.push_back(true);
                break;

            case OpCode::FALSE:
                stack.push_back(false);
                break;

            default:
                throw std::runtime_error("Unknown opcode");
        }
    }
}

Value VM::pop()
{
    if(stack.empty())
    {
        throw std::runtime_error("pop() called on empty stack");
    }
    Value v = stack.back();
    stack.pop_back();
    return v;
}

void VM::push(Value value)
{
    stack.push_back(std::move(value));
}

uint16_t VM::read_short()
{
    if(ip + 1 >= chunk->code.data() + chunk->code.size())
        throw std::runtime_error("read_short past end of bytecode");

    uint16_t high = *ip++;
    uint16_t low = *ip++;
    return (high << 8) | low;
}


bool VM::is_truthy(const Value& value)
{
    if(std::holds_alternative<std::nullptr_t>(value)) return false;
    if(auto b = std::get_if<bool>(&value)) return *b;
    return true;
}

bool VM::value_shallow_equal(const Value& x, const Value& y)
{
    if(x.index() != y.index())
        return false;

    if(std::holds_alternative<std::shared_ptr<Array>>(x))
        return std::get<std::shared_ptr<Array>>(x) ==
               std::get<std::shared_ptr<Array>>(y);

    return x == y;
}

void VM::print_value(const Value& value)
{
    std::cout << this->stringify(value);
}

std::string VM::stringify(const Value& value)
{
    std::ostringstream out;
    if (std::holds_alternative<std::nullptr_t>(value))
    {
        out << "null";
    }
    else if (std::holds_alternative<bool>(value))
    {
        out << (std::get<bool>(value) ? "true" : "false");
    }
    else if (std::holds_alternative<double>(value))
    {
        out << std::get<double>(value);
    }
    else if (std::holds_alternative<std::string>(value))
    {
        out << std::get<std::string>(value);
    }
    else if (std::holds_alternative<std::shared_ptr<Array>>(value))
    {
        auto array = std::get<std::shared_ptr<Array>>(value);
        std::cout << "[";

        for (size_t i = 0; i < array->elements.size(); ++i)
        {
            print_value(array->elements[i]);
            if (i + 1 < array->elements.size())
                std::cout << ", ";
        }

        out << "]";
    }
    else if (std::holds_alternative<std::shared_ptr<Function>>(value))
    {
        out << "<function>";
    }

    else
    {
        out << "<method>";
    }
    return out.str();
}

Value VM::read_constant()
{
    if(ip >= chunk->code.data() + chunk->code.size())
        runtime_error(RuntimeError(Token{END_OF_FILE, "", nullptr, 0}, "Reached end of bytecode while reading constant"));

    uint8_t index = *ip++;
    return chunk->constants.at(index);
}
