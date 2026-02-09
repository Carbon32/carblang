// I'm trying to keep the length of each file reasonable, so I'm gonna rely
// on macros to define the native methods here, this file will contain the
// native methods of every type, except for arrays

#define NATIVE_PRIMITIVE_TYPE \
    case NativeMethod::TYPE: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("type() takes no arguments"); \
    \
        const Value& v = method->receiver; \
    \
        if(std::holds_alternative<std::nullptr_t>(v)) push("null"); \
        else if(std::holds_alternative<bool>(v)) push("bool"); \
        else if(std::holds_alternative<double>(v)) push("number"); \
        else if(std::holds_alternative<std::string>(v)) push("string"); \
        else if(std::holds_alternative<std::shared_ptr<Array>>(v)) push("array"); \
        else if(std::holds_alternative<std::shared_ptr<Function>>(v)) push("function"); \
        else push("method"); \
    \
        break; \
    }

#define NATIVE_PRIMITIVE_TO_STRING \
    case NativeMethod::TO_STRING: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("to_string() takes no arguments"); \
    \
        push(stringify(method->receiver)); \
        break; \
    }

#define NATIVE_PRIMITIVE_POW \
    case NativeMethod::POW: \
    { \
        if(args.size() != 1) \
            throw std::runtime_error("pow() expects 1 argument"); \
    \
        double base = std::get<double>(method->receiver); \
        double exp = std::get<double>(args[0]); \
        push(std::pow(base, exp)); \
        break; \
    }

#define NATIVE_PRIMITIVE_SQRT \
    case NativeMethod::SQRT: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("sqrt() takes no arguments"); \
    \
        double x = std::get<double>(method->receiver); \
        if(x < 0) throw std::runtime_error("sqrt() of negative number"); \
        push(std::sqrt(x)); \
        break; \
    }

#define NATIVE_PRIMITIVE_FACT \
    case NativeMethod::FACT: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("fact() takes no arguments"); \
    \
        double val = std::get<double>(method->receiver); \
    \
        if(std::floor(val) != val) \
            throw std::runtime_error("fact() only works with whole numbers"); \
    \
        int n = static_cast<int>(val); \
    \
        if(n < 0) \
            throw std::runtime_error("fact() of negative number"); \
    \
        double r = 1; \
        for(int i = 2; i <= n; ++i) \
            r *= i; \
    \
        push(r); \
        break; \
    }

#define NATIVE_PRIMITIVE_TO_INT \
    case NativeMethod::TO_INT: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("to_int() takes no arguments"); \
    \
        double val = std::get<double>(method->receiver); \
    \
        int n = static_cast<int>(std::round(val)); \
    \
        push(static_cast<double>(n)); \
        break; \
    }

#define NATIVE_PRIMITIVE_BOOL_TO_INT \
    case NativeMethod::BOOL_TO_INT: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("to_int() takes no arguments"); \
    \
        bool value = std::get<bool>(method->receiver); \
        push(static_cast<double>(value)); \
        break; \
    }

#define NATIVE_PRIMITIVE_FLOOR \
    case NativeMethod::FLOOR: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("floor() takes no arguments"); \
    \
        push(std::floor(std::get<double>(method->receiver))); \
        break; \
    }

#define NATIVE_PRIMITIVE_CEIL \
    case NativeMethod::CEIL: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("ceil() takes no arguments"); \
    \
        push(std::ceil(std::get<double>(method->receiver))); \
        break; \
    }

#define NATIVE_PRIMITIVE_UPPER \
    case NativeMethod::UPPER: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("upper() takes no arguments"); \
    \
        auto s = std::get<std::string>(method->receiver); \
        std::transform(s.begin(), s.end(), s.begin(), ::toupper); \
        push(s); \
        break; \
    }

#define NATIVE_PRIMITIVE_LOWER \
    case NativeMethod::LOWER: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("lower() takes no arguments"); \
    \
        auto s = std::get<std::string>(method->receiver); \
        std::transform(s.begin(), s.end(), s.begin(), ::tolower); \
        push(s); \
        break; \
    }

#define NATIVE_PRIMITIVE_CAPITALIZE \
    case NativeMethod::CAPITALIZE: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("capitalize() takes no arguments"); \
    \
        auto s = std::get<std::string>(method->receiver); \
        if(!s.empty()) s[0] = std::toupper(s[0]); \
        push(s); \
        break; \
    }

