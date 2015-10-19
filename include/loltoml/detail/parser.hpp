#ifndef LOLTOML_DETAIL_PARSER_HPP
#define LOLTOML_DETAIL_PARSER_HPP

#include "loltoml/detail/common.hpp"
#include "loltoml/detail/input_stream.hpp"
#include "loltoml/error.hpp"

#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

LOLTOML_OPEN_NAMESPACE

namespace detail {


inline std::string escape_char(char ch) {
    if (ch == '\\') {
        return "\\\\";
    } else if (ch == '\'') {
        return "\\\'";
    } else if (ch == '\"') {
        return "\\\"";
    } else if (ch == '\b') {
        return "\\b";
    } else if (ch == '\t') {
        return "\\t";
    } else if (ch == '\r') {
        return "\\r";
    } else if (ch == '\n') {
        return "\\n";
    } else if (std::isprint(ch)) {
        return std::string(1, ch);
    } else {
        const char *hex_digits = "0123456789abcdef";
        char result[5] = "\\xYY";

        result[2] = hex_digits[static_cast<unsigned char>(ch) / 16];
        result[3] = hex_digits[static_cast<unsigned char>(ch) % 16];

        return result;
    }
}


inline bool iscontrol(char ch) {
    return static_cast<unsigned char>(ch) < 32;
}


inline bool is_key_character(char ch) {
    return (ch >= 'a' && ch <= 'z') ||
           (ch >= 'A' && ch <= 'Z') ||
           (ch >= '0' && ch <= '9') ||
           ch == '-' ||
           ch == '_';
}


typedef std::vector<std::string>::const_iterator key_iterator_t;


template<class Handler>
class parser_t {
    input_stream_t input;
    Handler &handler;

public:
    parser_t(std::istream &input, Handler &handler) :
        input(input),
        handler(handler)
    { }

    void parse() {
        handler.start_document();

        parse_expression();

        while (!input.eof()) {
            parse_new_line();
            parse_expression();
        }

        handler.finish_document();
    }

private:
    enum class toml_type_t {
        string,
        integer,
        floating_point,
        boolean,
        datetime,
        array,
        table
    };

    std::size_t last_char_offset() const {
        std::size_t processed = input.processed();
        return (processed == 0) ? 0 : (processed - 1);
    }

    template<std::size_t N>
    char parse_chars(const char (&expected)[N]) {
        static_assert(N > 0, "No expected characters specified");
        assert(expected[N - 1] == '\0');

        char result = input.get();
        for (std::size_t i = 0; i + 1 < N; ++i) {
            if (result == expected[i]) {
                return result;
            }
        }

        std::string list = "\'" + escape_char(expected[0]) + "\'";

        for (std::size_t i = 1; i + 1 < N; ++i) {
            list += ", \'" + escape_char(expected[i]) + "\'";
        }

        throw parser_error_t("Expected one of the following symbols: " + list, last_char_offset());
    }

    void skip_spaces() {
        while (!input.eof() && (input.peek() == ' ' || input.peek() == '\t')) {
            input.get();
        }
    }

    void parse_comment() {
        assert(input.peek() == '#');
        input.get();

        std::string comment;
        while (input.peek() == '\t' || !iscontrol(input.peek())) {
            comment.push_back(input.get());
        }

        handler.comment(comment);
    }

    void parse_new_line() {
        char ch = input.get();

        if (ch == '\r') {
            ch = input.get();
        }

        if (ch != '\n') {
            throw parser_error_t("Expected new-line", last_char_offset());
        }
    }

    void skip_spaces_and_empty_lines() {
        while (!input.eof()) {
            skip_spaces();

            if (input.peek() == '#') {
                parse_comment();
                parse_new_line();
            } else if (input.peek() == '\r' || input.peek() == '\n') {
                parse_new_line();
            } else {
                break;
            }
        }
    }

    void parse_expression() {
        skip_spaces();

        if (input.eof()) {
            return;
        } else if (input.peek() == '\r' || input.peek() == '\n') {
            return;
        } else if (input.peek() == '#') {
            parse_comment();
        } else if (input.peek() == '[') {
            parse_table_header();
            skip_spaces();
            if (!input.eof() && input.peek() == '#') {
                parse_comment();
            }
        } else {
            parse_kv_pair();
            skip_spaces();
            if (!input.eof() && input.peek() == '#') {
                parse_comment();
            }
        }
    }

