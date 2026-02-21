#define NATIVE_GLOBALS_INPUT \
    case NativeMethod::INPUT: \
    { \
        std::string prompt; \
    \
        if(!args.empty()) \
        { \
            if(!std::holds_alternative<std::string>(args[0])) \
                throw std::runtime_error("input() prompt must be a string"); \
            prompt = std::get<std::string>(args[0]); \
        } \
    \
        std::cout << prompt; \
        std::string line; \
        std::getline(std::cin, line); \
    \
        push(line); \
        break; \
    }

#define NATIVE_GLOBALS_RAND \
    case NativeMethod::RAND: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("random() takes no arguments"); \
    \
        static std::mt19937 rng{ std::random_device{}() }; \
        static std::uniform_real_distribution<double> dist(0.0, 1.0); \
    \
        push(dist(rng)); \
        break; \
    }

#define NATIVE_GLOBALS_RANDINT \
    case NativeMethod::RANDINT: \
    { \
        if(args.size() != 2) \
            throw std::runtime_error("random_integer() takes exactly 2 arguments"); \
    \
        if(!std::holds_alternative<double>(args[0]) || \
           !std::holds_alternative<double>(args[1])) \
            throw std::runtime_error("random_integer() arguments must be numbers"); \
    \
        int a = static_cast<int>(std::get<double>(args[0])); \
        int b = static_cast<int>(std::get<double>(args[1])); \
    \
        if(a > b) \
            throw std::runtime_error("random_integer() first argument must be <= second"); \
    \
        static std::mt19937 rng{ std::random_device{}() }; \
        std::uniform_int_distribution<int> dist(a, b); \
    \
        push(static_cast<double>(dist(rng))); \
        break; \
    }

#define NATIVE_GLOBALS_FILL \
    case NativeMethod::FILL: \
    { \
        auto array = std::make_shared<Array>(); \
    \
        for(const auto& value : args) \
            array->elements.push_back(value); \
    \
        push(array); \
        break; \
    }

#define NATIVE_GLOBALS_INIT \
    case NativeMethod::INIT: \
    { \
        if(args.size() != 2) \
            throw std::runtime_error("init() takes exactly 2 arguments"); \
    \
        if(!std::holds_alternative<double>(args[0])) \
            throw std::runtime_error("init() size must be a number"); \
    \
        int size = static_cast<int>(std::get<double>(args[0])); \
        if(size < 0) \
            throw std::runtime_error("init() size must be >= 0"); \
    \
        const auto& value = args[1]; \
    \
        auto array = std::make_shared<Array>(); \
        array->elements.reserve(size); \
        for(int i = 0; i < size; ++i) \
            array->elements.push_back(value); \
    \
        push(array); \
        break; \
    }

#define NATIVE_GLOBALS_ARRAY_INPUT \
    case NativeMethod::ARRAY_INPUT: \
    { \
        if(args.size() != 2) \
            throw std::runtime_error("array_input() takes exactly 2 arguments"); \
    \
        if(!std::holds_alternative<double>(args[0])) \
            throw std::runtime_error("array_input() count must be a number"); \
    \
        if(!std::holds_alternative<std::string>(args[1])) \
            throw std::runtime_error("array_input() prompt must be a string"); \
    \
        int count = static_cast<int>(std::get<double>(args[0])); \
        if(count < 0) \
            throw std::runtime_error("array_input() count must be >= 0"); \
    \
        const std::string& prompt = std::get<std::string>(args[1]); \
    \
        auto array = std::make_shared<Array>(); \
        array->elements.reserve(count); \
    \
        for(int i = 0; i < count; ++i) \
        { \
            std::cout << prompt; \
            std::string line; \
            std::getline(std::cin, line); \
            array->elements.push_back(line); \
        } \
    \
        push(array); \
        break; \
    }

#define NATIVE_GLOBALS_FORMAT \
    case NativeMethod::FORMAT: \
    { \
        if(args.empty()) \
            throw std::runtime_error("format() requires at least a format string"); \
        \
        if(!std::holds_alternative<std::string>(args[0])) \
            throw std::runtime_error("format() first argument must be a string"); \
        \
        std::string fmt = std::get<std::string>(args[0]); \
        std::string result; \
        size_t arg_index = 1; \
        \
        for(size_t i = 0; i < fmt.size(); ++i) \
        { \
            if(fmt[i] == '%') \
            { \
                if(i + 1 < fmt.size() && fmt[i + 1] == '%') \
                { \
                    result += '%'; \
                    ++i; \
                } \
                else \
                { \
                    if(arg_index >= args.size()) \
                        throw std::runtime_error("Not enough arguments for format()"); \
                    \
                    result += stringify(args[arg_index++]); \
                } \
            } \
            else \
            { \
                result += fmt[i]; \
            } \
        } \
        \
        if(arg_index != args.size()) \
            throw std::runtime_error("Too many arguments for format()"); \
        \
        push(result); \
        break; \
    }

