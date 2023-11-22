#include <internals.hpp>
#include <functional>
#include <algorithm>
#include <ranges>

auto cppdiag::internal::digit_count(std::size_t integer) -> std::size_t
{
    std::size_t digits = 0;
    do {
        integer /= 10;
        ++digits;
    } while (integer != 0);
    return digits;
}

auto cppdiag::internal::view_in(Message_string const message, std::string_view const buffer)
    -> std::string_view
{
    return buffer.substr(message.offset, message.length);
}

auto cppdiag::internal::find_nth_newline(
    std::string_view::iterator const begin,
    std::string_view::iterator const end,
    std::size_t const                n) -> std::string_view::iterator
{
    return std::find_if(begin, end, [n, i = std::size_t(0)](char const c) mutable {
        return (c == '\n') && (n == i++);
    });
}

auto cppdiag::internal::whitespace_prefix_length(std::string_view const string) -> std::size_t
{
    return string.empty() ? 0 : string.find_first_not_of(' ');
}

auto cppdiag::internal::whitespace_suffix_length(std::string_view const string) -> std::size_t
{
    return string.size() - string.find_last_not_of(' ') - 1;
}

auto cppdiag::internal::is_valid_position(Position const position) -> bool
{
    return position.line != 0 && position.column != 0;
}

auto cppdiag::internal::severity_color(Severity const severity, Colors const colors) -> Color
{
    switch (severity) {
    case cppdiag::Severity::error:
        return colors.error;
    case cppdiag::Severity::warning:
        return colors.warning;
    case cppdiag::Severity::hint:
        return colors.hint;
    case cppdiag::Severity::information:
        return colors.information;
    default:
        ALWAYS_ASSERT(false);
    }
}

auto cppdiag::internal::severity_string(Severity const severity) -> std::string_view
{
    switch (severity) {
    case Severity::error:
        return "Error";
    case Severity::warning:
        return "Warning";
    case cppdiag::Severity::hint:
        return "Hint";
    case cppdiag::Severity::information:
        return "Information";
    default:
        ALWAYS_ASSERT(false);
    }
}

auto cppdiag::internal::relevant_lines(
    std::string_view const source_string, Position const section_start, Position const section_stop)
    -> std::vector<std::string_view>
{
    ALWAYS_ASSERT(!source_string.empty());
    ALWAYS_ASSERT(is_valid_position(section_start));
    ALWAYS_ASSERT(is_valid_position(section_stop));
    ALWAYS_ASSERT(section_start <= section_stop);

    auto       source_it  = source_string.begin();
    auto const source_end = source_string.end();

    if (section_start.line != 1) {
        source_it = find_nth_newline(source_it, source_end, section_start.line - 2);
        // If a newline isn't found, the position was set incorrectly.
        ALWAYS_ASSERT(source_it != source_end);
        // Set the source iterator to the first character of the first relevant line.
        ++source_it;
    }

    std::vector<std::string_view> lines;
    lines.reserve(1 + section_stop.line - section_start.line);

    for (std::size_t line = section_start.line; line != section_stop.line; ++line) {
        auto const newline_it = std::find(source_it, source_end, '\n');
        // As the last relevant line is yet to be reached, a newline must be found.
        ALWAYS_ASSERT(newline_it != source_end);
        lines.emplace_back(source_it, newline_it);
        // Set the source iterator to the first character of the next relevant line.
        source_it = newline_it + 1;
    }

    // The final line can be terminated by either a newline or the end of input.
    lines.emplace_back(source_it, std::find(source_it, source_end, '\n'));

    return lines;
}

auto cppdiag::internal::strip_surrounding_whitespace(std::vector<std::string_view> lines)
    -> std::vector<std::string_view>
{
    if (!lines.empty()) {
        auto const shortest_prefix_length = std::ranges::min(std::views::transform(
            std::views::filter(lines, std::not_fn(std::ranges::empty)), whitespace_prefix_length));
        for (std::string_view& line : lines) {
            if (line.empty()) {
                continue;
            }
            line.remove_prefix(shortest_prefix_length);
            line.remove_suffix(whitespace_suffix_length(line));
        }
    }
    return lines;
}