    void parse_table_header() {
        assert(input.peek() == '[');
        input.get();

        bool array_item = false;

        if (input.peek() == '[') {
            input.get();
            array_item = true;
        }

        std::vector<std::string> path;
        while (true) {
            skip_spaces();

            path.emplace_back(parse_key());

            skip_spaces();

            if (input.peek() == ']') {
                input.get();

                if (array_item) {
                    parse_chars("]");
                }

                break;
            } else {
                parse_chars(".");
            }
        }

        if (array_item) {
            handler.array_table(path.cbegin(), path.cend());
        } else {
            handler.table(path.cbegin(), path.cend());
        }
    }

    void parse_kv_pair() {
        handler.key(parse_key());
        skip_spaces();
        parse_chars("=");
        skip_spaces();
        parse_value();
    }

    std::string parse_key() {
        std::string key;
        if (input.peek() == '"') {
            input.get();
            key = parse_basic_string();

            if (key.empty()) {
                throw parser_error_t("Expected a non-empty key", last_char_offset());
            }
        } else {
            // It must be at least one char.
            char ch = input.get();
            if (!is_key_character(ch)) {
                throw parser_error_t("Expected a non-empty key", last_char_offset());
            }

            key.push_back(ch);

            while (is_key_character(input.peek())) {
                key.push_back(input.get());
            }
        }

        return key;
    }

    toml_type_t parse_value() {
        switch (input.peek()) {
            case '{': {
                parse_inline_table();
                return toml_type_t::table;
            } break;
            case '[': {
                parse_array();
                return toml_type_t::array;
            } break;
            case '"': {
                parse_string();
                return toml_type_t::string;
            } break;
            case '\'': {
                parse_literal_string();
                return toml_type_t::string;
            } break;
            case 't': {
                parse_true();
                return toml_type_t::boolean;
            } break;
            case 'f': {
                parse_false();
                return toml_type_t::boolean;
            } break;
            default: {
                return parse_date_or_number();
            }
        }
    }

    void parse_array() {
        assert(input.peek() == '[');
        input.get();
        handler.start_array();
        skip_spaces_and_empty_lines();

        toml_type_t array_type;
        std::size_t size = 0;

        while (true) {
            if (input.peek() == ']') {
                input.get();
                handler.finish_array(size);
                return;
            }

            std::size_t item_offset = input.processed();
            toml_type_t current_item_type = parse_value();

            if (size > 0 && current_item_type != array_type) {
                throw parser_error_t("All array elements must be of the same type", item_offset);
            }

            ++size;
            array_type = current_item_type;

            // FIXME: Formal grammar from https://github.com/toml-lang/toml/pull/236 disallows new-lines between values and commas.
            skip_spaces_and_empty_lines();

            char ch = input.get();
            if (ch == ']') {
                handler.finish_array(size);
                return;
            } else if (ch == ',') {
                skip_spaces_and_empty_lines();
            } else {
                throw parser_error_t("Expected ',' or ']' after an array element", last_char_offset());
            }
        }
    }

    void parse_inline_table() {
        assert(input.peek() == '{');
        input.get();
        handler.start_inline_table();
        std::size_t size = 0;

        skip_spaces();

        if (input.peek() == '}') {
            input.get();
            handler.finish_inline_table(size);
            return;
        }

        while (true) {
            handler.key(parse_key());
            skip_spaces();
            parse_chars("=");
            skip_spaces();
            parse_value();
            skip_spaces();

            ++size;

            char ch = input.get();
            if (ch == '}') {
                handler.finish_inline_table(size);
                return;
            } else if (ch == ',') {
                skip_spaces();
            } else {
                throw parser_error_t("Expected ',' or '}' after an inline table element", last_char_offset());
            }
        }
    }

    void parse_true() {
        parse_chars("t");
        parse_chars("r");
        parse_chars("u");
        parse_chars("e");

        handler.boolean(true);
    }

    void parse_false() {
        parse_chars("f");
        parse_chars("a");
        parse_chars("l");
        parse_chars("s");
        parse_chars("e");

        handler.boolean(false);
    }

    unsigned int parse_hex_digit() {
        char ch = input.get();

        if (ch >= '0' && ch <= '9') {
            return ch - '0';
        } else if (ch >= 'A' && ch <= 'Z') {
            return ch - 'A' + 10;
        } else if (ch >= 'a' && ch <= 'z') {
            return ch - 'a' + 10;
        } else {
            throw parser_error_t("Expected hex-digit", last_char_offset());
        }
    }

    uint32_t parse_4_digit_codepoint() {
        uint32_t codepoint = parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();

        return codepoint;
    }

