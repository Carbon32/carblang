#pragma once

#include "base.hpp"

class Page
{
public:
    template <typename TagType, typename... Children>
    std::unique_ptr<TagType> make_tag(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::base> base(Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::link> link(const std::string &href = "", const std::string &rel = "");
    template <typename... Children>
    std::unique_ptr<Tags::meta> meta(const std::string &text = "", const std::string &name = "", const std::string &httpequiv = "", const std::string &charset = "", const std::string &itemprop = "");
    template <typename... Children>
    std::unique_ptr<Tags::title> title(const std::string &text = "");
    template <typename... Children>
    std::unique_ptr<Tags::style> style(const std::string &text = "");
    template <typename... Children>
    std::unique_ptr<Tags::address> address(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::article> article(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::aside> aside(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::footer> footer(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::header> header(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::h1> h1(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::h2> h2(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::h3> h3(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::h4> h4(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::h5> h5(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::h6> h6(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::main> main(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::nav> nav(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::section> section(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::div> div(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::p> p(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::ul> ul(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::ol> ol(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::li> li(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::dl> dl(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::dt> dt(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::dd> dd(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::figure> figure(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::figcaption> figcaption(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::blockquote> blockquote(const std::string &text = "", const std::string &cite = "");
    template <typename... Children>
    std::unique_ptr<Tags::pre> pre(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::hr> hr();
    template <typename... Children>
    std::unique_ptr<Tags::br> br();
    template <typename... Children>
    std::unique_ptr<Tags::wbr> wbr();
    template <typename... Children>
    std::unique_ptr<Tags::a> a(const std::string &text = "", const std::string &href = "#", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::span> span(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::strong> strong(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::em> em(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::table> table(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::thead> thead(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::tbody> tbody(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::tfoot> tfoot(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::tr> tr(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::td> td(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::th> th(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::img> img(const std::string &alt = "", const std::string &src = "", const std::string &srcset = "");
    template <typename... Children>
    std::unique_ptr<Tags::video> video(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::audio> audio(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::iframe> iframe(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::canvas> canvas(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::form> form(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::input> input(const std::string &value = "", const std::string &placeholder = "");
    template <typename... Children>
    std::unique_ptr<Tags::label> label(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::textarea> textarea(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::button> button(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::embed> embed(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::cite> cite(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::ins> ins(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::output> output(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::map> map(const std::string &text = "", const std::string &name = "");
    template <typename... Children>
    std::unique_ptr<Tags::area> area(const std::string &alt = "", const std::string &coords = "", const std::string &download = "", const std::string &href = "", const std::string &hreflang = "", const std::string &ping = "", const std::string &referrerpolicy = "", const std::string &rel = "", const std::string &shape = "", const std::string &target = "");
    template <typename... Children>
    std::unique_ptr<Tags::option> option(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::legend> legend(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::time> time(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::sub> sub(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::col> col(const std::string &text = "");
    template <typename... Children>
    std::unique_ptr<Tags::progress> progress(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::meter> meter(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::code> code(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::q> q(const std::string &text = "", const std::string &cite = "");
    template <typename... Children>
    std::unique_ptr<Tags::del_> del_(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::datalist> datalist(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::sup> sup(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::track> track(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::source> source(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::mark> mark(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::optgroup> optgroup(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::small> small(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::object> object(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::select> select(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::colgroup> colgroup(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::fieldset> fieldset(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::picture> picture(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::caption> caption(const std::string &text = "", Children &&...children);
    template <typename... Children>
    std::unique_ptr<Tags::param> param(const std::string &text = "", Children &&...children);
    template <typename... Children>
    void head(Children &&...tags);
    template <typename... Children>
    void body(Children &&...tags);
    std::string construct();

private:
    std::unique_ptr<Tags::html> html_tag = std::make_unique<Tags::html>();
    std::unique_ptr<Tags::head> head_tag = std::make_unique<Tags::head>();
    std::unique_ptr<Tags::body> body_tag = std::make_unique<Tags::body>();

    std::vector<std::shared_ptr<Tag>> html_children;
    std::vector<std::shared_ptr<Tag>> head_children;
    std::vector<std::shared_ptr<Tag>> body_children;
};