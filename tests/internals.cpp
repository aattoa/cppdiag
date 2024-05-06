#include <cppdiag/cppdiag.hpp>
#include <cppdiag/internals.hpp>
#include <cppunittest/unittest.hpp>

#define TEST(name) UNITTEST("cppdiag internal: " name)

TEST("digit_count")
{
    REQUIRE_EQUAL(1uz, cppdiag::internal::digit_count(0));
    REQUIRE_EQUAL(1uz, cppdiag::internal::digit_count(9));
    REQUIRE_EQUAL(2uz, cppdiag::internal::digit_count(10));
    REQUIRE_EQUAL(2uz, cppdiag::internal::digit_count(99));
    REQUIRE_EQUAL(3uz, cppdiag::internal::digit_count(100));
    REQUIRE_EQUAL(3uz, cppdiag::internal::digit_count(999));
}

TEST("whitespace_prefix_length")
{
    REQUIRE_EQUAL(0uz, cppdiag::internal::whitespace_prefix_length(""));
    REQUIRE_EQUAL(0uz, cppdiag::internal::whitespace_prefix_length("string"));
    REQUIRE_EQUAL(2uz, cppdiag::internal::whitespace_prefix_length("  string"));
}

TEST("whitespace_suffix_length")
{
    REQUIRE_EQUAL(0uz, cppdiag::internal::whitespace_suffix_length(""));
    REQUIRE_EQUAL(0uz, cppdiag::internal::whitespace_suffix_length("string"));
    REQUIRE_EQUAL(2uz, cppdiag::internal::whitespace_suffix_length("string  "));
}

TEST("find_nth_newline")
{
    auto const find = [](std::string_view const string, std::size_t const n) {
        auto const it = cppdiag::internal::find_nth_newline(string.begin(), string.end(), n);
        return std::distance(string.begin(), it);
    };
    REQUIRE_EQUAL(5, find("hello", 0));
    REQUIRE_EQUAL(5, find("hello", 1));
    REQUIRE_EQUAL(5, find("hello\n", 0));
    REQUIRE_EQUAL(6, find("hello\n", 1));
    REQUIRE_EQUAL(5, find("hello\nworld", 0));
    REQUIRE_EQUAL(11, find("hello\nworld", 1));
    REQUIRE_EQUAL(3, find("hel\nlo\nwor\nld", 0));
    REQUIRE_EQUAL(6, find("hel\nlo\nwor\nld", 1));
    REQUIRE_EQUAL(10, find("hel\nlo\nwor\nld", 2));
    REQUIRE_EQUAL(13, find("hel\nlo\nwor\nld", 3));
}

TEST("relevant_lines")
{
    using Lines = std::vector<std::string_view>;
    REQUIRE_EQUAL(
        cppdiag::internal::relevant_lines("abc def", { 1, 2 }, { 1, 5 }), Lines { "abc def" });
    REQUIRE_EQUAL(
        cppdiag::internal::relevant_lines("aaa\nbbb\nccc", { 1, 1 }, { 1, 2 }), Lines { "aaa" });
    REQUIRE_EQUAL(
        cppdiag::internal::relevant_lines("aaa\nbbb\nccc", { 2, 1 }, { 2, 2 }), Lines { "bbb" });
    REQUIRE_EQUAL(
        cppdiag::internal::relevant_lines("aaa\nbbb\nccc", { 3, 1 }, { 3, 2 }), Lines { "ccc" });
    REQUIRE_EQUAL(
        cppdiag::internal::relevant_lines("aaa\nbbb\nccc\nddd\neee", { 1, 1 }, { 5, 2 }),
        Lines { "aaa", "bbb", "ccc", "ddd", "eee" });
    REQUIRE_EQUAL(
        cppdiag::internal::relevant_lines("aaa\nbbb\nccc\nddd\neee", { 1, 1 }, { 3, 2 }),
        Lines { "aaa", "bbb", "ccc" });
    REQUIRE_EQUAL(
        cppdiag::internal::relevant_lines("aaa\nbbb\nccc\nddd\neee", { 3, 1 }, { 5, 2 }),
        Lines { "ccc", "ddd", "eee" });
}

TEST("strip_surrounding_whitespace")
{
    using Lines = std::vector<std::string_view>;
    REQUIRE_EQUAL(
        cppdiag::internal::strip_surrounding_whitespace(Lines {
            "aaa ",
            " bbb",
        }),
        Lines {
            "aaa",
            " bbb",
        });
    REQUIRE_EQUAL(
        cppdiag::internal::strip_surrounding_whitespace(Lines {
            "  aaa  ",
            " bbb   ",
        }),
        Lines {
            " aaa",
            "bbb",
        });
    REQUIRE_EQUAL(
        cppdiag::internal::strip_surrounding_whitespace(Lines {
            "      aaa      ",
            "   bbb     ",
        }),
        Lines {
            "   aaa",
            "bbb",
        });
    REQUIRE_EQUAL(
        cppdiag::internal::strip_surrounding_whitespace(Lines {
            "",
            "   bbb     ",
        }),
        Lines {
            "",
            "bbb",
        });
}