    uint32_t parse_8_digit_codepoint() {
        uint32_t codepoint = parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();
        codepoint = (codepoint << 4) + parse_hex_digit();

        return codepoint;
    }

    void process_codepoint(uint32_t codepoint, std::size_t escape_sequence_offset, std::string &output) {
        if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
            throw parser_error_t("Surrogate pairs are not allowed", escape_sequence_offset);
        }

        if (codepoint > 0x10FFFF) {
            throw parser_error_t("Codepoint must be less or equal than 0x10FFFF", escape_sequence_offset);
        }

        // FIXME: Can these casts to unsigned char be omitted?
        if (codepoint <= 0x7F) {
            output.push_back(static_cast<unsigned char>(codepoint));
        } else if (static_cast<unsigned char>(codepoint <= 0x7FF)) {
            output.push_back(static_cast<unsigned char>(0xC0 | (codepoint >> 6)));
            output.push_back(static_cast<unsigned char>(0x80 | (codepoint & 0x3F)));
        } else if (codepoint <= 0xFFFF) {
            output.push_back(static_cast<unsigned char>(0xE0 | (codepoint >> 12)));
            output.push_back(static_cast<unsigned char>(0x80 | ((codepoint >> 6) & 0x3F)));
            output.push_back(static_cast<unsigned char>(0x80 | (codepoint & 0x3F)));
        } else {
            output.push_back(static_cast<unsigned char>(0xF0 | (codepoint >> 18)));
            output.push_back(static_cast<unsigned char>(0x80 | ((codepoint >> 12) & 0x3F)));
            output.push_back(static_cast<unsigned char>(0x80 | ((codepoint >> 6) & 0x3F)));
            output.push_back(static_cast<unsigned char>(0x80 | (codepoint & 0x3F)));
        }
    }

    std::string parse_basic_string() {
        std::string result;

        while (true) {
            char ch = input.get();
            if (iscontrol(ch)) {
                throw parser_error_t("Control characters must be escaped", last_char_offset());
            } else if (ch == '"') {
                break;
            } else if (ch == '\\') {
                std::size_t escape_sequence_offset = last_char_offset();
                char ch = input.get();
                if (ch == 'b') {
                    result.push_back('\b');
                } else if (ch == 't') {
                    result.push_back('\t');
                } else if (ch == 'n') {
                    result.push_back('\n');
                } else if (ch == 'f') {
                    result.push_back('\f');
                } else if (ch == 'r') {
                    result.push_back('\r');
                } else if (ch == '"') {
                    result.push_back('"');
                } else if (ch == '\\') {
                    result.push_back('\\');
                } else if (ch == 'u') {
                    process_codepoint(parse_4_digit_codepoint(), escape_sequence_offset, result);
                } else if (ch == 'U') {
                    process_codepoint(parse_8_digit_codepoint(), escape_sequence_offset, result);
                } else {
                    throw parser_error_t("Invalid escape-sequence", escape_sequence_offset);
                }
            } else {
                result.push_back(ch);
            }
        }

        return result;
    }

    std::string parse_multiline_string() {
        std::string result;

        // Ignore first new-line after open quotes.
        if (input.peek() == '\r' || input.peek() == '\n') {
            parse_new_line();
        }

        while (true) {
            if (input.peek() == '\r' || input.peek() == '\n') {
                parse_new_line();
                result.push_back('\n');
                continue;
            }

            char ch = input.get();
            if (iscontrol(ch)) {
                throw parser_error_t("Control characters must be escaped", last_char_offset());
            } else if (ch == '"') {
                if (input.peek() == '"') {
                    input.get();
                    if (input.peek() == '"') {
                        input.get();
                        break;
                    }
                    result.push_back('"');
                }
                result.push_back('"');
            } else if (ch == '\\') {
                if (input.peek() == '\r' || input.peek() == '\n') {
                    parse_new_line();
                    while (std::isspace(input.peek())) {
                        input.get();
                    }
                    continue;
                }

                std::size_t escape_sequence_offset = last_char_offset();
                char ch = input.get();
                if (ch == 'b') {
                    result.push_back('\b');
                } else if (ch == 't') {
                    result.push_back('\t');
                } else if (ch == 'n') {
                    result.push_back('\n');
                } else if (ch == 'f') {
                    result.push_back('\f');
                } else if (ch == 'r') {
                    result.push_back('\r');
                } else if (ch == '"') {
                    result.push_back('"');
                } else if (ch == '\\') {
                    result.push_back('\\');
                } else if (ch == 'u') {
                    process_codepoint(parse_4_digit_codepoint(), escape_sequence_offset, result);
                } else if (ch == 'U') {
                    process_codepoint(parse_8_digit_codepoint(), escape_sequence_offset, result);
                } else {
                    throw parser_error_t("Invalid escape-sequence", escape_sequence_offset);
                }
            } else {
                result.push_back(ch);
            }
        }

        return result;
    }

