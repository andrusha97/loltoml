#ifndef LOLTOML_ERROR_HPP
#define LOLTOML_ERROR_HPP

#include "loltoml/detail/common.hpp"

#include <exception>
#include <string>

LOLTOML_OPEN_NAMESPACE


class parser_error_t :
    public std::exception
{
public:
    parser_error_t(std::string message, std::size_t position) :
        m_message(std::move(message)),
        m_position(position)
    { }

    virtual const char *what() const throw() {
        return "loltoml parser error";
    }

    const char *message() const {
        return m_message.c_str();
    }

    std::size_t position() const {
        return m_position;
    }

private:
    std::string m_message;
    std::size_t m_position;
};

class stream_error_t :
    public parser_error_t
{
public:
    stream_error_t(std::size_t processed) :
        parser_error_t("Unable to read data from the stream", processed)
    { }

    virtual const char *what() const throw() {
        return "loltoml stream error";
    }
};


LOLTOML_CLOSE_NAMESPACE

#endif // LOLTOML_ERROR_HPP
