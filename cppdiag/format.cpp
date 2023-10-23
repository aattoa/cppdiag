#include <cppdiag.hpp>
#include <internals.hpp>

namespace {
    auto format_section(
        cppdiag::Text_section const& /* section */,
        cppdiag::Level const /* level */,
        cppdiag::Colors const /* colors */,
        std::string& /* output */,
        std::string_view const /* message_buffer */) -> void
    {
        ALWAYS_ASSERT(false);
    }
} // namespace

auto cppdiag::Context::format_diagnostic(
    Diagnostic const& diagnostic, std::string& output, Colors const colors) -> void
{
    auto const original_output_size = output.size();
    try {
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
