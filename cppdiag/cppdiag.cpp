#include <cppdiag.hpp>
#include <algorithm>

namespace {
    // A position is valid if both its line and column components are nonzero.
    constexpr auto is_valid_position(cppdiag::Position const position) noexcept -> bool
    {
        return position.line != 0 && position.column != 0;
    }

    auto level_color(cppdiag::Level const level, cppdiag::Colors const colors) noexcept
        -> cppdiag::Color
    {
        switch (level) {
        case cppdiag::Level::error:
            return colors.error;
        case cppdiag::Level::warning:
            return colors.warning;
        case cppdiag::Level::note:
            return colors.note;
        default:
            assert(false);
            std::abort();
        }
    }

    auto level_string(cppdiag::Level const level) noexcept -> std::string_view
    {
        switch (level) {
        case cppdiag::Level::error:
            return "Error";
        case cppdiag::Level::warning:
            return "Warning";
        case cppdiag::Level::note:
            return "Note";
        default:
            assert(false);
            std::abort();
        }
    }

    auto format_section(cppdiag::Text_section const& section, std::string& output) -> void
    {
        (void)section;
        (void)output;
        assert(false);
        std::abort();
    }

    constexpr auto view_in(cppdiag::Message_string const message, std::string_view const buffer)
        -> std::string_view
    {
        return buffer.substr(message.offset, message.length);
    }

    constexpr auto find_nth_newline(auto const begin, auto const end, std::size_t const n)
    {
        return std::find_if(begin, end, [n, i = std::size_t(0)](char const c) mutable {
            return (c == '\n') && (n == i++);
        });
    }
} // namespace

auto cppdiag::internal::get_relevant_lines(
    std::string_view const source_string, Position const section_start, Position const section_stop)
    -> std::vector<std::string_view>
{
    assert(!source_string.empty());
    assert(is_valid_position(section_start));
    assert(is_valid_position(section_stop));
    assert(section_start <= section_stop);

    auto       source_it  = source_string.begin();
    auto const source_end = source_string.end();

    if (section_start.line != 1) {
        source_it = find_nth_newline(source_it, source_end, section_start.line - 2);
        // If a newline isn't found, the position was set incorrectly.
        assert(source_it != source_end);
        // Set the source iterator to the first character of the first relevant line.
        ++source_it;
    }

    std::vector<std::string_view> lines;
    lines.reserve(1 + section_stop.line - section_start.line);

    for (std::size_t line = section_start.line; line != section_stop.line; ++line) {
        auto const newline_it = std::find(source_it, source_end, '\n');
        // As the last relevant line is yet to be reached, a newline must be found.
        assert(newline_it != source_end);
        lines.emplace_back(source_it, newline_it);
        // Set the source iterator to the first character of the next relevant line.
        source_it = newline_it + 1;
    }

    // The final line can be terminated by either a newline or the end of input.
    lines.emplace_back(source_it, std::find(source_it, source_end, '\n'));

    return lines;
}

auto cppdiag::Context::format_diagnostic(
    Diagnostic const& diagnostic, std::string& output, Colors const colors) -> void
{
    auto const original_output_size = output.size();
    try {
        std::format_to(
            std::back_inserter(output),
            "{}{}:{} {}",
            level_color(diagnostic.level, colors).code,
            level_string(diagnostic.level),
            colors.normal.code,
            view_in(diagnostic.message, m_message_buffer));

        for (Text_section const& section : diagnostic.text_sections) {
            format_section(section, output);
        }

        if (diagnostic.help_note.has_value()) {
            std::format_to(
                std::back_inserter(output),
                "\n\n{}",
                view_in(diagnostic.help_note.value(), m_message_buffer));
        }
    }
    catch (...) {
        output.resize(original_output_size);
        throw;
    }
}

auto cppdiag::Context::format_diagnostic(Diagnostic const& diagnostic, Colors const colors)
    -> std::string
{
    std::string output;
    output.reserve(64);
    format_diagnostic(diagnostic, output, colors);
    return output;
}

auto cppdiag::Context::vformat_message(std::string_view const fmt, std::format_args const args)
    -> Message_string
{
    std::size_t const offset = m_message_buffer.size();
    std::vformat_to(std::back_inserter(m_message_buffer), fmt, args);
    return Message_string { .offset = offset, .length = m_message_buffer.size() - offset };
}