#define NATIVE_PRIMITIVE_SWAPCASE \
    case NativeMethod::SWAPCASE: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("swap() takes no arguments"); \
    \
        auto s = std::get<std::string>(method->receiver); \
        for(char& c : s) \
            c = std::islower(c) ? std::toupper(c) : std::tolower(c); \
        push(s); \
        break; \
    }

#define NATIVE_PRIMITIVE_FIND \
    case NativeMethod::FIND: \
    { \
        if(args.size() != 1) \
            throw std::runtime_error("find() expects 1 argument"); \
    \
        auto s = std::get<std::string>(method->receiver); \
        auto sub = std::get<std::string>(args[0]); \
        auto pos = s.find(sub); \
        push(pos == std::string::npos ? -1.0 : (double) pos); \
        break; \
    }

#define NATIVE_PRIMITIVE_FIND_LAST \
    case NativeMethod::FIND_LAST: \
    { \
        if(args.size() != 1) \
            throw std::runtime_error("find_last() expects 1 argument"); \
    \
        auto s = std::get<std::string>(method->receiver); \
        auto sub = std::get<std::string>(args[0]); \
    \
        auto pos = s.rfind(sub); \
    \
        push(pos == std::string::npos ? -1.0 : static_cast<double>(pos)); \
        break; \
    }


#define NATIVE_PRIMITIVE_STR_FIRST \
    case NativeMethod::STR_FIRST: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("first() takes no arguments"); \
    \
        auto s = std::get<std::string>(method->receiver); \
        push(s.empty() ? nullptr : std::string(1, s.front())); \
        break; \
    }

#define NATIVE_PRIMITIVE_STR_LAST \
    case NativeMethod::STR_LAST: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("last() takes no arguments"); \
    \
        auto s = std::get<std::string>(method->receiver); \
        push(s.empty() ? nullptr : std::string(1, s.back())); \
        break; \
    }

#define NATIVE_PRIMITIVE_STARTS_WITH \
    case NativeMethod::STARTS_WITH: \
    { \
        if(args.size() != 1) \
            throw std::runtime_error("starts_with() expects 1 argument"); \
    \
        auto s = std::get<std::string>(method->receiver); \
        auto p = std::get<std::string>(args[0]); \
        push(s.rfind(p, 0) == 0); \
        break; \
    }

#define NATIVE_PRIMITIVE_ENDS_WITH \
    case NativeMethod::ENDS_WITH: \
    { \
        if(args.size() != 1) \
            throw std::runtime_error("ends_with() expects 1 argument"); \
    \
        auto s = std::get<std::string>(method->receiver); \
        auto p = std::get<std::string>(args[0]); \
        push(s.size() >= p.size() && \
             s.compare(s.size() - p.size(), p.size(), p) == 0); \
        break; \
    }

#define NATIVE_PRIMITIVE_IS_NUMBER \
    case NativeMethod::IS_NUMBER: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("is_number() takes no arguments"); \
    \
        auto s = std::get<std::string>(method->receiver); \
        char* end; \
        std::strtod(s.c_str(), &end); \
        push(end != s.c_str() && *end == '\0'); \
        break; \
    }

#define NATIVE_PRIMITIVE_STR_TRIM \
    case NativeMethod::STR_TRIM: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("trim() takes no arguments"); \
    \
        auto s = std::get<std::string>(method->receiver); \
        auto ws = " \t\n\r"; \
        s.erase(0, s.find_first_not_of(ws)); \
        s.erase(s.find_last_not_of(ws) + 1); \
        push(s); \
        break; \
    }

#define NATIVE_PRIMITIVE_TO_ARRAY \
    case NativeMethod::TO_ARRAY: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("to_array() takes no arguments"); \
    \
        auto s = std::get<std::string>(method->receiver); \
        auto arr = std::make_shared<Array>(); \
        for(char c : s) \
            arr->elements.push_back(std::string(1, c)); \
        push(arr); \
        break; \
    }

