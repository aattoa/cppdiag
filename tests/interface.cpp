#include <cppdiag/cppdiag.hpp>
#include <cppunittest/unittest.hpp>

#define TEST(name) UNITTEST("cppdiag interface: " name)

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
    cppdiag::Message_buffer   message_buffer;
    cppdiag::Diagnostic const diagnostic {
        .text_sections = {},
        .message       = cppdiag::format_message(message_buffer, "qwe{}rty", 123),
        .help_note     = cppdiag::format_message(message_buffer, "hello"),
        .severity      = cppdiag::Severity::warning,
    };
    REQUIRE_EQUAL(
        cppdiag::format_diagnostic(diagnostic, message_buffer, mock_colors),
        "[wrn]Warning:[nrm] qwe123rty\n\nhello\n");
}

TEST("format diagnostic with a one-line text section")
{
    cppdiag::Message_buffer   message_buffer;
    cppdiag::Diagnostic const diagnostic {
        .text_sections { cppdiag::Text_section {
            .source_string  = "abc def ghi",
            .source_name    = "mock source",
            .start_position = { .line = 1, .column = 5 },
            .stop_position  = { .line = 1, .column = 7 },
            .note           = std::nullopt,
            .note_severity  = std::nullopt,
        } },
        .message   = cppdiag::format_message(message_buffer, "interesting message"),
        .help_note = cppdiag::format_message(message_buffer, "helpful note"),
        .severity  = cppdiag::Severity::error,
    };
    REQUIRE_EQUAL(
        cppdiag::format_diagnostic(diagnostic, message_buffer, mock_colors),
        "[err]Error:[nrm] interesting message\n\n"
        "[pos]  --> mock source:1:5-1:7[nrm]\n\n"
        "[pos] 1 |[nrm] abc def ghi\n"
        "         [err]^^^ Here[nrm]\n\n"
        "helpful note\n");
}

TEST("format diagnostic with multiple one-line text sections")
{
    cppdiag::Message_buffer   message_buffer;
    cppdiag::Diagnostic const diagnostic {
        .text_sections {
            cppdiag::Text_section {
                .source_string  = "abc def ghi",
                .source_name    = "mock source",
                .start_position = { .line = 1, .column = 5 },
                .stop_position  = { .line = 1, .column = 7 },
                .note           = cppdiag::format_message(message_buffer, "qwerty"),
                .note_severity  = std::nullopt,
            },
            cppdiag::Text_section {
                .source_string  = "abc def ghi",
                .source_name    = "mock source",
                .start_position = { .line = 1, .column = 1 },
                .stop_position  = { .line = 1, .column = 3 },
                .note           = cppdiag::format_message(message_buffer, "asdf"),
                .note_severity  = cppdiag::Severity::warning,
            },
        },
        .message   = cppdiag::format_message(message_buffer, "interesting message"),
        .help_note = cppdiag::format_message(message_buffer, "helpful note"),
        .severity  = cppdiag::Severity::error,
    };
    REQUIRE_EQUAL(
        cppdiag::format_diagnostic(diagnostic, message_buffer, mock_colors),
        "[err]Error:[nrm] interesting message\n\n"
        "[pos]  --> mock source:1:5-1:7[nrm]\n\n"
        "[pos] 1 |[nrm] abc def ghi\n"
        "         [err]^^^ qwerty[nrm]\n\n"
        "[pos]  --> mock source:1:1-1:3[nrm]\n\n"
        "[pos] 1 |[nrm] abc def ghi\n"
        "     [wrn]^^^ asdf[nrm]\n\n"
        "helpful note\n");
}
