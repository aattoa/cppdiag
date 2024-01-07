#include <internals.hpp>
#include <cppdiag.hpp>
#include <algorithm>
#include <ranges>

namespace {
    using namespace cppdiag::internal;

    using Out = std::back_insert_iterator<std::string>;

    auto write_diagnostic_message(
        Out const               out,
        cppdiag::Severity const severity,
        std::string_view const  message,
        cppdiag::Colors const   colors) -> void
    {
        std::format_to(
            out,
            "{}{}:{} {}",
            severity_color(severity, colors).code,
            severity_string(severity),
            colors.normal.code,
            message);
    }

    auto write_position_information(
        Out const                    out,
        cppdiag::Text_section const& section,
        std::size_t const            line_info_width,
        cppdiag::Colors const        colors) -> void
    {
        std::format_to(
            out,
            "{}{} --> {}:{}-{}{}\n",
            colors.position.code,
            Padding { ' ', line_info_width },
            section.source_name,
            section.start_position,
            section.stop_position,
            colors.normal.code);
    }

    auto write_numbered_line(
        Out const              out,
        std::string_view const line_string,
        std::size_t const      line_info_width,
        std::size_t const      line_number,
        cppdiag::Colors const  colors) -> void
    {
        std::format_to(
            out,
            "\n{} {:<{}} |{} {}",
            colors.position.code,
            line_number,
            line_info_width,
            colors.normal.code,
            line_string);
    }

    auto write_horizontal_highlight(
        Out const                      out,
        cppdiag::Text_section const&   section,
        cppdiag::Color const           note_color,
        cppdiag::Message_buffer const& message_buffer,
        cppdiag::Colors const          colors) -> void
    {
        std::format_to(
            out,
            "\n{}    {}{} {}{}",
            Padding { ' ', section.start_position.column },
            note_color.code,
            Padding { '^', section.stop_position.column - section.start_position.column + 1 },
            section.note.has_value() ? view_in(section.note.value(), message_buffer) : "Here",
            colors.normal.code);
    }

    auto write_section(
        Out const                      out,
        cppdiag::Text_section const&   section,
        cppdiag::Severity const        severity,
        cppdiag::Colors const          colors,
        cppdiag::Message_buffer const& message_buffer) -> void
    {
        auto const line_info_width = digit_count(section.stop_position.line);

        auto const lines = strip_surrounding_whitespace(
            relevant_lines(section.source_string, section.start_position, section.stop_position));
        ALWAYS_ASSERT(!lines.empty());

        write_position_information(out, section, line_info_width, colors);

        cppdiag::Color const note_color
            = severity_color(section.note_severity.value_or(severity), colors);

        if (lines.size() == 1) {
            write_numbered_line(
                out, lines.front(), line_info_width, section.start_position.line, colors);
            write_horizontal_highlight(out, section, note_color, message_buffer, colors);
        }
        else {
            std::size_t line_number = section.start_position.line;
            for (std::string_view const line : lines) {
                write_numbered_line(out, line, line_info_width, line_number++, colors);
            }
            // TODO: vertical highlight
        }
    }
} // namespace

auto cppdiag::format_diagnostic(
    Diagnostic const&     diagnostic,
    Message_buffer const& message_buffer,
    std::string&          output,
    Colors const          colors) -> void
{
    auto const original_output_size = output.size();
    try {
        write_diagnostic_message(
            std::back_inserter(output),
            diagnostic.severity,
            view_in(diagnostic.message, message_buffer),
            colors);
        for (Text_section const& section : diagnostic.text_sections) {
            output.append("\n\n");
            write_section(
                std::back_inserter(output), section, diagnostic.severity, colors, message_buffer);
        }
        if (diagnostic.help_note.has_value()) {
            output.append("\n\n").append(view_in(diagnostic.help_note.value(), message_buffer));
        }
        output.push_back('\n');
    }
    catch (...) {
        output.resize(original_output_size);
        throw;
    }
}

auto cppdiag::format_diagnostic(
    Diagnostic const&     diagnostic,
    Message_buffer const& message_buffer,
    Colors const          colors) -> std::string
{
    std::string output;
    output.reserve(64);
    format_diagnostic(diagnostic, message_buffer, output, colors);
    return output;
}
