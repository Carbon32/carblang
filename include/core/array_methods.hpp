// I'm trying to keep the length of each file reasonable, so I'm gonna rely
// on macros to define the native methods here, this file will contain the
// native methods of arrays

#define NATIVE_ARRAY_PUSH \
    case NativeMethod::PUSH: \
    { \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        for(auto& v : args) \
        { \
            array->elements.push_back(v); \
        } \
        push(nullptr); \
        break; \
    }

#define NATIVE_ARRAY_POP \
    case NativeMethod::POP: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("pop() takes no arguments"); \
    \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        if(array->elements.empty()) \
        { \
            push(nullptr); \
        } \
        else \
        { \
            push(array->elements.back()); \
            array->elements.pop_back(); \
        } \
        break; \
    }

#define NATIVE_ARRAY_LENGTH \
    case NativeMethod::LENGTH: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("length() takes no arguments"); \
    \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        push((double) array->elements.size()); \
        break; \
    }

#define NATIVE_ARRAY_IS_EMPTY \
    case NativeMethod::IS_EMPTY: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("is_empty() takes no arguments"); \
    \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        push(array->elements.empty()); \
        break; \
    }

#define NATIVE_ARRAY_CLEAR \
    case NativeMethod::CLEAR: \
     { \
        if(!args.empty()) \
            throw std::runtime_error("clear() takes no arguments"); \
    \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        array->elements.clear(); \
        push(nullptr); \
        break; \
    }

#define NATIVE_ARRAY_CONTAINS \
    case NativeMethod::CONTAINS: \
    { \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        if(args.size() != 1) \
            throw std::runtime_error("contains() expects 1 argument"); \
    \
        bool found = false; \
        for(auto& v : array->elements) \
            if(v == args[0]) { found = true; break; } \
    \
        push(found); \
        break; \
    }

#define NATIVE_ARRAY_INDEX_OF \
    case NativeMethod::INDEX_OF: \
    { \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        if(args.size() != 1) \
            throw std::runtime_error("index_of() expects 1 argument"); \
    \
        int idx = -1; \
        for(size_t i = 0; i < array->elements.size(); ++i) \
        { \
            if(array->elements[i] == args[0]) { \
                idx = (int) i; \
                break; \
            } \
        } \
    \
        push((double) idx); \
        break; \
    }

#define NATIVE_ARRAY_JOIN \
    case NativeMethod::JOIN: \
    { \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        std::string sep = args.empty() \
            ? "," \
            : std::get<std::string>(args[0]); \
    \
        if(args.size() != 1) \
            throw std::runtime_error("join() expects 1 argument"); \
    \
        std::ostringstream out; \
        const auto& elems = array->elements; \
    \
        for(size_t i = 0; i < elems.size(); ++i) \
        { \
            out << stringify(elems[i]); \
            if(i + 1 < elems.size()) \
                out << sep; \
        } \
    \
        push(out.str()); \
        break; \
    }

#define NATIVE_ARRAY_INSERT \
    case NativeMethod::INSERT: \
    { \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        if(args.size() != 2) \
            throw std::runtime_error("insert() expects 2 arguments"); \
    \
        int idx = (int) std::get<double>(args[0]); \
        if(idx < 0 || idx > array->elements.size()) \
            throw std::runtime_error("Index out of bounds"); \
    \
        array->elements.insert( \
            array->elements.begin() + idx, \
            args[1] \
        ); \
    \
        push(nullptr); \
        break; \
    }

#define NATIVE_ARRAY_REMOVE_AT \
    case NativeMethod::REMOVE_AT: \
    { \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        if(args.size() != 1) \
            throw std::runtime_error("remove() expects 1 argument"); \
    \
        int idx = (int) std::get<double>(args[0]); \
        if(idx < 0 || idx >= array->elements.size()) \
        { \
            push(nullptr); \
            break; \
        } \
    \
        Value v = array->elements[idx]; \
        array->elements.erase( \
            array->elements.begin() + idx \
        ); \
    \
        push(v); \
        break; \
    }

#define NATIVE_ARRAY_REVERSE \
    case NativeMethod::REVERSE: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("clear() takes no arguments"); \
    \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        std::reverse(array->elements.begin(), array->elements.end()); \
        push(nullptr); \
        break; \
    }

#define NATIVE_ARRAY_FIRST \
    case NativeMethod::FIRST: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("clear() takes no arguments"); \
    \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        if(array->elements.empty()) push(nullptr); \
        else push(array->elements.front()); \
        break; \
    }

#define NATIVE_ARRAY_LAST \
    case NativeMethod::LAST: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("clear() takes no arguments"); \
    \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        if(array->elements.empty()) push(nullptr); \
        else push(array->elements.back()); \
        break; \
    }

#define NATIVE_ARRAY_SLICE \
    case NativeMethod::SLICE: \
    { \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        if(args.size() < 1 || args.size() > 2) \
            throw std::runtime_error("slice() expects 1 or 2 arguments"); \
    \
        int start = (int) std::get<double>(args[0]); \
        int end = args.size() == 2 ? (int) std::get<double>(args[1]) : (int) array->elements.size(); \
    \
        if(start < 0) start = 0; \
        if(end > (int) array->elements.size()) end = (int) array->elements.size(); \
        if(end < start) end = start; \
    \
        auto new_array = std::make_shared<Array>(); \
        new_array->elements.insert( \
            new_array->elements.begin(), \
            array->elements.begin() + start, \
            array->elements.begin() + end \
        ); \
    \
        push(new_array); \
        break; \
    }

#define NATIVE_ARRAY_COPY \
    case NativeMethod::COPY: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("clear() takes no arguments"); \
    \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        auto new_array = std::make_shared<Array>(); \
        new_array->elements = array->elements; \
        push(new_array); \
        break; \
    }

