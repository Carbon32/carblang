#define NATIVE_HTML_HEAD                                                  \
    case NativeMethod::HTML_HEAD:                                         \
    {                                                                     \
        for (auto &arg : args)                                            \
        {                                                                 \
            if (!std::holds_alternative<std::shared_ptr<Tag>>(arg))       \
                throw std::runtime_error("head() expects tag arguments"); \
                                                                          \
            html_builder.head(std::get<std::shared_ptr<Tag>>(arg));       \
        }                                                                 \
                                                                          \
        push(nullptr);                                                    \
        break;                                                            \
    }

#define NATIVE_HTML_BODY                                                  \
    case NativeMethod::HTML_BODY:                                         \
    {                                                                     \
        for (auto &arg : args)                                            \
        {                                                                 \
            if (!std::holds_alternative<std::shared_ptr<Tag>>(arg))       \
                throw std::runtime_error("body() expects tag arguments"); \
                                                                          \
            html_builder.body(std::get<std::shared_ptr<Tag>>(arg));       \
        }                                                                 \
                                                                          \
        push(nullptr);                                                    \
        break;                                                            \
    }

#define NATIVE_HTML_LINK                                                \
    case NativeMethod::HTML_LINK:                                       \
    {                                                                   \
        std::string href = "";                                          \
        std::string rel = "";                                           \
                                                                        \
        if (args.size() > 0)                                            \
        {                                                               \
            if (!std::holds_alternative<std::string>(args[0]))          \
                throw std::runtime_error("link() expects string href"); \
            href = std::get<std::string>(args[0]);                      \
        }                                                               \
                                                                        \
        if (args.size() > 1)                                            \
        {                                                               \
            if (!std::holds_alternative<std::string>(args[1]))          \
                throw std::runtime_error("link() expects string rel");  \
            rel = std::get<std::string>(args[1]);                       \
        }                                                               \
                                                                        \
        auto tag = html_builder.link(href, rel);                        \
        push(std::shared_ptr<Tag>(std::move(tag)));                     \
        break;                                                          \
    }

#define NATIVE_HTML_META                                                        \
    case NativeMethod::HTML_META:                                               \
    {                                                                           \
        std::string text = "";                                                  \
        std::string name = "";                                                  \
        std::string httpequiv = "";                                             \
        std::string charset = "";                                               \
        std::string itemprop = "";                                              \
                                                                                \
        if (args.size() > 0)                                                    \
        {                                                                       \
            if (!std::holds_alternative<std::string>(args[0]))                  \
                throw std::runtime_error("meta() expects string text");         \
            text = std::get<std::string>(args[0]);                              \
        }                                                                       \
                                                                                \
        if (args.size() > 1)                                                    \
        {                                                                       \
            if (!std::holds_alternative<std::string>(args[1]))                  \
                throw std::runtime_error("meta() expects string name");         \
            name = std::get<std::string>(args[1]);                              \
        }                                                                       \
                                                                                \
        if (args.size() > 2)                                                    \
        {                                                                       \
            if (!std::holds_alternative<std::string>(args[2]))                  \
                throw std::runtime_error("meta() expects string httpequiv");    \
            httpequiv = std::get<std::string>(args[2]);                         \
        }                                                                       \
                                                                                \
        if (args.size() > 3)                                                    \
        {                                                                       \
            if (!std::holds_alternative<std::string>(args[3]))                  \
                throw std::runtime_error("meta() expects string charset");      \
            charset = std::get<std::string>(args[3]);                           \
        }                                                                       \
                                                                                \
        if (args.size() > 4)                                                    \
        {                                                                       \
            if (!std::holds_alternative<std::string>(args[4]))                  \
                throw std::runtime_error("meta() expects string itemprop");     \
            itemprop = std::get<std::string>(args[4]);                          \
        }                                                                       \
                                                                                \
        auto tag = html_builder.meta(text, name, httpequiv, charset, itemprop); \
        push(std::shared_ptr<Tag>(std::move(tag)));                             \
        break;                                                                  \
    }

#define NATIVE_HTML_TITLE                                           \
    case NativeMethod::HTML_TITLE:                                  \
    {                                                               \
        std::string text = "";                                      \
                                                                    \
        if (!args.empty())                                          \
        {                                                           \
            if (!std::holds_alternative<std::string>(args[0]))      \
                throw std::runtime_error("title() expects string"); \
                                                                    \
            text = std::get<std::string>(args[0]);                  \
        }                                                           \
                                                                    \
        auto tag = html_builder.title(text);                        \
        push(std::shared_ptr<Tag>(std::move(tag)));                 \
        break;                                                      \
    }

#define NATIVE_HTML_STYLE                                           \
    case NativeMethod::HTML_STYLE:                                  \
    {                                                               \
        std::string text = "";                                      \
                                                                    \
        if (!args.empty())                                          \
        {                                                           \
            if (!std::holds_alternative<std::string>(args[0]))      \
                throw std::runtime_error("style() expects string"); \
                                                                    \
            text = std::get<std::string>(args[0]);                  \
        }                                                           \
                                                                    \
        auto tag = html_builder.style(text);                        \
        push(std::shared_ptr<Tag>(std::move(tag)));                 \
        break;                                                      \
    }

#define NATIVE_HTML_ADDRESS                                                      \
    case NativeMethod::HTML_ADDRESS:                                             \
    {                                                                            \
        std::string text = "";                                                   \
        std::vector<std::shared_ptr<Tag>> children;                              \
                                                                                 \
        if (!args.empty())                                                       \
        {                                                                        \
            size_t i = 0;                                                        \
                                                                                 \
            if (std::holds_alternative<std::string>(args[0]))                    \
            {                                                                    \
                text = std::get<std::string>(args[0]);                           \
                i = 1;                                                           \
            }                                                                    \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))      \
            {                                                                    \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));     \
                i = 1;                                                           \
            }                                                                    \
            else                                                                 \
            {                                                                    \
                throw std::runtime_error("address() expects string or tag");     \
            }                                                                    \
                                                                                 \
            for (; i < args.size(); ++i)                                         \
            {                                                                    \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))      \
                    throw std::runtime_error("address() expects tag arguments"); \
                                                                                 \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));     \
            }                                                                    \
        }                                                                        \
                                                                                 \
        auto tag = html_builder.address(text);                                   \
                                                                                 \
        for (auto &c : children)                                                 \
            tag->child(c);                                                       \
                                                                                 \
        push(std::shared_ptr<Tag>(std::move(tag)));                              \
        break;                                                                   \
    }

#define NATIVE_HTML_ARTICLE                                                      \
    case NativeMethod::HTML_ARTICLE:                                             \
    {                                                                            \
        std::string text = "";                                                   \
        std::vector<std::shared_ptr<Tag>> children;                              \
                                                                                 \
        if (!args.empty())                                                       \
        {                                                                        \
            size_t i = 0;                                                        \
                                                                                 \
            if (std::holds_alternative<std::string>(args[0]))                    \
            {                                                                    \
                text = std::get<std::string>(args[0]);                           \
                i = 1;                                                           \
            }                                                                    \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))      \
            {                                                                    \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));     \
                i = 1;                                                           \
            }                                                                    \
            else                                                                 \
            {                                                                    \
                throw std::runtime_error("article() expects string or tag");     \
            }                                                                    \
                                                                                 \
            for (; i < args.size(); ++i)                                         \
            {                                                                    \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))      \
                    throw std::runtime_error("article() expects tag arguments"); \
                                                                                 \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));     \
            }                                                                    \
        }                                                                        \
                                                                                 \
        auto tag = html_builder.article(text);                                   \
                                                                                 \
        for (auto &c : children)                                                 \
            tag->child(c);                                                       \
                                                                                 \
        push(std::shared_ptr<Tag>(std::move(tag)));                              \
        break;                                                                   \
    }

#define NATIVE_HTML_ASIDE                                                      \
    case NativeMethod::HTML_ASIDE:                                             \
    {                                                                          \
        std::string text = "";                                                 \
        std::vector<std::shared_ptr<Tag>> children;                            \
                                                                               \
        if (!args.empty())                                                     \
        {                                                                      \
            size_t i = 0;                                                      \
                                                                               \
            if (std::holds_alternative<std::string>(args[0]))                  \
            {                                                                  \
                text = std::get<std::string>(args[0]);                         \
                i = 1;                                                         \
            }                                                                  \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))    \
            {                                                                  \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));   \
                i = 1;                                                         \
            }                                                                  \
            else                                                               \
            {                                                                  \
                throw std::runtime_error("aside() expects string or tag");     \
            }                                                                  \
                                                                               \
            for (; i < args.size(); ++i)                                       \
            {                                                                  \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))    \
                    throw std::runtime_error("aside() expects tag arguments"); \
                                                                               \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));   \
            }                                                                  \
        }                                                                      \
                                                                               \
        auto tag = html_builder.aside(text);                                   \
                                                                               \
        for (auto &c : children)                                               \
            tag->child(c);                                                     \
                                                                               \
        push(std::shared_ptr<Tag>(std::move(tag)));                            \
        break;                                                                 \
    }

