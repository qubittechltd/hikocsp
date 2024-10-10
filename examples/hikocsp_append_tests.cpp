#line 1 "/Users/mac/projects/hikocsp-main/examples//hikocsp_append_tests.cpp.csp"
#line 1
// Copyright Take Vos 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "hikocsp/generator.hpp"
#include <gtest/gtest.h>
#include <fmt/core.h>
#include <fmt/core.h>

[[nodiscard]] constexpr std::string append_page(std::vector<int> list, int a) noexcept
{
    auto out = std::string{};
#line 13
out += "\n"
  "foo\n";
#line 15
for (auto x: list) {
#line 16
out += "x=";
#line 16
out += fmt::format(("{}"), (x + a));
#line 16
out += "\x24";
#line 16
out += ", ";
#line 16

#line 17
}
#line 18
out += "bar\n";
#line 19

    return out;
}

TEST(append_example, append_page)
{
    auto result = append_page(std::vector{1, 2, 3}, 5);

    auto const expected = std::string{
        "\nfoo\n"
        "x=6$, x=7$, x=8$, "
        "bar\n"
    };

    ASSERT_EQ(result, expected);
}
