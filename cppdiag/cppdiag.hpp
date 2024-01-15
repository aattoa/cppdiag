#pragma once

#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace cppdiag {

    // Store diagnostic messages in one unified buffer.
    struct Message_buffer {
        std::string string;
    };

    // A relative view of a `Message_buffer`.
    struct Message_string {
        std::size_t offset {};
        std::size_t length {};
    };

    // The position of a character in a text file or other diagnostic source string.
    struct Position {
        std::uint32_t line   = 1;
        std::uint32_t column = 1;

        auto operator<=>(Position const&) const -> std::strong_ordering = default;
    };

    enum class Severity {
        error,
        warning,
        hint,
        information,
    };

    struct Color {
        std::string_view code;
    };

    struct Colors {
        Color normal;
        Color error;
        Color warning;
        Color hint;
        Color information;
        Color position;

        // Select the data member corresponding to the given severity.
        auto for_severity(Severity) const noexcept -> Color;

        // Returns a `Colors` object with sensible default values.
        static auto defaults() noexcept -> Colors;

        // Returns a default constructed `Colors` object.
        static auto none() noexcept -> Colors;
    };

    struct Severity_header {
        Color            severity_color;
        Color            restore_color;
        std::string_view severity_string;

        static auto make(Severity, Colors const&) noexcept -> Severity_header;
    };

    struct Text_section {
        std::string_view              source_string;
        std::string_view              source_name;
        Position                      start_position;
        Position                      stop_position;
        std::optional<Message_string> note;
        std::optional<Severity>       note_severity;
    };

    struct Diagnostic {
        std::vector<Text_section>     text_sections;
        Message_string                message;
        std::optional<Message_string> help_note;
        Severity                      severity {};
    };

    // Format `diagnostic` to `output` according to `colors`.
    auto format_diagnostic(
        Diagnostic const&     diagnostic,
        Message_buffer const& message_buffer,
        std::string&          output,
        Colors                colors = Colors::defaults()) -> void;

    // Format `diagnostic` to a new string according to `colors`.
    auto format_diagnostic(
        Diagnostic const&     diagnostic,
        Message_buffer const& message_buffer,
        Colors                colors = Colors::defaults()) -> std::string;

    auto vformat_message(Message_buffer&, std::string_view, std::format_args) -> Message_string;

    template <class... Args>
    auto format_message(
        Message_buffer&                   buffer,
        std::format_string<Args...> const fmt,
        Args&&... args) -> Message_string
    {
        return vformat_message(buffer, fmt.get(), std::make_format_args(args...));
    }

} // namespace cppdiag

template <class Char>
struct std::formatter<cppdiag::Severity_header, Char> {
    constexpr auto parse(auto& context) const
    {
        return context.begin();
    }

    constexpr auto format(cppdiag::Severity_header const header, auto& context) const
    {
        return std::format_to(
            context.out(),
            "{}{}:{} ",
            header.severity_color.code,
            header.severity_string,
            header.restore_color.code);
    }
};

// static_assert(std::formattable<cppdiag::Severity_header, char>);
