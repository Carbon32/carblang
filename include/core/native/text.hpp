#define NATIVE_GLOBALS_FORMAT                                                          \
    case NativeMethod::FORMAT:                                                         \
    {                                                                                  \
        if (args.empty())                                                              \
            throw std::runtime_error("format() requires at least a format string");    \
                                                                                       \
        if (!std::holds_alternative<std::string>(args[0]))                             \
            throw std::runtime_error("format() first argument must be a string");      \
                                                                                       \
        std::string fmt = std::get<std::string>(args[0]);                              \
        std::string result;                                                            \
        size_t arg_index = 1;                                                          \
                                                                                       \
        for (size_t i = 0; i < fmt.size(); ++i)                                        \
        {                                                                              \
            if (fmt[i] == '%')                                                         \
            {                                                                          \
                if (i + 1 < fmt.size() && fmt[i + 1] == '%')                           \
                {                                                                      \
                    result += '%';                                                     \
                    ++i;                                                               \
                }                                                                      \
                else                                                                   \
                {                                                                      \
                    if (arg_index >= args.size())                                      \
                        throw std::runtime_error("Not enough arguments for format()"); \
                                                                                       \
                    result += stringify(args[arg_index++]);                            \
                }                                                                      \
            }                                                                          \
            else                                                                       \
            {                                                                          \
                result += fmt[i];                                                      \
            }                                                                          \
        }                                                                              \
                                                                                       \
        if (arg_index != args.size())                                                  \
            throw std::runtime_error("Too many arguments for format()");               \
                                                                                       \
        push(result);                                                                  \
        break;                                                                         \
    }

#define NATIVE_GLOBALS_PRINT                     \
    case NativeMethod::PRINT:                    \
    {                                            \
        for (size_t i = 0; i < args.size(); ++i) \
        {                                        \
            std::cout << stringify(args[i]);     \
            if (i + 1 < args.size())             \
                std::cout << ' ';                \
        }                                        \
        push(nullptr);                           \
        break;                                   \
    }

#define NATIVE_GLOBALS_PRINTLN                   \
    case NativeMethod::PRINTLN:                  \
    {                                            \
        for (size_t i = 0; i < args.size(); ++i) \
        {                                        \
            std::cout << stringify(args[i]);     \
            if (i + 1 < args.size())             \
                std::cout << ' ';                \
        }                                        \
        std::cout << std::endl;                  \
        push(nullptr);                           \
        break;                                   \
    }

#define NATIVE_GLOBALS_PRINTF                                                          \
    case NativeMethod::PRINTF:                                                         \
    {                                                                                  \
        if (args.empty())                                                              \
            throw std::runtime_error("printf() requires at least a format string");    \
                                                                                       \
        if (!std::holds_alternative<std::string>(args[0]))                             \
            throw std::runtime_error("printf() first argument must be a string");      \
                                                                                       \
        std::string fmt = std::get<std::string>(args[0]);                              \
        size_t arg_index = 1;                                                          \
                                                                                       \
        for (size_t i = 0; i < fmt.size(); ++i)                                        \
        {                                                                              \
            if (fmt[i] == '%')                                                         \
            {                                                                          \
                if (i + 1 < fmt.size() && fmt[i + 1] == '%')                           \
                {                                                                      \
                    std::cout << '%';                                                  \
                    ++i;                                                               \
                }                                                                      \
                else                                                                   \
                {                                                                      \
                    if (arg_index >= args.size())                                      \
                        throw std::runtime_error("Not enough arguments for printf()"); \
                                                                                       \
                    std::cout << stringify(args[arg_index++]);                         \
                }                                                                      \
            }                                                                          \
            else                                                                       \
            {                                                                          \
                std::cout << fmt[i];                                                   \
            }                                                                          \
        }                                                                              \
                                                                                       \
        if (arg_index != args.size())                                                  \
            throw std::runtime_error("Too many arguments for printf()");               \
                                                                                       \
        push(nullptr);                                                                 \
        break;                                                                         \
    }

#define NATIVE_GLOBALS_INPUT                                                 \
    case NativeMethod::INPUT:                                                \
    {                                                                        \
        std::string prompt;                                                  \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            if (!std::holds_alternative<std::string>(args[0]))               \
                throw std::runtime_error("input() prompt must be a string"); \
            prompt = std::get<std::string>(args[0]);                         \
        }                                                                    \
                                                                             \
        std::cout << prompt;                                                 \
        std::string line;                                                    \
        std::getline(std::cin, line);                                        \
                                                                             \
        push(line);                                                          \
        break;                                                               \
    }

#define NATIVE_GLOBALS_PARSE_JSON                                              \
    case NativeMethod::PARSE_JSON:                                             \
    {                                                                          \
        if (args.size() != 1 || !std::holds_alternative<std::string>(args[0])) \
            throw std::runtime_error("parse() requires a string");             \
                                                                               \
        JsonParser parser(std::get<std::string>(args[0]));                     \
        push(parser.parse());                                                  \
        break;                                                                 \
    }

