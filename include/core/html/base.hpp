#pragma once

#include "deps.hpp"
#include "tag.hpp"

namespace Tags
{

    class base : public Tag
    {
    public:
        base() : Tag("base") {}
    };
    class head : public Tag
    {
    public:
        head() : Tag("head") {}
    };
    class link : public Tag
    {
    public:
        link(const std::string &href = "", const std::string &rel = "")
            : Tag("link") { attr("href", href)->attr("rel", rel); }
    };
    class meta : public Tag
    {
    public:
        meta(const std::string &text = "", const std::string &name = "",
             const std::string &httpequiv = "", const std::string &charset = "",
             const std::string &itemprop = "")
            : Tag("meta", text)
        {
            attr("name", name)->attr("http-equiv", httpequiv)->attr("charset", charset)->attr("itemprop", itemprop);
        }
    };

    class html : public Tag
    {
    public:
        html(const std::string &text = "") : Tag("html", text) {}
    };
    class style : public Tag
    {
    public:
        style(const std::string &text = "") : Tag("style", text) {}
    };
    class title : public Tag
    {
    public:
        title(const std::string &text = "") : Tag("title", text) {}
    };

    class body : public Tag
    {
    public:
        body(const std::string &text = "") : Tag("body", text) {}
    };
    class address : public Tag
    {
    public:
        address(const std::string &text = "") : Tag("address", text) {}
    };
    class article : public Tag
    {
    public:
        article(const std::string &text = "") : Tag("article", text) {}
    };
    class aside : public Tag
    {
    public:
        aside(const std::string &text = "") : Tag("aside", text) {}
    };
    class footer : public Tag
    {
    public:
        footer(const std::string &text = "") : Tag("footer", text) {}
    };
    class header : public Tag
    {
    public:
        header(const std::string &text = "") : Tag("header", text) {}
    };
    class h1 : public Tag
    {
    public:
        h1(const std::string &text = "") : Tag("h1", text) {}
    };
    class h2 : public Tag
    {
    public:
        h2(const std::string &text = "") : Tag("h2", text) {}
    };
    class h3 : public Tag
    {
    public:
        h3(const std::string &text = "") : Tag("h3", text) {}
    };
    class h4 : public Tag
    {
    public:
        h4(const std::string &text = "") : Tag("h4", text) {}
    };
    class h5 : public Tag
    {
    public:
        h5(const std::string &text = "") : Tag("h5", text) {}
    };
    class h6 : public Tag
    {
    public:
        h6(const std::string &text = "") : Tag("h6", text) {}
    };
    class main : public Tag
    {
    public:
        main(const std::string &text = "") : Tag("main", text) {}
    };
    class nav : public Tag
    {
    public:
        nav(const std::string &text = "") : Tag("nav", text) {}
    };
    class section : public Tag
    {
    public:
        section(const std::string &text = "") : Tag("section", text) {}
    };

    class blockquote : public Tag
    {
    public:
        blockquote(const std::string &text = "", const std::string &cite = "") : Tag("blockquote", text) { attr("cite", cite); }
    };
    class dd : public Tag
    {
    public:
        dd(const std::string &text = "") : Tag("dd", text) {}
    };
    class div : public Tag
    {
    public:
        div(const std::string &text = "") : Tag("div", text) {}
    };
    class dl : public Tag
    {
    public:
        dl(const std::string &text = "") : Tag("dl", text) {}
    };
    class dt : public Tag
    {
    public:
        dt(const std::string &text = "") : Tag("dt", text) {}
    };
    class figcaption : public Tag
    {
    public:
        figcaption(const std::string &text = "") : Tag("figcaption", text) {}
    };
    class figure : public Tag
    {
    public:
        figure(const std::string &text = "") : Tag("figure", text) {}
    };
    class hr : public Tag
    {
    public:
        hr() : Tag("hr", "", true) {}
    };
    class li : public Tag
    {
    public:
        li(const std::string &text = "") : Tag("li", text) {}
    };
    class ol : public Tag
    {
    public:
        ol(const std::string &text = "") : Tag("ol", text) {}
    };
    class p : public Tag
    {
    public:
        p(const std::string &text = "") : Tag("p", text) {}
    };
    class pre : public Tag
    {
    public:
        pre(const std::string &text = "") : Tag("pre", text) {}
    };
    class ul : public Tag
    {
    public:
        ul(const std::string &text = "") : Tag("ul", text) {}
    };

    class a : public Tag
    {
    public:
        a(const std::string &text = "", const std::string &href = "#") : Tag("a", text) { attr("href", href); }
    };

    class br : public Tag
    {
    public:
        br() : Tag("br", "", true) {}
    };
    class cite : public Tag
    {
    public:
        cite(const std::string &text = "") : Tag("cite", text) {}
    };
    class code : public Tag
    {
    public:
        code(const std::string &text = "") : Tag("code", text) {}
    };
    class em : public Tag
    {
    public:
        em(const std::string &text = "") : Tag("em", text) {}
    };
    class mark : public Tag
    {
    public:
        mark(const std::string &text = "") : Tag("mark", text) {}
    };
    class q : public Tag
    {
    public:
        q(const std::string &text = "", const std::string &cite = "") : Tag("q", text) { attr("cite", cite); }
    };
    class small : public Tag
    {
    public:
        small(const std::string &text = "") : Tag("small", text) {}
    };
    class span : public Tag
    {
    public:
        span(const std::string &text = "") : Tag("span", text) {}
    };
    class strong : public Tag
    {
    public:
        strong(const std::string &text = "") : Tag("strong", text) {}
    };
    class sub : public Tag
    {
    public:
        sub(const std::string &text = "") : Tag("sub", text) {}
    };
    class sup : public Tag
    {
    public:
        sup(const std::string &text = "") : Tag("sup", text) {}
    };
    class time : public Tag
    {
    public:
        time(const std::string &text = "") : Tag("time", text) {}
    };
    class wbr : public Tag
    {
    public:
        wbr() : Tag("wbr", "", true) {}
    };