#define NATIVE_HTML_FOOTER                                                      \
    case NativeMethod::HTML_FOOTER:                                             \
    {                                                                           \
        std::string text = "";                                                  \
        std::vector<std::shared_ptr<Tag>> children;                             \
                                                                                \
        if (!args.empty())                                                      \
        {                                                                       \
            size_t i = 0;                                                       \
                                                                                \
            if (std::holds_alternative<std::string>(args[0]))                   \
            {                                                                   \
                text = std::get<std::string>(args[0]);                          \
                i = 1;                                                          \
            }                                                                   \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))     \
            {                                                                   \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));    \
                i = 1;                                                          \
            }                                                                   \
            else                                                                \
            {                                                                   \
                throw std::runtime_error("footer() expects string or tag");     \
            }                                                                   \
                                                                                \
            for (; i < args.size(); ++i)                                        \
            {                                                                   \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))     \
                    throw std::runtime_error("footer() expects tag arguments"); \
                                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));    \
            }                                                                   \
        }                                                                       \
                                                                                \
        auto tag = html_builder.footer(text);                                   \
                                                                                \
        for (auto &c : children)                                                \
            tag->child(c);                                                      \
                                                                                \
        push(std::shared_ptr<Tag>(std::move(tag)));                             \
        break;                                                                  \
    }

#define NATIVE_HTML_HEADER                                                      \
    case NativeMethod::HTML_HEADER:                                             \
    {                                                                           \
        std::string text = "";                                                  \
        std::vector<std::shared_ptr<Tag>> children;                             \
                                                                                \
        if (!args.empty())                                                      \
        {                                                                       \
            size_t i = 0;                                                       \
                                                                                \
            if (std::holds_alternative<std::string>(args[0]))                   \
            {                                                                   \
                text = std::get<std::string>(args[0]);                          \
                i = 1;                                                          \
            }                                                                   \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))     \
            {                                                                   \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));    \
                i = 1;                                                          \
            }                                                                   \
            else                                                                \
            {                                                                   \
                throw std::runtime_error("header() expects string or tag");     \
            }                                                                   \
                                                                                \
            for (; i < args.size(); ++i)                                        \
            {                                                                   \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))     \
                    throw std::runtime_error("header() expects tag arguments"); \
                                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));    \
            }                                                                   \
        }                                                                       \
                                                                                \
        auto tag = html_builder.header(text);                                   \
                                                                                \
        for (auto &c : children)                                                \
            tag->child(c);                                                      \
                                                                                \
        push(std::shared_ptr<Tag>(std::move(tag)));                             \
        break;                                                                  \
    }

#define NATIVE_HTML_H1                                                       \
    case NativeMethod::HTML_H1:                                              \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
            {                                                                \
                throw std::runtime_error("h1() expects string or tag");      \
            }                                                                \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("h1() expects tag arguments");  \
                                                                             \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.h1(text);                                    \
                                                                             \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
                                                                             \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_H2                                                       \
    case NativeMethod::HTML_H2:                                              \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
            {                                                                \
                throw std::runtime_error("h2() expects string or tag");      \
            }                                                                \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("h2() expects tag arguments");  \
                                                                             \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.h2(text);                                    \
                                                                             \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
                                                                             \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_H3                                                       \
    case NativeMethod::HTML_H3:                                              \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
            {                                                                \
                throw std::runtime_error("h3() expects string or tag");      \
            }                                                                \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("h3() expects tag arguments");  \
                                                                             \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.h3(text);                                    \
                                                                             \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
                                                                             \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_H4                                                       \
    case NativeMethod::HTML_H4:                                              \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
            {                                                                \
                throw std::runtime_error("h4() expects string or tag");      \
            }                                                                \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("h4() expects tag arguments");  \
                                                                             \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.h4(text);                                    \
                                                                             \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
                                                                             \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_H5                                                       \
    case NativeMethod::HTML_H5:                                              \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
            {                                                                \
                throw std::runtime_error("h5() expects string or tag");      \
            }                                                                \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("h5() expects tag arguments");  \
                                                                             \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.h5(text);                                    \
                                                                             \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
                                                                             \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_H6                                                       \
    case NativeMethod::HTML_H6:                                              \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
            {                                                                \
                throw std::runtime_error("h6() expects string or tag");      \
            }                                                                \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("h6() expects tag arguments");  \
                                                                             \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.h6(text);                                    \
                                                                             \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
                                                                             \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_MAIN                                                      \
    case NativeMethod::HTML_MAIN:                                             \
    {                                                                         \
        std::string text = "";                                                \
        std::vector<std::shared_ptr<Tag>> children;                           \
                                                                              \
        if (!args.empty())                                                    \
        {                                                                     \
            size_t i = 0;                                                     \
                                                                              \
            if (std::holds_alternative<std::string>(args[0]))                 \
            {                                                                 \
                text = std::get<std::string>(args[0]);                        \
                i = 1;                                                        \
            }                                                                 \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))   \
            {                                                                 \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));  \
                i = 1;                                                        \
            }                                                                 \
            else                                                              \
            {                                                                 \
                throw std::runtime_error("main() expects string or tag");     \
            }                                                                 \
                                                                              \
            for (; i < args.size(); ++i)                                      \
            {                                                                 \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))   \
                    throw std::runtime_error("main() expects tag arguments"); \
                                                                              \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));  \
            }                                                                 \
        }                                                                     \
                                                                              \
        auto tag = html_builder.main(text);                                   \
        for (auto &c : children)                                              \
            tag->child(c);                                                    \
        push(std::shared_ptr<Tag>(std::move(tag)));                           \
        break;                                                                \
    }

#define NATIVE_HTML_NAV                                                      \
    case NativeMethod::HTML_NAV:                                             \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
            {                                                                \
                throw std::runtime_error("nav() expects string or tag");     \
            }                                                                \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("nav() expects tag arguments"); \
                                                                             \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.nav(text);                                   \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_SECTION                                                      \
    case NativeMethod::HTML_SECTION:                                             \
    {                                                                            \
        std::string text = "";                                                   \
        std::vector<std::shared_ptr<Tag>> children;                              \
                                                                                 \
        if (!args.empty())                                                       \
        {                                                                        \
            size_t i = 0;                                                        \
                                                                                 \
            if (std::holds_alternative<std::string>(args[0]))                    \
            {                                                                    \
                text = std::get<std::string>(args[0]);                           \
                i = 1;                                                           \
            }                                                                    \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))      \
            {                                                                    \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));     \
                i = 1;                                                           \
            }                                                                    \
            else                                                                 \
            {                                                                    \
                throw std::runtime_error("section() expects string or tag");     \
            }                                                                    \
                                                                                 \
            for (; i < args.size(); ++i)                                         \
            {                                                                    \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))      \
                    throw std::runtime_error("section() expects tag arguments"); \
                                                                                 \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));     \
            }                                                                    \
        }                                                                        \
                                                                                 \
        auto tag = html_builder.section(text);                                   \
        for (auto &c : children)                                                 \
            tag->child(c);                                                       \
        push(std::shared_ptr<Tag>(std::move(tag)));                              \
        break;                                                                   \
    }

#define NATIVE_HTML_DIV                                                      \
    case NativeMethod::HTML_DIV:                                             \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
            {                                                                \
                throw std::runtime_error("div() expects string or tag");     \
            }                                                                \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("div() expects tag arguments"); \
                                                                             \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.div(text);                                   \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_P                                                        \
    case NativeMethod::HTML_P:                                               \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
            {                                                                \
                throw std::runtime_error("p() expects string or tag");       \
            }                                                                \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("p() expects tag arguments");   \
                                                                             \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.p(text);                                     \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_UL                                                       \
    case NativeMethod::HTML_UL:                                              \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
            {                                                                \
                throw std::runtime_error("ul() expects string or tag");      \
            }                                                                \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("ul() expects tag arguments");  \
                                                                             \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.ul(text);                                    \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_OL                                                       \
    case NativeMethod::HTML_OL:                                              \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
            {                                                                \
                throw std::runtime_error("ol() expects string or tag");      \
            }                                                                \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("ol() expects tag arguments");  \
                                                                             \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.ol(text);                                    \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_LI                                                       \
    case NativeMethod::HTML_LI:                                              \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
            {                                                                \
                throw std::runtime_error("li() expects string or tag");      \
            }                                                                \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("li() expects tag arguments");  \
                                                                             \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.li(text);                                    \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_DL                                                       \
    case NativeMethod::HTML_DL:                                              \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
            {                                                                \
                throw std::runtime_error("dl() expects string or tag");      \
            }                                                                \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("dl() expects tag arguments");  \
                                                                             \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.dl(text);                                    \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_DT                                                       \
    case NativeMethod::HTML_DT:                                              \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
            {                                                                \
                throw std::runtime_error("dt() expects string or tag");      \
            }                                                                \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("dt() expects tag arguments");  \
                                                                             \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.dt(text);                                    \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_DD                                                       \
    case NativeMethod::HTML_DD:                                              \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
            {                                                                \
                throw std::runtime_error("dd() expects string or tag");      \
            }                                                                \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("dd() expects tag arguments");  \
                                                                             \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.dd(text);                                    \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_FIGURE                                                      \
    case NativeMethod::HTML_FIGURE:                                             \
    {                                                                           \
        std::string text = "";                                                  \
        std::vector<std::shared_ptr<Tag>> children;                             \
                                                                                \
        if (!args.empty())                                                      \
        {                                                                       \
            size_t i = 0;                                                       \
                                                                                \
            if (std::holds_alternative<std::string>(args[0]))                   \
            {                                                                   \
                text = std::get<std::string>(args[0]);                          \
                i = 1;                                                          \
            }                                                                   \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))     \
            {                                                                   \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));    \
                i = 1;                                                          \
            }                                                                   \
            else                                                                \
            {                                                                   \
                throw std::runtime_error("figure() expects string or tag");     \
            }                                                                   \
                                                                                \
            for (; i < args.size(); ++i)                                        \
            {                                                                   \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))     \
                    throw std::runtime_error("figure() expects tag arguments"); \
                                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));    \
            }                                                                   \
        }                                                                       \
                                                                                \
        auto tag = html_builder.figure(text);                                   \
        for (auto &c : children)                                                \
            tag->child(c);                                                      \
        push(std::shared_ptr<Tag>(std::move(tag)));                             \
        break;                                                                  \
    }

