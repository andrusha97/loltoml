#include <gtest/gtest.h>

#include "loltoml/parse.hpp"

#include <cassert>
#include <cctype>
#include <initializer_list>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>


struct sax_event_t {
    enum type_t {
        invalid_type,
        start_document,
        finish_document,
        comment,
        table,
        table_array_item,
        key,
        start_array,
        finish_array,
        start_inline_table,
        finish_inline_table,
        boolean,
        string,
        datetime,
        integer,
        floating_point
    };

    type_t type;
    std::vector<std::string> keys;
    std::string string_data;
    bool bool_data;
    std::int64_t int_data;
    double float_data;

    sax_event_t(type_t type = invalid_type) :
        type(type)
    { }

    sax_event_t(type_t type, const std::vector<std::string> &value) :
        type(type),
        keys(value)
    { }

    sax_event_t(type_t type, loltoml::key_iterator_t begin, loltoml::key_iterator_t end) :
        type(type),
        keys(begin, end)
    { }

    sax_event_t(type_t type, std::initializer_list<std::string> value) :
        type(type),
        keys(value)
    { }

    sax_event_t(type_t type, const std::string &value) :
        type(type),
        string_data(value)
    { }

    template<class T>
    sax_event_t(type_t type, T value, typename std::enable_if<std::is_same<T, bool>::value>::type * = 0) :
        type(type),
        bool_data(value)
    { }

    template<class T>
    sax_event_t(type_t type, T value, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type * = 0) :
        type(type),
        int_data(value)
    { }

    template<class T>
    sax_event_t(type_t type, T value, typename std::enable_if<std::is_floating_point<T>::value>::type * = 0) :
        type(type),
        float_data(value)
    { }
};

inline bool operator==(const sax_event_t &left, const sax_event_t &right) {
    if (left.type != right.type) {
        return false;
    }

    switch (left.type) {
    case sax_event_t::table:
    case sax_event_t::table_array_item:
        return left.keys == right.keys;
    case sax_event_t::comment:
    case sax_event_t::key:
    case sax_event_t::string:
    case sax_event_t::datetime:
        return left.string_data == right.string_data;
    case sax_event_t::finish_array:
    case sax_event_t::finish_inline_table:
    case sax_event_t::integer:
        return left.int_data == right.int_data;
    case sax_event_t::boolean:
        return left.bool_data == right.bool_data;
    case sax_event_t::floating_point:
        return left.float_data == right.float_data;
    default:
        return true;
    }
}

inline std::string escape_string(const std::string &s) {
    const char *hex_digits = "0123456789abcdef";
    std::string result;

    for (auto it = s.begin(); it != s.end(); ++it) {
        char ch = *it;

        if (ch == '\\') {
            result += "\\\\";
        } else if (ch == '\'') {
            result += "\\\'";
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
        } else if (std::isprint(ch)) {
            result += std::string(1, ch);
        } else {
            result.push_back('\\');
            result.push_back('x');
            result.push_back(hex_digits[static_cast<unsigned char>(ch) / 16]);
            result.push_back(hex_digits[static_cast<unsigned char>(ch) % 16]);
        }
    }

    return result;
}

inline std::ostream &operator<<(std::ostream &output, const sax_event_t &event) {
    switch(event.type) {
    case sax_event_t::invalid_type: {
        output << "{" << "invalid" << "}";
    } break;
    case sax_event_t::start_document: {
        output << "{" << "start_document" << "}";
    } break;
    case sax_event_t::finish_document: {
        output << "{" << "finish_document" << "}";
    } break;
    case sax_event_t::comment: {
        output << "{" << "comment" << ", '" << escape_string(event.string_data) << "'}";
    } break;
    case sax_event_t::table: {
        output << "{" << "table" << ", {";

        bool first = true;
        for (auto it = event.keys.begin(); it != event.keys.end(); ++it) {
            if (!first) {
                output << ", ";
            } else {
                first = false;
            }

            output << "'" << escape_string(*it) << "'";
        }

        output << "}}";
    } break;
    case sax_event_t::table_array_item: {
        output << "{" << "table_array_item" << ", {";

        bool first = true;
        for (auto it = event.keys.begin(); it != event.keys.end(); ++it) {
            if (!first) {
                output << ", ";
            } else {
                first = false;
            }

            output << "'" << escape_string(*it) << "'";
        }

        output << "}}";
    } break;
    case sax_event_t::key: {
        output << "{" << "key" << ", '" << escape_string(event.string_data) << "'}";
    } break;
    case sax_event_t::start_array: {
        output << "{" << "start_array" << "}";
    } break;
    case sax_event_t::finish_array: {
        output << "{" << "finish_array" << ", " << event.int_data << "}";
    } break;
    case sax_event_t::start_inline_table: {
        output << "{" << "start_inline_table" << "}";
    } break;
    case sax_event_t::finish_inline_table: {
        output << "{" << "finish_inline_table" << ", " << event.int_data << "}";
    } break;
    case sax_event_t::boolean: {
        output << "{" << "boolean" << ", " << event.bool_data << "}";
    } break;
    case sax_event_t::string: {
        output << "{" << "string" << ", '" << escape_string(event.string_data) << "'}";
    } break;
    case sax_event_t::datetime: {
        output << "{" << "datetime" << ", '" << escape_string(event.string_data) << "'}";
    } break;
    case sax_event_t::integer: {
        output << "{" << "integer" << ", " << event.int_data << "}";
    } break;
    case sax_event_t::floating_point: {
        output << "{" << "floating_point" << ", " << event.float_data << "}";
    } break;
    default: assert(false);
    }

    return output;
}

struct events_aggregator_t {
    std::vector<sax_event_t> events;

    void start_document() {
        events.emplace_back(sax_event_t::start_document);
    }

    void finish_document() {
        events.emplace_back(sax_event_t::finish_document);
    }

    void comment(const std::string &value) {
        events.emplace_back(sax_event_t::comment, value);
    }

    void array_table(loltoml::key_iterator_t begin, loltoml::key_iterator_t end) {
        events.emplace_back(sax_event_t::table_array_item, begin, end);
    }

    void table(loltoml::key_iterator_t begin, loltoml::key_iterator_t end) {
        events.emplace_back(sax_event_t::table, begin, end);
    }

    void key(const std::string &key) {
        events.emplace_back(sax_event_t::key, key);
    }

    void start_array() {
        events.emplace_back(sax_event_t::start_array);
    }

    void finish_array(std::size_t array_size) {
        events.emplace_back(sax_event_t::finish_array, array_size);
    }

    void start_inline_table() {
        events.emplace_back(sax_event_t::start_inline_table);
    }

    void finish_inline_table(std::size_t table_size) {
        events.emplace_back(sax_event_t::finish_inline_table, table_size);
    }

    void boolean(bool value) {
        events.emplace_back(sax_event_t::boolean, value);
    }

    void string(const std::string &value) {
        events.emplace_back(sax_event_t::string, value);
    }

    void datetime(const std::string &value) {
        events.emplace_back(sax_event_t::datetime, value);
    }

    void integer(std::int64_t value) {
        events.emplace_back(sax_event_t::integer, value);
    }

    void floating_point(double value) {
        events.emplace_back(sax_event_t::floating_point, value);
    }
};

struct comments_skipper_t :
    public events_aggregator_t
{
    void comment(const std::string &) { }
};
