#include <cppdiag.hpp>
#include <catch2/catch_test_macros.hpp>

#define TEST(name) TEST_CASE("cppdiag internal " name, "[cppdiag]")

TEST("get_relevant_lines")
{
    using Lines = std::vector<std::string_view>;
    REQUIRE(
        cppdiag::internal::get_relevant_lines("hello, world!", { 1, 2 }, { 1, 5 })
        == Lines { "hello, world!" });
    REQUIRE(
        cppdiag::internal::get_relevant_lines("aaa\nbbb\nccc", { 1, 1 }, { 1, 2 })
        == Lines { "aaa" });
    REQUIRE(
        cppdiag::internal::get_relevant_lines("aaa\nbbb\nccc", { 2, 1 }, { 2, 2 })
        == Lines { "bbb" });
    REQUIRE(
        cppdiag::internal::get_relevant_lines("aaa\nbbb\nccc", { 3, 1 }, { 3, 2 })
        == Lines { "ccc" });
    REQUIRE(
        cppdiag::internal::get_relevant_lines("aaa\nbbb\nccc\nddd\neee", { 1, 1 }, { 5, 2 })
        == Lines { "aaa", "bbb", "ccc", "ddd", "eee" });
    REQUIRE(
        cppdiag::internal::get_relevant_lines("aaa\nbbb\nccc\nddd\neee", { 1, 1 }, { 3, 2 })
        == Lines { "aaa", "bbb", "ccc" });
    REQUIRE(
        cppdiag::internal::get_relevant_lines("aaa\nbbb\nccc\nddd\neee", { 3, 1 }, { 5, 2 })
        == Lines { "ccc", "ddd", "eee" });
}