#define NATIVE_GLOBALS_STRINGIFY                                              \
    case NativeMethod::STRINGIFY:                                             \
    {                                                                         \
        if (args.size() != 1)                                                 \
            throw std::runtime_error("stringify() takes exactly 1 argument"); \
                                                                              \
        push(stringify(args[0]));                                             \
        break;                                                                \
    }

#define NATIVE_GLOBALS_TO_JSON                                            \
    case NativeMethod::TO_JSON:                                           \
    {                                                                     \
        if (args.size() != 1)                                             \
            throw std::runtime_error("json() requires 1 argument");       \
        if (!std::holds_alternative<std::shared_ptr<Dict>>(args[0]))      \
            throw std::runtime_error("json() only accepts dictionaries"); \
                                                                          \
        push(json_stringify(args[0]));                                    \
        break;                                                            \
    }

#define NATIVE_GLOBALS_PARSE_CSV                                               \
    case NativeMethod::PARSE_CSV:                                              \
    {                                                                          \
        if (args.size() != 1 || !std::holds_alternative<std::string>(args[0])) \
            throw std::runtime_error("parse_csv() requires a string");         \
                                                                               \
        CsvParser parser(std::get<std::string>(args[0]));                      \
        auto raw = parser.parse();                                             \
        auto outer = std::make_shared<Array>();                                \
        for (const auto &row : raw)                                            \
        {                                                                      \
            auto inner = std::make_shared<Array>();                            \
            for (const auto &cell : row)                                       \
            {                                                                  \
                inner->elements.push_back(cell);                               \
            }                                                                  \
            outer->elements.push_back(inner);                                  \
        }                                                                      \
        push(outer);                                                           \
        break;                                                                 \
    }

#define NATIVE_GLOBALS_TO_CSV                                           \
    case NativeMethod::TO_CSV:                                          \
    {                                                                   \
        if (args.size() != 1)                                           \
            throw std::runtime_error("csv() requires 1 argument");      \
                                                                        \
        if (!std::holds_alternative<std::shared_ptr<Array>>(args[0]))   \
            throw std::runtime_error("csv() expects an array of rows"); \
                                                                        \
        push(csv_stringify(args[0]));                                   \
        break;                                                          \
    }

#define NATIVE_GLOBALS_JSON_TO_HTML                                                       \
    case NativeMethod::JSON_TO_HTML:                                                      \
    {                                                                                     \
        if (args.size() != 1)                                                             \
            throw std::runtime_error("json_to_html() requires 1 argument");               \
                                                                                          \
        std::function<std::shared_ptr<Tag>(const Value &)> to_dom;                        \
                                                                                          \
        to_dom = [&](const Value &v) -> std::shared_ptr<Tag>                              \
        {                                                                                 \
            if (std::holds_alternative<std::nullptr_t>(v))                                \
                return std::make_shared<Tag>("span", "null");                             \
                                                                                          \
            if (std::holds_alternative<bool>(v))                                          \
                return std::make_shared<Tag>("span",                                      \
                                             std::get<bool>(v) ? "true" : "false");       \
                                                                                          \
            if (std::holds_alternative<double>(v))                                        \
                return std::make_shared<Tag>("span",                                      \
                                             std::to_string(std::get<double>(v)));        \
                                                                                          \
            if (std::holds_alternative<std::string>(v))                                   \
                return std::make_shared<Tag>("span",                                      \
                                             std::get<std::string>(v));                   \
                                                                                          \
            /* ARRAY */                                                                   \
            if (std::holds_alternative<std::shared_ptr<Array>>(v))                        \
            {                                                                             \
                auto arr = std::get<std::shared_ptr<Array>>(v);                           \
                auto ul = std::make_shared<Tag>("ul");                                    \
                                                                                          \
                for (const auto &el : arr->elements)                                      \
                {                                                                         \
                    auto li = std::make_shared<Tag>("li");                                \
                    li->child(to_dom(el));                                                \
                    ul->child(li);                                                        \
                }                                                                         \
                                                                                          \
                return ul;                                                                \
            }                                                                             \
                                                                                          \
            /* OBJECT */                                                                  \
            if (std::holds_alternative<std::shared_ptr<Dict>>(v))                         \
            {                                                                             \
                auto dict = std::get<std::shared_ptr<Dict>>(v);                           \
                auto div = std::make_shared<Tag>("div");                                  \
                                                                                          \
                for (const auto &[key, val] : dict->entries)                              \
                {                                                                         \
                    auto row = std::make_shared<Tag>("div");                              \
                                                                                          \
                    /* key */                                                             \
                    auto k = std::make_shared<Tag>("span", key + ": ");                   \
                    row->child(k);                                                        \
                                                                                          \
                    /* value (IMPORTANT FIX: NO WRAPPER) */                               \
                    row->child(to_dom(val));                                              \
                                                                                          \
                    div->child(row);                                                      \
                }                                                                         \
                                                                                          \
                return div;                                                               \
            }                                                                             \
                                                                                          \
            return std::make_shared<Tag>("span", "unknown");                              \
        };                                                                                \
                                                                                          \
        auto style = std::make_shared<Tag>("style");                                      \
        style->text =                                                                     \
            "div{font-family:system-ui,-apple-system,Segoe UI,Roboto,sans-serif;"         \
            "font-size:14px;line-height:1.45;}"                                           \
                                                                                          \
            "div>div{display:flex;gap:10px;align-items:flex-start;"                       \
            "padding:2px 6px;border-radius:6px;}"                                         \
                                                                                          \
            "div>div:hover{background:rgba(0,0,0,0.05);}"                                 \
                                                                                          \
            "div>div>span:first-child{color:#2563eb;font-weight:600;white-space:nowrap;}" \
                                                                                          \
            "div>div>span:nth-child(2){color:#111827;}"                                   \
                                                                                          \
            "ul{list-style:none;margin:2px 0 2px 12px;padding-left:12px;"                 \
            "border-left:1px solid rgba(0,0,0,0.12);}"                                    \
                                                                                          \
            "li{margin:2px 0;padding-left:8px;position:relative;}"                        \
                                                                                          \
            "li::before{content:'';position:absolute;left:-6px;top:10px;"                 \
            "width:6px;height:1px;background:rgba(0,0,0,0.2);}";                          \
                                                                                          \
        auto root = std::make_shared<Tag>("div");                                         \
        root->child(style);                                                               \
        root->child(to_dom(args[0]));                                                     \
                                                                                          \
        push(root);                                                                       \
        break;                                                                            \
    }

