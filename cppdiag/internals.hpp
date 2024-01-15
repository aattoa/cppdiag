#pragma once

#include <cppdiag.hpp>
#include <cpputil/util.hpp>

#include <vector>
#include <cstdint>
#include <cassert>
#include <string_view>

namespace cppdiag::internal {

    auto digit_count(std::size_t) -> std::size_t;

    auto whitespace_prefix_length(std::string_view) -> std::size_t;

    auto whitespace_suffix_length(std::string_view) -> std::size_t;

    auto view_in(Message_string, Message_buffer const&) -> std::string_view;

    auto is_valid_position(Position) -> bool;

    auto severity_string(Severity) -> std::string_view;

    auto find_nth_newline(std::string_view::iterator, std::string_view::iterator, std::size_t)
        -> std::string_view::iterator;

    auto relevant_lines(std::string_view source, Position start, Position stop)
        -> std::vector<std::string_view>;

    auto strip_surrounding_whitespace(std::vector<std::string_view>)
        -> std::vector<std::string_view>;

    struct Padding {
        char        fill {};
        std::size_t width {};
    };

} // namespace cppdiag::internal

template <>
struct std::formatter<cppdiag::internal::Padding> {
    constexpr auto parse(auto& context)
    {
        return context.begin();
    }

    constexpr auto format(cppdiag::internal::Padding const padding, auto& context) const
    {
        auto out = context.out();
        for (std::size_t i = 0; i != padding.width; ++i) {
            *out++ = padding.fill;
        }
        return out;
    }
};

template <>
struct std::formatter<cppdiag::Position> {
    constexpr auto parse(auto& context)
    {
        return context.begin();
    }

    constexpr auto format(cppdiag::Position const position, auto& context) const
    {
        return std::format_to(context.out(), "{}:{}", position.line, position.column);
    }
};
