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
                if(it == globals.end()) throw std::runtime_error("Undefined variable '" + name + "'");
                stack.push_back(it->second);
                break;
            }

            case OpCode::SET_GLOBAL:
            {
                uint8_t nameIndex = *ip++;
                const std::string& name = std::get<std::string>(chunk->constants[nameIndex]);
                auto it = globals.find(name);
                if(it == globals.end()) throw std::runtime_error("Undefined variable '" + name + "'");
                globals[name] = stack.back();
                break;
            }

            case OpCode::GET_LOCAL:
            {
                uint8_t slot = *ip++;
                stack.push_back(stack[slot]);
                break;
            }

            case OpCode::SET_LOCAL:
            {
                uint8_t slot = *ip++;
                stack[slot] = stack.back();
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

            case OpCode::RETURN:
                return;

            default:
                throw std::runtime_error("Unknown opcode");
        }
    }
}

Value VM::pop()
{
    if(stack.empty())
        throw std::runtime_error("Runtime Error: pop() called on empty stack");
    Value v = stack.back();
    stack.pop_back();
    return v;
}


uint16_t VM::read_short()
{
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

void VM::print_value(const Value& value)
{
    if(std::holds_alternative<double>(value)) std::cout << std::get<double>(value);
    else if(std::holds_alternative<std::string>(value)) std::cout << std::get<std::string>(value);
    else if(std::holds_alternative<bool>(value)) std::cout << (std::get<bool>(value) ? "true" : "false");
    else std::cout << "null";
}