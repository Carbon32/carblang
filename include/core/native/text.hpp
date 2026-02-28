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

#define NATIVE_GLOBALS_PRINT \
    case NativeMethod::PRINT: \
    { \
        for(size_t i = 0; i < args.size(); ++i) \
        { \
            std::cout << stringify(args[i]); \
            if(i + 1 < args.size()) \
                std::cout << ' '; \
        } \
        push(nullptr); \
        break; \
    }

#define NATIVE_GLOBALS_PRINTLN \
    case NativeMethod::PRINTLN: \
    { \
        for(size_t i = 0; i < args.size(); ++i) \
        { \
            std::cout << stringify(args[i]); \
            if(i + 1 < args.size()) \
                std::cout << ' '; \
        } \
        std::cout << std::endl; \
        push(nullptr); \
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

#define NATIVE_GLOBALS_TO_JSON \
    case NativeMethod::TO_JSON: \
    { \
        if(args.size() != 1) \
            throw std::runtime_error("json() requires 1 argument"); \
        if(!std::holds_alternative<std::shared_ptr<Dict>>(args[0])) \
            throw std::runtime_error("json() only accepts dictionaries"); \
        \
        push(json_stringify(args[0])); \
        break; \
    }