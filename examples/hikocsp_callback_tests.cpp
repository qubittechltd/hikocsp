#line 1 "/Users/mac/projects/hikocsp-main/examples//hikocsp_callback_tests.cpp.csp"
#line 1
// Copyright Take Vos 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "hikocsp/generator.hpp"
#include <gtest/gtest.h>
#include <fmt/core.h>
#include <fmt/core.h>

template<typename Callback>
constexpr void callback_page(std::vector<int> list, int a, Callback const &sink) noexcept
{
#line 12
sink("\n"
  "foo\n");
#line 14
for (auto x: list) {
#line 15
sink("x=");
#line 15
sink(fmt::format(("{}"), (x + a)));
#line 15
sink("\x24");
#line 15
sink(", ");
#line 15

#line 16
}
#line 17
sink("bar\n");
#line 18
}

TEST(callback_example, callback_page)
{
    auto result = std::string{};
    callback_page(std::vector{1, 2, 3}, 5, [&result](std::string_view x) { result += x; });

    auto const expected = std::string{
        "\nfoo\n"
        "x=6$, x=7$, x=8$, "
        "bar\n"
    };

    ASSERT_EQ(result, expected);
}
