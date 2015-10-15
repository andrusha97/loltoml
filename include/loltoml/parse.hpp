#ifndef LOLTOML_PARSE_HPP
#define LOLTOML_PARSE_HPP

#include "loltoml/detail/common.hpp"
#include "loltoml/detail/parser.hpp"


LOLTOML_OPEN_NAMESPACE

typedef detail::key_iterator_t key_iterator_t;

template<class Handler>
void parse(std::istream &input, Handler &handler) {
    detail::parser_t<Handler> parser(input, handler);
    parser.parse();
}

LOLTOML_CLOSE_NAMESPACE


#endif // LOLTOML_PARSE_HPP
