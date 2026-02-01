#include "core/core.hpp"

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

                if(!std::holds_alternative<double>(a) || !std::holds_alternative<double>(b))
                        throw std::runtime_error("Operands must be numbers");

                double x = std::get<double>(a);
                double y = std::get<double>(b);

                switch(opcode)
                {
                    case OpCode::ADD: stack.push_back(x + y); break;
                    case OpCode::SUBTRACT: stack.push_back(x - y); break;
                    case OpCode::MULTIPLY: stack.push_back(x * y); break;
                    case OpCode::DIVIDE:
                        if(y == 0) throw std::runtime_error("Division by zero");
                        stack.push_back(x / y);
                        break;
                    default: break;
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
                            case NativeMethod::PUSH:
                                for(auto& v : args)
                                {
                                    method->receiver->elements.push_back(v);
                                }
                                push(nullptr);
                                break;

                            case NativeMethod::POP:
                                if(method->receiver->elements.empty())
                                {
                                    push(nullptr);
                                }
                                else
                                {
                                    push(method->receiver->elements.back());
                                    method->receiver->elements.pop_back();
                                }
                                break;

                            case NativeMethod::LENGTH:
                                if(!args.empty())
                                    throw std::runtime_error("length() takes no arguments");
                                push((double)method->receiver->elements.size());
                                break;

                            case NativeMethod::IS_EMPTY:
                                push(method->receiver->elements.empty());
                                break;

                            case NativeMethod::CLEAR:
                                method->receiver->elements.clear();
                                push(nullptr);
                                break;

                            case NativeMethod::CONTAINS:
                            {
                                if(args.size() != 1)
                                    throw std::runtime_error("contains() expects 1 argument");

                                bool found = false;
                                for(auto& v : method->receiver->elements)
                                    if (v == args[0]) { found = true; break; }

                                push(found);
                                break;
                            }

                            case NativeMethod::INDEX_OF:
                            {
                                if(args.size() != 1)
                                    throw std::runtime_error("index_of() expects 1 argument");

                                int idx = -1;
                                for(size_t i = 0; i < method->receiver->elements.size(); ++i)
                                {
                                    if(method->receiver->elements[i] == args[0]) {
                                        idx = (int)i;
                                        break;
                                    }
                                }

                                push((double)idx);
                                break;
                            }

                            case NativeMethod::JOIN:
                            {
                                std::string sep = args.empty()
                                    ? ","
                                    : std::get<std::string>(args[0]);

                                std::ostringstream out;
                                const auto& elems = method->receiver->elements;

                                for(size_t i = 0; i < elems.size(); ++i)
                                {
                                    out << stringify(elems[i]);
                                    if(i + 1 < elems.size())
                                        out << sep;
                                }

                                push(out.str());
                                break;
                            }

                            case NativeMethod::INSERT:
                            {
                                if(args.size() != 2)
                                    throw std::runtime_error("insert() expects 2 arguments");

                                int idx = (int)std::get<double>(args[0]);
                                if(idx < 0 || idx > method->receiver->elements.size())
                                    throw std::runtime_error("Index out of bounds");

                                method->receiver->elements.insert(
                                    method->receiver->elements.begin() + idx,
                                    args[1]
                                );

                                push(nullptr);
                                break;
                            }

                            case NativeMethod::REMOVE_AT:
                            {
                                if(args.size() != 1)
                                    throw std::runtime_error("remove() expects 1 argument");

                                int idx = (int)std::get<double>(args[0]);
                                if(idx < 0 || idx >= method->receiver->elements.size())
                                {
                                    push(nullptr);
                                    break;
                                }

                                Value v = method->receiver->elements[idx];
                                method->receiver->elements.erase(
                                    method->receiver->elements.begin() + idx
                                );

                                push(v);
                                break;
                            }

                            case NativeMethod::REVERSE:
                            {
                                if(!args.empty())
                                    throw std::runtime_error("reverse() takes no arguments");

                                std::reverse(method->receiver->elements.begin(), method->receiver->elements.end());
                                push(nullptr);
                                break;
                            }

                            case NativeMethod::FIRST:
                            {
                                if(!args.empty())
                                    throw std::runtime_error("first() takes no arguments");

                                if(method->receiver->elements.empty()) push(nullptr);
                                else push(method->receiver->elements.front());
                                break;
                            }

                            case NativeMethod::LAST:
                            {
                                if(!args.empty())
                                    throw std::runtime_error("last() takes no arguments");

                                if(method->receiver->elements.empty()) push(nullptr);
                                else push(method->receiver->elements.back());
                                break;
                            }

                            case NativeMethod::SLICE:
                            {
                                if(args.size() < 1 || args.size() > 2)
                                    throw std::runtime_error("slice() expects 1 or 2 arguments");

                                int start = (int)std::get<double>(args[0]);
                                int end = args.size() == 2 ? (int)std::get<double>(args[1]) : (int)method->receiver->elements.size();

                                if(start < 0) start = 0;
                                if(end > (int)method->receiver->elements.size()) end = (int)method->receiver->elements.size();
                                if(end < start) end = start;

                                auto new_array = std::make_shared<Array>();
                                new_array->elements.insert(
                                    new_array->elements.begin(),
                                    method->receiver->elements.begin() + start,
                                    method->receiver->elements.begin() + end
                                );

                                push(new_array);
                                break;
                            }

                            case NativeMethod::COPY:
                            {
                                if(!args.empty())
                                    throw std::runtime_error("copy() takes no arguments");

                                auto new_array = std::make_shared<Array>();
                                new_array->elements = method->receiver->elements;
                                push(new_array);
                                break;
                            }

                            case NativeMethod::CONCAT:
                            {
                                if(args.size() != 1)
                                    throw std::runtime_error("concat() expects 1 argument");

                                if(!std::holds_alternative<std::shared_ptr<Array>>(args[0]))
                                    throw std::runtime_error("concat() argument must be an array");

                                auto other = std::get<std::shared_ptr<Array>>(args[0]);
                                auto new_array = std::make_shared<Array>();
                                new_array->elements = method->receiver->elements;
                                new_array->elements.insert(new_array->elements.end(), other->elements.begin(), other->elements.end());

                                push(new_array);
                                break;
                            }

                            case NativeMethod::SWAP:
                            {
                                if(args.size() != 2)
                                    throw std::runtime_error("swap() expects 2 arguments");

                                int i = (int)std::get<double>(args[0]);
                                int j = (int)std::get<double>(args[1]);

                                if(i < 0 || j < 0 ||
                                    i >= method->receiver->elements.size() ||
                                    j >= method->receiver->elements.size())
                                    throw std::runtime_error("Index out of bounds");

                                std::swap(method->receiver->elements[i],
                                          method->receiver->elements[j]);

                                push(nullptr);
                                break;
                            }

                            case NativeMethod::COUNT:
                            {
                                if(args.size() != 1)
                                    throw std::runtime_error("count() expects 1 argument");

                                int c = 0;
                                for(auto& v : method->receiver->elements)
                                    if(v == args[0]) c++;

                                push((double)c);
                                break;
                            }

                            case NativeMethod::EQUALS:
                            {
                                if(args.size() != 1 ||
                                    !std::holds_alternative<std::shared_ptr<Array>>(args[0]))
                                {
                                    push(false);
                                    break;
                                }

                                auto other = std::get<std::shared_ptr<Array>>(args[0]);
                                auto& a = method->receiver->elements;
                                auto& b = other->elements;
                                bool res = true;

                                if(a.size() != b.size()) {
                                    res = false;
                                    break;
                                }

                                for(size_t i = 0; i < a.size(); ++i) {
                                    if(res != false)
                                    {
                                        if(!value_shallow_equal(a[i], b[i])) {
                                            res = false;
                                            break;
                                        }
                                    }
                                }

                                push(res);
                                break;
                            }


                           case NativeMethod::LAST_INDEX_OF:
                            {
                                if(args.size() != 1)
                                    throw std::runtime_error("last_index() expects 1 argument");

                
                                int idx = -1;
                                for(size_t i = method->receiver->elements.size() - 1; i >= 0; --i) {
                                    if(method->receiver->elements[i] == args[0]) {
                                        idx = (int) i;
                                        break;
                                    }
                                }

                                push((double)idx);
                                break;
                            }


                            case NativeMethod::TRIM:
                            {
                                if(args.size() != 1)
                                    throw std::runtime_error("trim() expects 1 argument");

                                int n = (int)std::get<double>(args[0]);
                                if (n < 0) n = 0;
                                if (n < method->receiver->elements.size())
                                    method->receiver->elements.resize(n);

                                push(nullptr);
                                break;
                            }

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
                Value array_val = pop();

                if(!std::holds_alternative<std::shared_ptr<Array>>(array_val))
                    throw std::runtime_error("Can only index arrays");

                if(!std::holds_alternative<double>(index_val))
                    throw std::runtime_error("Array index must be a number");

                auto array = std::get<std::shared_ptr<Array>>(array_val);
                int index = static_cast<int>(std::get<double>(index_val));

                if(index < 0 || index >= (int)array->elements.size())
                    throw std::runtime_error("Array index out of bounds");

                push(array->elements[index]);
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

                // Eh, make this a little better later on
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
