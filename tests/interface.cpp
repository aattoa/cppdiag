#include <cppdiag.hpp>
#include <catch2/catch_test_macros.hpp>

#define TEST(name) TEST_CASE("cppdiag interface " name, "[cppdiag]")

namespace {
    constexpr cppdiag::Colors mock_colors {
        .normal      = { .code = "[nrm]" },
        .error       = { .code = "[err]" },
        .warning     = { .code = "[wrn]" },
        .hint        = { .code = "[hnt]" },
        .information = { .code = "[inf]" },
        .position    = { .code = "[pos]" },
    };
}

TEST("format diagnostic without text sections")
{
    cppdiag::Context          context;
    cppdiag::Diagnostic const diagnostic {
        .text_sections = {},
        .message       = context.format_message("qwe{}rty", 123),
        .help_note     = context.format_message("hello"),
        .severity      = cppdiag::Severity::warning,
    };
    REQUIRE(
        context.format_diagnostic(diagnostic, mock_colors)
        == "[wrn]Warning:[nrm] qwe123rty\n\nhello\n");
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
            .note_severity  = std::nullopt,
        } },
        .message   = context.format_message("interesting message"),
        .help_note = context.format_message("helpful note"),
        .severity  = cppdiag::Severity::error,
    };
    REQUIRE(
        context.format_diagnostic(diagnostic, mock_colors)
        == "[err]Error:[nrm] interesting message\n\n"
           "[pos]  --> mock source:1:5-1:7[nrm]\n\n"
           "[pos] 1 |[nrm] abc def ghi\n"
           "         [err]^^^ Here[nrm]\n\n"
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
                .note_severity  = std::nullopt,
            },
            cppdiag::Text_section {
                .source_string  = "abc def ghi",
                .source_name    = "mock source",
                .start_position = { .line = 1, .column = 1 },
                .stop_position  = { .line = 1, .column = 3 },
                .note           = context.format_message("asdf"),
                .note_severity  = cppdiag::Severity::warning,
            },
        },
        .message   = context.format_message("interesting message"),
        .help_note = context.format_message("helpful note"),
        .severity  = cppdiag::Severity::error,
    };
    REQUIRE(
        context.format_diagnostic(diagnostic, mock_colors)
        == "[err]Error:[nrm] interesting message\n\n"
           "[pos]  --> mock source:1:5-1:7[nrm]\n\n"
           "[pos] 1 |[nrm] abc def ghi\n"
           "         [err]^^^ qwerty[nrm]\n\n"
           "[pos]  --> mock source:1:1-1:3[nrm]\n\n"
           "[pos] 1 |[nrm] abc def ghi\n"
           "     [wrn]^^^ asdf[nrm]\n\n"
           "helpful note\n");
}