#define NATIVE_HTML_FIGCAPTION                                                      \
    case NativeMethod::HTML_FIGCAPTION:                                             \
    {                                                                               \
        std::string text = "";                                                      \
        std::vector<std::shared_ptr<Tag>> children;                                 \
                                                                                    \
        if (!args.empty())                                                          \
        {                                                                           \
            size_t i = 0;                                                           \
                                                                                    \
            if (std::holds_alternative<std::string>(args[0]))                       \
            {                                                                       \
                text = std::get<std::string>(args[0]);                              \
                i = 1;                                                              \
            }                                                                       \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))         \
            {                                                                       \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));        \
                i = 1;                                                              \
            }                                                                       \
            else                                                                    \
            {                                                                       \
                throw std::runtime_error("figcaption() expects string or tag");     \
            }                                                                       \
                                                                                    \
            for (; i < args.size(); ++i)                                            \
            {                                                                       \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))         \
                    throw std::runtime_error("figcaption() expects tag arguments"); \
                                                                                    \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));        \
            }                                                                       \
        }                                                                           \
                                                                                    \
        auto tag = html_builder.figcaption(text);                                   \
        for (auto &c : children)                                                    \
            tag->child(c);                                                          \
        push(std::shared_ptr<Tag>(std::move(tag)));                                 \
        break;                                                                      \
    }

#define NATIVE_HTML_BLOCKQUOTE                                                  \
    case NativeMethod::HTML_BLOCKQUOTE:                                         \
    {                                                                           \
        std::string text = "";                                                  \
        std::string cite = "";                                                  \
        std::vector<std::shared_ptr<Tag>> children;                             \
                                                                                \
        size_t i = 0;                                                           \
                                                                                \
        if (args.size() > 0 && std::holds_alternative<std::string>(args[0]))    \
        {                                                                       \
            text = std::get<std::string>(args[0]);                              \
            i = 1;                                                              \
        }                                                                       \
                                                                                \
        if (args.size() > 1 && std::holds_alternative<std::string>(args[1]))    \
        {                                                                       \
            cite = std::get<std::string>(args[1]);                              \
            i = 2;                                                              \
        }                                                                       \
                                                                                \
        for (; i < args.size(); ++i)                                            \
        {                                                                       \
            if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))         \
                throw std::runtime_error("blockquote() expects tag arguments"); \
                                                                                \
            children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));        \
        }                                                                       \
                                                                                \
        auto tag = html_builder.blockquote(text, cite);                         \
                                                                                \
        for (auto &c : children)                                                \
            tag->child(c);                                                      \
                                                                                \
        push(std::shared_ptr<Tag>(std::move(tag)));                             \
        break;                                                                  \
    }

#define NATIVE_HTML_PRE                                                      \
    case NativeMethod::HTML_PRE:                                             \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
            {                                                                \
                throw std::runtime_error("pre() expects string or tag");     \
            }                                                                \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("pre() expects tag arguments"); \
                                                                             \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.pre(text);                                   \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_HR                              \
    case NativeMethod::HTML_HR:                     \
    {                                               \
        auto tag = html_builder.hr();               \
        push(std::shared_ptr<Tag>(std::move(tag))); \
        break;                                      \
    }

#define NATIVE_HTML_BR                              \
    case NativeMethod::HTML_BR:                     \
    {                                               \
        auto tag = html_builder.br();               \
        push(std::shared_ptr<Tag>(std::move(tag))); \
        break;                                      \
    }

#define NATIVE_HTML_WBR                             \
    case NativeMethod::HTML_WBR:                    \
    {                                               \
        auto tag = html_builder.wbr();              \
        push(std::shared_ptr<Tag>(std::move(tag))); \
        break;                                      \
    }

#define NATIVE_HTML_A                                                                      \
    case NativeMethod::HTML_A:                                                             \
    {                                                                                      \
        std::string text = "";                                                             \
        std::string href = "#";                                                            \
        std::vector<std::shared_ptr<Tag>> children;                                        \
                                                                                           \
        size_t i = 0;                                                                      \
                                                                                           \
        if (args.size() > 0 && std::holds_alternative<std::string>(args[0]))               \
        {                                                                                  \
            text = std::get<std::string>(args[0]);                                         \
            i = 1;                                                                         \
        }                                                                                  \
        else if (args.size() > 0 && std::holds_alternative<std::shared_ptr<Tag>>(args[0])) \
        {                                                                                  \
            children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));                   \
            i = 1;                                                                         \
        }                                                                                  \
        else if (args.size() > 0)                                                          \
        {                                                                                  \
            throw std::runtime_error("a() expects string or tag");                         \
        }                                                                                  \
                                                                                           \
        if (args.size() > i && std::holds_alternative<std::string>(args[i]))               \
        {                                                                                  \
            href = std::get<std::string>(args[i]);                                         \
            ++i;                                                                           \
        }                                                                                  \
                                                                                           \
        for (; i < args.size(); ++i)                                                       \
        {                                                                                  \
            if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))                    \
                throw std::runtime_error("a() expects tag arguments");                     \
                                                                                           \
            children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));                   \
        }                                                                                  \
                                                                                           \
        auto tag = html_builder.a(text, href);                                             \
                                                                                           \
        for (auto &c : children)                                                           \
            tag->child(c);                                                                 \
                                                                                           \
        push(std::shared_ptr<Tag>(std::move(tag)));                                        \
        break;                                                                             \
    }

#define NATIVE_HTML_SPAN                                                      \
    case NativeMethod::HTML_SPAN:                                             \
    {                                                                         \
        std::string text = "";                                                \
        std::vector<std::shared_ptr<Tag>> children;                           \
                                                                              \
        if (!args.empty())                                                    \
        {                                                                     \
            size_t i = 0;                                                     \
                                                                              \
            if (std::holds_alternative<std::string>(args[0]))                 \
            {                                                                 \
                text = std::get<std::string>(args[0]);                        \
                i = 1;                                                        \
            }                                                                 \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))   \
            {                                                                 \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));  \
                i = 1;                                                        \
            }                                                                 \
            else                                                              \
            {                                                                 \
                throw std::runtime_error("span() expects string or tag");     \
            }                                                                 \
                                                                              \
            for (; i < args.size(); ++i)                                      \
            {                                                                 \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))   \
                    throw std::runtime_error("span() expects tag arguments"); \
                                                                              \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));  \
            }                                                                 \
        }                                                                     \
                                                                              \
        auto tag = html_builder.span(text);                                   \
        for (auto &c : children)                                              \
            tag->child(c);                                                    \
        push(std::shared_ptr<Tag>(std::move(tag)));                           \
        break;                                                                \
    }

#define NATIVE_HTML_STRONG                                                      \
    case NativeMethod::HTML_STRONG:                                             \
    {                                                                           \
        std::string text = "";                                                  \
        std::vector<std::shared_ptr<Tag>> children;                             \
                                                                                \
        if (!args.empty())                                                      \
        {                                                                       \
            size_t i = 0;                                                       \
                                                                                \
            if (std::holds_alternative<std::string>(args[0]))                   \
            {                                                                   \
                text = std::get<std::string>(args[0]);                          \
                i = 1;                                                          \
            }                                                                   \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))     \
            {                                                                   \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));    \
                i = 1;                                                          \
            }                                                                   \
            else                                                                \
            {                                                                   \
                throw std::runtime_error("strong() expects string or tag");     \
            }                                                                   \
                                                                                \
            for (; i < args.size(); ++i)                                        \
            {                                                                   \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))     \
                    throw std::runtime_error("strong() expects tag arguments"); \
                                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));    \
            }                                                                   \
        }                                                                       \
                                                                                \
        auto tag = html_builder.strong(text);                                   \
        for (auto &c : children)                                                \
            tag->child(c);                                                      \
        push(std::shared_ptr<Tag>(std::move(tag)));                             \
        break;                                                                  \
    }
