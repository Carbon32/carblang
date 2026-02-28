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