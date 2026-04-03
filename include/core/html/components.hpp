#pragma once

#include "core.hpp"

template <typename... Children>
std::unique_ptr<Tags::div> Container(Children &&...children);
std::unique_ptr<Tags::header> Navigation(std::initializer_list<std::pair<std::string, std::string>> routes);
std::unique_ptr<Tags::footer> Footer(std::initializer_list<std::pair<std::string, std::string>> links);
std::unique_ptr<Tags::button> Button(const std::string &text, const std::string &on_click);
std::unique_ptr<Tags::div> Card(const std::string &title, const std::string &body, const std::string &footer_text);
