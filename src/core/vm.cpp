#include "core/core.hpp"

bool compare_values(const Value &a, const Value &b, TokenType op)
{
    if (!std::holds_alternative<double>(a) || !std::holds_alternative<double>(b))
        throw std::runtime_error("Comparison operators <, <=, >, >= can only be used with numbers");

    double x = std::get<double>(a);
    double y = std::get<double>(b);

    switch (op)
    {
    case GREATER:
        return x > y;
    case GREATER_EQUAL:
        return x >= y;
    case LESS:
        return x < y;
    case LESS_EQUAL:
        return x <= y;
    default:
        throw std::runtime_error("Invalid numeric comparison operator");
    }
}

static size_t checked_sequence_index(const Value &value, size_t length, const char *kind)
{
    const auto *number = std::get_if<double>(&value);
    if (!number || !std::isfinite(*number))
        throw std::runtime_error(std::string(kind) + " index must be a finite number");

    const double index = std::trunc(*number);
    if (index < 0 || index >= static_cast<double>(length))
        throw std::runtime_error(std::string(kind) + " index out of bounds");

    return static_cast<size_t>(index);
}

static size_t checked_size_count(const Value &value, size_t maximum, const char *kind)
{
    const auto *number = std::get_if<double>(&value);
    if (!number || !std::isfinite(*number))
        throw std::runtime_error(std::string(kind) + " must be a finite number");
    const double count = std::trunc(*number);
    if (count < 0 || static_cast<long double>(count) > static_cast<long double>(maximum))
        throw std::runtime_error(std::string(kind) + " is out of range");
    return static_cast<size_t>(count);
}

static size_t checked_combined_size(size_t left, size_t right, size_t maximum, const char *kind)
{
    if (right > maximum - left)
        throw std::runtime_error(std::string(kind) + " is too large");
    return left + right;
}

static std::vector<std::string> runtime_type_arguments(const std::string &type)
{
    const size_t open = type.find('[');
    if (open == std::string::npos || type.back() != ']')
        return {};

    std::vector<std::string> arguments;
    size_t start = open + 1;
    int depth = 0;
    for (size_t i = start; i < type.size() - 1; ++i)
    {
        if (type[i] == '[')
            ++depth;
        else if (type[i] == ']')
            --depth;
        else if (type[i] == ',' && depth == 0)
        {
            arguments.push_back(type.substr(start, i - start));
            start = i + 1;
        }
    }
    arguments.push_back(type.substr(start, type.size() - 1 - start));
    return arguments;
}

static bool runtime_type_matches(const Value &value, const std::string &type)
{
    if (type == "any" || type == "mixed")
        return true;

    const size_t open = type.find('[');
    const std::string base = open == std::string::npos ? type : type.substr(0, open);
    const auto arguments = runtime_type_arguments(type);
    if (base == "number") return std::holds_alternative<double>(value);
    if (base == "string") return std::holds_alternative<std::string>(value);
    if (base == "bool") return std::holds_alternative<bool>(value);
    if (base == "null" || base == "nothing") return std::holds_alternative<std::nullptr_t>(value);
    if (base == "function") return std::holds_alternative<std::shared_ptr<Function>>(value);
    if (base == "class") return std::holds_alternative<std::shared_ptr<Class>>(value);
    if (base == "bound_method") return std::holds_alternative<std::shared_ptr<UserBoundMethod>>(value);
    if (base == "native_method") return std::holds_alternative<std::shared_ptr<BoundMethod>>(value);
    if (base == "tag") return std::holds_alternative<std::shared_ptr<Tag>>(value);
    if (base == "texture2d") return std::holds_alternative<std::shared_ptr<Texture2D>>(value);
    if (base == "text_element") return std::holds_alternative<std::shared_ptr<TextElement>>(value);

    if (base == "array")
    {
        const auto *array = std::get_if<std::shared_ptr<Array>>(&value);
        if (!array || !*array)
            return false;
        if (arguments.size() == 1)
            for (const auto &element : (*array)->elements)
                if (!runtime_type_matches(element, arguments.front()))
                    return false;
        return true;
    }

    if (base == "dict")
    {
        const auto *dict = std::get_if<std::shared_ptr<Dict>>(&value);
        if (!dict || !*dict)
            return false;
        if (arguments.size() == 2)
            for (const auto &[key, element] : (*dict)->entries)
                if (!runtime_type_matches(Value{key}, arguments.front()) ||
                    !runtime_type_matches(element, arguments.back()))
                    return false;
        return true;
    }

    const auto *instance = std::get_if<std::shared_ptr<Instance>>(&value);
    if (!instance || !*instance)
        return false;
    for (auto klass = (*instance)->klass; klass; klass = klass->super_class)
        if (klass->name == base)
            return true;
    return false;
}

static size_t clamped_sequence_index(const Value &value, size_t length, const char *kind)
{
    const auto *number = std::get_if<double>(&value);
    if (!number || !std::isfinite(*number))
        throw std::runtime_error(std::string(kind) + " must be a finite number");
    const double index = std::trunc(*number);
    if (index <= 0)
        return 0;
    if (index >= static_cast<double>(length))
        return length;
    return static_cast<size_t>(index);
}

static std::string repeat_string(const std::string &value, double count)
{
    if (!std::isfinite(count) || std::floor(count) != count)
        throw std::runtime_error("Cannot multiply string by non-integer number");
    if (count <= 0 || value.empty())
        return {};

    std::string result;
    const size_t max_repetitions = result.max_size() / value.size();
    if (static_cast<long double>(count) > static_cast<long double>(max_repetitions))
        throw std::runtime_error("String repetition result is too large");

    const size_t repetitions = static_cast<size_t>(count);
    result.reserve(value.size() * repetitions);
    for (size_t i = 0; i < repetitions; ++i)
        result.append(value);
    return result;
}

