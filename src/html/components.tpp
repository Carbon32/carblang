#include "core/html/components.hpp"

template <typename... Children>
std::unique_ptr<Tags::div> Container(Children &&...children)
{
    auto div = html_builder.div("");

    div->attr("style",
              "display: flex;"
              "gap: 1rem;"
              "justify-content: center;"
              "flex-wrap: wrap;"
              "padding: 2rem;");

    (div->child(std::move(std::unique_ptr<Tag>(children.release()))), ...);

    return div;
}