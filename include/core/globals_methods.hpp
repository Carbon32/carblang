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