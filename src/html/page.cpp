#include "core/html/page.hpp"

std::string Page::construct()
{

    this->body_tag->attr("style",
                         "display: flex;"
                         "flex-direction: column;"
                         "min-height: 100vh;");

    for (auto &child : this->head_children)
        this->head_tag->child(std::move(child));

    for (auto &child : this->body_children)
        this->body_tag->child(std::move(child));

    this->html_children.push_back(std::move(this->head_tag));
    this->html_children.push_back(std::move(this->body_tag));

    for (auto &child : this->html_children)
        this->html_tag->child(std::move(child));

    return this->html_tag->str();
}