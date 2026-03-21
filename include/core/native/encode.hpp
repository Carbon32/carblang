#define NATIVE_GLOBALS_BASE                                                                       \
    case NativeMethod::BASE:                                                                      \
    {                                                                                             \
        if (args.size() != 2)                                                                     \
            throw std::runtime_error("base() expects 2 arguments, number and base");              \
                                                                                                  \
        if (!std::holds_alternative<double>(args[0]) || !std::holds_alternative<double>(args[1])) \
            throw std::runtime_error("base() arguments must be numbers");                         \
                                                                                                  \
        double num = std::get<double>(args[0]);                                                   \
        int base = static_cast<int>(std::get<double>(args[1]));                                   \
                                                                                                  \
        if (base < 2 || base > 16)                                                                \
            throw std::runtime_error("base() only supports bases from 2 to 16");                  \
                                                                                                  \
        long long n = static_cast<long long>(num);                                                \
        bool negative = n < 0;                                                                    \
        if (negative)                                                                             \
            n = -n;                                                                               \
                                                                                                  \
        std::string digits = "0123456789ABCDEF";                                                  \
        std::string result;                                                                       \
                                                                                                  \
        do                                                                                        \
        {                                                                                         \
            result = digits[n % base] + result;                                                   \
            n /= base;                                                                            \
        } while (n > 0);                                                                          \
                                                                                                  \
        if (negative)                                                                             \
            result = "-" + result;                                                                \
        push(result);                                                                             \
        break;                                                                                    \
    }