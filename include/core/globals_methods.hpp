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

#define NATIVE_GLOBALS_EXISTS \
    case NativeMethod::EXISTS: \
    { \
        if(args.size() != 1 || !std::holds_alternative<std::string>(args[0])) \
            throw std::runtime_error("exists() requires a file path string"); \
        \
        const std::string& path = std::get<std::string>(args[0]); \
        push(std::filesystem::exists(path)); \
        break; \
    }

#define NATIVE_GLOBALS_IS_FILE \
    case NativeMethod::IS_FILE: \
    { \
        if(args.size() != 1 || !std::holds_alternative<std::string>(args[0])) \
            throw std::runtime_error("is_file() requires a file path string"); \
        \
        const std::string& path = std::get<std::string>(args[0]); \
        push(std::filesystem::is_regular_file(path)); \
        break; \
    }

#define NATIVE_GLOBALS_IS_DIRECTORY \
    case NativeMethod::IS_DIRECTORY: \
    { \
        if(args.size() != 1 || !std::holds_alternative<std::string>(args[0])) \
            throw std::runtime_error("is_directory() requires a file path string"); \
        \
        const std::string& path = std::get<std::string>(args[0]); \
        push(std::filesystem::is_directory(path)); \
        break; \
    }

#define NATIVE_GLOBALS_MAKE_DIRECTORY \
    case NativeMethod::MAKE_DIRECTORY: \
    { \
        if(args.size() != 1 || !std::holds_alternative<std::string>(args[0])) \
            throw std::runtime_error("make_directory() requires a path string"); \
        \
        const std::string& path = std::get<std::string>(args[0]); \
        std::error_code ec; \
        bool created = std::filesystem::create_directory(path, ec); \
        \
        if(ec) \
            throw std::runtime_error("Failed to create directory: " + ec.message()); \
        \
        push(created); \
        break; \
    }

#define NATIVE_GLOBALS_REMOVE_DIRECTORY \
    case NativeMethod::REMOVE_DIRECTORY: \
    { \
        if(args.size() != 1 || !std::holds_alternative<std::string>(args[0])) \
            throw std::runtime_error("remove_directory() requires a path string"); \
        \
        const std::string& path = std::get<std::string>(args[0]); \
        std::error_code ec; \
        bool removed = std::filesystem::remove(path, ec); \
        \
        if(ec) \
            throw std::runtime_error("Failed to remove directory: " + ec.message()); \
        \
        push(removed); \
        break; \
    }

#define NATIVE_GLOBALS_LIST_DIRECTORIES \
    case NativeMethod::LIST_DIRECTORIES: \
    { \
        if(args.size() != 1 || !std::holds_alternative<std::string>(args[0])) \
            throw std::runtime_error("list_directories() requires a path string"); \
        \
        const std::string& path = std::get<std::string>(args[0]); \
        \
        if(!std::filesystem::is_directory(path)) \
            throw std::runtime_error("Path is not a directory"); \
        \
        auto result = std::make_shared<Array>(); \
        \
        for(const auto& entry : std::filesystem::directory_iterator(path)) \
        { \
            if(entry.is_directory()) \
                result->elements.push_back(entry.path().filename().string()); \
        } \
        \
        push(result); \
        break; \
    }

#define NATIVE_GLOBALS_LIST_FILES \
    case NativeMethod::LIST_FILES: \
    { \
        if(args.size() != 1 || !std::holds_alternative<std::string>(args[0])) \
            throw std::runtime_error("list_files() requires a directory path string"); \
        \
        const std::string& path = std::get<std::string>(args[0]); \
        \
        if(!std::filesystem::is_directory(path)) \
            throw std::runtime_error("Path is not a directory"); \
        \
        auto result = std::make_shared<Array>(); \
        \
        for(const auto& entry : std::filesystem::directory_iterator(path)) \
        { \
            if(entry.is_regular_file()) \
                result->elements.push_back(entry.path().filename().string()); \
        } \
        \
        push(result); \
        break; \
    }

#define NATIVE_GLOBALS_SYSTEM \
    case NativeMethod::SYSTEM: \
    { \
        if(args.size() != 1 || !std::holds_alternative<std::string>(args[0])) \
            throw std::runtime_error("system() requires a command string"); \
        \
        const std::string& cmd = std::get<std::string>(args[0]); \
        int result = std::system(cmd.c_str()); \
        push(static_cast<double>(result)); \
        break; \
    }

#define NATIVE_GLOBALS_EXIT \
    case NativeMethod::EXIT: \
    { \
        if(args.size() != 1 || !std::holds_alternative<double>(args[0])) \
            throw std::runtime_error("exit() requires a numeric exit code"); \
        \
        int code = static_cast<int>(std::get<double>(args[0])); \
        std::exit(code); \
        break; \
    }

inline std::tm get_local_time(std::time_t tt)
{
    std::tm local_tm{};
    #ifdef _WIN32
        localtime_s(&local_tm, &tt);
    #else
        localtime_r(&tt, &local_tm);
    #endif
        return local_tm;
}

#define NATIVE_GLOBALS_DATE \
    case NativeMethod::DATE: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("date() takes no arguments"); \
        \
        auto now = std::chrono::system_clock::now(); \
        std::time_t tt = std::chrono::system_clock::to_time_t(now); \
        std::tm local_tm = get_local_time(tt); \
        \
        auto dict = std::make_shared<Dict>(); \
        dict->set("year",   static_cast<double>(local_tm.tm_year + 1900)); \
        dict->set("month",  static_cast<double>(local_tm.tm_mon + 1)); \
        dict->set("day",    static_cast<double>(local_tm.tm_mday)); \
        dict->set("hour",   static_cast<double>(local_tm.tm_hour)); \
        dict->set("minute", static_cast<double>(local_tm.tm_min)); \
        dict->set("second", static_cast<double>(local_tm.tm_sec)); \
        \
        push(dict); \
        break; \
    }

#define NATIVE_GLOBALS_COPY \
    case NativeMethod::OS_COPY: \
    { \
        if(args.size() != 2 || \
           !std::holds_alternative<std::string>(args[0]) || \
           !std::holds_alternative<std::string>(args[1])) \
            throw std::runtime_error("copy() requires source and destination strings"); \
        \
        const std::string& src = std::get<std::string>(args[0]); \
        const std::string& dst = std::get<std::string>(args[1]); \
        \
        std::error_code ec; \
        std::filesystem::copy_file( \
            src, dst, \
            std::filesystem::copy_options::overwrite_existing, \
            ec); \
        \
        if(ec) \
            throw std::runtime_error("copy() failed: " + ec.message()); \
        \
        push(true); \
        break; \
    }

#define NATIVE_GLOBALS_RENAME \
    case NativeMethod::RENAME: \
    { \
        if(args.size() != 2 || \
           !std::holds_alternative<std::string>(args[0]) || \
           !std::holds_alternative<std::string>(args[1])) \
            throw std::runtime_error("rename() requires old and new path strings"); \
        \
        const std::string& oldp = std::get<std::string>(args[0]); \
        const std::string& newp = std::get<std::string>(args[1]); \
        \
        std::error_code ec; \
        std::filesystem::rename(oldp, newp, ec); \
        \
        if(ec) \
            throw std::runtime_error("rename() failed: " + ec.message()); \
        \
        push(true); \
        break; \
    }