    class area : public Tag
    {
    public:
        area(const std::string &alt = "", const std::string &coords = "", const std::string &download = "", const std::string &href = "",
             const std::string &hreflang = "", const std::string &ping = "", const std::string &referrerpolicy = "", const std::string &rel = "",
             const std::string &shape = "", const std::string &target = "") : Tag("area", "", true)
        {
            attr("alt", alt)->attr("coords", coords)->attr("download", download)->attr("href", href)->attr("hreflang", hreflang)->attr("ping", ping)->attr("referrerpolicy", referrerpolicy)->attr("rel", rel)->attr("shape", shape)->attr("target", target);
        }
    };
    class audio : public Tag
    {
    public:
        audio(const std::string &text = "") : Tag("audio", text) {}
    };
    class img : public Tag
    {
    public:
        img(const std::string &alt = "", const std::string &src = "", const std::string &srcset = "") : Tag("img", "", true) { attr("alt", alt)->attr("src", src)->attr("srcset", srcset); }
    };
    class map : public Tag
    {
    public:
        map(const std::string &text = "", const std::string &name = "") : Tag("map", text) { attr("name", name); }
    };
    class track : public Tag
    {
    public:
        track(const std::string &text = "") : Tag("track", text, true) {}
    };
    class video : public Tag
    {
    public:
        video(const std::string &text = "") : Tag("video", text) {}
    };

    class embed : public Tag
    {
    public:
        embed(const std::string &text = "") : Tag("embed", text, true) {}
    };
    class iframe : public Tag
    {
    public:
        iframe(const std::string &text = "") : Tag("iframe", text) {}
    };
    class object : public Tag
    {
    public:
        object(const std::string &text = "") : Tag("object", text) {}
    };
    class param : public Tag
    {
    public:
        param(const std::string &text = "") : Tag("param", text, true) {}
    };
    class picture : public Tag
    {
    public:
        picture(const std::string &text = "") : Tag("picture", text) {}
    };
    class source : public Tag
    {
    public:
        source(const std::string &text = "") : Tag("source", text, true) {}
    };

    class canvas : public Tag
    {
    public:
        canvas(const std::string &text = "") : Tag("canvas", text) {}
    };

    class del_ : public Tag
    {
    public:
        del_(const std::string &text = "") : Tag("del", text) {}
    };
    class ins : public Tag
    {
    public:
        ins(const std::string &text = "") : Tag("ins", text) {}
    };

    class caption : public Tag
    {
    public:
        caption(const std::string &text = "") : Tag("caption", text) {}
    };
    class col : public Tag
    {
    public:
        col(const std::string &text = "") : Tag("col", text, true) {}
    };
    class colgroup : public Tag
    {
    public:
        colgroup(const std::string &text = "") : Tag("colgroup", text) {}
    };
    class table : public Tag
    {
    public:
        table(const std::string &text = "") : Tag("table", text) {}
    };
    class tbody : public Tag
    {
    public:
        tbody(const std::string &text = "") : Tag("tbody", text) {}
    };
    class td : public Tag
    {
    public:
        td(const std::string &text = "") : Tag("td", text) {}
    };
    class tfoot : public Tag
    {
    public:
        tfoot(const std::string &text = "") : Tag("tfoot", text) {}
    };
    class th : public Tag
    {
    public:
        th(const std::string &text = "") : Tag("th", text) {}
    };
    class thead : public Tag
    {
    public:
        thead(const std::string &text = "") : Tag("thead", text) {}
    };
    class tr : public Tag
    {
    public:
        tr(const std::string &text = "") : Tag("tr", text) {}
    };

    class button : public Tag
    {
    public:
        button(const std::string &text = "") : Tag("button", text) {}
    };
    class datalist : public Tag
    {
    public:
        datalist(const std::string &text = "") : Tag("datalist", text) {}
    };
    class fieldset : public Tag
    {
    public:
        fieldset(const std::string &text = "") : Tag("fieldset", text) {}
    };
    class form : public Tag
    {
    public:
        form(const std::string &text = "") : Tag("form", text) {}
    };
    class input : public Tag
    {
    public:
        input(const std::string &value = "", const std::string &placeholder = "") : Tag("input", "", true) { attr("value", value)->attr("placeholder", placeholder); }
    };
    class label : public Tag
    {
    public:
        label(const std::string &text = "") : Tag("label", text) {}
    };
    class legend : public Tag
    {
    public:
        legend(const std::string &text = "") : Tag("legend", text) {}
    };
    class meter : public Tag
    {
    public:
        meter(const std::string &text = "") : Tag("meter", text) {}
    };
    class optgroup : public Tag
    {
    public:
        optgroup(const std::string &text = "") : Tag("optgroup", text) {}
    };
    class option : public Tag
    {
    public:
        option(const std::string &text = "") : Tag("option", text) {}
    };
    class output : public Tag
    {
    public:
        output(const std::string &text = "") : Tag("output", text) {}
    };
    class progress : public Tag
    {
    public:
        progress(const std::string &text = "") : Tag("progress", text) {}
    };
    class select : public Tag
    {
    public:
        select(const std::string &text = "") : Tag("select", text) {}
    };
    class textarea : public Tag
    {
    public:
        textarea(const std::string &text = "") : Tag("textarea", text) {}
    };

}