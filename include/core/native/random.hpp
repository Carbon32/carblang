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