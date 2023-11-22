#pragma once

#include <cstdint>
#include <compare>

#include <vector>
#include <optional>

#include <format>
#include <string>
#include <string_view>

namespace cppdiag {

    struct Message_string {
        std::size_t offset {};
        std::size_t length {};
    };

    struct Color {
        std::string_view code;
    };

    struct Colors {
        Color normal {};
        Color error {};
        Color warning {};
        Color hint {};
        Color information {};
        Color position {};

        static auto defaults() noexcept -> Colors;
    };

    struct Position {
        std::size_t line   = 1;
        std::size_t column = 1;

        auto operator<=>(Position const&) const = default;
    };

    enum class Severity {
        error,
        warning,
        hint,
        information,
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

    class Context {
        std::string m_message_buffer;
    public:
        // Format `diagnostic` to `output` according to `colors`.
        auto format_diagnostic(
            Diagnostic const& diagnostic,
            std::string&      output,
            Colors            colors = Colors::defaults()) const -> void;

        // Format `diagnostic` to a new string according to `colors`.
        auto format_diagnostic(
            Diagnostic const& diagnostic, Colors colors = Colors::defaults()) const -> std::string;

        auto message(std::string_view) -> Message_string;

        auto vformat_message(std::string_view, std::format_args) -> Message_string;

        template <class... Args>
        auto format_message(std::format_string<Args...> const fmt, Args&&... args) -> Message_string
        {
            return vformat_message(fmt.get(), std::make_format_args(args...));
        }
    };

} // namespace cppdiag
