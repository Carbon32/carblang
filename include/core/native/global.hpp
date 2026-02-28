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
        else if(std::holds_alternative<std::shared_ptr<Class>>(v)) \
        { \
            auto klass = std::get<std::shared_ptr<Class>>(v); \
            push("<class " + klass->name + ">"); \
        } \
        else if(std::holds_alternative<std::shared_ptr<Instance>>(v)) \
        { \
            auto instance = std::get<std::shared_ptr<Instance>>(v); \
            push("<" + instance->klass->name + " instance>"); \
        } \
        else if(std::holds_alternative<std::shared_ptr<UserBoundMethod>>(v)) \
        { \
            push("<bound method>"); \
        } \
        else if(std::holds_alternative<std::shared_ptr<BoundMethod>>(v)) \
        { \
            push("<native method>"); \
        } \
        else if(std::holds_alternative<std::shared_ptr<Dict>>(v)) \
        { \
            push("<dict>"); \
        } \
        else push("<unknown>"); \
    \
        break; \
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

#define NATIVE_GLOBALS_INSTANCE_METHODS \
    case NativeMethod::INSTANCE_METHODS: \
    { \
        if(args.size() != 1) \
            throw std::runtime_error("methods() expects 1 argument"); \
        \
        if(!std::holds_alternative<std::shared_ptr<Instance>>(args[0])) \
            throw std::runtime_error("methods() argument must be an Instance"); \
        \
        auto instance = std::get<std::shared_ptr<Instance>>(args[0]); \
        if(!instance) throw std::runtime_error("The instance is null"); \
        if(!instance->klass) throw std::runtime_error("The instance has no class"); \
        \
        auto methods_array = std::make_shared<Array>(); \
        std::unordered_set<std::string> seen; \
        \
        \
        for(auto klass = instance->klass; klass != nullptr; klass = klass->super_class) \
        { \
            for(const auto& [name, func] : klass->methods) \
            { \
                if(!name.empty() && seen.insert(name).second) \
                    methods_array->elements.push_back(name); \
            } \
        } \
        \
        \
        for(const auto& [field_name, value] : instance->fields) \
        { \
            if(std::holds_alternative<std::shared_ptr<UserBoundMethod>>(value) || \
               std::holds_alternative<std::shared_ptr<BoundMethod>>(value)) \
            { \
                if(!field_name.empty() && seen.insert(field_name).second) \
                    methods_array->elements.push_back(field_name); \
            } \
        } \
        \
        push(methods_array); \
        break; \
    }