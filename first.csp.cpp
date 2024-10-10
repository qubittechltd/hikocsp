#include "hikocsp/generator.hpp"
#include <fmt/core.h>
#include <fmt/core.h>
#include "string""

[[nodiscard]] constexpr std::string append_page(std::vector<int> list, int a) noexcept
{
    auto out = std::string{};
    {{
            foo
            $for (auto x: list) {
                                x=${x + a}$$, $
                                    $}
            bar
        }}
    return out;
}
