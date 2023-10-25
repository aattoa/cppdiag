#include <cppdiag.hpp>
#include <catch2/catch_test_macros.hpp>

#define TEST(name) TEST_CASE("cppdiag interface " name, "[cppdiag]")

namespace {
    constexpr cppdiag::Colors mock_colors {
        .normal        = { .code = "[norm]" },
        .error         = { .code = "[erro]" },
        .warning       = { .code = "[warn]" },
        .note          = { .code = "[note]" },
        .position_info = { .code = "[info]" },
    };
}

TEST("format diagnostic without text sections")
{
    cppdiag::Context          context;
    cppdiag::Diagnostic const diagnostic {
        .text_sections = {},
        .message       = context.format_message("qwe{}rty", 123),
        .help_note     = context.format_message("hello"),
        .level         = cppdiag::Level::warning,
    };
    REQUIRE(
        context.format_diagnostic(diagnostic, mock_colors)
        == "[warn]Warning:[norm] qwe123rty\n\nhello\n");
}

TEST("format diagnostic with a one-line text section")
{
    cppdiag::Context          context;
    cppdiag::Diagnostic const diagnostic {
        .text_sections { cppdiag::Text_section {
            .source_string  = "abc def ghi",
            .source_name    = "mock source",
            .start_position = { .line = 1, .column = 5 },
            .stop_position  = { .line = 1, .column = 7 },
            .note           = std::nullopt,
            .note_color     = std::nullopt,
        } },
        .message   = context.format_message("interesting message"),
        .help_note = context.format_message("helpful note"),
        .level     = cppdiag::Level::error,
    };
    REQUIRE(
        context.format_diagnostic(diagnostic, mock_colors)
        == "[erro]Error:[norm] interesting message\n\n"
           "[info]  --> mock source:1:5-1:7[norm]\n\n"
           "[info] 1 |[norm] abc def ghi\n"
           "         [erro]^^^ Here[norm]\n\n"
           "helpful note\n");
}

TEST("format diagnostic with multiple one-line text sections")
{
    cppdiag::Context          context;
    cppdiag::Diagnostic const diagnostic {
        .text_sections {
            cppdiag::Text_section {
                .source_string  = "abc def ghi",
                .source_name    = "mock source",
                .start_position = { .line = 1, .column = 5 },
                .stop_position  = { .line = 1, .column = 7 },
                .note           = context.format_message("qwerty"),
                .note_color     = std::nullopt,
            },
            cppdiag::Text_section {
                .source_string  = "abc def ghi",
                .source_name    = "mock source",
                .start_position = { .line = 1, .column = 1 },
                .stop_position  = { .line = 1, .column = 3 },
                .note           = context.format_message("asdf"),
                .note_color     = mock_colors.warning,
            },
        },
        .message   = context.format_message("interesting message"),
        .help_note = context.format_message("helpful note"),
        .level     = cppdiag::Level::error,
    };
    REQUIRE(
        context.format_diagnostic(diagnostic, mock_colors)
        == "[erro]Error:[norm] interesting message\n\n"
           "[info]  --> mock source:1:5-1:7[norm]\n\n"
           "[info] 1 |[norm] abc def ghi\n"
           "         [erro]^^^ qwerty[norm]\n\n"
           "[info]  --> mock source:1:1-1:3[norm]\n\n"
           "[info] 1 |[norm] abc def ghi\n"
           "     [warn]^^^ asdf[norm]\n\n"
           "helpful note\n");
}