#define NATIVE_HTML_EM                                                       \
    case NativeMethod::HTML_EM:                                              \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
            {                                                                \
                throw std::runtime_error("em() expects string or tag");      \
            }                                                                \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("em() expects tag arguments");  \
                                                                             \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.em(text);                                    \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_TABLE                                                      \
    case NativeMethod::HTML_TABLE:                                             \
    {                                                                          \
        std::string text = "";                                                 \
        std::vector<std::shared_ptr<Tag>> children;                            \
                                                                               \
        if (!args.empty())                                                     \
        {                                                                      \
            size_t i = 0;                                                      \
                                                                               \
            if (std::holds_alternative<std::string>(args[0]))                  \
            {                                                                  \
                text = std::get<std::string>(args[0]);                         \
                i = 1;                                                         \
            }                                                                  \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))    \
            {                                                                  \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));   \
                i = 1;                                                         \
            }                                                                  \
            else                                                               \
            {                                                                  \
                throw std::runtime_error("table() expects string or tag");     \
            }                                                                  \
                                                                               \
            for (; i < args.size(); ++i)                                       \
            {                                                                  \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))    \
                    throw std::runtime_error("table() expects tag arguments"); \
                                                                               \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));   \
            }                                                                  \
        }                                                                      \
                                                                               \
        auto tag = html_builder.table(text);                                   \
        for (auto &c : children)                                               \
            tag->child(c);                                                     \
        push(std::shared_ptr<Tag>(std::move(tag)));                            \
        break;                                                                 \
    }

#define NATIVE_HTML_THEAD                                                      \
    case NativeMethod::HTML_THEAD:                                             \
    {                                                                          \
        std::string text = "";                                                 \
        std::vector<std::shared_ptr<Tag>> children;                            \
                                                                               \
        if (!args.empty())                                                     \
        {                                                                      \
            size_t i = 0;                                                      \
            if (std::holds_alternative<std::string>(args[0]))                  \
            {                                                                  \
                text = std::get<std::string>(args[0]);                         \
                i = 1;                                                         \
            }                                                                  \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))    \
            {                                                                  \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));   \
                i = 1;                                                         \
            }                                                                  \
            else                                                               \
                throw std::runtime_error("thead() expects string or tag");     \
                                                                               \
            for (; i < args.size(); ++i)                                       \
            {                                                                  \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))    \
                    throw std::runtime_error("thead() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));   \
            }                                                                  \
        }                                                                      \
        auto tag = html_builder.thead(text);                                   \
        for (auto &c : children)                                               \
            tag->child(c);                                                     \
        push(std::shared_ptr<Tag>(std::move(tag)));                            \
        break;                                                                 \
    }

#define NATIVE_HTML_TBODY                                                      \
    case NativeMethod::HTML_TBODY:                                             \
    {                                                                          \
        std::string text = "";                                                 \
        std::vector<std::shared_ptr<Tag>> children;                            \
                                                                               \
        if (!args.empty())                                                     \
        {                                                                      \
            size_t i = 0;                                                      \
            if (std::holds_alternative<std::string>(args[0]))                  \
            {                                                                  \
                text = std::get<std::string>(args[0]);                         \
                i = 1;                                                         \
            }                                                                  \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))    \
            {                                                                  \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));   \
                i = 1;                                                         \
            }                                                                  \
            else                                                               \
                throw std::runtime_error("tbody() expects string or tag");     \
                                                                               \
            for (; i < args.size(); ++i)                                       \
            {                                                                  \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))    \
                    throw std::runtime_error("tbody() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));   \
            }                                                                  \
        }                                                                      \
        auto tag = html_builder.tbody(text);                                   \
        for (auto &c : children)                                               \
            tag->child(c);                                                     \
        push(std::shared_ptr<Tag>(std::move(tag)));                            \
        break;                                                                 \
    }

#define NATIVE_HTML_TFOOT                                                      \
    case NativeMethod::HTML_TFOOT:                                             \
    {                                                                          \
        std::string text = "";                                                 \
        std::vector<std::shared_ptr<Tag>> children;                            \
                                                                               \
        if (!args.empty())                                                     \
        {                                                                      \
            size_t i = 0;                                                      \
            if (std::holds_alternative<std::string>(args[0]))                  \
            {                                                                  \
                text = std::get<std::string>(args[0]);                         \
                i = 1;                                                         \
            }                                                                  \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))    \
            {                                                                  \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));   \
                i = 1;                                                         \
            }                                                                  \
            else                                                               \
                throw std::runtime_error("tfoot() expects string or tag");     \
                                                                               \
            for (; i < args.size(); ++i)                                       \
            {                                                                  \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))    \
                    throw std::runtime_error("tfoot() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));   \
            }                                                                  \
        }                                                                      \
        auto tag = html_builder.tfoot(text);                                   \
        for (auto &c : children)                                               \
            tag->child(c);                                                     \
        push(std::shared_ptr<Tag>(std::move(tag)));                            \
        break;                                                                 \
    }

#define NATIVE_HTML_TR                                                       \
    case NativeMethod::HTML_TR:                                              \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
        size_t i = 0;                                                        \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
                throw std::runtime_error("tr() expects string or tag");      \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("tr() expects tag arguments");  \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.tr(text);                                    \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_TH                                                       \
    case NativeMethod::HTML_TH:                                              \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
        size_t i = 0;                                                        \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
                throw std::runtime_error("tr() expects string or tag");      \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("tr() expects tag arguments");  \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.th(text);                                    \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_TD                                                       \
    case NativeMethod::HTML_TD:                                              \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
        size_t i = 0;                                                        \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
                throw std::runtime_error("tr() expects string or tag");      \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("tr() expects tag arguments");  \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.td(text);                                    \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_IMG                                                   \
    case NativeMethod::HTML_IMG:                                          \
    {                                                                     \
        std::string alt = "";                                             \
        std::string src = "";                                             \
        std::string srcset = "";                                          \
                                                                          \
        if (args.size() > 0)                                              \
        {                                                                 \
            if (!std::holds_alternative<std::string>(args[0]))            \
                throw std::runtime_error("img() expects string alt");     \
            alt = std::get<std::string>(args[0]);                         \
        }                                                                 \
                                                                          \
        if (args.size() > 1)                                              \
        {                                                                 \
            if (!std::holds_alternative<std::string>(args[1]))            \
                throw std::runtime_error("img() expects string src");     \
            src = std::get<std::string>(args[1]);                         \
        }                                                                 \
                                                                          \
        if (args.size() > 2)                                              \
        {                                                                 \
            if (!std::holds_alternative<std::string>(args[2]))            \
                throw std::runtime_error("img() expects string srcset");  \
            srcset = std::get<std::string>(args[2]);                      \
        }                                                                 \
                                                                          \
        if (args.size() > 3)                                              \
            throw std::runtime_error("img() does not accept child tags"); \
                                                                          \
        auto tag = html_builder.img(alt, src, srcset);                    \
        push(std::shared_ptr<Tag>(std::move(tag)));                       \
        break;                                                            \
    }

#define NATIVE_HTML_VIDEO                                                      \
    case NativeMethod::HTML_VIDEO:                                             \
    {                                                                          \
        std::string text = "";                                                 \
        std::vector<std::shared_ptr<Tag>> children;                            \
                                                                               \
        if (!args.empty())                                                     \
        {                                                                      \
            size_t i = 0;                                                      \
                                                                               \
            if (std::holds_alternative<std::string>(args[0]))                  \
            {                                                                  \
                text = std::get<std::string>(args[0]);                         \
                i = 1;                                                         \
            }                                                                  \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))    \
            {                                                                  \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));   \
                i = 1;                                                         \
            }                                                                  \
            else                                                               \
            {                                                                  \
                throw std::runtime_error("video() expects string or tag");     \
            }                                                                  \
                                                                               \
            for (; i < args.size(); ++i)                                       \
            {                                                                  \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))    \
                    throw std::runtime_error("video() expects tag arguments"); \
                                                                               \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));   \
            }                                                                  \
        }                                                                      \
                                                                               \
        auto tag = html_builder.video(text);                                   \
        for (auto &c : children)                                               \
            tag->child(c);                                                     \
        push(std::shared_ptr<Tag>(std::move(tag)));                            \
        break;                                                                 \
    }

#define NATIVE_HTML_AUDIO                                                      \
    case NativeMethod::HTML_AUDIO:                                             \
    {                                                                          \
        std::string text = "";                                                 \
        std::vector<std::shared_ptr<Tag>> children;                            \
                                                                               \
        if (!args.empty())                                                     \
        {                                                                      \
            size_t i = 0;                                                      \
                                                                               \
            if (std::holds_alternative<std::string>(args[0]))                  \
            {                                                                  \
                text = std::get<std::string>(args[0]);                         \
                i = 1;                                                         \
            }                                                                  \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))    \
            {                                                                  \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));   \
                i = 1;                                                         \
            }                                                                  \
            else                                                               \
            {                                                                  \
                throw std::runtime_error("audio() expects string or tag");     \
            }                                                                  \
                                                                               \
            for (; i < args.size(); ++i)                                       \
            {                                                                  \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))    \
                    throw std::runtime_error("audio() expects tag arguments"); \
                                                                               \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));   \
            }                                                                  \
        }                                                                      \
                                                                               \
        auto tag = html_builder.audio(text);                                   \
        for (auto &c : children)                                               \
            tag->child(c);                                                     \
        push(std::shared_ptr<Tag>(std::move(tag)));                            \
        break;                                                                 \
    }

