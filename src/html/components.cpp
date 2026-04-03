#include "core/html/components.hpp"

std::unique_ptr<Tags::header> Navigation(std::initializer_list<std::pair<std::string, std::string>> routes)
{
    std::string nav_style =
        "display: flex;"
        "justify-content: center;"
        "gap: 2rem;"
        "flex-wrap: wrap;"
        "background-color: #111;"
        "padding: 1rem;"
        "font-family: 'Segoe UI', Roboto, Helvetica, sans-serif;"
        "font-size: 1rem;"
        "letter-spacing: 0.5px;";

    auto nav = html_builder.nav("");
    nav->attr("style", nav_style);

    for (const auto &[text, href] : routes)
    {
        auto link = html_builder.a(text, href);
        link->attr("style",
                   "color: #fff;"
                   "text-decoration: none;"
                   "font-weight: 500;"
                   "padding-bottom: 3px;"
                   "border-bottom: 2px solid transparent;"
                   "transition: border-color 0.3s, color 0.3s;"
                   "white-space: nowrap;");

        link->attr("onmouseover",
                   "this.style.borderColor='#00ffff';"
                   "this.style.color='#00ffff';");
        link->attr("onmouseout",
                   "this.style.borderColor='transparent';"
                   "this.style.color='#fff';");

        nav->child(std::move(link));
    }

    return html_builder.header("", std::move(nav));
}

std::unique_ptr<Tags::footer> Footer(std::initializer_list<std::pair<std::string, std::string>> links)
{
    std::string footer_style =
        "display: flex;"
        "justify-content: center;"
        "gap: 1.5rem;"
        "flex-wrap: wrap;"
        "background-color: #111;"
        "padding: 1.5rem;"
        "font-family: 'Segoe UI', Roboto, Helvetica, sans-serif;"
        "font-size: 0.9rem;"
        "color: #bbb;"
        "margin-top: auto;";

    auto footer = html_builder.footer("");
    footer->attr("style", footer_style);

    for (const auto &[text, href] : links)
    {
        auto link = html_builder.a(text, href);
        link->attr("style",
                   "color: #bbb;"
                   "text-decoration: none;"
                   "font-weight: 400;"
                   "border-bottom: 1px dotted transparent;"
                   "transition: border-color 0.3s, color 0.3s;");

        link->attr("onmouseover",
                   "this.style.borderColor='#00ffff';"
                   "this.style.color='#00ffff';");
        link->attr("onmouseout",
                   "this.style.borderColor='transparent';"
                   "this.style.color='#bbb';");

        footer->child(std::move(link));
    }

    return footer;
}

std::unique_ptr<Tags::button> Button(const std::string &text, const std::string &on_click)
{
    std::string btn_style =
        "background-color: #00aaff;"
        "width: 20%;"
        "margin: 1rem;"
        "color: #fff;"
        "border: none;"
        "padding: 0.4rem 1rem;"
        "font-size: 1rem;"
        "font-family: 'Segoe UI', Roboto, Helvetica, sans-serif;"
        "border-radius: 5px;"
        "cursor: pointer;"
        "transition: background-color 0.3s, transform 0.2s;";

    auto btn = html_builder.button(text);
    btn->attr("style", btn_style);
    btn->attr("onclick", on_click);

    btn->attr("onmouseover",
              "this.style.backgroundColor='#00ffff';"
              "this.style.transform='scale(1.05)';");
    btn->attr("onmouseout",
              "this.style.backgroundColor='#00aaff';"
              "this.style.transform='scale(1)';");

    return btn;
}

std::unique_ptr<Tags::div> Card(const std::string &title, const std::string &body, const std::string &footer_text)
{
    std::string card_style =
        "background-color: #1a1a1a;"
        "color: #fff;"
        "border-radius: 10px;"
        "padding: 1rem;"
        "width: 250px;"
        "box-shadow: 0 4px 8px rgba(0,0,0,0.3);"
        "font-family: 'Segoe UI', Roboto, Helvetica, sans-serif;"
        "transition: transform 0.2s, box-shadow 0.2s;";

    auto card = html_builder.div("");
    card->attr("style", card_style);

    auto h3 = html_builder.h3(title);
    h3->attr("style", "margin: 0 0 0.5rem 0; font-size: 1.2rem;");
    card->child(std::move(h3));

    auto p = html_builder.p(body);
    p->attr("style", "margin: 0 0 1rem 0; font-size: 0.95rem; line-height: 1.4;");
    card->child(std::move(p));

    auto footer = html_builder.div(footer_text);
    footer->attr("style", "font-size: 0.85rem; color: #bbb;");
    card->child(std::move(footer));

    card->attr("onmouseover",
               "this.style.transform='translateY(-5px)';"
               "this.style.boxShadow='0 8px 16px rgba(0,255,255,0.3)';");
    card->attr("onmouseout",
               "this.style.transform='translateY(0)';"
               "this.style.boxShadow='0 4px 8px rgba(0,0,0,0.3)';");

    return card;
}
