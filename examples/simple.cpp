#include <loltoml/parse.hpp>

#include <fstream>
#include <iostream>


struct handler_t {
    void start_document() {
        std::cout << "Start document" << std::endl;
    }

    void finish_document() {
        std::cout << "Finish document" << std::endl;
    }

    void comment(const std::string &comment) {
        std::cout << "Comment: " << comment << std::endl;
    }

    void array_table(loltoml::key_iterator_t begin, loltoml::key_iterator_t end) {
        std::cout << "Array table: ";

        for (; begin != end; ++begin) {
            std::cout << *begin << " ";
        }

        std::cout << std::endl;
    }

    void table(loltoml::key_iterator_t begin, loltoml::key_iterator_t end) {
        std::cout << "Table: ";

        for (; begin != end; ++begin) {
            std::cout << *begin << " ";
        }

        std::cout << std::endl;
    }

    void key(const std::string &key) {
        std::cout << "Key: " << key << std::endl;
    }

    void start_array() {
        std::cout << "Start array" << std::endl;
    }

    void finish_array(std::size_t array_size) {
        std::cout << "Finish array: " << array_size << std::endl;
    }

    void start_inline_table() {
        std::cout << "Start inline table" << std::endl;
    }

    void finish_inline_table(std::size_t table_size) {
        std::cout << "Finish table: " << table_size << std::endl;
    }

    void boolean(bool value) {
        std::cout << "Boolean: " << value << std::endl;
    }

    void string(const std::string &value) {
        std::cout << "String: " << value << std::endl;
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
    handler_t handler;

    loltoml::parse(std::cin, handler);

    return 0;
}