#define NATIVE_HTML_IFRAME                                                      \
    case NativeMethod::HTML_IFRAME:                                             \
    {                                                                           \
        std::string text = "";                                                  \
        std::vector<std::shared_ptr<Tag>> children;                             \
                                                                                \
        if (!args.empty())                                                      \
        {                                                                       \
            size_t i = 0;                                                       \
                                                                                \
            if (std::holds_alternative<std::string>(args[0]))                   \
            {                                                                   \
                text = std::get<std::string>(args[0]);                          \
                i = 1;                                                          \
            }                                                                   \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))     \
            {                                                                   \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));    \
                i = 1;                                                          \
            }                                                                   \
            else                                                                \
            {                                                                   \
                throw std::runtime_error("iframe() expects string or tag");     \
            }                                                                   \
                                                                                \
            for (; i < args.size(); ++i)                                        \
            {                                                                   \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))     \
                    throw std::runtime_error("iframe() expects tag arguments"); \
                                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));    \
            }                                                                   \
        }                                                                       \
                                                                                \
        auto tag = html_builder.iframe(text);                                   \
        for (auto &c : children)                                                \
            tag->child(c);                                                      \
        push(std::shared_ptr<Tag>(std::move(tag)));                             \
        break;                                                                  \
    }

#define NATIVE_HTML_CANVAS                                                      \
    case NativeMethod::HTML_CANVAS:                                             \
    {                                                                           \
        std::string text = "";                                                  \
        std::vector<std::shared_ptr<Tag>> children;                             \
                                                                                \
        if (!args.empty())                                                      \
        {                                                                       \
            size_t i = 0;                                                       \
                                                                                \
            if (std::holds_alternative<std::string>(args[0]))                   \
            {                                                                   \
                text = std::get<std::string>(args[0]);                          \
                i = 1;                                                          \
            }                                                                   \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))     \
            {                                                                   \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));    \
                i = 1;                                                          \
            }                                                                   \
            else                                                                \
                throw std::runtime_error("canvas() expects string or tag");     \
                                                                                \
            for (; i < args.size(); ++i)                                        \
            {                                                                   \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))     \
                    throw std::runtime_error("canvas() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));    \
            }                                                                   \
        }                                                                       \
                                                                                \
        auto tag = html_builder.canvas(text);                                   \
        for (auto &c : children)                                                \
            tag->child(c);                                                      \
        push(std::shared_ptr<Tag>(std::move(tag)));                             \
        break;                                                                  \
    }

#define NATIVE_HTML_FORM                                                      \
    case NativeMethod::HTML_FORM:                                             \
    {                                                                         \
        std::string text = "";                                                \
        std::vector<std::shared_ptr<Tag>> children;                           \
                                                                              \
        if (!args.empty())                                                    \
        {                                                                     \
            size_t i = 0;                                                     \
                                                                              \
            if (std::holds_alternative<std::string>(args[0]))                 \
            {                                                                 \
                text = std::get<std::string>(args[0]);                        \
                i = 1;                                                        \
            }                                                                 \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))   \
            {                                                                 \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));  \
                i = 1;                                                        \
            }                                                                 \
            else                                                              \
                throw std::runtime_error("form() expects string or tag");     \
                                                                              \
            for (; i < args.size(); ++i)                                      \
            {                                                                 \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))   \
                    throw std::runtime_error("form() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));  \
            }                                                                 \
        }                                                                     \
                                                                              \
        auto tag = html_builder.form(text);                                   \
        for (auto &c : children)                                              \
            tag->child(c);                                                    \
        push(std::shared_ptr<Tag>(std::move(tag)));                           \
        break;                                                                \
    }

#define NATIVE_HTML_INPUT                                                   \
    case NativeMethod::HTML_INPUT:                                          \
    {                                                                       \
        std::string value = "";                                             \
        std::string placeholder = "";                                       \
                                                                            \
        if (args.size() > 0)                                                \
        {                                                                   \
            if (!std::holds_alternative<std::string>(args[0]))              \
                throw std::runtime_error("input() expects string value");   \
            value = std::get<std::string>(args[0]);                         \
        }                                                                   \
                                                                            \
        if (args.size() > 1)                                                \
        {                                                                   \
            if (!std::holds_alternative<std::string>(args[1]))              \
                throw std::runtime_error("input() expects placeholder");    \
            placeholder = std::get<std::string>(args[1]);                   \
        }                                                                   \
                                                                            \
        if (args.size() > 2)                                                \
            throw std::runtime_error("input() does not accept child tags"); \
                                                                            \
        auto tag = html_builder.input(value, placeholder);                  \
        push(std::shared_ptr<Tag>(std::move(tag)));                         \
        break;                                                              \
    }

#define NATIVE_HTML_LABEL                                                      \
    case NativeMethod::HTML_LABEL:                                             \
    {                                                                          \
        std::string text = "";                                                 \
        std::vector<std::shared_ptr<Tag>> children;                            \
                                                                               \
        if (!args.empty())                                                     \
        {                                                                      \
            size_t i = 0;                                                      \
                                                                               \
            if (std::holds_alternative<std::string>(args[0]))                  \
            {                                                                  \
                text = std::get<std::string>(args[0]);                         \
                i = 1;                                                         \
            }                                                                  \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))    \
            {                                                                  \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));   \
                i = 1;                                                         \
            }                                                                  \
            else                                                               \
                throw std::runtime_error("label() expects string or tag");     \
                                                                               \
            for (; i < args.size(); ++i)                                       \
            {                                                                  \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))    \
                    throw std::runtime_error("label() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));   \
            }                                                                  \
        }                                                                      \
                                                                               \
        auto tag = html_builder.label(text);                                   \
        for (auto &c : children)                                               \
            tag->child(c);                                                     \
        push(std::shared_ptr<Tag>(std::move(tag)));                            \
        break;                                                                 \
    }

#define NATIVE_HTML_TEXTAREA                                                      \
    case NativeMethod::HTML_TEXTAREA:                                             \
    {                                                                             \
        std::string text = "";                                                    \
        std::vector<std::shared_ptr<Tag>> children;                               \
                                                                                  \
        if (!args.empty())                                                        \
        {                                                                         \
            size_t i = 0;                                                         \
                                                                                  \
            if (std::holds_alternative<std::string>(args[0]))                     \
            {                                                                     \
                text = std::get<std::string>(args[0]);                            \
                i = 1;                                                            \
            }                                                                     \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))       \
            {                                                                     \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));      \
                i = 1;                                                            \
            }                                                                     \
            else                                                                  \
                throw std::runtime_error("textarea() expects string or tag");     \
                                                                                  \
            for (; i < args.size(); ++i)                                          \
            {                                                                     \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))       \
                    throw std::runtime_error("textarea() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));      \
            }                                                                     \
        }                                                                         \
                                                                                  \
        auto tag = html_builder.textarea(text);                                   \
        for (auto &c : children)                                                  \
            tag->child(c);                                                        \
        push(std::shared_ptr<Tag>(std::move(tag)));                               \
        break;                                                                    \
    }

#define NATIVE_HTML_BUTTON                                                      \
    case NativeMethod::HTML_BUTTON:                                             \
    {                                                                           \
        std::string text = "";                                                  \
        std::vector<std::shared_ptr<Tag>> children;                             \
                                                                                \
        if (!args.empty())                                                      \
        {                                                                       \
            size_t i = 0;                                                       \
                                                                                \
            if (std::holds_alternative<std::string>(args[0]))                   \
            {                                                                   \
                text = std::get<std::string>(args[0]);                          \
                i = 1;                                                          \
            }                                                                   \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))     \
            {                                                                   \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));    \
                i = 1;                                                          \
            }                                                                   \
            else                                                                \
                throw std::runtime_error("button() expects string or tag");     \
                                                                                \
            for (; i < args.size(); ++i)                                        \
            {                                                                   \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))     \
                    throw std::runtime_error("button() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));    \
            }                                                                   \
        }                                                                       \
                                                                                \
        auto tag = html_builder.button(text);                                   \
        for (auto &c : children)                                                \
            tag->child(c);                                                      \
        push(std::shared_ptr<Tag>(std::move(tag)));                             \
        break;                                                                  \
    }

#define NATIVE_HTML_CITE                                                      \
    case NativeMethod::HTML_CITE:                                             \
    {                                                                         \
        std::string text = "";                                                \
        std::vector<std::shared_ptr<Tag>> children;                           \
                                                                              \
        if (!args.empty())                                                    \
        {                                                                     \
            size_t i = 0;                                                     \
                                                                              \
            if (std::holds_alternative<std::string>(args[0]))                 \
            {                                                                 \
                text = std::get<std::string>(args[0]);                        \
                i = 1;                                                        \
            }                                                                 \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))   \
            {                                                                 \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));  \
                i = 1;                                                        \
            }                                                                 \
            else                                                              \
                throw std::runtime_error("cite() expects string or tag");     \
                                                                              \
            for (; i < args.size(); ++i)                                      \
            {                                                                 \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))   \
                    throw std::runtime_error("cite() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));  \
            }                                                                 \
        }                                                                     \
                                                                              \
        auto tag = html_builder.cite(text);                                   \
        for (auto &c : children)                                              \
            tag->child(c);                                                    \
        push(std::shared_ptr<Tag>(std::move(tag)));                           \
        break;                                                                \
    }

#define NATIVE_HTML_INS                                                      \
    case NativeMethod::HTML_INS:                                             \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
                throw std::runtime_error("ins() expects string or tag");     \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("ins() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.ins(text);                                   \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_OUTPUT                                                      \
    case NativeMethod::HTML_OUTPUT:                                             \
    {                                                                           \
        std::string text = "";                                                  \
        std::vector<std::shared_ptr<Tag>> children;                             \
                                                                                \
        if (!args.empty())                                                      \
        {                                                                       \
            size_t i = 0;                                                       \
                                                                                \
            if (std::holds_alternative<std::string>(args[0]))                   \
            {                                                                   \
                text = std::get<std::string>(args[0]);                          \
                i = 1;                                                          \
            }                                                                   \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))     \
            {                                                                   \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));    \
                i = 1;                                                          \
            }                                                                   \
            else                                                                \
                throw std::runtime_error("output() expects string or tag");     \
                                                                                \
            for (; i < args.size(); ++i)                                        \
            {                                                                   \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))     \
                    throw std::runtime_error("output() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));    \
            }                                                                   \
        }                                                                       \
                                                                                \
        auto tag = html_builder.output(text);                                   \
        for (auto &c : children)                                                \
            tag->child(c);                                                      \
        push(std::shared_ptr<Tag>(std::move(tag)));                             \
        break;                                                                  \
    }