#define NATIVE_GLOBALS_CSV_TO_HTML                                                        \
    case NativeMethod::CSV_TO_HTML:                                                       \
    {                                                                                     \
        if (args.size() != 1 || !std::holds_alternative<std::shared_ptr<Array>>(args[0])) \
            throw std::runtime_error("csv_to_html() requires an array of rows");          \
                                                                                          \
        auto rows = std::get<std::shared_ptr<Array>>(args[0]);                            \
                                                                                          \
        std::function<std::shared_ptr<Tag>(const Value &)> cell_to_dom;                   \
                                                                                          \
        cell_to_dom = [&](const Value &v) -> std::shared_ptr<Tag>                         \
        {                                                                                 \
            if (std::holds_alternative<std::nullptr_t>(v))                                \
                return std::make_shared<Tag>("span", "");                                 \
                                                                                          \
            if (std::holds_alternative<bool>(v))                                          \
                return std::make_shared<Tag>("span",                                      \
                                             std::get<bool>(v) ? "true" : "false");       \
                                                                                          \
            if (std::holds_alternative<double>(v))                                        \
                return std::make_shared<Tag>("span",                                      \
                                             std::to_string(std::get<double>(v)));        \
                                                                                          \
            if (std::holds_alternative<std::string>(v))                                   \
                return std::make_shared<Tag>("span",                                      \
                                             std::get<std::string>(v));                   \
                                                                                          \
            return std::make_shared<Tag>("span", "unknown");                              \
        };                                                                                \
                                                                                          \
        auto table = std::make_shared<Tag>("div");                                        \
                                                                                          \
        for (const auto &row_v : rows->elements)                                          \
        {                                                                                 \
            if (!std::holds_alternative<std::shared_ptr<Array>>(row_v))                   \
                continue;                                                                 \
                                                                                          \
            auto row = std::get<std::shared_ptr<Array>>(row_v);                           \
            auto row_div = std::make_shared<Tag>("div");                                  \
                                                                                          \
            for (const auto &cell_v : row->elements)                                      \
            {                                                                             \
                row_div->child(cell_to_dom(cell_v));                                      \
            }                                                                             \
                                                                                          \
            table->child(row_div);                                                        \
        }                                                                                 \
                                                                                          \
        auto style = std::make_shared<Tag>("style");                                      \
        style->text =                                                                     \
            "div{font-family:system-ui,-apple-system,Segoe UI,Roboto,sans-serif;"         \
            "font-size:14px;line-height:1.45;}"                                           \
                                                                                          \
            "div>div>div{display:flex;flex-direction:row;align-items:stretch;"            \
            "border-bottom:1px solid rgba(0,0,0,0.08);} "                                 \
                                                                                          \
            "div>div>div:hover{background:rgba(0,0,0,0.05);} "                            \
                                                                                          \
            "div>div>div>span{padding:6px 10px;"                                          \
            "border-right:1px solid rgba(0,0,0,0.06);"                                    \
            "white-space:nowrap;}"                                                        \
                                                                                          \
            "div>div>div>span:last-child{border-right:none;}"                             \
                                                                                          \
            "div>div>div:first-child{font-weight:600;"                                    \
            "background:rgba(0,0,0,0.03);} "                                              \
                                                                                          \
            "div>div>div>span{min-width:120px;}"                                          \
                                                                                          \
            "div>div>div:nth-child(even){background:rgba(0,0,0,0.02);} ";                 \
                                                                                          \
        auto root = std::make_shared<Tag>("div");                                         \
        root->child(style);                                                               \
        root->child(table);                                                               \
                                                                                          \
        push(root);                                                                       \
        break;                                                                            \
    }