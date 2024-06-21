#include <cppdiag/cppdiag.hpp>
#include <cppunittest/unittest.hpp>

#define TEST(name) UNITTEST("cppdiag interface: " name)

static constexpr cppdiag::Colors mock_colors {
    .normal      = { "[nrm]" },
    .error       = { "[err]" },
    .warning     = { "[wrn]" },
    .hint        = { "[hnt]" },
    .information = { "[inf]" },
    .position    = { "[pos]" },
};

TEST("format diagnostic without text sections")
{
    cppdiag::Diagnostic const diagnostic {
        .text_sections = {},
        .message       = "qwe123rty",
        .help_note     = "hello",
        .severity      = cppdiag::Severity::warning,
    };
    REQUIRE_EQUAL(
        cppdiag::format_diagnostic(diagnostic, mock_colors),
        "[wrn]Warning:[nrm] qwe123rty\n\nhello\n");
}

TEST("format diagnostic with a one-line text section")
{
    cppdiag::Diagnostic const diagnostic {
        .text_sections { cppdiag::Text_section {
            .source_string  = "abc def ghi",
            .source_name    = "mock source",
            .start_position = { .line = 1, .column = 5 },
            .stop_position  = { .line = 1, .column = 7 },
            .note           = std::nullopt,
            .note_severity  = std::nullopt,
        } },
        .message   = "interesting message",
        .help_note = "helpful note",
        .severity  = cppdiag::Severity::error,
    };
    REQUIRE_EQUAL(
        cppdiag::format_diagnostic(diagnostic, mock_colors),
        "[err]Error:[nrm] interesting message\n\n"
        "[pos]  --> mock source:1:5-1:7[nrm]\n\n"
        "[pos] 1 |[nrm] abc def ghi\n"
        "         [err]^^^ Here[nrm]\n\n"
        "helpful note\n");
}

TEST("format diagnostic with multiple one-line text sections")
{
    cppdiag::Diagnostic const diagnostic {
        .text_sections {
            cppdiag::Text_section {
                .source_string  = "abc def ghi",
                .source_name    = "mock source",
                .start_position = { .line = 1, .column = 5 },
                .stop_position  = { .line = 1, .column = 7 },
                .note           = "qwerty",
                .note_severity  = std::nullopt,
            },
            cppdiag::Text_section {
                .source_string  = "abc def ghi",
                .source_name    = "mock source",
                .start_position = { .line = 1, .column = 1 },
                .stop_position  = { .line = 1, .column = 3 },
                .note           = "asdf",
                .note_severity  = cppdiag::Severity::warning,
            },
        },
        .message   = "interesting message",
        .help_note = "helpful note",
        .severity  = cppdiag::Severity::error,
    };
    REQUIRE_EQUAL(
        cppdiag::format_diagnostic(diagnostic, mock_colors),
        "[err]Error:[nrm] interesting message\n\n"
        "[pos]  --> mock source:1:5-1:7[nrm]\n\n"
        "[pos] 1 |[nrm] abc def ghi\n"
        "         [err]^^^ qwerty[nrm]\n\n"
        "[pos]  --> mock source:1:1-1:3[nrm]\n\n"
        "[pos] 1 |[nrm] abc def ghi\n"
        "     [wrn]^^^ asdf[nrm]\n\n"
        "helpful note\n");
}