#define NATIVE_PRIMITIVE_REPLACE \
    case NativeMethod::REPLACE: \
    { \
        if(args.size() != 2) \
            throw std::runtime_error("replace() expects 2 arguments"); \
    \
        auto s = std::get<std::string>(method->receiver); \
        auto target = std::get<std::string>(args[0]); \
        auto replacement = std::get<std::string>(args[1]); \
    \
        if(target.empty()) \
            throw std::runtime_error("replace() target cannot be empty"); \
    \
        size_t pos = 0; \
        while((pos = s.find(target, pos)) != std::string::npos) \
        { \
            s.replace(pos, target.length(), replacement); \
            pos += replacement.length(); \
        } \
    \
        push(s); \
        break; \
    }

#define NATIVE_PRIMITIVE_IS_SPACE \
    case NativeMethod::IS_SPACE: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("is_space() takes no arguments"); \
    \
        auto s = std::get<std::string>(method->receiver); \
        if(s.size() != 1) \
            throw std::runtime_error("is_space() expects a single character string"); \
    \
        char c = s[0]; \
        bool result = std::isspace(static_cast<unsigned char>(c)); \
    \
        push(result ? true : false); \
        break; \
    }

#define NATIVE_PRIMITIVE_IS_ALL_SPACES \
    case NativeMethod::IS_ALL_SPACES: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("is_all_spaces() takes no arguments"); \
    \
        auto s = std::get<std::string>(method->receiver); \
    \
        bool all_spaces = true; \
        for(char c : s) { \
            if(!std::isspace(static_cast<unsigned char>(c))) \
            { \
                all_spaces = false; \
                break; \
            } \
        } \
    \
        push(all_spaces ? true : false); \
        break; \
    }

#define NATIVE_PRIMITIVE_TO_NUMBER \
    case NativeMethod::TO_NUMBER: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("to_number() takes no arguments"); \
        \
        auto s = std::get<std::string>(method->receiver); \
        \
        try \
        { \
            size_t idx; \
            double val = std::stod(s, &idx); \
        \
            if(idx != s.size()) \
                throw std::runtime_error("Invalid numeric string"); \
        \
            push(val); \
            break; \
        } \
        catch(const std::invalid_argument&) \
        { \
            throw std::runtime_error("Not a valid number"); \
        } \
        catch(const std::out_of_range&) \
        { \
            throw std::runtime_error("Number out of range"); \
        } \
    }

#define NATIVE_PRIMITIVE_STR_IS_EMPTY \
    case NativeMethod::STR_IS_EMPTY: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("is_empty() takes no arguments"); \
    \
        auto s = std::get<std::string>(method->receiver); \
        bool result = s.empty(); \
        push(result ? true : false); \
        break; \
    }

#define NATIVE_PRIMITIVE_STR_LENGTH \
    case NativeMethod::STR_LENGTH: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("length() takes no arguments"); \
    \
        auto s = std::get<std::string>(method->receiver); \
        push(static_cast<double>(s.length())); \
        break; \
    }

#define NATIVE_PRIMITIVE_STR_SLICE \
    case NativeMethod::STR_SLICE: \
    { \
        if(args.size() < 2 || args.size() > 2) \
            throw std::runtime_error("slice() expects 2 arguments"); \
    \
        auto s = std::get<std::string>(method->receiver); \
        int start = static_cast<int>(std::get<double>(args[0])); \
        int len = static_cast<int>(std::get<double>(args[1])); \
    \
        if(start < 0 || len < 0 || start >= static_cast<int>(s.size())) \
            throw std::runtime_error("slice() invalid range"); \
    \
        std::string result = s.substr(start, len); \
        push(result); \
        break; \
    }

#define NATIVE_PRIMITIVE_STR_COUNT \
    case NativeMethod::STR_COUNT: \
    { \
        if(args.size() != 1) \
            throw std::runtime_error("count() expects 1 argument"); \
    \
        auto s = std::get<std::string>(method->receiver); \
        auto sub = std::get<std::string>(args[0]); \
    \
        if(sub.empty()) \
            throw std::runtime_error("count() the first argument cannot be empty"); \
    \
        int count = 0; \
        size_t pos = 0; \
        while((pos = s.find(sub, pos)) != std::string::npos) \
        { \
            ++count; \
            pos += sub.size(); \
        } \
    \
        push(static_cast<double>(count)); \
        break; \
    }