void VM::init_globals()
{
    globals.reserve(256);
    const_variables.reserve(16);
    globals["math"] = init_math();
    globals["regex"] = init_regex();
    globals["timing"] = init_time();
    globals["encode"] = init_encode();
    globals["text"] = init_text();
    globals["random"] = init_random();
    globals["array"] = init_array();
    globals["html"] = init_builder();

#ifndef WEB_MODE
    globals["os"] = init_os();
    globals["gui"] = init_gui();
    globals["profiler"] = init_profiler();
    globals["file"] = init_file();

    const_variables.insert("file");
    const_variables.insert("profiler");
    const_variables.insert("os");
    const_variables.insert("gui");
#endif

    const_variables.insert("math");
    const_variables.insert("regex");
    const_variables.insert("timing");
    const_variables.insert("encode");
    const_variables.insert("text");
    const_variables.insert("random");
    const_variables.insert("array");

    const_variables.insert("html");

    const_variables.insert("scan");
    const_variables.insert("format");
    const_variables.insert("printf");
    const_variables.insert("print");
    const_variables.insert("println");
    const_variables.insert("exit");
    const_variables.insert("methods");

    globals["scan"] = make_native_method(nullptr, NativeMethod::INPUT);
    globals["format"] = make_native_method(nullptr, NativeMethod::FORMAT);
    globals["printf"] = make_native_method(nullptr, NativeMethod::PRINTF);
    globals["print"] = make_native_method(nullptr, NativeMethod::PRINT);
    globals["println"] = make_native_method(nullptr, NativeMethod::PRINTLN);
    globals["exit"] = make_native_method(nullptr, NativeMethod::EXIT);
    globals["methods"] = make_native_method(nullptr, NativeMethod::INSTANCE_METHODS);

    globals["a"] = make_native_method(nullptr, NativeMethod::HTML_A);
    globals["address"] = make_native_method(nullptr, NativeMethod::HTML_ADDRESS);
    globals["area"] = make_native_method(nullptr, NativeMethod::HTML_AREA);
    globals["article"] = make_native_method(nullptr, NativeMethod::HTML_ARTICLE);
    globals["aside"] = make_native_method(nullptr, NativeMethod::HTML_ASIDE);
    globals["audio"] = make_native_method(nullptr, NativeMethod::HTML_AUDIO);
    // globals["base"] = make_native_method(nullptr, NativeMethod::HTML_BASE);
    globals["blockquote"] = make_native_method(nullptr, NativeMethod::HTML_BLOCKQUOTE);
    globals["body"] = make_native_method(nullptr, NativeMethod::HTML_BODY);
    globals["br"] = make_native_method(nullptr, NativeMethod::HTML_BR);
    globals["button"] = make_native_method(nullptr, NativeMethod::HTML_BUTTON);
    globals["canvas"] = make_native_method(nullptr, NativeMethod::HTML_CANVAS);
    globals["caption"] = make_native_method(nullptr, NativeMethod::HTML_CAPTION);
    globals["cite"] = make_native_method(nullptr, NativeMethod::HTML_CITE);
    globals["code"] = make_native_method(nullptr, NativeMethod::HTML_CODE);
    globals["col"] = make_native_method(nullptr, NativeMethod::HTML_COL);
    globals["colgroup"] = make_native_method(nullptr, NativeMethod::HTML_COLGROUP);
    globals["construct"] = make_native_method(nullptr, NativeMethod::HTML_CONSTRUCT);
    globals["datalist"] = make_native_method(nullptr, NativeMethod::HTML_DATALIST);
    globals["dd"] = make_native_method(nullptr, NativeMethod::HTML_DD);
    globals["del"] = make_native_method(nullptr, NativeMethod::HTML_DEL);
    globals["div"] = make_native_method(nullptr, NativeMethod::HTML_DIV);
    globals["dl"] = make_native_method(nullptr, NativeMethod::HTML_DL);
    globals["dt"] = make_native_method(nullptr, NativeMethod::HTML_DT);
    globals["em"] = make_native_method(nullptr, NativeMethod::HTML_EM);
    // globals["embed"] = make_native_method(nullptr, NativeMethod::HTML_EMBED);
    globals["fieldset"] = make_native_method(nullptr, NativeMethod::HTML_FIELDSET);
    globals["figcaption"] = make_native_method(nullptr, NativeMethod::HTML_FIGCAPTION);
    globals["figure"] = make_native_method(nullptr, NativeMethod::HTML_FIGURE);
    globals["footer"] = make_native_method(nullptr, NativeMethod::HTML_FOOTER);
    globals["form"] = make_native_method(nullptr, NativeMethod::HTML_FORM);
    globals["h1"] = make_native_method(nullptr, NativeMethod::HTML_H1);
    globals["h2"] = make_native_method(nullptr, NativeMethod::HTML_H2);
    globals["h3"] = make_native_method(nullptr, NativeMethod::HTML_H3);
    globals["h4"] = make_native_method(nullptr, NativeMethod::HTML_H4);
    globals["h5"] = make_native_method(nullptr, NativeMethod::HTML_H5);
    globals["h6"] = make_native_method(nullptr, NativeMethod::HTML_H6);
    globals["head"] = make_native_method(nullptr, NativeMethod::HTML_HEAD);
    globals["header"] = make_native_method(nullptr, NativeMethod::HTML_HEADER);
    globals["hr"] = make_native_method(nullptr, NativeMethod::HTML_HR);
    globals["iframe"] = make_native_method(nullptr, NativeMethod::HTML_IFRAME);
    globals["img"] = make_native_method(nullptr, NativeMethod::HTML_IMG);
    globals["input"] = make_native_method(nullptr, NativeMethod::HTML_INPUT);
    globals["ins"] = make_native_method(nullptr, NativeMethod::HTML_INS);
    globals["label"] = make_native_method(nullptr, NativeMethod::HTML_LABEL);
    globals["legend"] = make_native_method(nullptr, NativeMethod::HTML_LEGEND);
    globals["li"] = make_native_method(nullptr, NativeMethod::HTML_LI);
    globals["link"] = make_native_method(nullptr, NativeMethod::HTML_LINK);
    globals["main"] = make_native_method(nullptr, NativeMethod::HTML_MAIN);
    globals["map"] = make_native_method(nullptr, NativeMethod::HTML_MAP);
    globals["mark"] = make_native_method(nullptr, NativeMethod::HTML_MARK);
    globals["meta"] = make_native_method(nullptr, NativeMethod::HTML_META);
    globals["meter"] = make_native_method(nullptr, NativeMethod::HTML_METER);
    globals["nav"] = make_native_method(nullptr, NativeMethod::HTML_NAV);
    globals["object"] = make_native_method(nullptr, NativeMethod::HTML_OBJECT);
    globals["ol"] = make_native_method(nullptr, NativeMethod::HTML_OL);
    globals["optgroup"] = make_native_method(nullptr, NativeMethod::HTML_OPTGROUP);
    globals["option"] = make_native_method(nullptr, NativeMethod::HTML_OPTION);
    globals["output"] = make_native_method(nullptr, NativeMethod::HTML_OUTPUT);
    globals["p"] = make_native_method(nullptr, NativeMethod::HTML_P);
    globals["param"] = make_native_method(nullptr, NativeMethod::HTML_PARAM);
    globals["picture"] = make_native_method(nullptr, NativeMethod::HTML_PICTURE);
    globals["pre"] = make_native_method(nullptr, NativeMethod::HTML_PRE);
    globals["progress"] = make_native_method(nullptr, NativeMethod::HTML_PROGRESS);
    globals["q"] = make_native_method(nullptr, NativeMethod::HTML_Q);
    globals["section"] = make_native_method(nullptr, NativeMethod::HTML_SECTION);
    globals["select"] = make_native_method(nullptr, NativeMethod::HTML_SELECT);
    globals["small"] = make_native_method(nullptr, NativeMethod::HTML_SMALL);
    globals["source"] = make_native_method(nullptr, NativeMethod::HTML_SOURCE);
    globals["span"] = make_native_method(nullptr, NativeMethod::HTML_SPAN);
    globals["strong"] = make_native_method(nullptr, NativeMethod::HTML_STRONG);
    globals["style"] = make_native_method(nullptr, NativeMethod::HTML_STYLE);
    globals["sub"] = make_native_method(nullptr, NativeMethod::HTML_SUB);
    globals["sup"] = make_native_method(nullptr, NativeMethod::HTML_SUP);
    globals["table"] = make_native_method(nullptr, NativeMethod::HTML_TABLE);
    globals["tbody"] = make_native_method(nullptr, NativeMethod::HTML_TBODY);
    globals["td"] = make_native_method(nullptr, NativeMethod::HTML_TD);
    globals["textarea"] = make_native_method(nullptr, NativeMethod::HTML_TEXTAREA);
    globals["tfoot"] = make_native_method(nullptr, NativeMethod::HTML_TFOOT);
    globals["th"] = make_native_method(nullptr, NativeMethod::HTML_TH);
    globals["thead"] = make_native_method(nullptr, NativeMethod::HTML_THEAD);
    globals["time"] = make_native_method(nullptr, NativeMethod::HTML_TIME);
    globals["title"] = make_native_method(nullptr, NativeMethod::HTML_TITLE);
    globals["tr"] = make_native_method(nullptr, NativeMethod::HTML_TR);
    globals["track"] = make_native_method(nullptr, NativeMethod::HTML_TRACK);
    globals["ul"] = make_native_method(nullptr, NativeMethod::HTML_UL);
    globals["video"] = make_native_method(nullptr, NativeMethod::HTML_VIDEO);
    globals["wbr"] = make_native_method(nullptr, NativeMethod::HTML_WBR);

    const_variables.insert("scan");
    const_variables.insert("format");
    const_variables.insert("printf");
    const_variables.insert("print");
    const_variables.insert("println");
    const_variables.insert("exit");
    const_variables.insert("methods");
    const_variables.insert("a");
    const_variables.insert("address");
    const_variables.insert("area");
    const_variables.insert("article");
    const_variables.insert("aside");
    const_variables.insert("audio");
    // const_variables.insert("base");
    const_variables.insert("blockquote");
    const_variables.insert("body");
    const_variables.insert("br");
    const_variables.insert("button");
    const_variables.insert("canvas");
    const_variables.insert("caption");
    const_variables.insert("cite");
    const_variables.insert("code");
    const_variables.insert("col");
    const_variables.insert("colgroup");
    const_variables.insert("construct");
    const_variables.insert("datalist");
    const_variables.insert("dd");
    const_variables.insert("del");
    const_variables.insert("div");
    const_variables.insert("dl");
    const_variables.insert("dt");
    const_variables.insert("em");
    // const_variables.insert("embed");
    const_variables.insert("fieldset");
    const_variables.insert("figcaption");
    const_variables.insert("figure");
    const_variables.insert("footer");
    const_variables.insert("form");
    const_variables.insert("h1");
    const_variables.insert("h2");
    const_variables.insert("h3");
    const_variables.insert("h4");
    const_variables.insert("h5");
    const_variables.insert("h6");
    const_variables.insert("head");
    const_variables.insert("header");
    const_variables.insert("hr");
    const_variables.insert("iframe");
    const_variables.insert("img");
    const_variables.insert("input");
    const_variables.insert("ins");
    const_variables.insert("label");
    const_variables.insert("legend");
    const_variables.insert("li");
    const_variables.insert("link");
    const_variables.insert("main");
    const_variables.insert("map");
    const_variables.insert("mark");
    const_variables.insert("meta");
    const_variables.insert("meter");
    const_variables.insert("nav");
    const_variables.insert("object");
    const_variables.insert("ol");
    const_variables.insert("optgroup");
    const_variables.insert("option");
    const_variables.insert("output");
    const_variables.insert("p");
    const_variables.insert("param");
    const_variables.insert("picture");
    const_variables.insert("pre");
    const_variables.insert("progress");
    const_variables.insert("q");
    const_variables.insert("section");
    const_variables.insert("select");
    const_variables.insert("small");
    const_variables.insert("source");
    const_variables.insert("span");
    const_variables.insert("strong");
    const_variables.insert("style");
    const_variables.insert("sub");
    const_variables.insert("sup");
    const_variables.insert("table");
    const_variables.insert("tbody");
    const_variables.insert("td");
    const_variables.insert("textarea");
    const_variables.insert("tfoot");
    const_variables.insert("th");
    const_variables.insert("thead");
    const_variables.insert("time");
    const_variables.insert("title");
    const_variables.insert("tr");
    const_variables.insert("track");
    const_variables.insert("ul");
    const_variables.insert("video");
    const_variables.insert("wbr");
}

void VM::interpret(Chunk &new_chunk)
{
    stack.clear();
    frames.clear();
    global_cache.clear();
    method_cache.clear();
    native_args.clear();
    if (new_chunk.code.empty())
        throw std::runtime_error("Cannot interpret an empty chunk");

    stack.reserve(std::max(stack.capacity(), std::min(new_chunk.code.size(), size_t{1024})));
    frames.reserve(64);
    global_cache.reserve(std::max(global_cache.bucket_count(), size_t{64}));
    method_cache.reserve(std::max(method_cache.bucket_count(), size_t{32}));
    native_args.reserve(std::max(native_args.capacity(), size_t{8}));
    chunk = &new_chunk;
    ip = chunk->code.data();
    run();
}

