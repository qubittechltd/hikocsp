#line 1 "/Users/mac/projects/hikocsp-main/examples//hikocsp_yield_tests.cpp.csp"
#line 1
// Copyright Take Vos 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "hikocsp/generator.hpp"
#include <gtest/gtest.h>
#include <fmt/core.h>
#include <fmt/core.h>

[[nodiscard]] constexpr std::string reverse_filter(std::string str) noexcept
{
    std::reverse(str.begin(), str.end());
    return str;
}

[[nodiscard]] constexpr std::string duplicate_filter(std::string str) noexcept
{
    return str + str;
}

[[nodiscard]] csp::generator<std::string> yield_page(std::vector<int> list, int a) noexcept
{
#line 22
co_yield "\n"
  "foo\n";
#line 24
for (auto x: list) {
#line 25
co_yield "x + a = ";
#line 25
co_yield (reverse_filter)(fmt::format(("{}"), (x)));
#line 25
co_yield " + ";
#line 25
co_yield ([](auto const &x){return x;})(fmt::format(("{}"), (a)));
#line 25
co_yield " = ";
#line 25
co_yield (duplicate_filter)(fmt::format(("{}"), (x + a)));
#line 25
co_yield ",\n";
#line 26
}
#line 27
co_yield "bar\n";
#line 28
}

TEST(yield_example, yield_page)
{
    auto result = std::string{};
    for (auto const &s: yield_page(std::vector{12, 34, 56}, 42)) {
        result += s;
    }

    auto const expected = std::string{
        "\nfoo\n"
        "x + a = 21 + 42 = 5454,\n"
        "x + a = 43 + 42 = 7676,\n"
        "x + a = 65 + 42 = 9898,\n"
        "bar\n"
    };

    ASSERT_EQ(result, expected);
}
