#include <cppdiag.hpp>
#include <internals.hpp>
#include <catch2/catch_test_macros.hpp>

#define TEST(name) TEST_CASE("cppdiag internal " name, "[cppdiag]")

TEST("digit_count")
{
    REQUIRE(cppdiag::internal::digit_count(0) == 1);
    REQUIRE(cppdiag::internal::digit_count(9) == 1);
    REQUIRE(cppdiag::internal::digit_count(10) == 2);
    REQUIRE(cppdiag::internal::digit_count(99) == 2);
    REQUIRE(cppdiag::internal::digit_count(100) == 3);
    REQUIRE(cppdiag::internal::digit_count(999) == 3);
}

TEST("whitespace_prefix_length")
{
    REQUIRE(cppdiag::internal::whitespace_prefix_length("") == 0);
    REQUIRE(cppdiag::internal::whitespace_prefix_length("string") == 0);
    REQUIRE(cppdiag::internal::whitespace_prefix_length("  string") == 2);
}

TEST("whitespace_suffix_length")
{
    REQUIRE(cppdiag::internal::whitespace_suffix_length("") == 0);
    REQUIRE(cppdiag::internal::whitespace_suffix_length("string") == 0);
    REQUIRE(cppdiag::internal::whitespace_suffix_length("string  ") == 2);
}

TEST("find_nth_newline")
{
    auto const find = [](std::string_view const string, std::size_t const n) {
        auto const it = cppdiag::internal::find_nth_newline(string.begin(), string.end(), n);
        return std::distance(string.begin(), it);
    };
    REQUIRE(find("hello", 0) == 5);
    REQUIRE(find("hello", 1) == 5);
    REQUIRE(find("hello\n", 0) == 5);
    REQUIRE(find("hello\n", 1) == 6);
    REQUIRE(find("hello\nworld", 0) == 5);
    REQUIRE(find("hello\nworld", 1) == 11);
    REQUIRE(find("hel\nlo\nwor\nld", 0) == 3);
    REQUIRE(find("hel\nlo\nwor\nld", 1) == 6);
    REQUIRE(find("hel\nlo\nwor\nld", 2) == 10);
    REQUIRE(find("hel\nlo\nwor\nld", 3) == 13);
}

TEST("relevant_lines")
{
    using Lines = std::vector<std::string_view>;
    REQUIRE(
        cppdiag::internal::relevant_lines("abc def", { 1, 2 }, { 1, 5 }) == Lines { "abc def" });
    REQUIRE(
        cppdiag::internal::relevant_lines("aaa\nbbb\nccc", { 1, 1 }, { 1, 2 }) == Lines { "aaa" });
    REQUIRE(
        cppdiag::internal::relevant_lines("aaa\nbbb\nccc", { 2, 1 }, { 2, 2 }) == Lines { "bbb" });
    REQUIRE(
        cppdiag::internal::relevant_lines("aaa\nbbb\nccc", { 3, 1 }, { 3, 2 }) == Lines { "ccc" });
    REQUIRE(
        cppdiag::internal::relevant_lines("aaa\nbbb\nccc\nddd\neee", { 1, 1 }, { 5, 2 })
        == Lines { "aaa", "bbb", "ccc", "ddd", "eee" });
    REQUIRE(
        cppdiag::internal::relevant_lines("aaa\nbbb\nccc\nddd\neee", { 1, 1 }, { 3, 2 })
        == Lines { "aaa", "bbb", "ccc" });
    REQUIRE(
        cppdiag::internal::relevant_lines("aaa\nbbb\nccc\nddd\neee", { 3, 1 }, { 5, 2 })
        == Lines { "ccc", "ddd", "eee" });
}

TEST("strip_surrounding_whitespace")
{
    using Lines = std::vector<std::string_view>;
    REQUIRE(
        cppdiag::internal::strip_surrounding_whitespace(Lines {
            "aaa ",
            " bbb",
        })
        == Lines {
            "aaa",
            " bbb",
        });
    REQUIRE(
        cppdiag::internal::strip_surrounding_whitespace(Lines {
            "  aaa  ",
            " bbb   ",
        })
        == Lines {
            " aaa",
            "bbb",
        });
    REQUIRE(
        cppdiag::internal::strip_surrounding_whitespace(Lines {
            "      aaa      ",
            "   bbb     ",
        })
        == Lines {
            "   aaa",
            "bbb",
        });
    REQUIRE(
        cppdiag::internal::strip_surrounding_whitespace(Lines {
            "",
            "   bbb     ",
        })
        == Lines {
            "",
            "bbb",
        });
}
