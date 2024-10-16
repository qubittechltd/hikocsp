hikocsp - C++ Server Pages (CSP)
================================
The introduction of `auto`, ranged based `for`-loops and `fmt::format()`
makes the C++ language appropriate as a host language for text-templates;
similar to PHP and JSP (Java Server Pages).

Here is an example of a CSP file.
- It starts in C++ mode.
- Template-mode is entered at `{{` and exits at `}}`.
- Placeholders `${}` are used to format C++ expressions.
- A single C++ line starts with a `$` until the end of the line.
- Filtering placeholders is done using the `` ` `` backtick inside a
  placeholder. A placeholder with only filters sets the default filters.

```
#include <string>
#include <fmt/core.h>
#include <vector>
#include <generator>
#include "escape_functions.hpp"

[[nodiscard]] std::generator<std::string> generate_page(std::vector<int> list, int b) noexcept
{{{
${`sgml_escape`email_escape}
    <html>
        <head><title>Page</title></head>
        <body>
        A list of values.
            <li>
            $for (auto value: list) {
                <ul><a href="value_page?${value `url_escape}">${value} + ${b} = ${value + b}</a></ul>
            $}
            <li>
            escape double close bracket: ${"}}"}
            escape dollar: ${"$"}
        </body>
    </html> 
}}}
```

hikocsp.exe usage
-----------------
```
hikocsp [ options ] filename.csp
hikocsp [ options ] --input=filename.csp
hikocsp --help
```

  Argument                 | Description
 :------------------------ |:-----------------------
  \-h, \-\-help            | Print the help page to stderr.
  \-o, \-\-output=\<path\> | The filename of the result.
  \-i, \-\-input=\<path\>  | The filename of the result. Otherwise stdout is used.
  \-\-append=\<name\>      | Generate code that appends text to the `name` variable.
  \-\-callback=\<name\>    | Generate code that passed text to the callback function `name()`.
  \-\-disable-line         | Disable generation of #line directives.
  
CSP Template format
-------------------

### Verbatim C++
The template starts in verbatim C++ mode. Verbatim C++ code will be
copied directly into the generated code.

### Text
A text block is started when `{{` is found in verbatim C++ code.
If there are more than two consecutive open braces, then the last
two braces count as the terminator and the previous open braces are still part
of the verbatim C++ code.

The text block is terminated when `}}` is found. If more than two consecutive
close braces are found then the first two braces count as the terminator
and the subsequent close braces are part of the verbatim C++ code.

The generated code will use `co_yield` to output the text from the current
co-routine.
  
### placeholder
There are several versions of placeholders:
 - **Empty:** `${}`
 - **Escape:** `${` *string-literal* `}`
 - **Simple:** `${` *expression* ( `` ` `` *filter* )\* `}`
 - **Format:** `${` *format-string* ( `,` *expression* )+ ( `` ` `` *filter* )\* `}`
 - **Filter:** `${` `` ` `` *filter* ( `` ` `` *filter* )\* `}`

The **empty-placeholder** does nothing. It was added so that a placeholder can
be placed during development of a template without having any side effects.

The **escape-placeholder** allows the placement of special character sequences,
for example: `${"$"}` or `${"}}"}`. This is a special case of the
**simple-placeholder** where the text is not passed through any filters
to allow the implementation to perform extra optimizations.
The string-literal must start and end in a double quote `"` without any
white-spaces.

The **simple-placeholder** is syntactic sugar for the following
**format-placeholder**:
 - `${"{}",` *expression* ( `` ` `` *filter* )\* `}`

The **format-placeholder** formats one or more expression using a format-string
for *fmt::format()*. Then the result is passed through each filter in
left-to-right order. The result is then returned from the co-routine using
*co_yield*. If no explicit filters are specified then the default filters
are used.

The **filter-placeholder** replaces the default filters to be used in subsequent
placeholders that do not have explicit filters specified.

C++ expressions (including *expression*, *format-string* and *filter*) are
terminated when one of the following characters appears outside
of a sub-expression or string-literal:
 -  `,`, `` ` ``, `}`, `)`, `]`, `$` or `@`.

Filters are called with a single *std::string* argument and should return a
*std::string* argument. The filter expression in a placeholder may be any
C++ expression which resolves into a callable object. An empty filter expression
(which consists of just the leading back-tick `` ` ``) is replaced with the
following lambda:
 - `[](auto const &x) { return x; }`.

### Escape dollar
To escape a dollar, use a double dollar `$$`.

### C++ line
A single line of verbatim C++ code can be use inside a text-block.
It starts with a `$` and ends in a line-feed `\n`.
The line of C++ code is directly copied into the generated code.

A C++ line can't start with a open-brace `{`  or dollar `$` character. If you
need to use the open-brace then use an extra white space `$ {` to
differentiate from a placeholder that starts with `${`.

### Supress line-feed
The C++ line can also be used to consume the new-line character at the end
of a piece of text to control when line-feeds that will appear in the output.
For example to generate a sequence of numbers on a single line:

```
List of numbers: $
$for (auto i = 0; i != 10; ++i) {
${i}, $
$}
```

Technically the `$` at the end-of-line will be interpreted as a verbatim C++
line.

Syntax
------

We are using the following rules for the BNF grammar below:
 - `*`: zero or more.
 - `+`: once or more.
 - `?`: zero or once.
 - `!`: once, but do not consume.
 - `|`: Or
 - `(` `)`: Group.
 - `'` `'`: Literal.

```
template := ( verbatim | text )*

#
# Verbatim C++ code is terminated when '{{' is found at the end of
# a sequence of end-braces outside of a string-literal.
#
verbatim := CHAR+

text :=  '{{' ( CHAR+ | verbatim-line | placeholder | escape )* '}}'

verbatim-line := '$' CHAR* '\n'

escape := '$$'

placeholder := '${' ( expression ( ',' expression )* )? ( '`' expression )* '}'

#
# A C++ expression is terminated when an end-expression character is
# found outside of a sub-expression or string-literal.
#
expression := CHAR* end-expression!
end-expression := '$' | '`' | '@' | ',' | ']' | ')' | '}'
```
