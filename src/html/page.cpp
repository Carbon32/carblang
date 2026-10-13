#include "core/html/page.hpp"

std::string Page::construct()
{
    auto html_tag = std::make_shared<Tags::html>();
    auto head_tag = std::make_shared<Tags::head>();
    auto body_tag = std::make_shared<Tags::body>();

    body_tag->attr("style",
                   "display: flex;"
                   "flex-direction: column;"
                   "min-height: 100vh;");

    for (const auto &child : head_children)
        head_tag->child(child);
    for (const auto &child : body_children)
        body_tag->child(child);

    html_tag->child(std::move(head_tag));
    html_tag->child(std::move(body_tag));
    return html_tag->str();
}
