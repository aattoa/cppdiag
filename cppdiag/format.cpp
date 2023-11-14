#include <internals.hpp>
#include <cppdiag.hpp>
#include <algorithm>
#include <ranges>

namespace {
    auto format_section(
        cppdiag::Text_section const& section,
        cppdiag::Level const         level,
        cppdiag::Colors const        colors,
        std::string&                 output,
        std::string_view const       message_buffer) -> void
    {
        cppdiag::Color const note_color
            = section.note_color.value_or(cppdiag::internal::level_color(level, colors));

        std::size_t const line_info_width
            = cppdiag::internal::digit_count(section.stop_position.line);

        auto const lines
            = cppdiag::internal::strip_surrounding_whitespace(cppdiag::internal::relevant_lines(
                section.source_string, section.start_position, section.stop_position));
        ALWAYS_ASSERT(!lines.empty());

        auto line_number = section.start_position.line;

        // Write position details
        std::format_to(
            std::back_inserter(output),
            "{}{:{}} --> {}:{}-{}{}\n",
            colors.position_info.code,
            "",
            line_info_width,
            section.source_name,
            section.start_position,
            section.stop_position,
            colors.normal.code);

        for (std::string_view const line : lines) {
            // Write the line number and the line itself
            std::format_to(
                std::back_inserter(output),
                "\n{} {:<{}} |{} {}",
                colors.position_info.code,
                line_number++,
                line_info_width,
                colors.normal.code,
                line);
        }

        if (lines.size() == 1) {
            // Write whitespace padding before the carets
            std::format_to(
                std::back_inserter(output), "\n{:{}}    ", "", section.start_position.column);

            // Write colored carets under the relevant section
            std::format_to(
                std::back_inserter(output),
                "{}{:^>{}}",
                note_color.code,
                "",
                (section.stop_position.column - section.start_position.column) + 1);

            // Write the section note and reset color
            std::format_to(
                std::back_inserter(output),
                " {}{}",
                section.note.has_value()
                    ? cppdiag::internal::view_in(section.note.value(), message_buffer)
                    : "Here",
                colors.normal.code);
        }
    }
} // namespace

auto cppdiag::Context::format_diagnostic(
    Diagnostic const& diagnostic, std::string& output, Colors const colors) const -> void
{
    auto const original_output_size = output.size();
    try {
        // Write the diagnostic header and the message
        std::format_to(
            std::back_inserter(output),
            "{}{}:{} {}",
            internal::level_color(diagnostic.level, colors).code,
            internal::level_string(diagnostic.level),
            colors.normal.code,
            internal::view_in(diagnostic.message, m_message_buffer));

        for (Text_section const& section : diagnostic.text_sections) {
            output.append("\n\n");
            format_section(section, diagnostic.level, colors, output, m_message_buffer);
        }
        if (diagnostic.help_note.has_value()) {
            output.append("\n\n").append(
                internal::view_in(diagnostic.help_note.value(), m_message_buffer));
        }
        output.push_back('\n');
    }
    catch (...) {
        output.resize(original_output_size);
        throw;
    }
}

auto cppdiag::Context::format_diagnostic(Diagnostic const& diagnostic, Colors const colors) const
    -> std::string
{
    std::string output;
    output.reserve(64);
    format_diagnostic(diagnostic, output, colors);
    return output;
}
