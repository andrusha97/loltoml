#ifndef LOLTOML_DETAIL_INPUT_STREAM_HPP
#define LOLTOML_DETAIL_INPUT_STREAM_HPP

#include "loltoml/detail/common.hpp"
#include "loltoml/error.hpp"

#include <iostream>
#include <string>

LOLTOML_OPEN_NAMESPACE

namespace detail {


class input_stream_t {
public:
    explicit input_stream_t(std::istream &input) :
        m_backend(input),
        m_processed(0),
        m_emit_eol(true)
    { }

    char peek() {
        auto ch = m_backend.peek();

        if (ch != std::char_traits<char>::eof()) {
            return ch;
        } else if (m_backend.bad()) {
            throw stream_error_t(processed());
        } else {
            if (m_emit_eol) {
                return '\n';
            } else {
                throw parser_error_t("Unexpected EOF", processed());
            }
        }
    }

    char get() {
        char result;

        if (m_backend.get(result)) {
            ++m_processed;
            return result;
        } else if (m_backend.bad()) {
            throw stream_error_t(processed());
        } else {
            if (m_emit_eol) {
                m_emit_eol = false;
                return '\n';
            } else {
                throw parser_error_t("Unexpected EOF", processed());
            }
        }
    }

    bool eof() const {
        return m_backend.eof();
    }

    std::size_t processed() const {
        return m_processed;
    }

private:
    std::istream &m_backend;
    std::size_t m_processed;
    bool m_emit_eol;
};


} // namespace detail

LOLTOML_CLOSE_NAMESPACE

#endif // LOLTOML_DETAIL_INPUT_STREAM_HPP
