#define NATIVE_GLOBALS_POW \
    case NativeMethod::POW: \
    { \
        if(args.size() != 2) \
            throw std::runtime_error("pow() expects 2 arguments: base and exponent"); \
        \
        if(!std::holds_alternative<double>(args[0]) || \
           !std::holds_alternative<double>(args[1])) \
            throw std::runtime_error("pow() arguments must be numbers"); \
        \
        double base = std::get<double>(args[0]); \
        double exp = std::get<double>(args[1]); \
        push(std::pow(base, exp)); \
        break; \
    }

#define NATIVE_GLOBALS_SQRT \
    case NativeMethod::SQRT: \
    { \
        if(args.size() != 1) \
            throw std::runtime_error("sqrt() expects 1 argument"); \
        \
        if(!std::holds_alternative<double>(args[0])) \
            throw std::runtime_error("sqrt() argument must be a number"); \
        \
        double x = std::get<double>(args[0]); \
        if(x < 0) throw std::runtime_error("sqrt() of negative number"); \
        push(std::sqrt(x)); \
        break; \
    }

#define NATIVE_GLOBALS_FACT \
    case NativeMethod::FACT: \
    { \
        if(args.size() != 1) \
            throw std::runtime_error("fact() expects 1 argument"); \
        \
        if(!std::holds_alternative<double>(args[0])) \
            throw std::runtime_error("fact() argument must be a number"); \
        \
        double val = std::get<double>(args[0]); \
        if(std::floor(val) != val) \
            throw std::runtime_error("fact() only works with whole numbers"); \
        \
        int n = static_cast<int>(val); \
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

#define NATIVE_GLOBALS_FLOOR \
    case NativeMethod::FLOOR: \
    { \
        if(args.size() != 1) \
            throw std::runtime_error("floor() expects 1 argument"); \
        \
        if(!std::holds_alternative<double>(args[0])) \
            throw std::runtime_error("floor() argument must be a number"); \
        \
        push(std::floor(std::get<double>(args[0]))); \
        break; \
    }

#define NATIVE_GLOBALS_CEIL \
    case NativeMethod::CEIL: \
    { \
        if(args.size() != 1) \
            throw std::runtime_error("ceil() expects 1 argument"); \
        \
        if(!std::holds_alternative<double>(args[0])) \
            throw std::runtime_error("ceil() argument must be a number"); \
        \
        push(std::ceil(std::get<double>(args[0]))); \
        break; \
    }