    void parse_string() {
        assert(input.peek() == '"');
        input.get();

        if (input.peek() == '"') {
            input.get();

            if (input.peek() == '"') {
                input.get();
                handler.string(parse_multiline_string());
            } else {
                handler.string("");
            }
        } else {
            handler.string(parse_basic_string());
        }
    }

    void parse_literal_string() {
        assert(input.peek() == '\'');
        input.get();

        if (input.peek() == '\'') {
            input.get();

            if (input.peek() == '\'') {
                input.get();

                // Ignore first new-line after open quotes.
                if (input.peek() == '\r' || input.peek() == '\n') {
                    parse_new_line();
                }

                std::string string;
                while (true) {
                    if (input.peek() == '\r' || input.peek() == '\n') {
                        parse_new_line();
                        string.push_back('\n');
                        continue;
                    }

                    char ch = input.get();
                    if (ch == '\'') {
                        if (input.peek() == '\'') {
                            input.get();
                            if (input.peek() == '\'') {
                                input.get();
                                handler.string(string);
                                return;
                            }
                            string.push_back('\'');
                        }
                        string.push_back('\'');
                    } else if (iscontrol(ch) && ch != '\t') {
                        throw parser_error_t("Control characters are not allowed", last_char_offset());
                    } else {
                        string.push_back(ch);
                    }
                }
            } else {
                handler.string("");
            }
        } else {
            std::string string;

            while (true) {
                char ch = input.get();
                if (iscontrol(ch) && ch != '\t') {
                    throw parser_error_t("Control characters are not allowed", last_char_offset());
                } else if (ch == '\'') {
                    break;
                }

                string.push_back(ch);
            }

            handler.string(string);
        }
    }

    char parse_datetime_digit() {
        char ch = input.get();
        if (std::isdigit(ch)) {
            return ch;
        } else {
            throw parser_error_t("Bad datetime. Expected digit.", last_char_offset());
        }
    }

