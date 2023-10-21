#include <cppdiag.hpp>
#include <catch2/catch_test_macros.hpp>

#define TEST(name) TEST_CASE("cppdiag interface " name, "[cppdiag]")

TEST("diagnostic formatting")
{
    cppdiag::Context          context;
    cppdiag::Diagnostic const diagnostic {
        .text_sections = {},
        .message       = context.format_message("qwerty {}", "asdf"),
        .help_note     = context.format_message("hello"),
        .level         = cppdiag::Level::warning,
    };
    cppdiag::Colors const colors {
        .normal        = { .code = "[normal]" },
        .error         = { .code = "[error]" },
        .warning       = { .code = "[warning]" },
        .note          = { .code = "[note]" },
        .position_info = { .code = "[info]" },
    };
    REQUIRE(
        context.format_diagnostic(diagnostic, colors)
        == "[warning]Warning:[normal] qwerty asdf\n\nhello");
}
