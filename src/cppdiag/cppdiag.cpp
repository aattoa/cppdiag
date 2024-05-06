#include <cppdiag/internals.hpp>
#include <cppdiag/cppdiag.hpp>

auto cppdiag::Colors::for_severity(Severity const severity) const noexcept -> Color
{
    switch (severity) {
    case cppdiag::Severity::error:
        return error;
    case cppdiag::Severity::warning:
        return warning;
    case cppdiag::Severity::hint:
        return hint;
    case cppdiag::Severity::information:
        return information;
    default:
        cpputil::unreachable();
    }
}

auto cppdiag::Colors::defaults() noexcept -> Colors
{
    return Colors {
        .normal      = Color { .code = "\033[0m" },
        .error       = Color { .code = "\033[31m" },
        .warning     = Color { .code = "\033[33m" },
        .hint        = Color { .code = "\033[32m" },
        .information = Color { .code = "\033[34m" },
        .position    = Color { .code = "\033[36m" },
    };
}

auto cppdiag::Colors::none() noexcept -> Colors
{
    return Colors {};
}

auto cppdiag::Severity_header::make(Severity const severity, Colors const& colors) noexcept
    -> Severity_header
{
    return {
        .severity_color  = colors.for_severity(severity),
        .restore_color   = colors.normal,
        .severity_string = internal::severity_string(severity),
    };
}

auto cppdiag::vformat_message(
    cppdiag::Message_buffer& message_buffer,
    std::string_view const   fmt,
    std::format_args const   args) -> Message_string
{
    std::size_t const offset = message_buffer.string.size();
    std::vformat_to(std::back_inserter(message_buffer.string), fmt, args);
    return Message_string { .offset = offset, .length = message_buffer.string.size() - offset };
}