    toml_type_t parse_date_or_number() {
        const std::size_t max_int64_digits = 19;
        const char *max_int64_string = "9223372036854775807";
        const char *min_int64_string = "9223372036854775808";

        const std::size_t max_double_length = 800;

        std::size_t value_offset = input.processed();

        char buffer[max_double_length + 1];
        buffer[0] = '+';
        char *digits = buffer + 1;

        std::size_t next_index = 0;

        if (std::isdigit(input.peek())) {
            digits[next_index++] = input.get();
            if (std::isdigit(input.peek())) {
                digits[next_index++] = input.get();
                if (std::isdigit(input.peek())) {
                    digits[next_index++] = input.get();
                    if (std::isdigit(input.peek())) {
                        digits[next_index++] = input.get();

                        if (input.peek() == '-') {
                            digits[next_index++] = input.get();

                            digits[next_index++] = parse_datetime_digit();
                            digits[next_index++] = parse_datetime_digit();
                            digits[next_index++] = parse_chars("-");
                            digits[next_index++] = parse_datetime_digit();
                            digits[next_index++] = parse_datetime_digit();
                            digits[next_index++] = parse_chars("tT");
                            digits[next_index++] = parse_datetime_digit();
                            digits[next_index++] = parse_datetime_digit();
                            digits[next_index++] = parse_chars(":");
                            digits[next_index++] = parse_datetime_digit();
                            digits[next_index++] = parse_datetime_digit();
                            digits[next_index++] = parse_chars(":");
                            digits[next_index++] = parse_datetime_digit();
                            digits[next_index++] = parse_datetime_digit();

                            if (input.peek() == '.') {
                                digits[next_index++] = input.get();
                                digits[next_index++] = parse_datetime_digit();

                                while (std::isdigit(input.peek())) {
                                    digits[next_index++] = input.get();
                                }
                            }

                            if (input.peek() == 'z' || input.peek() == 'Z') {
                                digits[next_index++] = input.get();
                            } else {
                                digits[next_index++] = parse_chars("+-");
                                digits[next_index++] = parse_datetime_digit();
                                digits[next_index++] = parse_datetime_digit();
                                digits[next_index++] = parse_chars(":");
                                digits[next_index++] = parse_datetime_digit();
                                digits[next_index++] = parse_datetime_digit();
                            }

                            handler.datetime(std::string(digits, digits + next_index));
                            return toml_type_t::datetime;
                        }
                    }
                }
            }
        } else if (input.peek() == '-') {
            input.get();
            buffer[0] = '-';
        } else if (input.peek() == '+') {
            input.get();
        } else {
            input.get();
            throw parser_error_t("Unexpected character", last_char_offset());
        }

        bool last_digit = next_index > 0;
        while (true) {
            if (next_index == max_double_length) {
                throw parser_error_t("Number is too long", last_char_offset());
            }

            if (std::isdigit(input.peek())) {
                digits[next_index++] = input.get();
                last_digit = true;
            } else if (last_digit && input.peek() == '_') {
                input.get();
                last_digit = false;
            } else {
                break;
            }
        }

        if (!last_digit) {
            throw parser_error_t("Unexpected number end", last_char_offset());
        }

        if (digits[0] == '0' && next_index > 1) {
            throw parser_error_t("Leading zeros are not allowed", input.processed() - next_index);
        }

        if (input.peek() != '.' && input.peek() != 'e' && input.peek() != 'E') {
            if (next_index == max_int64_digits)
            {
                if (buffer[0] == '-' && strncmp(digits, min_int64_string, max_int64_digits) > 0) {
                    throw parser_error_t("The number cannot be represented as 64-bit signed integer",
                                         value_offset);
                }

                if (buffer[0] != '-' && strncmp(digits, max_int64_string, max_int64_digits) > 0) {
                    throw parser_error_t("The number cannot be represented as 64-bit signed integer",
                                         value_offset);
                }
            } else if (next_index > max_int64_digits) {
                throw parser_error_t("The number cannot be represented as 64-bit signed integer",
                                     value_offset);
            }

            std::int64_t result = 0;

            if (buffer[0] == '-') {
                for (auto it = digits; it < digits + next_index; ++it) {
                    result = 10 * result - (*it - '0');
                }
            } else {
                for (auto it = digits; it < digits + next_index; ++it) {
                    result = 10 * result + (*it - '0');
                }
            }

            handler.integer(result);
            return toml_type_t::integer;
        }

        if (input.peek() == '.') {
            digits[next_index++] = '.';
            input.get();

            last_digit = false;
            while (true) {
                if (next_index == max_double_length) {
                    throw parser_error_t("Number is too long", last_char_offset());
                }

                if (std::isdigit(input.peek())) {
                    digits[next_index++] = input.get();
                    last_digit = true;
                } else if (last_digit && input.peek() == '_') {
                    input.get();
                    last_digit = false;
                } else {
                    break;
                }
            }

            if (!last_digit) {
                throw parser_error_t("Unexpected number end", last_char_offset());
            }
        }

        if (input.peek() == 'e' || input.peek() == 'E') {
            digits[next_index++] = input.get();

            if (input.peek() == '+' || input.peek() == '-') {
                digits[next_index++] = input.get();
            }

            char first_digit = input.peek();
            std::size_t exponent_start = input.processed();
            std::size_t exponent_size = 0;
            last_digit = false;
            while (true) {
                if (next_index == max_double_length) {
                    throw parser_error_t("Number is too long", last_char_offset());
                }

                if (std::isdigit(input.peek())) {
                    digits[next_index++] = input.get();
                    last_digit = true;
                    ++exponent_size;
                } else if (last_digit && input.peek() == '_') {
                    input.get();
                    last_digit = false;
                } else {
                    break;
                }
            }

            if (!last_digit) {
                throw parser_error_t("Unexpected number end", last_char_offset());
            }

            if (exponent_size > 1 && first_digit == '0') {
                throw parser_error_t("Leading zeros are not allowed in exponent", exponent_start);
            }
        }

        digits[next_index] = '\0';
        char *end = nullptr;
        double result = std::strtod(buffer, &end);

        if (end != digits + next_index) {
            throw parser_error_t("Bad number", value_offset);
        }

        if (std::isnan(result) || std::isinf(result)) {
            throw parser_error_t("The number can not be represented as 64-bit floating point number",
                                 value_offset);
        }

        handler.floating_point(result);
        return toml_type_t::floating_point;
    }
};


} // namespace detail

LOLTOML_CLOSE_NAMESPACE

#endif // LOLTOML_DETAIL_PARSER_HPP