#define NATIVE_HTML_MAP                                                                \
    case NativeMethod::HTML_MAP:                                                       \
    {                                                                                  \
        std::string text = "";                                                         \
        std::string name = "";                                                         \
        std::vector<std::shared_ptr<Tag>> children;                                    \
                                                                                       \
        size_t i = 0;                                                                  \
                                                                                       \
        if (i < args.size() && std::holds_alternative<std::string>(args[i]))           \
        {                                                                              \
            text = std::get<std::string>(args[i++]);                                   \
        }                                                                              \
                                                                                       \
        if (i < args.size() && std::holds_alternative<std::string>(args[i]))           \
        {                                                                              \
            name = std::get<std::string>(args[i++]);                                   \
        }                                                                              \
                                                                                       \
        for (; i < args.size(); ++i)                                                   \
        {                                                                              \
            if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))                \
                throw std::runtime_error("map() expects tag arguments after strings"); \
                                                                                       \
            children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));               \
        }                                                                              \
                                                                                       \
        auto tag = html_builder.map(text, name);                                       \
        for (auto &c : children)                                                       \
            tag->child(c);                                                             \
        push(std::shared_ptr<Tag>(std::move(tag)));                                    \
        break;                                                                         \
    }

#define NATIVE_HTML_AREA                                                          \
    case NativeMethod::HTML_AREA:                                                 \
    {                                                                             \
        std::string alt = "";                                                     \
        std::string coords = "";                                                  \
        std::string download = "";                                                \
        std::string href = "";                                                    \
        std::string hreflang = "";                                                \
        std::string ping = "";                                                    \
        std::string referrerpolicy = "";                                          \
        std::string rel = "";                                                     \
        std::string shape = "";                                                   \
        std::string target = "";                                                  \
                                                                                  \
        for (size_t i = 0; i < args.size(); ++i)                                  \
        {                                                                         \
            if (!std::holds_alternative<std::string>(args[i]))                    \
                throw std::runtime_error("area() expects only string arguments"); \
        }                                                                         \
                                                                                  \
        if (args.size() > 0)                                                      \
            alt = std::get<std::string>(args[0]);                                 \
        if (args.size() > 1)                                                      \
            coords = std::get<std::string>(args[1]);                              \
        if (args.size() > 2)                                                      \
            download = std::get<std::string>(args[2]);                            \
        if (args.size() > 3)                                                      \
            href = std::get<std::string>(args[3]);                                \
        if (args.size() > 4)                                                      \
            hreflang = std::get<std::string>(args[4]);                            \
        if (args.size() > 5)                                                      \
            ping = std::get<std::string>(args[5]);                                \
        if (args.size() > 6)                                                      \
            referrerpolicy = std::get<std::string>(args[6]);                      \
        if (args.size() > 7)                                                      \
            rel = std::get<std::string>(args[7]);                                 \
        if (args.size() > 8)                                                      \
            shape = std::get<std::string>(args[8]);                               \
        if (args.size() > 9)                                                      \
            target = std::get<std::string>(args[9]);                              \
                                                                                  \
        if (args.size() > 10)                                                     \
            throw std::runtime_error("area() takes at most 10 arguments");        \
                                                                                  \
        auto tag = html_builder.area(alt, coords, download, href,                 \
                                     hreflang, ping, referrerpolicy,              \
                                     rel, shape, target);                         \
        push(std::shared_ptr<Tag>(std::move(tag)));                               \
        break;                                                                    \
    }

#define NATIVE_HTML_OPTION                                                      \
    case NativeMethod::HTML_OPTION:                                             \
    {                                                                           \
        std::string text = "";                                                  \
        std::vector<std::shared_ptr<Tag>> children;                             \
                                                                                \
        if (!args.empty())                                                      \
        {                                                                       \
            size_t i = 0;                                                       \
                                                                                \
            if (std::holds_alternative<std::string>(args[0]))                   \
            {                                                                   \
                text = std::get<std::string>(args[0]);                          \
                i = 1;                                                          \
            }                                                                   \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))     \
            {                                                                   \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));    \
                i = 1;                                                          \
            }                                                                   \
            else                                                                \
                throw std::runtime_error("option() expects string or tag");     \
                                                                                \
            for (; i < args.size(); ++i)                                        \
            {                                                                   \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))     \
                    throw std::runtime_error("option() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));    \
            }                                                                   \
        }                                                                       \
                                                                                \
        auto tag = html_builder.option(text);                                   \
        for (auto &c : children)                                                \
            tag->child(c);                                                      \
        push(std::shared_ptr<Tag>(std::move(tag)));                             \
        break;                                                                  \
    }

#define NATIVE_HTML_LEGEND                                                      \
    case NativeMethod::HTML_LEGEND:                                             \
    {                                                                           \
        std::string text = "";                                                  \
        std::vector<std::shared_ptr<Tag>> children;                             \
                                                                                \
        if (!args.empty())                                                      \
        {                                                                       \
            size_t i = 0;                                                       \
                                                                                \
            if (std::holds_alternative<std::string>(args[0]))                   \
            {                                                                   \
                text = std::get<std::string>(args[0]);                          \
                i = 1;                                                          \
            }                                                                   \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))     \
            {                                                                   \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));    \
                i = 1;                                                          \
            }                                                                   \
            else                                                                \
                throw std::runtime_error("legend() expects string or tag");     \
                                                                                \
            for (; i < args.size(); ++i)                                        \
            {                                                                   \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))     \
                    throw std::runtime_error("legend() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));    \
            }                                                                   \
        }                                                                       \
                                                                                \
        auto tag = html_builder.legend(text);                                   \
        for (auto &c : children)                                                \
            tag->child(c);                                                      \
        push(std::shared_ptr<Tag>(std::move(tag)));                             \
        break;                                                                  \
    }

#define NATIVE_HTML_TIME                                                      \
    case NativeMethod::HTML_TIME:                                             \
    {                                                                         \
        std::string text = "";                                                \
        std::vector<std::shared_ptr<Tag>> children;                           \
                                                                              \
        if (!args.empty())                                                    \
        {                                                                     \
            size_t i = 0;                                                     \
                                                                              \
            if (std::holds_alternative<std::string>(args[0]))                 \
            {                                                                 \
                text = std::get<std::string>(args[0]);                        \
                i = 1;                                                        \
            }                                                                 \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))   \
            {                                                                 \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));  \
                i = 1;                                                        \
            }                                                                 \
            else                                                              \
                throw std::runtime_error("time() expects string or tag");     \
                                                                              \
            for (; i < args.size(); ++i)                                      \
            {                                                                 \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))   \
                    throw std::runtime_error("time() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));  \
            }                                                                 \
        }                                                                     \
                                                                              \
        auto tag = html_builder.time(text);                                   \
        for (auto &c : children)                                              \
            tag->child(c);                                                    \
        push(std::shared_ptr<Tag>(std::move(tag)));                           \
        break;                                                                \
    }

#define NATIVE_HTML_SUB                                                      \
    case NativeMethod::HTML_SUB:                                             \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
                throw std::runtime_error("sub() expects string or tag");     \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("sub() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.sub(text);                                   \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_COL                                                  \
    case NativeMethod::HTML_COL:                                         \
    {                                                                    \
        if (!args.empty())                                               \
            throw std::runtime_error("col() does not accept arguments"); \
                                                                         \
        auto tag = html_builder.col();                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                      \
        break;                                                           \
    }

#define NATIVE_HTML_PROGRESS                                                      \
    case NativeMethod::HTML_PROGRESS:                                             \
    {                                                                             \
        std::string text = "";                                                    \
        std::vector<std::shared_ptr<Tag>> children;                               \
                                                                                  \
        if (!args.empty())                                                        \
        {                                                                         \
            size_t i = 0;                                                         \
                                                                                  \
            if (std::holds_alternative<std::string>(args[0]))                     \
            {                                                                     \
                text = std::get<std::string>(args[0]);                            \
                i = 1;                                                            \
            }                                                                     \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))       \
            {                                                                     \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));      \
                i = 1;                                                            \
            }                                                                     \
            else                                                                  \
                throw std::runtime_error("progress() expects string or tag");     \
                                                                                  \
            for (; i < args.size(); ++i)                                          \
            {                                                                     \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))       \
                    throw std::runtime_error("progress() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));      \
            }                                                                     \
        }                                                                         \
                                                                                  \
        auto tag = html_builder.progress(text);                                   \
        for (auto &c : children)                                                  \
            tag->child(c);                                                        \
        push(std::shared_ptr<Tag>(std::move(tag)));                               \
        break;                                                                    \
    }

#define NATIVE_HTML_METER                                                      \
    case NativeMethod::HTML_METER:                                             \
    {                                                                          \
        std::string text = "";                                                 \
        std::vector<std::shared_ptr<Tag>> children;                            \
                                                                               \
        if (!args.empty())                                                     \
        {                                                                      \
            size_t i = 0;                                                      \
                                                                               \
            if (std::holds_alternative<std::string>(args[0]))                  \
            {                                                                  \
                text = std::get<std::string>(args[0]);                         \
                i = 1;                                                         \
            }                                                                  \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))    \
            {                                                                  \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));   \
                i = 1;                                                         \
            }                                                                  \
            else                                                               \
                throw std::runtime_error("meter() expects string or tag");     \
                                                                               \
            for (; i < args.size(); ++i)                                       \
            {                                                                  \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))    \
                    throw std::runtime_error("meter() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));   \
            }                                                                  \
        }                                                                      \
                                                                               \
        auto tag = html_builder.meter(text);                                   \
        for (auto &c : children)                                               \
            tag->child(c);                                                     \
        push(std::shared_ptr<Tag>(std::move(tag)));                            \
        break;                                                                 \
    }

#define NATIVE_HTML_CODE                                                      \
    case NativeMethod::HTML_CODE:                                             \
    {                                                                         \
        std::string text = "";                                                \
        std::vector<std::shared_ptr<Tag>> children;                           \
                                                                              \
        if (!args.empty())                                                    \
        {                                                                     \
            size_t i = 0;                                                     \
                                                                              \
            if (std::holds_alternative<std::string>(args[0]))                 \
            {                                                                 \
                text = std::get<std::string>(args[0]);                        \
                i = 1;                                                        \
            }                                                                 \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))   \
            {                                                                 \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));  \
                i = 1;                                                        \
            }                                                                 \
            else                                                              \
                throw std::runtime_error("code() expects string or tag");     \
                                                                              \
            for (; i < args.size(); ++i)                                      \
            {                                                                 \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))   \
                    throw std::runtime_error("code() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));  \
            }                                                                 \
        }                                                                     \
                                                                              \
        auto tag = html_builder.code(text);                                   \
        for (auto &c : children)                                              \
            tag->child(c);                                                    \
        push(std::shared_ptr<Tag>(std::move(tag)));                           \
        break;                                                                \
    }

#define NATIVE_HTML_Q                                                                \
    case NativeMethod::HTML_Q:                                                       \
    {                                                                                \
        std::string text = "";                                                       \
        std::string cite = "";                                                       \
        std::vector<std::shared_ptr<Tag>> children;                                  \
                                                                                     \
        size_t i = 0;                                                                \
                                                                                     \
        if (i < args.size() && std::holds_alternative<std::string>(args[i]))         \
        {                                                                            \
            text = std::get<std::string>(args[i++]);                                 \
        }                                                                            \
                                                                                     \
        if (i < args.size() && std::holds_alternative<std::string>(args[i]))         \
        {                                                                            \
            cite = std::get<std::string>(args[i++]);                                 \
        }                                                                            \
                                                                                     \
        for (; i < args.size(); ++i)                                                 \
        {                                                                            \
            if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))              \
                throw std::runtime_error("q() expects tag arguments after strings"); \
                                                                                     \
            children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));             \
        }                                                                            \
                                                                                     \
        auto tag = html_builder.q(text, cite);                                       \
        for (auto &c : children)                                                     \
            tag->child(c);                                                           \
        push(std::shared_ptr<Tag>(std::move(tag)));                                  \
        break;                                                                       \
    }

