#include "core/html/deps.hpp"
#include "core/html/tag.hpp"

template <typename T>
std::unique_ptr<T> with_attr(std::unique_ptr<T> tag, const std::string &key, const std::string &value)
{
    tag->attr(key, value);
    return tag;
}

Tag::Tag(const std::string &name, const std::string &text, bool self_closing)
    : name(name), text(text), self_closing(self_closing) {}

Tag::Tag(const std::string &name, std::vector<std::shared_ptr<Tag>> children)
    : name(name), self_closing(false)
{
    for (auto &c : children)
        this->children.push_back(std::move(c));
}

Tag *Tag::attr(const std::string &key, const std::string &value)
{
    auto it = attributes.find(key);

    if (it == attributes.end())
    {
        attributes[key] = value;
    }
    else
    {
        if (!it->second.empty())
            it->second += " ";

        it->second += value;
    }

    return this;
}

Tag &Tag::id(const std::string &id_name)
{
    attributes["id"] = id_name;
    return *this;
}

Tag &Tag::child(std::shared_ptr<Tag> child)
{
    if (self_closing)
        throw std::runtime_error("Cannot add children to self-closing tag");

    children.push_back(std::move(child));
    return *this;
}

std::string Tag::str() const
{
    std::ostringstream oss;
    oss << "<" << name;
    for (const auto &[k, v] : attributes)
        oss << " " << k << "=\"" << v << "\"";

    if (self_closing)
    {
        oss << " />";
    }
    else
    {
        oss << ">";
        oss << text;
        for (const auto &c : children)
            oss << c->str();
        oss << "</" << name << ">";
    }

    return oss.str();
}
