#include <loltoml/parse.hpp>

#include <fstream>
#include <iostream>
#include <stack>

/*
 * Here we will track some state in the handler to print back the TOML document while parsing it.
 * Though it will be pretty poor formated.
 */


std::string escape_string(const std::string &s) {
    const char *hex_digits = "0123456789abcdef";
    std::string result;

    for (auto it = s.begin(); it != s.end(); ++it) {
        char ch = *it;

        if (ch == '\\') {
            result += "\\\\";
        } else if (ch == '\"') {
            result += "\\\"";
        } else if (ch == '\b') {
            result += "\\b";
        } else if (ch == '\t') {
            result += "\\t";
        } else if (ch == '\r') {
            result += "\\r";
        } else if (ch == '\n') {
            result += "\\n";
        } else if (static_cast<unsigned char>(ch) < 32) {
            result.push_back('\\');
            result.push_back('u');
            result.push_back('0');
            result.push_back('0');
            result.push_back(hex_digits[static_cast<unsigned char>(ch) / 16]);
            result.push_back(hex_digits[static_cast<unsigned char>(ch) % 16]);
        } else {
            result += std::string(1, ch);
        }
    }

    return result;
}

void print_string(const std::string &s) {
    std::cout << "\"" << escape_string(s) << "\"";
}


struct handler_t {
    // Here we track information about current sequence (document scope, array or inline table)
    // to print commas and end-of-line's correctly.
    enum class sequence_type_t {
        array,
        table
    };

    struct sequence_state_t {
        sequence_type_t type;
        size_t processed_items;
    };

    std::stack<sequence_state_t> sequences_stack;

    void print_value_prefix() {
        if (sequences_stack.empty()) {
            return;
        }

        ++sequences_stack.top().processed_items;
    }

    void print_value_suffix() {
        if (sequences_stack.empty()) {
            std::cout << std::endl;
        } else if (sequences_stack.top().type == sequence_type_t::array) {
            std::cout << ", ";
        }
    }

    void print_key_prefix() {
        if (sequences_stack.empty()) {
            return;
        }

        if (sequences_stack.top().type == sequence_type_t::table) {
            if (sequences_stack.top().processed_items > 0) {
                std::cout << ", ";
            }
        }
    }

    // The Handler interface implementation.
    void start_document() { }

    void finish_document() { }

    void comment(const std::string &comment) {
        std::cout << "#" << comment << std::endl;
    }

    void array_table(loltoml::key_iterator_t begin, loltoml::key_iterator_t end) {
        std::cout << "[[";

        bool first = true;
        for (; begin != end; ++begin) {
            if (!first) {
                std::cout << ".";
            } else {
                first = false;
            }

            print_string(*begin);
        }

        std::cout << "]]" << std::endl;
    }

    void table(loltoml::key_iterator_t begin, loltoml::key_iterator_t end) {
        std::cout << "[";

        bool first = true;
        for (; begin != end; ++begin) {
            if (!first) {
                std::cout << ".";
            } else {
                first = false;
            }

            print_string(*begin);
        }

        std::cout << "]" << std::endl;
    }

    void key(const std::string &key) {
        print_key_prefix();
        print_string(key);
        std::cout << " = ";
    }

    void start_array() {
        print_value_prefix();
        sequences_stack.push({sequence_type_t::array, 0});
        std::cout << "[";
    }

    void finish_array(std::size_t) {
        sequences_stack.pop();
        std::cout << "]";
        print_value_suffix();
    }

    void start_inline_table() {
        print_value_prefix();
        sequences_stack.push({sequence_type_t::table, 0});
        std::cout << "{";
    }

    void finish_inline_table(std::size_t) {
        sequences_stack.pop();
        std::cout << "}";
        print_value_suffix();
    }

    void boolean(bool value) {
        print_value_prefix();
        std::cout << (value ? "true" : "false");
        print_value_suffix();
    }

    void string(const std::string &value) {
        print_value_prefix();
        print_string(value);
        print_value_suffix();
    }

    void datetime(const std::string &value) {
        print_value_prefix();
        std::cout << value;
        print_value_suffix();
    }

    void integer(std::int64_t value) {
        print_value_prefix();
        std::cout << value;
        print_value_suffix();
    }

    void floating_point(double value) {
        print_value_prefix();

        std::string printed = std::to_string(value);

        // Kostyli-kostyliki.
        if (printed.find_first_of(".eE") == std::string::npos) {
            printed += ".0";
        }

        std::cout << printed;
        print_value_suffix();
    }
};

int main() {
    handler_t handler;

    loltoml::parse(std::cin, handler);

    return 0;
}