#define NATIVE_ARRAY_CONCAT \
    case NativeMethod::CONCAT: \
    { \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        if(args.size() != 1) \
            throw std::runtime_error("concat() expects 1 argument"); \
    \
        if(!std::holds_alternative<std::shared_ptr<Array>>(args[0])) \
            throw std::runtime_error("concat() argument must be an array"); \
    \
        auto other = std::get<std::shared_ptr<Array>>(args[0]); \
        auto new_array = std::make_shared<Array>(); \
        new_array->elements = array->elements; \
        new_array->elements.insert(new_array->elements.end(), other->elements.begin(), other->elements.end()); \
    \
        push(new_array); \
        break; \
    }

#define NATIVE_ARRAY_SWAP \
    case NativeMethod::SWAP: \
    { \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        if(args.size() != 2) \
            throw std::runtime_error("swap() expects 2 arguments"); \
    \
        int i = (int) std::get<double>(args[0]); \
        int j = (int) std::get<double>(args[1]); \
    \
        if(i < 0 || j < 0 || \
            i >= array->elements.size() || \
            j >= array->elements.size()) \
            throw std::runtime_error("Index out of bounds"); \
    \
        std::swap(array->elements[i], \
                  array->elements[j]); \
    \
        push(nullptr); \
        break; \
    }

#define NATIVE_ARRAY_COUNT \
    case NativeMethod::COUNT: \
    { \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        if(args.size() != 1) \
            throw std::runtime_error("count() expects 1 argument"); \
    \
        int c = 0; \
        for(auto& v : array->elements) \
            if(v == args[0]) c++; \
    \
        push((double) c); \
        break; \
    }

#define NATIVE_ARRAY_EQUALS \
    case NativeMethod::EQUALS: \
    { \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        if(args.size() != 1 || \
            !std::holds_alternative<std::shared_ptr<Array>>(args[0])) \
        { \
            push(false); \
            break; \
        } \
    \
        auto other = std::get<std::shared_ptr<Array>>(args[0]); \
        auto& a = array->elements; \
        auto& b = other->elements; \
        bool res = true; \
    \
        if(a.size() != b.size()) { \
            res = false; \
            break; \
        } \
    \
        for(size_t i = 0; i < a.size(); ++i) { \
            if(res != false) \
            { \
                if(!value_shallow_equal(a[i], b[i])) { \
                    res = false; \
                    break; \
                } \
            } \
        } \
    \
        push(res); \
        break; \
    }

#define NATIVE_ARRAY_LAST_INDEX_OF \
    case NativeMethod::LAST_INDEX_OF: \
    { \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        if(args.size() != 1) \
            throw std::runtime_error("last_index() expects 1 argument"); \
    \
    \
        int idx = -1; \
        for(int i = (int) array->elements.size() - 1; i >= 0; --i) { \
            if(array->elements[i] == args[0]) { \
                idx = (int) i; \
                break; \
            } \
        } \
    \
        push((double) idx); \
        break; \
    }

#define NATIVE_ARRAY_SUM \
    case NativeMethod::SUM: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("sum() takes no arguments"); \
    \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        double total = 0; \
        for(auto& v : array->elements) \
        { \
            if(!std::holds_alternative<double>(v)) \
                throw std::runtime_error("sum() only works on numeric arrays"); \
            total += std::get<double>(v); \
        } \
        push(total); \
        break; \
    }

#define NATIVE_ARRAY_MIN \
    case NativeMethod::MIN: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("min() takes no arguments"); \
    \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        if(array->elements.empty()) \
            throw std::runtime_error("min() of empty array"); \
    \
        double min_val = std::numeric_limits<double>::max(); \
        for(auto& v : array->elements) \
        { \
            if(!std::holds_alternative<double>(v)) \
                throw std::runtime_error("min() only works on numeric arrays"); \
            min_val = std::min(min_val, std::get<double>(v)); \
        } \
        push(min_val); \
        break; \
    }

#define NATIVE_ARRAY_MAX \
    case NativeMethod::MAX: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("max() takes no arguments"); \
    \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        if(array->elements.empty()) \
            throw std::runtime_error("max() of empty array"); \
    \
        double max_val = std::numeric_limits<double>::lowest(); \
        for(auto& v : array->elements) \
        { \
            if(!std::holds_alternative<double>(v)) \
                throw std::runtime_error("max() only works on numeric arrays"); \
            max_val = std::max(max_val, std::get<double>(v)); \
        } \
        push(max_val); \
        break; \
    }

#define NATIVE_ARRAY_AVERAGE \
    case NativeMethod::AVERAGE: \
    { \
        if(!args.empty()) \
            throw std::runtime_error("average() takes no arguments"); \
    \
        auto& array = std::get<std::shared_ptr<Array>>(method->receiver); \
        if(array->elements.empty()) \
            throw std::runtime_error("average() of empty array"); \
    \
        double total = 0; \
        for(auto& v : array->elements) \
        { \
            if(!std::holds_alternative<double>(v)) \
                throw std::runtime_error("average() only works on numeric arrays"); \
            total += std::get<double>(v); \
        } \
        push(total / array->elements.size()); \
        break; \
    }


#define NATIVE_ARRAY_TRIM \
    case NativeMethod::TRIM: \
    { \
        auto& array = std::get<std::shared_ptr<Array>>( \
            method->receiver); \
        if(args.size() != 1) \
            throw std::runtime_error("trim() expects 1 argument"); \
    \
        int n = (int) std::get<double>(args[0]); \
        if(n < 0) n = 0; \
        if((size_t) n < array->elements.size()) \
            array->elements.resize(n); \
    \
        push(nullptr); \
        break; \
    }
