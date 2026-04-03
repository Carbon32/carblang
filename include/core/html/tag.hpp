#pragma once

template <typename T>
std::unique_ptr<T> with_attr(std::unique_ptr<T> tag, const std::string &key, const std::string &value);

class Tag
{
public:
    std::string name;
    std::string text;
    std::map<std::string, std::string> attributes;
    std::vector<std::shared_ptr<Tag>> children;
    bool self_closing;

    Tag(const std::string &name, const std::string &text = "", bool self_closing = false);
    Tag(const std::string &name, std::vector<std::shared_ptr<Tag>> children);
    Tag *attr(const std::string &key, const std::string &value);
    Tag &id(const std::string &id_name);
    Tag &child(std::shared_ptr<Tag> child);
    std::string str() const;
};
