#include "loltoml/parse.hpp"

#include <fstream>
#include <iostream>

struct handler_t {
    void start_document() {
        std::cout << "Start TOML document" << std::endl;
    }

    void finish_document() {
        std::cout << "Finish TOML document" << std::endl;
    }

    void table_array_item(loltoml::key_iterator_t begin, loltoml::key_iterator_t end) {
        std::cout << "Table array item: ";

        for (auto it = begin; it != end; ++it) {
            std::cout << "\'" << *it << "\' ";
        }

        std::cout << std::endl;
    }

    void table(loltoml::key_iterator_t begin, loltoml::key_iterator_t end) {
        std::cout << "Table: ";

        for (auto it = begin; it != end; ++it) {
            std::cout << "\'" << *it << "\' ";
        }

        std::cout << std::endl;
    }

    void key(const std::string &key) {
        std::cout << "Key: " << key << std::endl;
    }

    void start_array() {
        std::cout << "Start array [" << std::endl;
    }

    void finish_array(std::size_t array_size) {
        std::cout << "] Finish array " << array_size << std::endl;
    }

    void start_inline_table() {
        std::cout << "Start inline table {" << std::endl;
    }

    void finish_inline_table(std::size_t table_size) {
        std::cout << "} Finish inline table " << table_size << std::endl;
    }

    void boolean(bool value) {
        std::cout << "Bool: " << value << std::endl;
    }

    void string(const std::string &s) {
        std::cout << "String: " << s << std::endl;
    }

    void datetime(const std::string &value) {
        std::cout << "Datetime: " << value << std::endl;
    }

    void integer(std::int64_t value) {
        std::cout << "Integer: " << value << std::endl;
    }

    void floating_point(double value) {
        std::cout << "Float: " << value << std::endl;
    }
};

int main() {
    std::ifstream input("../test.toml");

    try {
        handler_t handler;
        loltoml::parse(input, handler);
    } catch (const loltoml::parser_error_t &e) {
        std::cerr << "Parser error: " << e.position() << ": " << e.message() << std::endl;
        return 1;
    }

    return 1;
}
