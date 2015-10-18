#ifndef LOLTOML_ERROR_HPP
#define LOLTOML_ERROR_HPP

#include "loltoml/detail/common.hpp"

#include <exception>
#include <string>

LOLTOML_OPEN_NAMESPACE


//! Parser throws it if the input stream contains invalid TOML document.
class parser_error_t :
    public std::exception
{
public:
    /*!
     * \param[in] message Message describing the error.
     * \param[in] offset Position of the error in the input stream.
     * \throws std::bad_alloc
     */
    parser_error_t(std::string message, std::size_t offset) :
        m_message(std::move(message)),
        m_offset(offset)
    { }

    virtual ~parser_error_t() throw() { }

    virtual const char *what() const throw() {
        return "loltoml parser error";
    }

    //! \returns Message describing the error.
    const char *message() const {
        return m_message.c_str();
    }

    //! \returns Position of the error in the input stream.
    std::size_t offset() const {
        return m_offset;
    }

private:
    std::string m_message;
    std::size_t m_offset;
};

//! Thrown by the parser when it's unable to read from the input stream.
class stream_error_t :
    public parser_error_t
{
public:
    /*!
     * \param[in] processed Number of bytes processed before the error occured.
     * \throws std::bad_alloc
     */
    stream_error_t(std::size_t processed) :
        parser_error_t("Unable to read data from the stream", processed)
    { }

    virtual ~stream_error_t() throw() { }

    virtual const char *what() const throw() {
        return "loltoml stream error";
    }
};


LOLTOML_CLOSE_NAMESPACE

#endif // LOLTOML_ERROR_HPP
