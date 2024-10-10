#include <string>
#include <fmt/core.h>
#include <vector>
#include <generator>
#include "escape_functions.hpp"

[[nodiscard]] std::generator<std::string> generate_page(std::vector<int> list, int b) noexcept
{
co_yield "\n";
co_yield "\n"
  "    <html>\n"
  "        <head><title>Page</title></head>\n"
  "        <body>\n"
  "        A list of values.\n"
  "            <li>\n"
  "            ";
for (auto value: list) {
co_yield "                <ul><a href=\"value_page?";
co_yield (url_escape)(fmt::format(("{}"), (value )));
co_yield "\">";
co_yield (email_escape)((sgml_escape)(fmt::format(("{}"), (value))));
co_yield " + ";
co_yield (email_escape)((sgml_escape)(fmt::format(("{}"), (b))));
co_yield " = ";
co_yield (email_escape)((sgml_escape)(fmt::format(("{}"), (value + b))));
co_yield "</a></ul>\n"
  "            ";
}
co_yield "            <li>\n"
  "            escape double close bracket: ";
co_yield "}}";
co_yield "\n"
  "            escape dollar: ";
co_yield "$";
co_yield "\n"
  "        </body>\n"
  "    </html>\n";
}
