#ifndef LOLTOML_PARSE_HPP
#define LOLTOML_PARSE_HPP

#include "loltoml/detail/common.hpp"
#include "loltoml/detail/parser.hpp"

LOLTOML_OPEN_NAMESPACE


/*! Type of iterators passed by loltoml::parse() to methods table() and array_table() of the handler.
 *
 * It "points" to a std::string. Satisfies the InputIterator concept.
 */
typedef detail::key_iterator_t key_iterator_t;


/*! Parse a TOML document.
 *
 * This function reads a TOML document from the input stream and
 * feeds parsed tokens to the handler in order of their occurance in the document.
 * This function only completes successfuly if the input stream contains a valid TOML document
 * as specified in https://github.com/toml-lang/toml/tree/v0.4.0
 * The only exceptions are:
 * - Though the spec states that a valid TOML document is utf-8 encoded, the parser doesn't validate encoding at the time.
 * - Since it's a SAX-style parser, it cannot track uniqueness of keys in tables.
 *
 * The handler must have the following methods:
 * - void start_document() - called at the start of parsing
 * - void finish_document() - called after parsing completes successfuly
 * - void comment(const std::string &comment) - called to handle a comment
 * - void array_table(loltoml::key_iterator_t begin, loltoml::key_iterator_t end) - called when [[array table]] appears.
 *     (begin, end) is the sequence of keys between [[ and ]].
 * - void table(loltoml::key_iterator_t begin, loltoml::key_iterator_t end) - called when [table] appears.
 *     (begin, end) is the sequence of keys between [ and ].
 * - void key(const std::string &key) - called to handle the key in a key-value pair (key = value)
 *     inside a table, array table or inline table.
 * - void start_array() - called when array starts, i.e. '[' token appears.
 *     Note: It's called only in the right context, not for '[' from the table specification.
 * - void finish_array(std::size_t array_size) - handles end of an array.
 *     Parser passes the array size to this method, because it may be useful in some cases.
 * - void start_inline_table() - handles start of an inline table (a '{' token).
 * - void finish_inline_table(std::size_t table_size) - handles end of an inline table. Size of the table is provided.
 * - void boolean(bool value) - handles boolean value.
 * - void string(const std::string &value) - handles a string (all kinds of strings: basic, multiline, literal, multiline literal).
 * - void datetime(const std::string &value) - handles datetime value.
 *     Though format of datetime values is validated, they're provided as strings, because I don't know what else to do with them.
 * - void integer(std::int64_t value) - handles an integer value.
 * - void floating_point(double value) - handles a float value.
 *
 * \tparam Handler Type of the handler.
 * \param[in, out] input Stream containing a TOML document. It must be utf-8 encoded.
 * \param[out] handler Parser will feed SAX-events to this object.
 * \throws loltoml::parser_error_t if the input contains an invalid TOML document or just cannot be read.
 * \throws loltoml::stream_error_t if input.bad() becomes true.
 */
template<class Handler>
inline void parse(std::istream &input, Handler &handler) {
    detail::parser_t<Handler> parser(input, handler);
    parser.parse();
}


LOLTOML_CLOSE_NAMESPACE

#endif // LOLTOML_PARSE_HPP