void VM::run()
{

    for (;;)
    {
        const uint8_t *instruction = ip;
        OpCode opcode = static_cast<OpCode>(read_byte());

        // I use this to track opcode executions, I will leave it here for now
        /* std::cout << "[DEBUG] Executing opcode: " << static_cast<int>(opcode) << ", stack: [";
        for (size_t i = 0; i < stack.size(); ++i)
        {
            std::cout << stringify(stack[i]);
            if (i + 1 < stack.size())
                std::cout << ", ";
        }
        std::cout << "]" << std::endl; */

        switch (opcode)
        {
        case OpCode::CONSTANT:
        {
            stack.push_back(chunk->constants.at(read_short()));
            break;
        }

        case OpCode::CONSTANT_SHORT:
        {
            stack.push_back(chunk->constants.at(read_byte()));
            break;
        }

        case OpCode::ADD:
        case OpCode::SUBTRACT:
        case OpCode::MULTIPLY:
        case OpCode::DIVIDE:
        case OpCode::POWER:
        {
            Value b = pop();
            Value a = pop();

            if (const double *x = std::get_if<double>(&a))
            {
                if (const double *y = std::get_if<double>(&b))
                {
                    switch (opcode)
                    {
                    case OpCode::ADD:
                        push(*x + *y);
                        break;
                    case OpCode::SUBTRACT:
                        push(*x - *y);
                        break;
                    case OpCode::MULTIPLY:
                        push(*x * *y);
                        break;
                    case OpCode::POWER:
                        push(std::pow(*x, *y));
                        break;
                    case OpCode::DIVIDE:
                        if (*y == 0)
                            throw std::runtime_error("Division by zero");
                        push(*x / *y);
                        break;
                    default:
                        break;
                    }
                    break;
                }
            }

            if (opcode == OpCode::ADD &&
                (std::holds_alternative<std::string>(a) ||
                 std::holds_alternative<std::string>(b)))
            {
                stack.push_back(stringify(a) + stringify(b));
                break;
            }

            if (opcode == OpCode::ADD &&
                std::holds_alternative<std::shared_ptr<Array>>(a) &&
                std::holds_alternative<std::shared_ptr<Array>>(b))
            {
                auto left = std::get<std::shared_ptr<Array>>(a);
                auto right = std::get<std::shared_ptr<Array>>(b);

                auto result = std::make_shared<Array>();
                const size_t combined_size = checked_combined_size(
                    left->elements.size(), right->elements.size(), result->elements.max_size(), "Array concatenation");
                result->elements.reserve(combined_size);

                result->elements.insert(
                    result->elements.end(),
                    left->elements.begin(),
                    left->elements.end());

                result->elements.insert(
                    result->elements.end(),
                    right->elements.begin(),
                    right->elements.end());

                stack.push_back(result);
                break;
            }

            if (std::holds_alternative<std::string>(a) && std::holds_alternative<double>(b))
            {
                if (opcode == OpCode::MULTIPLY)
                {
                    stack.push_back(repeat_string(std::get<std::string>(a), std::get<double>(b)));
                    break;
                }
            }

            if (std::holds_alternative<double>(a) && std::holds_alternative<std::string>(b))
            {
                if (opcode == OpCode::MULTIPLY)
                {
                    stack.push_back(repeat_string(std::get<std::string>(b), std::get<double>(a)));
                    break;
                }
            }

            if (std::holds_alternative<std::shared_ptr<Array>>(a) && std::holds_alternative<double>(b))
            {
                auto arr = std::get<std::shared_ptr<Array>>(a);
                double scalar = std::get<double>(b);

                auto result = std::make_shared<Array>();
                result->elements.reserve(arr->elements.size());

                for (const auto &v : arr->elements)
                {
                    if (!std::holds_alternative<double>(v))
                        throw std::runtime_error("Array must contain only numbers");

                    double x = std::get<double>(v);
                    double r;

                    switch (opcode)
                    {
                    case OpCode::ADD:
                        r = x + scalar;
                        break;
                    case OpCode::SUBTRACT:
                        r = x - scalar;
                        break;
                    case OpCode::MULTIPLY:
                        r = x * scalar;
                        break;
                    case OpCode::DIVIDE:
                        if (scalar == 0)
                            throw std::runtime_error("Division by zero");
                        r = x / scalar;
                        break;
                    default:
                        r = 0;
                    }

                    result->elements.push_back(r);
                }

                stack.push_back(result);
                break;
            }

            if (opcode == OpCode::ADD &&
                std::holds_alternative<std::shared_ptr<Tag>>(a) &&
                std::holds_alternative<std::shared_ptr<Tag>>(b))
            {
                auto parent = std::get<std::shared_ptr<Tag>>(a);
                auto child = std::get<std::shared_ptr<Tag>>(b);

                parent->child(child);

                stack.push_back(parent);
                break;
            }

            throw std::runtime_error("Operands must be numbers: " + stringify(a) + " " + stringify(b));
        }

        case OpCode::PERCENT:
        {
            Value b = pop();
            Value a = pop();

            if (std::holds_alternative<double>(a) && std::holds_alternative<double>(b))
            {
                double x = std::get<double>(a);
                double y = std::get<double>(b);
                if (y == 0.0)
                {
                    throw std::runtime_error("Division by zero");
                }
                stack.push_back(std::fmod(x, y));
            }

            else
            {
                throw std::runtime_error("Operands must be numbers");
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
            if (!std::holds_alternative<double>(v))
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

        case OpCode::GREATER_EQUAL:
        {
            auto b = pop();
            auto a = pop();
            push(compare_values(a, b, GREATER_EQUAL));
            break;
        }

        case OpCode::LESS_EQUAL:
        {
            auto b = pop();
            auto a = pop();
            push(compare_values(a, b, LESS_EQUAL));
            break;
        }

        case OpCode::NOT_EQUAL:
        {
            auto b = pop();
            auto a = pop();
            push(a != b);
            break;
        }

        case OpCode::GREATER:
        case OpCode::LESS:
        {
            Value b = pop();
            Value a = pop();
            if (!std::holds_alternative<double>(a) || !std::holds_alternative<double>(b))
                throw std::runtime_error("Operands must be numbers");

            double x = std::get<double>(a);
            double y = std::get<double>(b);

            stack.push_back(opcode == OpCode::GREATER ? x > y : x < y);
            break;
        }

        case OpCode::POP:
        {
            Value v = pop();

            // I use this to track how pop works, leave it here for now
            // std::cout << "[DEBUG] POP -> " << stringify(v) << std::endl;
            break;
        }

        case OpCode::DEFINE_CONST:
        {
            uint16_t name_index = read_short();
            const std::string &name =
                std::get<std::string>(chunk->constants.at(name_index));

            if (globals.contains(name))
                throw std::runtime_error("Cannot redefine constant \"" + name + "\"");

            globals[name] = pop();
            const_variables.insert(name);
            break;
        }

        case OpCode::DEFINE_GLOBAL:
        {
            uint16_t name_index = read_short();
            const std::string &name =
                std::get<std::string>(chunk->constants.at(name_index));

            if (const_variables.contains(name))
                throw std::runtime_error("Cannot redefine constant \"" + name + "\"");

            globals[name] = pop();
            break;
        }

        case OpCode::GET_GLOBAL:
        {
            uint16_t name_index = read_short();
            auto cached = global_cache.find(instruction);
            if (cached != global_cache.end())
            {
                stack.push_back(*cached->second.value);
                break;
            }

            const std::string &name = std::get<std::string>(chunk->constants.at(name_index));
            auto it = globals.find(name);
            if (it == globals.end())
                throw std::runtime_error("Undefined variable \"" + name + "\"");

            global_cache.emplace(instruction, GlobalCacheEntry{&it->second, const_variables.contains(name)});
            stack.push_back(it->second);
            break;
        }

        case OpCode::SET_GLOBAL:
        {
            uint16_t name_index = read_short();
            auto cached = global_cache.find(instruction);
            if (cached != global_cache.end())
            {
                if (cached->second.is_const)
                {
                    const std::string &name = std::get<std::string>(chunk->constants.at(name_index));
                    throw std::runtime_error("Cannot reassign constant \"" + name + "\"");
                }

                *cached->second.value = stack.back();
                break;
            }

            const std::string &name = std::get<std::string>(chunk->constants.at(name_index));
            auto it = globals.find(name);
            if (it == globals.end())
                throw std::runtime_error("Undefined variable \"" + name + "\"");

            const bool is_const = const_variables.contains(name);
            if (is_const)
                throw std::runtime_error("Cannot reassign constant \"" + name + "\"");

            global_cache.emplace(instruction, GlobalCacheEntry{&it->second, is_const});
            it->second = stack.back();
            break;
        }

        case OpCode::INCREMENT_GLOBAL:
        {
            uint16_t name_index = read_short();
            const bool keep_result = read_byte() != 0;
            auto cached = global_cache.find(instruction);
            Value *target = nullptr;
            bool is_const = false;

            if (cached != global_cache.end())
            {
                target = cached->second.value;
                is_const = cached->second.is_const;
            }
            else
            {
                const std::string &name = std::get<std::string>(chunk->constants.at(name_index));
                auto it = globals.find(name);
                if (it == globals.end())
                    throw std::runtime_error("Undefined variable \"" + name + "\"");

                is_const = const_variables.contains(name);
                target = &it->second;
                global_cache.emplace(instruction, GlobalCacheEntry{target, is_const});
            }

            if (is_const)
            {
                const std::string &name = std::get<std::string>(chunk->constants.at(name_index));
                throw std::runtime_error("Cannot reassign constant \"" + name + "\"");
            }

            Value updated = add_one(*target);
            *target = updated;
            if (keep_result)
                push(std::move(updated));
            break;
        }

        case OpCode::INCREMENT_LOCAL:
        {
            const uint8_t slot = read_byte();
            const bool keep_result = read_byte() != 0;
            if (frames.empty())
                throw std::runtime_error("INCREMENT_LOCAL called with no active frame");

            const size_t index = frames.back().stack_start + slot;
            if (index >= stack.size())
                throw std::runtime_error("INCREMENT_LOCAL out of bounds");

            Value updated = add_one(stack[index]);
            stack[index] = updated;
            if (keep_result)
                push(std::move(updated));
            break;
        }

        case OpCode::UPDATE_GLOBAL:
        {
            const uint16_t name_index = read_short();
            const double delta = read_byte() == 1 ? 1.0 : -1.0;
            const uint8_t result_mode = read_byte();
            auto cached = global_cache.find(instruction);
            Value *target = nullptr;
            bool is_const = false;

            if (cached != global_cache.end())
            {
                target = cached->second.value;
                is_const = cached->second.is_const;
            }
            else
            {
                const std::string &name = std::get<std::string>(chunk->constants.at(name_index));
                auto it = globals.find(name);
                if (it == globals.end())
                    throw std::runtime_error("Undefined variable \"" + name + "\"");
                target = &it->second;
                is_const = const_variables.contains(name);
                global_cache.emplace(instruction, GlobalCacheEntry{target, is_const});
            }

            if (is_const)
            {
                const std::string &name = std::get<std::string>(chunk->constants.at(name_index));
                throw std::runtime_error("Cannot reassign constant \"" + name + "\"");
            }

            const double *number = std::get_if<double>(target);
            if (!number)
                throw std::runtime_error("Increment and decrement require a number");
            const double previous = *number;
            *target = previous + delta;

            if (result_mode == 1)
                push(*target);
            else if (result_mode == 2)
                push(previous);
            else if (result_mode != 0)
                throw std::runtime_error("Invalid increment/decrement result mode");
            break;
        }

        case OpCode::UPDATE_LOCAL:
        {
            const uint8_t slot = read_byte();
            const double delta = read_byte() == 1 ? 1.0 : -1.0;
            const uint8_t result_mode = read_byte();
            if (frames.empty())
                throw std::runtime_error("UPDATE_LOCAL called with no active frame");

            const size_t index = frames.back().stack_start + slot;
            if (index >= stack.size())
                throw std::runtime_error("UPDATE_LOCAL out of bounds");

            const double *number = std::get_if<double>(&stack[index]);
            if (!number)
                throw std::runtime_error("Increment and decrement require a number");
            const double previous = *number;
            stack[index] = previous + delta;

            if (result_mode == 1)
                push(stack[index]);
            else if (result_mode == 2)
                push(previous);
            else if (result_mode != 0)
                throw std::runtime_error("Invalid increment/decrement result mode");
            break;
        }

        case OpCode::UPDATE_PROPERTY:
        {
            const uint16_t name_index = read_short();
            const std::string &name = std::get<std::string>(chunk->constants.at(name_index));
            const double delta = read_byte() == 1 ? 1.0 : -1.0;
            const uint8_t result_mode = read_byte();
            Value object = pop();
            auto instance = std::get_if<std::shared_ptr<Instance>>(&object);
            if (!instance || !*instance)
                throw std::runtime_error("Only instance fields can be incremented or decremented");

            auto field = (*instance)->fields.find(name);
            if (field == (*instance)->fields.end())
                throw std::runtime_error("Undefined property \"" + name + "\"");
            const double *number = std::get_if<double>(&field->second);
            if (!number)
                throw std::runtime_error("Increment and decrement require a number");
            const double previous = *number;
            field->second = previous + delta;

            if (result_mode == 1)
                push(field->second);
            else if (result_mode == 2)
                push(previous);
            else if (result_mode != 0)
                throw std::runtime_error("Invalid increment/decrement result mode");
            break;
        }

        case OpCode::UPDATE_INDEX:
        {
            const double delta = read_byte() == 1 ? 1.0 : -1.0;
            const uint8_t result_mode = read_byte();
            Value index_value = pop();
            Value container_value = pop();
            double previous;
            double updated;

            if (auto array = std::get_if<std::shared_ptr<Array>>(&container_value))
            {
                if (!*array)
                    throw std::runtime_error("Cannot increment an uninitialized array");
                const size_t index = checked_sequence_index(index_value, (*array)->elements.size(), "Array");
                const double *number = std::get_if<double>(&(*array)->elements[index]);
                if (!number)
                    throw std::runtime_error("Increment and decrement require a number");
                previous = *number;
                updated = previous + delta;
                (*array)->elements[index] = updated;
            }
            else if (auto dict = std::get_if<std::shared_ptr<Dict>>(&container_value))
            {
                if (!*dict)
                    throw std::runtime_error("Cannot increment an uninitialized dictionary");
                if (!std::holds_alternative<std::string>(index_value))
                    throw std::runtime_error("Dictionary key must be string");
                const std::string &key = std::get<std::string>(index_value);
                Value current = (*dict)->get(key);
                const double *number = std::get_if<double>(&current);
                if (!number)
                    throw std::runtime_error("Increment and decrement require a number");
                previous = *number;
                updated = previous + delta;
                (*dict)->set(key, updated);
            }
            else
                throw std::runtime_error("Only array and dictionary elements can be incremented or decremented");

            if (result_mode == 1)
                push(updated);
            else if (result_mode == 2)
                push(previous);
            else if (result_mode != 0)
                throw std::runtime_error("Invalid increment/decrement result mode");
            break;
        }

        case OpCode::JUMP_IF_GLOBAL_COMPARE:
        {
            const uint16_t name_index = read_short();
            const uint16_t limit_index = read_short();
            const TokenType comparison = static_cast<TokenType>(read_byte());
            const uint16_t offset = read_short();

            auto cached = global_cache.find(instruction);
            Value *target = nullptr;
            if (cached != global_cache.end())
            {
                target = cached->second.value;
            }
            else
            {
                const std::string &name = std::get<std::string>(chunk->constants.at(name_index));
                auto it = globals.find(name);
                if (it == globals.end())
                    throw std::runtime_error("Undefined variable \"" + name + "\"");

                target = &it->second;
                global_cache.emplace(instruction, GlobalCacheEntry{target, const_variables.contains(name)});
            }

            if (!compare_values(*target, chunk->constants.at(limit_index), comparison))
                ip += offset;
            break;
        }

        case OpCode::JUMP_IF_LOCAL_COMPARE:
        {
            const uint8_t slot = read_byte();
            const uint16_t limit_index = read_short();
            const TokenType comparison = static_cast<TokenType>(read_byte());
            const uint16_t offset = read_short();

            if (frames.empty())
                throw std::runtime_error("JUMP_IF_LOCAL_COMPARE called with no active frame");

            const size_t index = frames.back().stack_start + slot;
            if (index >= stack.size())
                throw std::runtime_error("JUMP_IF_LOCAL_COMPARE out of bounds");

            if (!compare_values(stack[index], chunk->constants.at(limit_index), comparison))
                ip += offset;
            break;
        }

        case OpCode::LOOP_GLOBAL_INCREMENT_COMPARE:
        {
            const uint16_t name_index = read_short();
            const uint16_t limit_index = read_short();
            const TokenType comparison = static_cast<TokenType>(read_byte());
            const uint8_t update_mode = read_byte();
            const uint16_t offset = read_short();

            auto cached = global_cache.find(instruction);
            Value *target = nullptr;
            bool is_const = false;
            if (cached != global_cache.end())
            {
                target = cached->second.value;
                is_const = cached->second.is_const;
            }
            else
            {
                const std::string &name = std::get<std::string>(chunk->constants.at(name_index));
                auto it = globals.find(name);
                if (it == globals.end())
                    throw std::runtime_error("Undefined variable \"" + name + "\"");
                target = &it->second;
                is_const = const_variables.contains(name);
                global_cache.emplace(instruction, GlobalCacheEntry{target, is_const});
            }

            if (is_const)
                throw std::runtime_error("Cannot reassign constant \"" +
                                         std::get<std::string>(chunk->constants.at(name_index)) + "\"");

            if (update_mode == 0)
                *target = add_one(*target);
            else
            {
                const double *number = std::get_if<double>(target);
                if (!number)
                    throw std::runtime_error("Increment and decrement require a number");
                *target = *number + (update_mode == 1 ? 1.0 : -1.0);
            }
            if (compare_values(*target, chunk->constants.at(limit_index), comparison))
                ip -= offset;
            break;
        }

        case OpCode::LOOP_LOCAL_INCREMENT_COMPARE:
        {
            const uint8_t slot = read_byte();
            const uint16_t limit_index = read_short();
            const TokenType comparison = static_cast<TokenType>(read_byte());
            const uint8_t update_mode = read_byte();
            const uint16_t offset = read_short();

            if (frames.empty())
                throw std::runtime_error("LOOP_LOCAL_INCREMENT_COMPARE called with no active frame");
            const size_t index = frames.back().stack_start + slot;
            if (index >= stack.size())
                throw std::runtime_error("LOOP_LOCAL_INCREMENT_COMPARE out of bounds");

            if (update_mode == 0)
                stack[index] = add_one(stack[index]);
            else
            {
                const double *number = std::get_if<double>(&stack[index]);
                if (!number)
                    throw std::runtime_error("Increment and decrement require a number");
                stack[index] = *number + (update_mode == 1 ? 1.0 : -1.0);
            }
            if (compare_values(stack[index], chunk->constants.at(limit_index), comparison))
                ip -= offset;
            break;
        }

        case OpCode::FAST_FORWARD_GLOBAL_LOOP:
        {
            const uint16_t name_index = read_short();
            const uint16_t limit_index = read_short();
            const TokenType comparison = static_cast<TokenType>(read_byte());
            const uint8_t update_mode = read_byte();

            const std::string &name = std::get<std::string>(chunk->constants.at(name_index));
            auto it = globals.find(name);
            if (it == globals.end())
                throw std::runtime_error("Undefined variable \"" + name + "\"");
            Value &target = it->second;
            const Value &limit = chunk->constants.at(limit_index);
            const double *number = std::get_if<double>(&target);
            if (!number)
            {
                if (!compare_values(target, limit, comparison))
                    break;
                throw std::runtime_error("Increment and decrement require a number");
            }

            const double initial = *number;
            if (!compare_values(target, limit, comparison))
                break;
            if (const_variables.contains(name))
                throw std::runtime_error("Cannot reassign constant \"" + name + "\"");

            const double bound = std::get<double>(limit);
            const double direction = update_mode == 1 ? 1.0 : -1.0;
            constexpr double max_exact_integer = 9007199254740992.0;
            double count = -1.0;
            if (std::isfinite(initial) && std::isfinite(bound) &&
                std::trunc(initial) == initial && std::trunc(bound) == bound &&
                std::abs(initial) <= max_exact_integer && std::abs(bound) <= max_exact_integer)
            {
                if (update_mode == 1 && comparison == LESS)
                    count = bound - initial;
                else if (update_mode == 1 && comparison == LESS_EQUAL)
                    count = bound - initial + 1.0;
                else if (update_mode == 2 && comparison == GREATER)
                    count = initial - bound;
                else if (update_mode == 2 && comparison == GREATER_EQUAL)
                    count = initial - bound + 1.0;
            }

            if (count >= 0.0 && std::isfinite(count))
            {
                const double result = initial + direction * count;
                if (std::isfinite(result) && std::abs(result) <= max_exact_integer &&
                    !compare_values(Value(result), limit, comparison))
                {
                    target = result;
                    break;
                }
            }

            double current = initial;
            while (compare_values(Value(current), limit, comparison))
                current += direction;
            target = current;
            break;
        }

        case OpCode::FAST_FORWARD_LOCAL_LOOP:
        {
            const uint8_t slot = read_byte();
            const uint16_t limit_index = read_short();
            const TokenType comparison = static_cast<TokenType>(read_byte());
            const uint8_t update_mode = read_byte();

            if (frames.empty())
                throw std::runtime_error("FAST_FORWARD_LOCAL_LOOP called with no active frame");
            const size_t index = frames.back().stack_start + slot;
            if (index >= stack.size())
                throw std::runtime_error("FAST_FORWARD_LOCAL_LOOP out of bounds");

            Value &target = stack[index];
            const Value &limit = chunk->constants.at(limit_index);
            const double *number = std::get_if<double>(&target);
            if (!number)
            {
                if (!compare_values(target, limit, comparison))
                    break;
                throw std::runtime_error("Increment and decrement require a number");
            }

            const double initial = *number;
            if (!compare_values(target, limit, comparison))
                break;

            const double bound = std::get<double>(limit);
            const double direction = update_mode == 1 ? 1.0 : -1.0;
            constexpr double max_exact_integer = 9007199254740992.0;
            double count = -1.0;
            if (std::isfinite(initial) && std::isfinite(bound) &&
                std::trunc(initial) == initial && std::trunc(bound) == bound &&
                std::abs(initial) <= max_exact_integer && std::abs(bound) <= max_exact_integer)
            {
                if (update_mode == 1 && comparison == LESS)
                    count = bound - initial;
                else if (update_mode == 1 && comparison == LESS_EQUAL)
                    count = bound - initial + 1.0;
                else if (update_mode == 2 && comparison == GREATER)
                    count = initial - bound;
                else if (update_mode == 2 && comparison == GREATER_EQUAL)
                    count = initial - bound + 1.0;
            }

            if (count >= 0.0 && std::isfinite(count))
            {
                const double result = initial + direction * count;
                if (std::isfinite(result) && std::abs(result) <= max_exact_integer &&
                    !compare_values(Value(result), limit, comparison))
                {
                    target = result;
                    break;
                }
            }

            double current = initial;
            while (compare_values(Value(current), limit, comparison))
                current += direction;
            target = current;
            break;
        }

        case OpCode::GET_LOCAL:
        {
            uint8_t slot = read_byte();

            // Another debugging line, keep it here
            /* std::cout << "[DEBUG] GET_LOCAL slot=" << int(slot)
                      << ", stack.size=" << stack.size();

            if (!frames.empty())
                std::cout << ", frame.stack_start=" << frames.back().stack_start;

            std::cout << std::endl; */

            if (frames.empty())
                throw std::runtime_error("GET_LOCAL called with no active frame");

            size_t idx = frames.back().stack_start + slot;
            if (idx >= stack.size())
            {
                std::cerr << "[ERROR] GET_LOCAL out of bounds, idx=" << idx
                          << ", stack.size=" << stack.size() << std::endl;
                throw std::runtime_error("GET_LOCAL out of bounds");
            }

            stack.push_back(stack[idx]);
            break;
        }

        case OpCode::SET_LOCAL:
        {
            uint8_t slot = read_byte();

            // Keep this here for debug
            /* std::cout << "[DEBUG] SET_LOCAL slot=" << int(slot)
                      << ", stack.size=" << stack.size();

            if (!frames.empty())
                std::cout << ", frame.stack_start=" << frames.back().stack_start;

            std::cout << std::endl; */

            if (frames.empty())
                throw std::runtime_error("SET_LOCAL called with no active frame");

            size_t idx = frames.back().stack_start + slot;
            if (idx >= stack.size())
            {
                std::cerr << "[ERROR] SET_LOCAL out of bounds, idx=" << idx
                          << ", stack.size=" << stack.size() << std::endl;
                throw std::runtime_error("SET_LOCAL out of bounds");
            }

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
            if (is_truthy(condition))
                ip += offset;
            break;
        }

        case OpCode::JUMP_IF_FALSE:
        {
            uint16_t offset = read_short();
            Value condition = pop();
            if (!is_truthy(condition))
                ip += offset;
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
            uint16_t constant_index = read_short();
            Value constant = chunk->constants.at(constant_index);

            if (!std::holds_alternative<std::shared_ptr<Function>>(constant))
                throw std::runtime_error("CLOSURE constant is not a function");

            stack.push_back(constant);
            break;
        }

        case OpCode::CALL:
        {
            uint8_t arg_count = read_byte();

            // For debugging calls
            // std::cout << "[DEBUG] CALL with " << int(arg_count) << " args" << std::endl;

            if (stack.size() < arg_count + 1)
                throw std::runtime_error("Not enough values on the stack for CALL");

            Value callee = stack[stack.size() - 1 - arg_count];
            size_t callee_index = stack.size() - 1 - arg_count;

            if (std::holds_alternative<std::shared_ptr<Class>>(callee))
            {
                auto klass = std::get<std::shared_ptr<Class>>(callee);
                size_t callee_index = stack.size() - 1 - arg_count;
                auto instance = std::make_shared<Instance>(klass);

                stack[callee_index] = instance;

                auto init = klass->find_method("init");

                if (init)
                {
                    if (arg_count != init->arity)
                    {
                        throw std::runtime_error(
                            "Constructor expected " +
                            std::to_string(init->arity) +
                            " arguments but got " +
                            std::to_string(arg_count));
                    }

                    frames.push_back({chunk, ip, callee_index, true, init});

                    chunk = &init->chunk;
                    ip = chunk->code.data();

                    continue;
                }

                stack.erase(stack.begin() + callee_index + 1, stack.begin() + callee_index + 1 + arg_count);
                break;
            }

            else if (std::holds_alternative<std::shared_ptr<UserBoundMethod>>(callee))
            {
                auto bound = std::get<std::shared_ptr<UserBoundMethod>>(callee);
                auto func = bound->method;
                auto receiver = bound->receiver;

                const std::string caller_class = !frames.empty() && frames.back().function
                    ? frames.back().function->owner_class
                    : std::string{};
                if (!func->is_public && func->owner_class != caller_class)
                    throw std::runtime_error("Cannot call private method \"" + func->name + "\"");

                if (arg_count != func->arity)
                    throw std::runtime_error(
                        "Expected " + std::to_string(func->arity) +
                        " arguments but got " + std::to_string(arg_count));

                size_t callee_index = stack.size() - 1 - arg_count;

                stack[callee_index] = receiver;

                frames.push_back({chunk, ip, callee_index, false, func});

                chunk = &func->chunk;
                ip = chunk->code.data();
                continue;
            }

            if (std::holds_alternative<std::shared_ptr<BoundMethod>>(callee))
            {
                size_t local_index = stack.size() - 1 - arg_count;
                auto method = std::get<std::shared_ptr<BoundMethod>>(stack[local_index]);
                auto &args = native_args;
                args.clear();
                args.reserve(arg_count);
                for (size_t i = 0; i < arg_count; ++i)
                    args.push_back(std::move(stack[local_index + 1 + i]));
                stack.resize(local_index);

                switch (method->method)
                {
                    NATIVE_GLOBALS_INPUT
                    NATIVE_GLOBALS_RAND
                    NATIVE_GLOBALS_RANDINT
                    NATIVE_GLOBALS_FILL
                    NATIVE_GLOBALS_INIT
                    NATIVE_GLOBALS_ARRAY_INPUT
                    NATIVE_GLOBALS_FORMAT
                    NATIVE_GLOBALS_PRINTF
                    NATIVE_GLOBALS_PRINT
                    NATIVE_GLOBALS_PRINTLN
                    NATIVE_GLOBALS_READ_FILE
                    NATIVE_GLOBALS_WRITE_FILE
                    NATIVE_GLOBALS_APPEND_FILE
                    NATIVE_GLOBALS_PARSE_JSON
                    NATIVE_GLOBALS_TO_JSON
                    NATIVE_GLOBALS_PARSE_CSV
                    NATIVE_GLOBALS_TO_CSV
                    NATIVE_GLOBALS_STRINGIFY
                    NATIVE_GLOBALS_ERASE_FILE
                    NATIVE_GLOBALS_INSTANCE_METHODS
                    NATIVE_GLOBALS_JSON_TO_HTML
                    NATIVE_GLOBALS_CSV_TO_HTML

                    NATIVE_GLOBALS_POW
                    NATIVE_GLOBALS_SQRT
                    NATIVE_GLOBALS_FACT
                    NATIVE_GLOBALS_FLOOR
                    NATIVE_GLOBALS_CEIL
                    NATIVE_GLOBALS_ABS
                    NATIVE_GLOBALS_LOG
                    NATIVE_GLOBALS_LOG10
                    NATIVE_GLOBALS_LOG2
                    NATIVE_GLOBALS_MATH_MAX
                    NATIVE_GLOBALS_MATH_MIN
                    NATIVE_GLOBALS_MATH_AVERAGE
                    NATIVE_GLOBALS_SIN
                    NATIVE_GLOBALS_COS
                    NATIVE_GLOBALS_TAN
                    NATIVE_GLOBALS_ASIN
                    NATIVE_GLOBALS_ACOS
                    NATIVE_GLOBALS_ATAN
                    NATIVE_GLOBALS_DEGREES
                    NATIVE_GLOBALS_RADIANS
                    NATIVE_GLOBALS_IS_ODD
                    NATIVE_GLOBALS_IS_EVEN
                    NATIVE_GLOBALS_IS_PRIME
                    NATIVE_GLOBALS_GCD
                    NATIVE_GLOBALS_LCM
                    NATIVE_GLOBALS_NEXT_PRIME
                    NATIVE_GLOBALS_SUM_DIGITS
                    NATIVE_GLOBALS_FIBONACCI
                    NATIVE_GLOBALS_PALINDROME
                    NATIVE_GLOBALS_ROUND
                    NATIVE_GLOBALS_PI
                    NATIVE_GLOBALS_PRECISION

                    NATIVE_GLOBALS_RANDOM_COLOR_RGB
                    NATIVE_GLOBALS_RANDOM_COLOR_HEX

                    NATIVE_GLOBALS_MATCH

                    NATIVE_GLOBALS_SECOND_TO_MINUTE
                    NATIVE_GLOBALS_SECOND_TO_HOUR
                    NATIVE_GLOBALS_SECOND_TO_DAY
                    NATIVE_GLOBALS_MINUTE_TO_SECOND
                    NATIVE_GLOBALS_MINUTE_TO_HOUR
                    NATIVE_GLOBALS_MINUTE_TO_DAY
                    NATIVE_GLOBALS_HOUR_TO_SECOND
                    NATIVE_GLOBALS_HOUR_TO_MINUTE
                    NATIVE_GLOBALS_HOUR_TO_DAY
                    NATIVE_GLOBALS_LEAP_YEAR
                    NATIVE_GLOBALS_TODAY

                    NATIVE_GLOBALS_BASE

                    NATIVE_GLOBALS_EXISTS
                    NATIVE_GLOBALS_IS_FILE
                    NATIVE_GLOBALS_IS_DIRECTORY
                    NATIVE_GLOBALS_MAKE_DIRECTORY
                    NATIVE_GLOBALS_REMOVE_DIRECTORY
                    NATIVE_GLOBALS_LIST_DIRECTORIES
                    NATIVE_GLOBALS_LIST_FILES
                    NATIVE_GLOBALS_SYSTEM
                    NATIVE_GLOBALS_EXIT
                    NATIVE_GLOBALS_DATE
                    NATIVE_GLOBALS_COPY
                    NATIVE_GLOBALS_RENAME

                    NATIVE_GLOBALS_PROFILE_START
                    NATIVE_GLOBALS_PROFILE_END
                    NATIVE_GLOBALS_PROFILE_REPORT
                    NATIVE_GLOBALS_PROFILE_RESET

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
                    NATIVE_PRIMITIVE_TO_INT
                    NATIVE_PRIMITIVE_BOOL_TO_INT
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

                    NATIVE_DICT_CONTAINS
                    NATIVE_DICT_REMOVE
                    NATIVE_DICT_LENGTH
                    NATIVE_DICT_KEYS
                    NATIVE_DICT_VALUES
                    NATIVE_DICT_POP
                    NATIVE_DICT_COPY
                    NATIVE_DICT_CLEAR

                    NATIVE_HTML_A
                    NATIVE_HTML_ADDRESS
                    NATIVE_HTML_AREA
                    NATIVE_HTML_ARTICLE
                    NATIVE_HTML_ASIDE
                    NATIVE_HTML_AUDIO
                    // NATIVE_HTML_BASE
                    NATIVE_HTML_BLOCKQUOTE
                    NATIVE_HTML_BODY
                    NATIVE_HTML_BR
                    NATIVE_HTML_BUTTON
                    NATIVE_HTML_CANVAS
                    NATIVE_HTML_CAPTION
                    NATIVE_HTML_CITE
                    NATIVE_HTML_CODE
                    NATIVE_HTML_COL
                    NATIVE_HTML_COLGROUP
                    NATIVE_HTML_CONSTRUCT
                    NATIVE_HTML_DATALIST
                    NATIVE_HTML_DD
                    NATIVE_HTML_DEL
                    NATIVE_HTML_DIV
                    NATIVE_HTML_DL
                    NATIVE_HTML_DT
                    NATIVE_HTML_EM
                    // NATIVE_HTML_EMBED
                    NATIVE_HTML_FIELDSET
                    NATIVE_HTML_FIGCAPTION
                    NATIVE_HTML_FIGURE
                    NATIVE_HTML_FOOTER
                    NATIVE_HTML_FORM
                    NATIVE_HTML_H1
                    NATIVE_HTML_H2
                    NATIVE_HTML_H3
                    NATIVE_HTML_H4
                    NATIVE_HTML_H5
                    NATIVE_HTML_H6
                    NATIVE_HTML_HEAD
                    NATIVE_HTML_HEADER
                    NATIVE_HTML_HR
                    NATIVE_HTML_IFRAME
                    NATIVE_HTML_IMG
                    NATIVE_HTML_INPUT
                    NATIVE_HTML_INS
                    NATIVE_HTML_LABEL
                    NATIVE_HTML_LEGEND
                    NATIVE_HTML_LI
                    NATIVE_HTML_LINK
                    NATIVE_HTML_MAIN
                    NATIVE_HTML_MAP
                    NATIVE_HTML_MARK
                    NATIVE_HTML_META
                    NATIVE_HTML_METER
                    NATIVE_HTML_NAV
                    NATIVE_HTML_OBJECT
                    NATIVE_HTML_OL
                    NATIVE_HTML_OPTGROUP
                    NATIVE_HTML_OPTION
                    NATIVE_HTML_OUTPUT
                    NATIVE_HTML_P
                    NATIVE_HTML_PARAM
                    NATIVE_HTML_PICTURE
                    NATIVE_HTML_PRE
                    NATIVE_HTML_PROGRESS
                    NATIVE_HTML_Q
                    NATIVE_HTML_SECTION
                    NATIVE_HTML_SELECT
                    NATIVE_HTML_SMALL
                    NATIVE_HTML_SOURCE
                    NATIVE_HTML_SPAN
                    NATIVE_HTML_STRONG
                    NATIVE_HTML_STYLE
                    NATIVE_HTML_SUB
                    NATIVE_HTML_SUP
                    NATIVE_HTML_TABLE
                    NATIVE_HTML_TBODY
                    NATIVE_HTML_TD
                    NATIVE_HTML_TEXTAREA
                    NATIVE_HTML_TFOOT
                    NATIVE_HTML_TH
                    NATIVE_HTML_THEAD
                    NATIVE_HTML_TIME
                    NATIVE_HTML_TITLE
                    NATIVE_HTML_TR
                    NATIVE_HTML_TRACK
                    NATIVE_HTML_UL
                    NATIVE_HTML_VIDEO
                    NATIVE_HTML_WBR
                    NATIVE_HTML_ATTR
                    NATIVE_HTML_APPEND

                    NATIVE_GUI_INIT
                    NATIVE_GUI_START
                    NATIVE_GUI_TICK
                    NATIVE_GUI_IS_RUNNING
                    NATIVE_GUI_DRAW_TEXT
                    NATIVE_GUI_DRAW_TEXT_COLOR
                    NATIVE_GUI_DRAW_RECT
                    NATIVE_GUI_DRAW_RECT_ADV
                    NATIVE_GUI_DRAW_CIRCLE
                    NATIVE_GUI_LOAD_TEXTURE
                    NATIVE_GUI_DRAW_TEXTURE
                    NATIVE_GUI_DRAW_TEXTURE_SIZED
                    NATIVE_GUI_DRAW_TEXTURE_FLIPPED
                    NATIVE_GUI_KEY_PRESSED
                    NATIVE_GUI_KEY_DOWN
                    NATIVE_GUI_GET_CHAR
                    NATIVE_GUI_KEY_RELEASED
                    NATIVE_GUI_MEASURE_TEXT
                    NATIVE_GUI_MOUSE_PRESSED
                    NATIVE_GUI_MOUSE_RELEASED
                    NATIVE_GUI_MOUSE_POS
                    NATIVE_GUI_RECTANGLE_MOUSE_COLLISION
                    NATIVE_GUI_RECTANGLE_COLLISION
                    NATIVE_GUI_DELTA_TIME
                    NATIVE_GUI_CREATE_RECTANGLE
                    NATIVE_GUI_CLEAR_TASKS
                    NATIVE_GUI_SET_BACKGROUND_COLOR
                    NATIVE_GUI_DRAW_TEXT_ELEMENT
                    NATIVE_GUI_CREATE_TEXT
                    NATIVE_GUI_SET_TEXT
                    NATIVE_GUI_UNLOAD_TEXTURE

                default:
                    throw std::runtime_error("Unrecognized method");
                }

                args.clear();
                continue;
            }

            if (!std::holds_alternative<std::shared_ptr<Function>>(callee))
                throw std::runtime_error("Can only call functions");

            auto function = std::get<std::shared_ptr<Function>>(callee);

            if (arg_count != function->arity)
            {
                throw std::runtime_error(
                    "Expected " + std::to_string(function->arity) +
                    " arguments but got " + std::to_string(arg_count));
            }

            frames.push_back({chunk, ip, callee_index, false, function});

            stack.erase(stack.begin() + callee_index);
            chunk = &function->chunk;
            ip = chunk->code.data();
            break;
        }

        case OpCode::RETURN:
        {
            if (frames.empty())
                return;

            CallFrame frame = frames.back();
            frames.pop_back();

            Value return_value;

            if (frame.is_constructor)
            {
                // print_stack();
                if (stack.size() > frame.stack_start + 1)
                {
                    Value v = pop();
                    if (!std::holds_alternative<IgnoreReturnValue>(v))
                    {
                        if (std::holds_alternative<std::nullptr_t>(v) || std::holds_alternative<std::shared_ptr<Instance>>(v))
                        {
                            return_value = v;
                            stack.resize(frame.stack_start);
                            stack.push_back(return_value);
                            chunk = frame.chunk;
                            ip = frame.ip;
                            break;
                        }
                        else
                        {
                            throw std::runtime_error("The class method init can only return null or the instance itself");
                        }
                    }
                }

                return_value = stack[frame.stack_start];
                stack.resize(frame.stack_start);
            }

            else
            {
                return_value = pop();
                stack.resize(frame.stack_start);
            }

            stack.push_back(return_value);
            chunk = frame.chunk;
            ip = frame.ip;
            break;
        }

        case OpCode::ARRAY:
        {
            uint8_t count = read_byte();

            auto array = std::make_shared<Array>();
            array->elements.resize(count);
            for (int i = count - 1; i >= 0; --i)
                array->elements[i] = pop();

            stack.push_back(array);
            break;
        }

        case OpCode::GET_INDEX:
        {
            Value index_val = pop();
            Value container_val = pop();

            if (std::holds_alternative<std::shared_ptr<Array>>(container_val))
            {
                auto array = std::get<std::shared_ptr<Array>>(container_val);
                size_t index = checked_sequence_index(index_val, array->elements.size(), "Array");
                push(array->elements[index]);
            }
            else if (std::holds_alternative<std::string>(container_val))
            {
                const auto &str = std::get<std::string>(container_val);
                size_t index = checked_sequence_index(index_val, str.size(), "String");
                push(std::string(1, str[index]));
            }
            else if (std::holds_alternative<std::shared_ptr<Dict>>(container_val))
            {
                auto dict = std::get<std::shared_ptr<Dict>>(container_val);
                if (!std::holds_alternative<std::string>(index_val))
                    throw std::runtime_error("Dictionary key must be string");
                push(dict->get(std::get<std::string>(index_val)));
            }
            else
                throw std::runtime_error("Cannot index this type");

            break;
        }

        case OpCode::SET_INDEX:
        {
            Value value = pop();
            Value index_val = pop();
            Value container_val = pop();

            if (std::holds_alternative<std::shared_ptr<Array>>(container_val))
            {
                auto array = std::get<std::shared_ptr<Array>>(container_val);
                size_t index = checked_sequence_index(index_val, array->elements.size(), "Array");
                array->elements[index] = value;
                push(value);
            }
            else if (std::holds_alternative<std::shared_ptr<Dict>>(container_val))
            {
                auto dict = std::get<std::shared_ptr<Dict>>(container_val);
                if (!std::holds_alternative<std::string>(index_val))
                    throw std::runtime_error("Dictionary key must be string");
                dict->set(std::get<std::string>(index_val), value);
                push(value);
            }
            else
                throw std::runtime_error("Cannot assign index to this type");

            break;
        }

        case OpCode::GET_PROPERTY:
        {
            std::string name = std::get<std::string>(read_constant());
            Value object = pop();

            // For properties stuff
            // std::cout << "[DEBUG] GET_PROPERTY name=" << name
            //    << ", object=" << stringify(object) << std::endl;

            if (name == "type")
            {
                push(make_native_method(object, NativeMethod::TYPE));
                break;
            }

            if (std::holds_alternative<bool>(object))
            {
                if (name == "to_int")
                {
                    push(make_native_method(object, NativeMethod::BOOL_TO_INT));
                    break;
                }
                if (name == "to_string")
                {
                    push(make_native_method(object, NativeMethod::TO_STRING));
                    break;
                }
                throw std::runtime_error("Undefined property \"" + name + "\"");
                break;
            }

            if (std::holds_alternative<double>(object))
            {
                if (name == "to_string")
                {
                    push(make_native_method(object, NativeMethod::TO_STRING));
                    break;
                }
                if (name == "to_int")
                {
                    push(make_native_method(object, NativeMethod::TO_INT));
                    break;
                }
                throw std::runtime_error("Undefined property \"" + name + "\"");
                break;
            }

            if (std::holds_alternative<std::string>(object))
            {
                if (name == "to_number")
                {
                    push(make_native_method(object, NativeMethod::TO_NUMBER));
                    break;
                }
                if (name == "upper")
                {
                    push(make_native_method(object, NativeMethod::UPPER));
                    break;
                }
                if (name == "lower")
                {
                    push(make_native_method(object, NativeMethod::LOWER));
                    break;
                }
                if (name == "capitalize")
                {
                    push(make_native_method(object, NativeMethod::CAPITALIZE));
                    break;
                }
                if (name == "swap")
                {
                    push(make_native_method(object, NativeMethod::SWAPCASE));
                    break;
                }
                if (name == "find")
                {
                    push(make_native_method(object, NativeMethod::FIND));
                    break;
                }
                if (name == "find_last")
                {
                    push(make_native_method(object, NativeMethod::FIND_LAST));
                    break;
                }
                if (name == "first")
                {
                    push(make_native_method(object, NativeMethod::STR_FIRST));
                    break;
                }
                if (name == "last")
                {
                    push(make_native_method(object, NativeMethod::STR_LAST));
                    break;
                }
                if (name == "starts_with")
                {
                    push(make_native_method(object, NativeMethod::STARTS_WITH));
                    break;
                }
                if (name == "ends_with")
                {
                    push(make_native_method(object, NativeMethod::ENDS_WITH));
                    break;
                }
                if (name == "is_number")
                {
                    push(make_native_method(object, NativeMethod::IS_NUMBER));
                    break;
                }
                if (name == "is_space")
                {
                    push(make_native_method(object, NativeMethod::IS_SPACE));
                    break;
                }
                if (name == "is_all_spaces")
                {
                    push(make_native_method(object, NativeMethod::IS_ALL_SPACES));
                    break;
                }
                if (name == "replace")
                {
                    push(make_native_method(object, NativeMethod::REPLACE));
                    break;
                }
                if (name == "trim")
                {
                    push(make_native_method(object, NativeMethod::STR_TRIM));
                    break;
                }
                if (name == "to_array")
                {
                    push(make_native_method(object, NativeMethod::TO_ARRAY));
                    break;
                }
                if (name == "is_empty")
                {
                    push(make_native_method(object, NativeMethod::STR_IS_EMPTY));
                    break;
                }
                if (name == "length")
                {
                    push(make_native_method(object, NativeMethod::STR_LENGTH));
                    break;
                }
                if (name == "count")
                {
                    push(make_native_method(object, NativeMethod::STR_COUNT));
                    break;
                }
                if (name == "slice")
                {
                    push(make_native_method(object, NativeMethod::STR_SLICE));
                    break;
                }
                throw std::runtime_error("Undefined property \"" + name + "\"");
                break;
            }

            if (std::holds_alternative<std::shared_ptr<Array>>(object))
            {
                auto array = std::get<std::shared_ptr<Array>>(object);

                if (name == "length")
                {
                    push(make_native_method(array, NativeMethod::LENGTH));
                    break;
                }
                if (name == "push")
                {
                    push(make_native_method(array, NativeMethod::PUSH));
                    break;
                }
                if (name == "pop")
                {
                    push(make_native_method(array, NativeMethod::POP));
                    break;
                }
                if (name == "is_empty")
                {
                    push(make_native_method(array, NativeMethod::IS_EMPTY));
                    break;
                }
                if (name == "clear")
                {
                    push(make_native_method(array, NativeMethod::CLEAR));
                    break;
                }
                if (name == "contains")
                {
                    push(make_native_method(array, NativeMethod::CONTAINS));
                    break;
                }
                if (name == "first_index")
                {
                    push(make_native_method(array, NativeMethod::INDEX_OF));
                    break;
                }
                if (name == "insert")
                {
                    push(make_native_method(array, NativeMethod::INSERT));
                    break;
                }
                if (name == "remove")
                {
                    push(make_native_method(array, NativeMethod::REMOVE_AT));
                    break;
                }
                if (name == "join")
                {
                    push(make_native_method(array, NativeMethod::JOIN));
                    break;
                }
                if (name == "last_index")
                {
                    push(make_native_method(array, NativeMethod::LAST_INDEX_OF));
                    break;
                }
                if (name == "equals")
                {
                    push(make_native_method(array, NativeMethod::EQUALS));
                    break;
                }
                if (name == "count")
                {
                    push(make_native_method(array, NativeMethod::COUNT));
                    break;
                }
                if (name == "swap")
                {
                    push(make_native_method(array, NativeMethod::SWAP));
                    break;
                }
                if (name == "concat")
                {
                    push(make_native_method(array, NativeMethod::CONCAT));
                    break;
                }
                if (name == "copy")
                {
                    push(make_native_method(array, NativeMethod::COPY));
                    break;
                }
                if (name == "slice")
                {
                    push(make_native_method(array, NativeMethod::SLICE));
                    break;
                }
                if (name == "last")
                {
                    push(make_native_method(array, NativeMethod::LAST));
                    break;
                }
                if (name == "first")
                {
                    push(make_native_method(array, NativeMethod::FIRST));
                    break;
                }
                if (name == "reverse")
                {
                    push(make_native_method(array, NativeMethod::REVERSE));
                    break;
                }
                if (name == "trim")
                {
                    push(make_native_method(array, NativeMethod::TRIM));
                    break;
                }
                if (name == "sum")
                {
                    push(make_native_method(array, NativeMethod::SUM));
                    break;
                }
                if (name == "min")
                {
                    push(make_native_method(array, NativeMethod::MIN));
                    break;
                }
                if (name == "max")
                {
                    push(make_native_method(array, NativeMethod::MAX));
                    break;
                }
                if (name == "average")
                {
                    push(make_native_method(array, NativeMethod::AVERAGE));
                    break;
                }
                throw std::runtime_error("Undefined property \"" + name + "\"");
                break;
            }

            if (std::holds_alternative<std::shared_ptr<Tag>>(object))
            {
                auto tag = std::get<std::shared_ptr<Tag>>(object);

                if (name == "attr")
                {
                    push(make_native_method(tag, NativeMethod::HTML_ATTR));
                    break;
                }

                if (name == "append")
                {
                    push(make_native_method(tag, NativeMethod::HTML_APPEND));
                    break;
                }
                throw std::runtime_error("Undefined property \"" + name + "\"");
                break;
            }

            if (std::holds_alternative<std::shared_ptr<Dict>>(object))
            {
                auto dict = std::get<std::shared_ptr<Dict>>(object);

                if (name == "contains")
                {
                    push(make_native_method(dict, NativeMethod::DICT_CONTAINS));
                    break;
                }
                if (name == "remove")
                {
                    push(make_native_method(dict, NativeMethod::DICT_REMOVE));
                    break;
                }
                if (name == "length")
                {
                    push(make_native_method(dict, NativeMethod::DICT_LENGTH));
                    break;
                }
                if (name == "keys")
                {
                    push(make_native_method(dict, NativeMethod::DICT_KEYS));
                    break;
                }
                if (name == "values")
                {
                    push(make_native_method(dict, NativeMethod::DICT_VALUES));
                    break;
                }
                if (name == "pop")
                {
                    push(make_native_method(dict, NativeMethod::DICT_POP));
                    break;
                }
                if (name == "copy")
                {
                    push(make_native_method(dict, NativeMethod::DICT_COPY));
                    break;
                }
                if (name == "clear")
                {
                    push(make_native_method(dict, NativeMethod::DICT_CLEAR));
                    break;
                }
                throw std::runtime_error("Undefined property \"" + name + "\"");
                break;
            }

            if (std::holds_alternative<std::shared_ptr<Instance>>(object))
            {
                auto instance = std::get<std::shared_ptr<Instance>>(object);
                const std::string caller_class = !frames.empty() && frames.back().function
                    ? frames.back().function->owner_class
                    : std::string{};
                const PropertyInfo *property = instance->klass->find_property(name);
                if (property && !property->is_public && property->owner_class != caller_class)
                    throw std::runtime_error("Cannot access private property \"" + name + "\"");

                auto it = instance->fields.find(name);
                if (it != instance->fields.end())
                {
                    push(it->second);
                    break;
                }

                std::shared_ptr<Function> method;
                auto cached = method_cache.find(instruction);
                if (cached != method_cache.end() && cached->second.klass == instance->klass)
                    method = cached->second.method;
                else
                {
                    method = instance->klass->find_method(name);
                    if (method)
                        method_cache.insert_or_assign(
                            instruction, MethodCacheEntry{instance->klass, method});
                }
                if (method)
                {
                    if (!method->is_public && method->owner_class != caller_class)
                        throw std::runtime_error("Cannot access private method \"" + name + "\"");
                    push(std::make_shared<UserBoundMethod>(instance, method));
                    break;
                }

                throw std::runtime_error("Undefined property \"" + name + "\"");
            }

            else
            {
                throw std::runtime_error("Cannot get property \"" + name + "\" of this value");
            }
        }

        case OpCode::DICT:
        {
            stack.push_back(std::make_shared<Dict>());
            break;
        }

        case OpCode::GET_ITEM:
        {
            Value key = pop();
            Value dict_val = pop();

            if (auto dict = std::get_if<std::shared_ptr<Dict>>(&dict_val))
            {
                if (!std::holds_alternative<std::string>(key))
                    throw std::runtime_error("Dictionary key must be a string");

                stack.push_back((*dict)->get(std::get<std::string>(key)));
            }
            else
                throw std::runtime_error("Not a dictionary");

            break;
        }

        case OpCode::SET_ITEM:
        {
            Value val = pop();
            Value key = pop();
            Value dict_val = pop();

            if (auto dict = std::get_if<std::shared_ptr<Dict>>(&dict_val))
            {
                if (!std::holds_alternative<std::string>(key))
                    throw std::runtime_error("Dictionary key must be a string");

                (*dict)->set(std::get<std::string>(key), std::move(val));
                stack.push_back(dict_val);
            }
            else
                throw std::runtime_error("Not a dictionary");

            break;
        }

        case OpCode::SET_PROPERTY:
        {
            Value name_val = read_constant();
            if (!std::holds_alternative<std::string>(name_val))
                throw std::runtime_error("Property name must be a string");
            std::string name = std::get<std::string>(name_val);

            Value value = pop();
            Value object = pop();
            if (!std::holds_alternative<std::shared_ptr<Instance>>(object))
                throw std::runtime_error("Only instances have fields");
            auto instance = std::get<std::shared_ptr<Instance>>(std::move(object));

            const std::string caller_class = !frames.empty() && frames.back().function
                ? frames.back().function->owner_class
                : std::string{};
            const PropertyInfo *property = instance->klass->find_property(name);
            if (property && !property->is_public && property->owner_class != caller_class)
                throw std::runtime_error("Cannot access private property \"" + name + "\"");

            if (!property && instance->klass->is_user_defined)
            {
                const bool in_constructor = !frames.empty() && frames.back().function &&
                    frames.back().function->name == "init" &&
                    frames.back().function->owner_class == instance->klass->name;
                if (!in_constructor)
                    throw std::runtime_error("Property \"" + name +
                                             "\" must be declared in init()");
            }

            if (property && property->type_name && !runtime_type_matches(value, *property->type_name))
                throw std::runtime_error("Property \"" + name + "\" expects type \"" +
                                         *property->type_name + "\"");

            instance->fields[name] = value;
            stack.push_back(std::move(value));
            break;
        }

        case OpCode::CLASS:
        {
            uint16_t name_index = read_short();
            Value name_val = chunk->constants.at(name_index);

            if (!std::holds_alternative<std::string>(name_val))
                throw std::runtime_error("Class name must be a string");

            std::string name = std::get<std::string>(name_val);

            auto klass = std::make_shared<Class>(
                name,
                std::unordered_map<std::string, std::shared_ptr<Function>>{});
            klass->is_user_defined = true;

            push(klass);
            break;
        }

        case OpCode::INHERIT:
        {
            auto sub_class_val = pop();
            auto super_class_val = pop();

            if (!std::holds_alternative<std::shared_ptr<Class>>(super_class_val) ||
                !std::holds_alternative<std::shared_ptr<Class>>(sub_class_val))
                throw std::runtime_error("INHERIT expects two classes on stack");

            auto super_class = std::get<std::shared_ptr<Class>>(super_class_val);
            auto sub_class = std::get<std::shared_ptr<Class>>(sub_class_val);
            if (super_class == sub_class)
                throw std::runtime_error("A class cannot inherit from itself");
            for (auto parent = super_class; parent; parent = parent->super_class)
                if (parent == sub_class)
                    throw std::runtime_error("Circular class inheritance");
            sub_class->super_class = super_class;
            for (auto &[method_name, method] : sub_class->methods)
                method->super_class = super_class;

            stack.push_back(sub_class);
            break;
        }

        case OpCode::GET_SUPER:
        {
            Value method_name_value = read_constant();
            if (!std::holds_alternative<std::string>(method_name_value))
                throw std::runtime_error("Superclass method name must be a string");
            const auto &method_name = std::get<std::string>(method_name_value);
            if (frames.empty() || !frames.back().function || !frames.back().function->super_class)
                throw std::runtime_error("No superclass for this method");
            auto instance = current_instance();
            auto method = frames.back().function->super_class->find_method(method_name);
            if (!method)
                throw std::runtime_error("Undefined superclass method \"" + method_name + "\"");
            push(std::make_shared<UserBoundMethod>(instance, method));
            break;
        }

        case OpCode::METHOD:
        {
            uint16_t name_index = read_short();
            Value name_val = chunk->constants.at(name_index);

            if (!std::holds_alternative<std::string>(name_val))
                throw std::runtime_error("Method name must be a string");

            std::string name = std::get<std::string>(name_val);

            auto function = pop_function();
            auto klass = pop_class();

            if (klass->methods.contains(name))
                throw std::runtime_error(
                    "Class \"" + klass->name + "\" already defines method \"" + name + "\"");

            klass->methods[name] = function;
            if (name == "init")
                klass->properties = function->declared_properties;
            push(klass);
            break;
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
    if (stack.empty())
    {
        throw std::runtime_error("pop() called on empty stack");
    }
    Value v = std::move(stack.back());
    stack.pop_back();
    return v;
}

Value VM::add_one(const Value &value)
{
    if (const double *number = std::get_if<double>(&value))
        return *number + 1.0;

    if (std::holds_alternative<std::string>(value))
        return stringify(value) + "1";

    if (const auto *array = std::get_if<std::shared_ptr<Array>>(&value))
    {
        auto result = std::make_shared<Array>();
        result->elements.reserve((*array)->elements.size());
        for (const Value &element : (*array)->elements)
        {
            const double *number = std::get_if<double>(&element);
            if (!number)
                throw std::runtime_error("Array must contain only numbers");
            result->elements.push_back(*number + 1.0);
        }
        return result;
    }

    throw std::runtime_error("Operands must be numbers: " + stringify(value) + " 1");
}

Value VM::pop_constant()
{
    return pop();
}

std::shared_ptr<Function> VM::pop_function()
{
    Value v = pop();
    if (!std::holds_alternative<std::shared_ptr<Function>>(v))
    {
        throw std::runtime_error("Expected a function on the stack");
    }
    return std::get<std::shared_ptr<Function>>(v);
}

std::shared_ptr<Class> VM::pop_class()
{
    Value v = pop();
    if (!std::holds_alternative<std::shared_ptr<Class>>(v))
    {
        throw std::runtime_error("Expected a class on the stack");
    }
    return std::get<std::shared_ptr<Class>>(v);
}

std::shared_ptr<Instance> VM::pop_instance()
{
    Value val = pop();
    if (!std::holds_alternative<std::shared_ptr<Instance>>(val))
        throw std::runtime_error("Expected Instance");
    return std::get<std::shared_ptr<Instance>>(val);
}

void VM::push(Value value)
{
    stack.push_back(std::move(value));
}

uint16_t VM::read_short()
{
    const uint8_t *end = chunk->code.data() + chunk->code.size();
    if (ip >= end || end - ip < 2)
        throw std::runtime_error("read_short() past end of bytecode");

    uint16_t high = *ip++;
    uint16_t low = *ip++;
    return (high << 8) | low;
}

uint8_t VM::read_byte()
{
    if (ip >= chunk->code.data() + chunk->code.size())
        throw std::runtime_error("read_byte() past end of bytecode");

    return *ip++;
}

Value VM::read_constant()
{
    return chunk->constants.at(read_short());
}

std::shared_ptr<Instance> VM::current_instance()
{
    if (frames.empty())
        throw std::runtime_error("No active frame for current_instance()");

    size_t idx = frames.back().stack_start;
    Value val = stack.at(idx);

    if (!std::holds_alternative<std::shared_ptr<Instance>>(val))
        throw std::runtime_error("Expected instance at stack_start");

    return std::get<std::shared_ptr<Instance>>(val);
}

bool VM::is_truthy(const Value &value)
{
    if (std::holds_alternative<std::nullptr_t>(value))
        return false;
    if (auto b = std::get_if<bool>(&value))
        return *b;
    return true;
}

bool VM::value_shallow_equal(const Value &x, const Value &y)
{
    if (x.index() != y.index())
        return false;

    if (std::holds_alternative<std::shared_ptr<Array>>(x))
        return std::get<std::shared_ptr<Array>>(x) ==
               std::get<std::shared_ptr<Array>>(y);

    return x == y;
}

void VM::print_value(const Value &value)
{
    std::cout << this->stringify(value);
}

std::string VM::stringify(const Value &value)
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
        double d = std::get<double>(value);

        if (std::isinf(d))
        {
            out << (d > 0 ? "Infinity" : "-Infinity");
        }
        else
        {
            std::ostringstream oss;
            oss << std::setprecision(std::numeric_limits<double>::digits10 + 1);

            if ((d != 0.0 && (std::abs(d) < 1e-4 || std::abs(d) >= 1e7)))
            {
                oss << std::scientific << d;
            }
            else
            {
                oss << std::defaultfloat << d;
            }

            out << oss.str();
        }
    }

    else if (std::holds_alternative<std::string>(value))
    {
        out << std::get<std::string>(value);
    }

    else if (std::holds_alternative<std::shared_ptr<Array>>(value))
    {
        auto array = std::get<std::shared_ptr<Array>>(value);
        out << "[";

        for (size_t i = 0; i < array->elements.size(); ++i)
        {
            const auto &elem = array->elements[i];

            if (std::holds_alternative<std::string>(elem))
                out << "\"" << std::get<std::string>(elem) << "\"";
            else
                out << stringify(elem);

            if (i + 1 < array->elements.size())
                out << ", ";
        }

        out << "]";
    }

    else if (std::holds_alternative<std::shared_ptr<Function>>(value))
    {
        out << "<function>";
    }

    else if (std::holds_alternative<std::shared_ptr<Class>>(value))
    {
        auto klass = std::get<std::shared_ptr<Class>>(value);
        out << "<class " << klass->name << ">";
    }

    else if (std::holds_alternative<std::shared_ptr<Instance>>(value))
    {
        auto instance = std::get<std::shared_ptr<Instance>>(value);
        out << "<" << instance->klass->name << " instance>";
    }

    else if (std::holds_alternative<std::shared_ptr<UserBoundMethod>>(value))
    {
        out << "<bound method>";
    }

    else if (std::holds_alternative<std::shared_ptr<BoundMethod>>(value))
    {
        out << "<native method>";
    }

    else if (std::holds_alternative<std::shared_ptr<Tag>>(value))
    {
        out << "<html tag>";
    }

    else if (std::holds_alternative<std::shared_ptr<Texture2D>>(value))
    {
        out << "<2d texture>";
    }

    else if (std::holds_alternative<std::shared_ptr<TextElement>>(value))
    {
        out << "<text element>";
    }

    else if (std::holds_alternative<IgnoreReturnValue>(value))
    {
        out << "<ignore>";
    }

    else if (std::holds_alternative<std::shared_ptr<Dict>>(value))
    {
        auto dict = std::get<std::shared_ptr<Dict>>(value);
        out << "{";
        size_t count = 0;

        for (const auto &[key, val] : dict->entries)
        {
            out << "\"" << key << "\": ";

            if (std::holds_alternative<std::string>(val))
                out << "\"" << std::get<std::string>(val) << "\"";
            else
                out << stringify(val);

            if (++count < dict->entries.size())
                out << ", ";
        }

        out << "}";
    }

    else
    {
        out << "<unknown>";
    }

    return out.str();
}

std::string VM::json_escape(const std::string &s)
{
    std::string out;
    out.reserve(s.size());

    for (unsigned char c : s)
    {
        switch (c)
        {
        case '"':
            out += "\\\"";
            break;
        case '\\':
            out += "\\\\";
            break;
        case '\b':
            out += "\\b";
            break;
        case '\f':
            out += "\\f";
            break;
        case '\n':
            out += "\\n";
            break;
        case '\r':
            out += "\\r";
            break;
        case '\t':
            out += "\\t";
            break;

        default:
        {
            if (c < 0x20)
            {
                std::ostringstream ss;
                ss << "\\u"
                   << std::hex
                   << std::uppercase
                   << std::setw(4)
                   << std::setfill('0')
                   << (int)c;
                out += ss.str();
            }
            else
            {
                out += c;
            }
        }
        }
    }

    return out;
}

std::string VM::json_number(double value)
{
    std::ostringstream out;
    out << std::setprecision(15) << value;

    std::string s = out.str();

    if (s.find('.') != std::string::npos)
    {
        while (!s.empty() && s.back() == '0')
            s.pop_back();

        if (!s.empty() && s.back() == '.')
            s.pop_back();
    }

    return s;
}

std::string VM::json_stringify(const Value &value)
{
    if (std::holds_alternative<std::nullptr_t>(value))
        return "null";

    if (std::holds_alternative<bool>(value))
        return std::get<bool>(value) ? "true" : "false";

    if (std::holds_alternative<double>(value))
        return json_number(std::get<double>(value));

    if (std::holds_alternative<std::string>(value))
        return "\"" + json_escape(std::get<std::string>(value)) + "\"";

    if (std::holds_alternative<std::shared_ptr<Array>>(value))
    {
        auto arr = std::get<std::shared_ptr<Array>>(value);

        std::string out = "[";
        for (size_t i = 0; i < arr->elements.size(); ++i)
        {
            out += json_stringify(arr->elements[i]);
            if (i + 1 < arr->elements.size())
                out += ",";
        }
        out += "]";
        return out;
    }

    if (std::holds_alternative<std::shared_ptr<Dict>>(value))
    {
        auto dict = std::get<std::shared_ptr<Dict>>(value);

        std::string out = "{";
        size_t count = 0;

        for (const auto &[key, val] : dict->entries)
        {
            out += "\"";
            out += json_escape(key);
            out += "\":";
            out += json_stringify(val);

            if (++count < dict->entries.size())
                out += ",";
        }

        out += "}";
        return out;
    }

    throw std::runtime_error("Value cannot be serialized to JSON");
}

std::string VM::csv_escape(const std::string &s)
{
    bool needs_quotes = false;

    for (char c : s)
    {
        if (c == ',' || c == '"' || c == '\n' || c == '\r')
        {
            needs_quotes = true;
            break;
        }
    }

    if (!needs_quotes)
        return s;

    std::string out = "\"";

    for (char c : s)
    {
        if (c == '"')
            out += "\"\"";
        else
            out += c;
    }

    out += "\"";
    return out;
}

std::string VM::csv_value(const Value &value)
{
    if (std::holds_alternative<std::nullptr_t>(value))
        return "";

    if (std::holds_alternative<bool>(value))
        return std::get<bool>(value) ? "true" : "false";

    if (std::holds_alternative<double>(value))
        return json_number(std::get<double>(value));

    if (std::holds_alternative<std::string>(value))
        return csv_escape(std::get<std::string>(value));

    throw std::runtime_error("Unsupported CSV value type (no nested arrays/objects)");
}

std::string VM::csv_stringify(const Value &value)
{
    if (!std::holds_alternative<std::shared_ptr<Array>>(value))
        throw std::runtime_error("CSV must be an array of rows");

    auto rows = std::get<std::shared_ptr<Array>>(value);

    std::string out;

    for (size_t i = 0; i < rows->elements.size(); ++i)
    {
        const Value &row_val = rows->elements[i];

        if (!std::holds_alternative<std::shared_ptr<Array>>(row_val))
            throw std::runtime_error("Each CSV row must be an array");

        auto row = std::get<std::shared_ptr<Array>>(row_val);

        for (size_t j = 0; j < row->elements.size(); ++j)
        {
            out += csv_value(row->elements[j]);

            if (j + 1 < row->elements.size())
                out += ",";
        }

        if (i + 1 < rows->elements.size())
            out += "\n";
    }

    return out;
}

void VM::print_stack()
{
    std::cout << "=== STACK (" << stack.size() << ") ===\n";

    for (size_t i = 0; i < stack.size(); ++i)
    {
        std::cout << "[" << i << "] ";

        std::cout << stringify(stack[i]) << std::endl;
    }

    std::cout << "====================\n";
}