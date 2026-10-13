#include "core/html/deps.hpp"
#include "core/html/tag.hpp"

namespace
{
void append_escaped(std::string &out, const std::string &text, bool attribute = false)
{
    for (char ch : text)
    {
        switch (ch)
        {
        case '&': out += "&amp;"; break;
        case '<': out += "&lt;"; break;
        case '>': out += "&gt;"; break;
        case '"': out += attribute ? "&quot;" : "\""; break;
        case '\'': out += attribute ? "&#39;" : "'"; break;
        default: out += ch; break;
        }
    }
}

void serialize(const Tag &tag, std::string &out, std::unordered_set<const Tag *> &active)
{
    if (tag.name == "text")
    {
        append_escaped(out, tag.text);
        return;
    }

    if (!active.insert(&tag).second)
        throw std::runtime_error("HTML tag tree contains a cycle");

    out += '<';
    append_escaped(out, tag.name, true);
    for (const auto &[key, value] : tag.attributes)
    {
        out += ' ';
        append_escaped(out, key, true);
        out += "=\"";
        append_escaped(out, value, true);
        out += '"';
    }

    if (tag.self_closing)
        out += " />";
    else
    {
        out += '>';
        append_escaped(out, tag.text);
        for (const auto &child : tag.children)
        {
            if (!child)
                throw std::runtime_error("HTML tag tree contains a null child");
            serialize(*child, out, active);
        }
        out += "</";
        append_escaped(out, tag.name, true);
        out += '>';
    }
    active.erase(&tag);
}
}

template <typename T>
std::unique_ptr<T> with_attr(std::unique_ptr<T> tag, const std::string &key, const std::string &value)
{
    if (!tag)
        throw std::invalid_argument("Cannot add an attribute to a null HTML tag");
    tag->attr(key, value);
    return tag;
}

Tag::Tag(const std::string &name, const std::string &text, bool self_closing)
    : name(name), text(text), self_closing(self_closing) {}

Tag::Tag(const std::string &name, std::vector<std::shared_ptr<Tag>> children)
    : name(name), self_closing(false)
{
    for (auto &c : children)
        this->child(std::move(c));
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
    if (!child)
        throw std::invalid_argument("Cannot add a null HTML child");

    std::vector<const Tag *> pending{child.get()};
    std::unordered_set<const Tag *> visited;
    while (!pending.empty())
    {
        const Tag *node = pending.back();
        pending.pop_back();
        if (node == this)
            throw std::runtime_error("Cannot create a cycle in an HTML tag tree");
        if (!visited.insert(node).second)
            continue;
        for (const auto &nested : node->children)
            if (nested)
                pending.push_back(nested.get());
    }

    children.push_back(std::move(child));
    return *this;
}

std::string Tag::str() const
{
    std::string result;
    std::unordered_set<const Tag *> active;
    serialize(*this, result, active);
    return result;
}