#define NATIVE_HTML_DEL                                                      \
    case NativeMethod::HTML_DEL:                                             \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
                throw std::runtime_error("del() expects string or tag");     \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("del() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.del_(text);                                  \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_DATALIST                                                      \
    case NativeMethod::HTML_DATALIST:                                             \
    {                                                                             \
        std::string text = "";                                                    \
        std::vector<std::shared_ptr<Tag>> children;                               \
                                                                                  \
        if (!args.empty())                                                        \
        {                                                                         \
            size_t i = 0;                                                         \
                                                                                  \
            if (std::holds_alternative<std::string>(args[0]))                     \
            {                                                                     \
                text = std::get<std::string>(args[0]);                            \
                i = 1;                                                            \
            }                                                                     \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))       \
            {                                                                     \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));      \
                i = 1;                                                            \
            }                                                                     \
            else                                                                  \
                throw std::runtime_error("datalist() expects string or tag");     \
                                                                                  \
            for (; i < args.size(); ++i)                                          \
            {                                                                     \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))       \
                    throw std::runtime_error("datalist() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));      \
            }                                                                     \
        }                                                                         \
                                                                                  \
        auto tag = html_builder.datalist(text);                                   \
        for (auto &c : children)                                                  \
            tag->child(c);                                                        \
        push(std::shared_ptr<Tag>(std::move(tag)));                               \
        break;                                                                    \
    }

