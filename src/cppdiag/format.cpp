#include <cpputil/util.hpp>
#include <cppdiag/internals.hpp>
#include <cppdiag/cppdiag.hpp>

namespace {
    using namespace cppdiag::internal;

    using Out = std::back_insert_iterator<std::string>;

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
        Out const                    out,
        cppdiag::Text_section const& section,
        cppdiag::Color const         note_color,
        cppdiag::Colors const        colors) -> void
    {
        std::format_to(
            out,
            "\n{}    {}{} {}{}",
            Padding { ' ', section.start_position.column },
            note_color.code,
            Padding { '^', section.stop_position.column - section.start_position.column + 1 },
            section.note.has_value() ? section.note.value() : std::string_view("Here"),
            colors.normal.code);
    }

    auto write_section(
        Out const                    out,
        cppdiag::Text_section const& section,
        cppdiag::Severity const      severity,
        cppdiag::Colors const        colors) -> void
    {
        auto const line_info_width = digit_count(section.stop_position.line);

        auto const lines = strip_surrounding_whitespace(
            relevant_lines(section.source_string, section.start_position, section.stop_position));
        cpputil::always_assert(!lines.empty());

        write_position_information(out, section, line_info_width, colors);

        auto const note_color = colors.for_severity(section.note_severity.value_or(severity));

        if (lines.size() == 1) {
            write_numbered_line(
                out, lines.front(), line_info_width, section.start_position.line, colors);
            write_horizontal_highlight(out, section, note_color, colors);
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
    std::string& output, Diagnostic const& diagnostic, Colors const colors) -> void
{
    auto const original_output_size = output.size();
    try {
        std::format_to(
            std::back_inserter(output),
            "{}{}",
            Severity_header::make(diagnostic.severity, colors),
            diagnostic.message);

        for (Text_section const& section : diagnostic.text_sections) {
            output.append("\n\n");
            write_section(std::back_inserter(output), section, diagnostic.severity, colors);
        }

        if (diagnostic.help_note.has_value()) {
            output.append("\n\n").append(diagnostic.help_note.value());
        }
        output.push_back('\n');
    }
    catch (...) {
        output.resize(original_output_size);
        throw;
    }
}

auto cppdiag::format_diagnostic(Diagnostic const& diagnostic, Colors const colors) -> std::string
{
    std::string output;
    output.reserve(64);
    format_diagnostic(output, diagnostic, colors);
    return output;
}
