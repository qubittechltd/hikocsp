
#include "src/hikocsp/csp_parser.hpp"
#include "src/hikocsp/csp_translator.hpp"
#include "src/hikocsp/option_parser.hpp"
#include <iostream>
#include <fstream>
#include <fmt/core.h>
#include <iostream>
#include <string>
#include <hikocsp/csp_token.hpp>
#include <hikocsp/generator.hpp>

inline int verbose = 0;
inline std::filesystem::path output_path = {};
inline std::filesystem::path input_path = {};
inline bool enable_line = true;
inline std::optional<std::string> callback_name = std::nullopt;
inline std::optional<std::string> append_name = std::nullopt;

void print_help()
{
    std::cerr << fmt::format(
        "hikocsp is an application to translate a CSP template into C++ code.\n"
        "\n"
        "Synopsys:\n"
        "  hikocsp --help\n"
        "  hikocsp [ <options> ] <path>\n"
        "  hikocsp [ <options> ] --input=<path>\n"
        "\n"
        "Options:\n"
        "  -h, --help          Show help and exit.\n"
        "  -v, --verbose       Increase verbosity level.\n"
        "  -i, --input=<path>  The path to the template file.\n"
        "  -o, --output=<path> The path to the generated code.\n"
        "  --callback=<name>   Use a callback function to sink template-text.\n"
        "  --append=<name>     Use a variable to append template-text to.\n"
        "  --disable-line      Disable generation of #line directives.\n"           
        "\n"
        "If the output-path is not specified it is constructed from the\n"
        "input-path after removing the extension.\n"
        "\n"
        "By default the generated code will co_yield the template-text.\n"
        "You may also use the --callback or --append option to change the\n"
        "way template-text is passed to the caller of the template generating\n"
        "function.\n");
}

int parse_options(int argc, char *argv[])
{
    auto options = csp::parse_options(argc, argv, "io");

    for (auto& option : options.options) {
        if (option == "-h" or option == "--help") {
            return 1;

        } else if (option == "-v" or option == "--verbose") {
            ++verbose;

        } else if (option == "-o" or option == "--output") {
            if (option.argument) {
                output_path = *option.argument;
            } else {
                std::cerr << fmt::format("Missing argument for : {}\n", to_string(option));
                return -1;
            }

        } else if (option == "-i" or option == "--input") {
            if (option.argument) {
                input_path = *option.argument;
            } else {
                std::cerr << fmt::format("Missing argument for : {}\n", to_string(option));
                return -1;
            }

        } else if (option == "--disable-line") {
            if (not option.argument) {
                enable_line = false;
            } else {
                std::cerr << fmt::format("Unexpected argument for : {}\n", to_string(option));
                return -1;
            }

        } else if (option == "--callback") {
            if (option.argument) {
                callback_name = *option.argument;
            } else {
                std::cerr << fmt::format("Missing argument for : {}\n", to_string(option));
                return -1;
            }

        } else if (option == "--append") {
            if (option.argument) {
                append_name = *option.argument;
            } else {
                std::cerr << fmt::format("Missing argument for : {}\n", to_string(option));
                return -1;
            }

        } else {
            std::cerr << fmt::format("Unknown option: {}\n", to_string(option));
            return -1;
        }
    }

    if (input_path.empty()) {
        if (options.arguments.size() == 1) {
            input_path = options.arguments.front();
        } else {
            std::cerr << fmt::format("Expecting a non-option argument intput-path.\n");
            return -1;
        }

    } else if (not options.arguments.empty()) {
        std::cerr << fmt::format("Unexpected non-option arguments {}.\n", options.arguments.front());
        return -1;
    }

    if (output_path.empty()) {
        if (not input_path.has_extension()) {
            std::cerr << fmt::format("Can not produce output-path from intput-path {}.\n", input_path.string());
            return -1;
        }

        output_path = input_path.parent_path() / input_path.stem();
        if (verbose > 0) {
            std::cerr << fmt::format(
                "Using output-path {} constructed from input-path {}.\n", output_path.string(), input_path.string());
        }
    }

    return 0;
}

[[nodiscard]] std::string read_file(std::filesystem::path const& path)
{
    auto f = std::ifstream(path);
    if (not f.is_open()) {
        throw std::runtime_error(fmt::format("Could not open file {}.", path.string()));
    }
    auto r = std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    f.close();
    return r;
}

int main(int argc, char *argv[])
{
    if (auto parse_state = parse_options(argc, argv)) {
        print_help();
        return parse_state == 1 ? 0 : -2;
    }

    try {
        auto text = read_file(input_path);

        csp::generator<csp::csp_token<const char *>> tokens = csp::parse_csp(text, input_path);

        auto config = csp::translate_csp_config{};
        config.enable_line = enable_line;
        config.callback_name = callback_name;
        config.append_name = append_name;

        auto f = std::ofstream(output_path);
        for (auto const& str : csp::translate_csp(tokens, input_path, config)) {
            f << str;
        }
        f.flush();
        f.close();

    } catch (std::exception const& e) {
        std::cerr << fmt::format("Could not translate template: {}.", e.what());
        return -1;
    }

    return 0;
}
