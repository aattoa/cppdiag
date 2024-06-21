#pragma once

#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace cppdiag {

    // The position of a character in a text file or other diagnostic source string.
    struct Position {
        std::uint32_t line   = 1;
        std::uint32_t column = 1;

        auto operator<=>(Position const&) const -> std::strong_ordering = default;
    };

    enum class Severity { error, warning, hint, information };

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
        std::string                source_string;
        std::string                source_name;
        Position                   start_position;
        Position                   stop_position;
        std::optional<std::string> note;
        std::optional<Severity>    note_severity;
    };

    struct Diagnostic {
        std::vector<Text_section>  text_sections;
        std::string                message;
        std::optional<std::string> help_note;
        Severity                   severity {};
    };

    // Format `diagnostic` to `output` according to `colors`.
    auto format_diagnostic(
        std::string&      output,
        Diagnostic const& diagnostic,
        Colors            colors = Colors::defaults()) -> void;

    // Format `diagnostic` to a new string according to `colors`.
    auto format_diagnostic(Diagnostic const& diagnostic, Colors colors = Colors::defaults())
        -> std::string;

} // namespace cppdiag

template <class Char>
struct std::formatter<cppdiag::Severity_header, Char> {
    static constexpr auto parse(auto& context)
    {
        return context.begin();
    }

    static constexpr auto format(cppdiag::Severity_header const header, auto& context)
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
