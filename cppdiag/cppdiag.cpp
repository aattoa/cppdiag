#include <cppdiag.hpp>

auto cppdiag::Colors::defaults() noexcept -> Colors
{
    return Colors {
        .normal        = Color { .code = "\033[0m" },
        .error         = Color { .code = "\033[91m" },
        .warning       = Color { .code = "\033[33m" },
        .note          = Color { .code = "\033[96m" },
        .position_info = Color { .code = "\033[36m" },
    };
}

auto cppdiag::Context::message(std::string_view const string) -> Message_string
{
    return format_message("{}", string);
}

auto cppdiag::Context::vformat_message(std::string_view const fmt, std::format_args const args)
    -> Message_string
{
    std::size_t const offset = m_message_buffer.size();
    std::vformat_to(std::back_inserter(m_message_buffer), fmt, args);
    return Message_string { .offset = offset, .length = m_message_buffer.size() - offset };
}