#define NATIVE_GLOBALS_PRINTF \
    case NativeMethod::PRINTF: \
    { \
        if(args.empty()) \
            throw std::runtime_error("printf() requires at least a format string"); \
        \
        if(!std::holds_alternative<std::string>(args[0])) \
            throw std::runtime_error("printf() first argument must be a string"); \
        \
        std::string fmt = std::get<std::string>(args[0]); \
        size_t arg_index = 1; \
        \
        for(size_t i = 0; i < fmt.size(); ++i) \
        { \
            if(fmt[i] == '%') \
            { \
                if(i + 1 < fmt.size() && fmt[i + 1] == '%') \
                { \
                    std::cout << '%'; \
                    ++i; \
                } \
                else \
                { \
                    if(arg_index >= args.size()) \
                        throw std::runtime_error("Not enough arguments for printf()"); \
                    \
                    std::cout << stringify(args[arg_index++]); \
                } \
            } \
            else \
            { \
                std::cout << fmt[i]; \
            } \
        } \
        \
        if(arg_index != args.size()) \
            throw std::runtime_error("Too many arguments for printf()"); \
        \
        push(nullptr); \
        break; \
    }

#define NATIVE_GLOBALS_READ_FILE \
    case NativeMethod::READ_FILE: \
    { \
        if(args.size() != 1) \
            throw std::runtime_error("read() takes exactly 1 argument"); \
        if(!std::holds_alternative<std::string>(args[0])) \
            throw std::runtime_error("read() argument must be a string"); \
        \
        const std::string& path = std::get<std::string>(args[0]); \
        std::ifstream file(path); \
        if(!file.is_open()) throw std::runtime_error("Failed to open file: " + path); \
        std::stringstream buffer; \
        buffer << file.rdbuf(); \
        push(buffer.str()); \
        break; \
    }

#define NATIVE_GLOBALS_WRITE_FILE \
    case NativeMethod::WRITE_FILE: \
    { \
        if(args.size() != 2) \
            throw std::runtime_error("write() takes exactly 2 arguments"); \
        if(!std::holds_alternative<std::string>(args[0]) || !std::holds_alternative<std::string>(args[1])) \
            throw std::runtime_error("write() arguments must be strings"); \
        \
        const std::string& path = std::get<std::string>(args[0]); \
        const std::string& content = std::get<std::string>(args[1]); \
        std::ofstream file(path, std::ios::trunc); \
        if(!file.is_open()) throw std::runtime_error("Failed to open file: " + path); \
        file << content; \
        push(true); \
        break; \
    }

#define NATIVE_GLOBALS_APPEND_FILE \
    case NativeMethod::APPEND_FILE: \
    { \
        if(args.size() != 2) \
            throw std::runtime_error("append() takes exactly 2 arguments"); \
        if(!std::holds_alternative<std::string>(args[0]) || !std::holds_alternative<std::string>(args[1])) \
            throw std::runtime_error("append() arguments must be strings"); \
        \
        const std::string& path = std::get<std::string>(args[0]); \
        const std::string& content = std::get<std::string>(args[1]); \
        std::ofstream file(path, std::ios::app); \
        if(!file.is_open()) throw std::runtime_error("Failed to open file: " + path); \
        file << content; \
        push(true); \
        break; \
    }

#define NATIVE_GLOBALS_ERASE_FILE \
    case NativeMethod::ERASE_FILE: \
    { \
        if(args.size() != 1 || !std::holds_alternative<std::string>(args[0])) \
            throw std::runtime_error("erase() requires a file path string"); \
        \
        const std::string& path = std::get<std::string>(args[0]); \
        \
        std::error_code ec; \
        bool removed = std::filesystem::remove(path, ec); \
        \
        if(ec) \
            throw std::runtime_error("Failed to erase file: " + ec.message()); \
        \
        push(removed); \
        break; \
    }

#define NATIVE_GLOBALS_PARSE_JSON \
    case NativeMethod::PARSE_JSON: \
    { \
        if(args.size() != 1 || !std::holds_alternative<std::string>(args[0])) \
            throw std::runtime_error("parse() requires a string"); \
        \
        JsonParser parser(std::get<std::string>(args[0])); \
        push(parser.parse()); \
        break; \
    }

#define NATIVE_GLOBALS_STRINGIFY \
    case NativeMethod::STRINGIFY: \
    { \
        if(args.size() != 1) \
            throw std::runtime_error("stringify() takes exactly 1 argument"); \
        \
        push(stringify(args[0])); \
        break; \
    }

#define NATIVE_GLOBALS_PROFILE_START \
    case NativeMethod::PROFILE_START: { \
        if(args.size() != 1 || !std::holds_alternative<std::string>(args[0])) \
            throw std::runtime_error("profile_start() requires a string name"); \
        profiler.start(std::get<std::string>(args[0])); \
        push(nullptr); \
        break; \
    }

#define NATIVE_GLOBALS_PROFILE_END \
    case NativeMethod::PROFILE_END: { \
        if(args.size() != 1 || !std::holds_alternative<std::string>(args[0])) \
            throw std::runtime_error("profile_end() requires a string name"); \
        profiler.end(std::get<std::string>(args[0])); \
        push(nullptr); \
        break; \
    }

#define NATIVE_GLOBALS_PROFILE_REPORT \
    case NativeMethod::PROFILE_REPORT: { \
        if(!args.empty()) \
            throw std::runtime_error("profile_report() takes no arguments"); \
        push(profiler.report()); \
        break; \
    }

#define NATIVE_GLOBALS_PROFILE_RESET \
    case NativeMethod::PROFILE_RESET: { \
        if(!args.empty()) \
            throw std::runtime_error("profile_reset() takes no arguments"); \
        profiler.reset(); \
        push(nullptr); \
        break; \
    }