#define NATIVE_HTML_SUP                                                      \
    case NativeMethod::HTML_SUP:                                             \
    {                                                                        \
        std::string text = "";                                               \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        if (!args.empty())                                                   \
        {                                                                    \
            size_t i = 0;                                                    \
                                                                             \
            if (std::holds_alternative<std::string>(args[0]))                \
            {                                                                \
                text = std::get<std::string>(args[0]);                       \
                i = 1;                                                       \
            }                                                                \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))  \
            {                                                                \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0])); \
                i = 1;                                                       \
            }                                                                \
            else                                                             \
                throw std::runtime_error("sup() expects string or tag");     \
                                                                             \
            for (; i < args.size(); ++i)                                     \
            {                                                                \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))  \
                    throw std::runtime_error("sup() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i])); \
            }                                                                \
        }                                                                    \
                                                                             \
        auto tag = html_builder.sup(text);                                   \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_TRACK                                                  \
    case NativeMethod::HTML_TRACK:                                         \
    {                                                                      \
        if (!args.empty())                                                 \
            throw std::runtime_error("track() does not accept arguments"); \
                                                                           \
        auto tag = html_builder.track();                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                        \
        break;                                                             \
    }

#define NATIVE_HTML_SOURCE                                                  \
    case NativeMethod::HTML_SOURCE:                                         \
    {                                                                       \
        if (!args.empty())                                                  \
            throw std::runtime_error("source() does not accept arguments"); \
                                                                            \
        auto tag = html_builder.source();                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                         \
        break;                                                              \
    }

#define NATIVE_HTML_MARK                                                      \
    case NativeMethod::HTML_MARK:                                             \
    {                                                                         \
        std::string text = "";                                                \
        std::vector<std::shared_ptr<Tag>> children;                           \
                                                                              \
        if (!args.empty())                                                    \
        {                                                                     \
            size_t i = 0;                                                     \
                                                                              \
            if (std::holds_alternative<std::string>(args[0]))                 \
            {                                                                 \
                text = std::get<std::string>(args[0]);                        \
                i = 1;                                                        \
            }                                                                 \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))   \
            {                                                                 \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));  \
                i = 1;                                                        \
            }                                                                 \
            else                                                              \
                throw std::runtime_error("mark() expects string or tag");     \
                                                                              \
            for (; i < args.size(); ++i)                                      \
            {                                                                 \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))   \
                    throw std::runtime_error("mark() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));  \
            }                                                                 \
        }                                                                     \
                                                                              \
        auto tag = html_builder.mark(text);                                   \
        for (auto &c : children)                                              \
            tag->child(c);                                                    \
        push(std::shared_ptr<Tag>(std::move(tag)));                           \
        break;                                                                \
    }

#define NATIVE_HTML_OPTGROUP                                                      \
    case NativeMethod::HTML_OPTGROUP:                                             \
    {                                                                             \
        std::string text = "";                                                    \
        std::vector<std::shared_ptr<Tag>> children;                               \
                                                                                  \
        if (!args.empty())                                                        \
        {                                                                         \
            size_t i = 0;                                                         \
                                                                                  \
            if (std::holds_alternative<std::string>(args[0]))                     \
            {                                                                     \
                text = std::get<std::string>(args[0]);                            \
                i = 1;                                                            \
            }                                                                     \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))       \
            {                                                                     \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));      \
                i = 1;                                                            \
            }                                                                     \
            else                                                                  \
                throw std::runtime_error("optgroup() expects string or tag");     \
                                                                                  \
            for (; i < args.size(); ++i)                                          \
            {                                                                     \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))       \
                    throw std::runtime_error("optgroup() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));      \
            }                                                                     \
        }                                                                         \
                                                                                  \
        auto tag = html_builder.optgroup(text);                                   \
        for (auto &c : children)                                                  \
            tag->child(c);                                                        \
        push(std::shared_ptr<Tag>(std::move(tag)));                               \
        break;                                                                    \
    }

#define NATIVE_HTML_SMALL                                                      \
    case NativeMethod::HTML_SMALL:                                             \
    {                                                                          \
        std::string text = "";                                                 \
        std::vector<std::shared_ptr<Tag>> children;                            \
                                                                               \
        if (!args.empty())                                                     \
        {                                                                      \
            size_t i = 0;                                                      \
                                                                               \
            if (std::holds_alternative<std::string>(args[0]))                  \
            {                                                                  \
                text = std::get<std::string>(args[0]);                         \
                i = 1;                                                         \
            }                                                                  \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))    \
            {                                                                  \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));   \
                i = 1;                                                         \
            }                                                                  \
            else                                                               \
                throw std::runtime_error("small() expects string or tag");     \
                                                                               \
            for (; i < args.size(); ++i)                                       \
            {                                                                  \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))    \
                    throw std::runtime_error("small() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));   \
            }                                                                  \
        }                                                                      \
                                                                               \
        auto tag = html_builder.small(text);                                   \
        for (auto &c : children)                                               \
            tag->child(c);                                                     \
        push(std::shared_ptr<Tag>(std::move(tag)));                            \
        break;                                                                 \
    }

#define NATIVE_HTML_OBJECT                                                      \
    case NativeMethod::HTML_OBJECT:                                             \
    {                                                                           \
        std::string text = "";                                                  \
        std::vector<std::shared_ptr<Tag>> children;                             \
                                                                                \
        if (!args.empty())                                                      \
        {                                                                       \
            size_t i = 0;                                                       \
                                                                                \
            if (std::holds_alternative<std::string>(args[0]))                   \
            {                                                                   \
                text = std::get<std::string>(args[0]);                          \
                i = 1;                                                          \
            }                                                                   \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))     \
            {                                                                   \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));    \
                i = 1;                                                          \
            }                                                                   \
            else                                                                \
                throw std::runtime_error("object() expects string or tag");     \
                                                                                \
            for (; i < args.size(); ++i)                                        \
            {                                                                   \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))     \
                    throw std::runtime_error("object() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));    \
            }                                                                   \
        }                                                                       \
                                                                                \
        auto tag = html_builder.object(text);                                   \
        for (auto &c : children)                                                \
            tag->child(c);                                                      \
        push(std::shared_ptr<Tag>(std::move(tag)));                             \
        break;                                                                  \
    }

#define NATIVE_HTML_SELECT                                                      \
    case NativeMethod::HTML_SELECT:                                             \
    {                                                                           \
        std::string text = "";                                                  \
        std::vector<std::shared_ptr<Tag>> children;                             \
                                                                                \
        if (!args.empty())                                                      \
        {                                                                       \
            size_t i = 0;                                                       \
                                                                                \
            if (std::holds_alternative<std::string>(args[0]))                   \
            {                                                                   \
                text = std::get<std::string>(args[0]);                          \
                i = 1;                                                          \
            }                                                                   \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))     \
            {                                                                   \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));    \
                i = 1;                                                          \
            }                                                                   \
            else                                                                \
                throw std::runtime_error("select() expects string or tag");     \
                                                                                \
            for (; i < args.size(); ++i)                                        \
            {                                                                   \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))     \
                    throw std::runtime_error("select() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));    \
            }                                                                   \
        }                                                                       \
                                                                                \
        auto tag = html_builder.select(text);                                   \
        for (auto &c : children)                                                \
            tag->child(c);                                                      \
        push(std::shared_ptr<Tag>(std::move(tag)));                             \
        break;                                                                  \
    }

#define NATIVE_HTML_COLGROUP                                                      \
    case NativeMethod::HTML_COLGROUP:                                             \
    {                                                                             \
        std::string text = "";                                                    \
        std::vector<std::shared_ptr<Tag>> children;                               \
                                                                                  \
        if (!args.empty())                                                        \
        {                                                                         \
            size_t i = 0;                                                         \
                                                                                  \
            if (std::holds_alternative<std::string>(args[0]))                     \
            {                                                                     \
                text = std::get<std::string>(args[0]);                            \
                i = 1;                                                            \
            }                                                                     \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))       \
            {                                                                     \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));      \
                i = 1;                                                            \
            }                                                                     \
            else                                                                  \
                throw std::runtime_error("colgroup() expects string or tag");     \
                                                                                  \
            for (; i < args.size(); ++i)                                          \
            {                                                                     \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))       \
                    throw std::runtime_error("colgroup() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));      \
            }                                                                     \
        }                                                                         \
                                                                                  \
        auto tag = html_builder.colgroup(text);                                   \
        for (auto &c : children)                                                  \
            tag->child(c);                                                        \
        push(std::shared_ptr<Tag>(std::move(tag)));                               \
        break;                                                                    \
    }

#define NATIVE_HTML_FIELDSET                                                      \
    case NativeMethod::HTML_FIELDSET:                                             \
    {                                                                             \
        std::string text = "";                                                    \
        std::vector<std::shared_ptr<Tag>> children;                               \
                                                                                  \
        if (!args.empty())                                                        \
        {                                                                         \
            size_t i = 0;                                                         \
                                                                                  \
            if (std::holds_alternative<std::string>(args[0]))                     \
            {                                                                     \
                text = std::get<std::string>(args[0]);                            \
                i = 1;                                                            \
            }                                                                     \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))       \
            {                                                                     \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));      \
                i = 1;                                                            \
            }                                                                     \
            else                                                                  \
                throw std::runtime_error("fieldset() expects string or tag");     \
                                                                                  \
            for (; i < args.size(); ++i)                                          \
            {                                                                     \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))       \
                    throw std::runtime_error("fieldset() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));      \
            }                                                                     \
        }                                                                         \
                                                                                  \
        auto tag = html_builder.fieldset(text);                                   \
        for (auto &c : children)                                                  \
            tag->child(c);                                                        \
        push(std::shared_ptr<Tag>(std::move(tag)));                               \
        break;                                                                    \
    }

#define NATIVE_HTML_PICTURE                                                  \
    case NativeMethod::HTML_PICTURE:                                         \
    {                                                                        \
        std::vector<std::shared_ptr<Tag>> children;                          \
                                                                             \
        for (auto &a : args)                                                 \
        {                                                                    \
            if (!std::holds_alternative<std::shared_ptr<Tag>>(a))            \
                throw std::runtime_error("picture() expects tag arguments"); \
            children.push_back(std::get<std::shared_ptr<Tag>>(a));           \
        }                                                                    \
                                                                             \
        auto tag = html_builder.picture();                                   \
        for (auto &c : children)                                             \
            tag->child(c);                                                   \
        push(std::shared_ptr<Tag>(std::move(tag)));                          \
        break;                                                               \
    }

#define NATIVE_HTML_CAPTION                                                      \
    case NativeMethod::HTML_CAPTION:                                             \
    {                                                                            \
        std::string text = "";                                                   \
        std::vector<std::shared_ptr<Tag>> children;                              \
                                                                                 \
        if (!args.empty())                                                       \
        {                                                                        \
            size_t i = 0;                                                        \
                                                                                 \
            if (std::holds_alternative<std::string>(args[0]))                    \
            {                                                                    \
                text = std::get<std::string>(args[0]);                           \
                i = 1;                                                           \
            }                                                                    \
            else if (std::holds_alternative<std::shared_ptr<Tag>>(args[0]))      \
            {                                                                    \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[0]));     \
                i = 1;                                                           \
            }                                                                    \
            else                                                                 \
                throw std::runtime_error("caption() expects string or tag");     \
                                                                                 \
            for (; i < args.size(); ++i)                                         \
            {                                                                    \
                if (!std::holds_alternative<std::shared_ptr<Tag>>(args[i]))      \
                    throw std::runtime_error("caption() expects tag arguments"); \
                children.push_back(std::get<std::shared_ptr<Tag>>(args[i]));     \
            }                                                                    \
        }                                                                        \
                                                                                 \
        auto tag = html_builder.caption(text);                                   \
        for (auto &c : children)                                                 \
            tag->child(c);                                                       \
        push(std::shared_ptr<Tag>(std::move(tag)));                              \
        break;                                                                   \
    }

#define NATIVE_HTML_PARAM                                                           \
    case NativeMethod::HTML_PARAM:                                                  \
    {                                                                               \
        if (!args.empty())                                                          \
        {                                                                           \
            if (!std::holds_alternative<std::string>(args[0]))                      \
                throw std::runtime_error("param() expects string or no arguments"); \
        }                                                                           \
                                                                                    \
        std::string text = args.empty() ? "" : std::get<std::string>(args[0]);      \
        auto tag = html_builder.param(text);                                        \
        push(std::shared_ptr<Tag>(std::move(tag)));                                 \
        break;                                                                      \
    }

#define NATIVE_HTML_CONSTRUCT           \
    case NativeMethod::HTML_CONSTRUCT:  \
    {                                   \
        push(html_builder.construct()); \
        break;                          \
    }
