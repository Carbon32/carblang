#include "core/html/page.hpp"

template <typename TagType, typename... Children>
std::unique_ptr<TagType> Page::make_tag(const std::string &text, Children &&...children)
{
    auto tag = std::make_unique<TagType>(text);
    (tag->child(std::move(children)), ...);
    return tag;
}

template <typename... Children>
std::unique_ptr<Tags::base> Page::base(Children &&...children) { return make_tag<Tags::base>("", std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::link> Page::link(const std::string &href, const std::string &rel) { return std::make_unique<Tags::link>(href, rel); }
template <typename... Children>
std::unique_ptr<Tags::meta> Page::meta(const std::string &text, const std::string &name, const std::string &httpequiv, const std::string &charset, const std::string &itemprop) { return std::make_unique<Tags::meta>(text, name, httpequiv, charset, itemprop); }
template <typename... Children>
std::unique_ptr<Tags::title> Page::title(const std::string &text) { return make_tag<Tags::title>(text); }
template <typename... Children>
std::unique_ptr<Tags::style> Page::style(const std::string &text) { return make_tag<Tags::style>(text); }
template <typename... Children>
std::unique_ptr<Tags::address> Page::address(const std::string &text, Children &&...children) { return make_tag<Tags::address>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::article> Page::article(const std::string &text, Children &&...children) { return make_tag<Tags::article>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::aside> Page::aside(const std::string &text, Children &&...children) { return make_tag<Tags::aside>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::footer> Page::footer(const std::string &text, Children &&...children) { return make_tag<Tags::footer>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::header> Page::header(const std::string &text, Children &&...children) { return make_tag<Tags::header>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::h1> Page::h1(const std::string &text, Children &&...children) { return make_tag<Tags::h1>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::h2> Page::h2(const std::string &text, Children &&...children) { return make_tag<Tags::h2>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::h3> Page::h3(const std::string &text, Children &&...children) { return make_tag<Tags::h3>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::h4> Page::h4(const std::string &text, Children &&...children) { return make_tag<Tags::h4>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::h5> Page::h5(const std::string &text, Children &&...children) { return make_tag<Tags::h5>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::h6> Page::h6(const std::string &text, Children &&...children) { return make_tag<Tags::h6>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::main> Page::main(const std::string &text, Children &&...children) { return make_tag<Tags::main>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::nav> Page::nav(const std::string &text, Children &&...children) { return make_tag<Tags::nav>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::section> Page::section(const std::string &text, Children &&...children) { return make_tag<Tags::section>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::div> Page::div(const std::string &text, Children &&...children)
{
    return make_tag<Tags::div>(text, std::forward<Children>(children)...);
}
template <typename... Children>
std::unique_ptr<Tags::p> Page::p(const std::string &text, Children &&...children) { return make_tag<Tags::p>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::ul> Page::ul(const std::string &text, Children &&...children) { return make_tag<Tags::ul>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::ol> Page::ol(const std::string &text, Children &&...children) { return make_tag<Tags::ol>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::li> Page::li(const std::string &text, Children &&...children) { return make_tag<Tags::li>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::dl> Page::dl(const std::string &text, Children &&...children) { return make_tag<Tags::dl>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::dt> Page::dt(const std::string &text, Children &&...children) { return make_tag<Tags::dt>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::dd> Page::dd(const std::string &text, Children &&...children) { return make_tag<Tags::dd>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::figure> Page::figure(const std::string &text, Children &&...children) { return make_tag<Tags::figure>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::figcaption> Page::figcaption(const std::string &text, Children &&...children) { return make_tag<Tags::figcaption>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::blockquote> Page::blockquote(const std::string &text, const std::string &cite) { return std::make_unique<Tags::blockquote>(text, cite); }
template <typename... Children>
std::unique_ptr<Tags::pre> Page::pre(const std::string &text, Children &&...children) { return make_tag<Tags::pre>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::hr> Page::hr() { return std::make_unique<Tags::hr>(); }
template <typename... Children>
std::unique_ptr<Tags::br> Page::br() { return std::make_unique<Tags::br>(); }
template <typename... Children>
std::unique_ptr<Tags::wbr> Page::wbr() { return std::make_unique<Tags::wbr>(); }

template <typename... Children>
std::unique_ptr<Tags::a> Page::a(const std::string &text, const std::string &href, Children &&...children)
{
    auto tag = std::make_unique<Tags::a>(text, href);
    (tag->child(std::move(children)), ...);
    return tag;
}
template <typename... Children>
std::unique_ptr<Tags::span> Page::span(const std::string &text, Children &&...children) { return make_tag<Tags::span>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::strong> Page::strong(const std::string &text, Children &&...children) { return make_tag<Tags::strong>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::em> Page::em(const std::string &text, Children &&...children) { return make_tag<Tags::em>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::table> Page::table(const std::string &text, Children &&...children) { return make_tag<Tags::table>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::thead> Page::thead(const std::string &text, Children &&...children) { return make_tag<Tags::thead>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::tbody> Page::tbody(const std::string &text, Children &&...children) { return make_tag<Tags::tbody>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::tfoot> Page::tfoot(const std::string &text, Children &&...children) { return make_tag<Tags::tfoot>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::tr> Page::tr(const std::string &text, Children &&...children) { return make_tag<Tags::tr>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::td> Page::td(const std::string &text, Children &&...children) { return make_tag<Tags::td>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::th> Page::th(const std::string &text, Children &&...children) { return make_tag<Tags::th>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::img> Page::img(const std::string &alt, const std::string &src, const std::string &srcset) { return std::make_unique<Tags::img>(alt, src, srcset); }
template <typename... Children>
std::unique_ptr<Tags::video> Page::video(const std::string &text, Children &&...children) { return make_tag<Tags::video>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::audio> Page::audio(const std::string &text, Children &&...children) { return make_tag<Tags::audio>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::iframe> Page::iframe(const std::string &text, Children &&...children) { return make_tag<Tags::iframe>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::canvas> Page::canvas(const std::string &text, Children &&...children) { return make_tag<Tags::canvas>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::form> Page::form(const std::string &text, Children &&...children) { return make_tag<Tags::form>(text, std::forward<Children>(children)...); }
template <typename... Children>
std::unique_ptr<Tags::input> Page::input(const std::string &value, const std::string &placeholder) { return std::make_unique<Tags::input>(value, placeholder); }
template <typename... Children>
std::unique_ptr<Tags::label> Page::label(const std::string &text, Children &&...children) { return make_tag<Tags::label>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::textarea> Page::textarea(const std::string &text, Children &&...children)
{
    return make_tag<Tags::textarea>(text, std::forward<Children>(children)...);
}

template <typename... Children>
std::unique_ptr<Tags::button> Page::button(const std::string &text, Children &&...children)
{
    return make_tag<Tags::button>(text, std::forward<Children>(children)...);
}

template <typename... Children>
std::unique_ptr<Tags::embed> Page::embed(const std::string &text, Children &&...children) { return make_tag<Tags::embed>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::cite> Page::cite(const std::string &text, Children &&...children) { return make_tag<Tags::cite>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::ins> Page::ins(const std::string &text, Children &&...children) { return make_tag<Tags::ins>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::output> Page::output(const std::string &text, Children &&...children) { return make_tag<Tags::output>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::map> Page::map(const std::string &text, const std::string &name) { return std::make_unique<Tags::map>(text, name); }

template <typename... Children>
std::unique_ptr<Tags::area> Page::area(const std::string &alt, const std::string &coords, const std::string &download, const std::string &href, const std::string &hreflang, const std::string &ping, const std::string &referrerpolicy, const std::string &rel, const std::string &shape, const std::string &target) { return std::make_unique<Tags::area>(alt, coords, download, href, hreflang, ping, referrerpolicy, rel, shape, target); }

template <typename... Children>
std::unique_ptr<Tags::option> Page::option(const std::string &text, Children &&...children) { return make_tag<Tags::option>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::legend> Page::legend(const std::string &text, Children &&...children) { return make_tag<Tags::legend>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::time> Page::time(const std::string &text, Children &&...children) { return make_tag<Tags::time>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::sub> Page::sub(const std::string &text, Children &&...children) { return make_tag<Tags::sub>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::col> Page::col(const std::string &text) { return std::make_unique<Tags::col>(text); }

template <typename... Children>
std::unique_ptr<Tags::progress> Page::progress(const std::string &text, Children &&...children) { return make_tag<Tags::progress>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::meter> Page::meter(const std::string &text, Children &&...children) { return make_tag<Tags::meter>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::code> Page::code(const std::string &text, Children &&...children) { return make_tag<Tags::code>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::q> Page::q(const std::string &text, const std::string &cite) { return std::make_unique<Tags::q>(text, cite); }

template <typename... Children>
std::unique_ptr<Tags::del_> Page::del_(const std::string &text, Children &&...children) { return make_tag<Tags::del_>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::datalist> Page::datalist(const std::string &text, Children &&...children) { return make_tag<Tags::datalist>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::sup> Page::sup(const std::string &text, Children &&...children) { return make_tag<Tags::sup>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::track> Page::track(const std::string &text, Children &&...children) { return make_tag<Tags::track>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::source> Page::source(const std::string &text, Children &&...children) { return make_tag<Tags::source>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::mark> Page::mark(const std::string &text, Children &&...children) { return make_tag<Tags::mark>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::optgroup> Page::optgroup(const std::string &text, Children &&...children) { return make_tag<Tags::optgroup>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::small> Page::small(const std::string &text, Children &&...children) { return make_tag<Tags::small>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::object> Page::object(const std::string &text, Children &&...children) { return make_tag<Tags::object>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::select> Page::select(const std::string &text, Children &&...children) { return make_tag<Tags::select>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::colgroup> Page::colgroup(const std::string &text, Children &&...children) { return make_tag<Tags::colgroup>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::fieldset> Page::fieldset(const std::string &text, Children &&...children) { return make_tag<Tags::fieldset>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::picture> Page::picture(const std::string &text, Children &&...children) { return make_tag<Tags::picture>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::caption> Page::caption(const std::string &text, Children &&...children) { return make_tag<Tags::caption>(text, std::forward<Children>(children)...); }

template <typename... Children>
std::unique_ptr<Tags::param> Page::param(const std::string &text, Children &&...children) { return make_tag<Tags::param>(text, std::forward<Children>(children)...); }

template <typename... Children>
void Page::head(Children &&...tags)
{
    (head_children.push_back(std::move(tags)), ...);
}

template <typename... Children>
void Page::body(Children &&...tags)
{
    (body_children.push_back(std::move(tags)), ...);
}
