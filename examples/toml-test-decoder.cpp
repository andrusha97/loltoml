#include <loltoml/parse.hpp>

#include <kora/dynamic.hpp>

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <set>
#include <stack>
#include <vector>

/*
 * This example implements a TOML decoder which can be used with the test suite from https://github.com/BurntSushi/toml-test
 * It adds key uniqueness ensurance on top of the SAX parser and therefore implements 100% spec compliant TOML parser.
 * Actually it builds kind of DOM, but rather unuseful for practical purposes.
 * It uses class kora::dynamic_t from https://github.com/leonidia/util to store the DOM.
 */


struct handler_t {
    std::set<std::vector<std::string>> assigned_keys;
    std::vector<std::string> path;
    kora::dynamic_t result;
    std::stack<kora::dynamic_t*> stack;

    void initialize_value(const kora::dynamic_t &value, bool table = false) {
        assert(!stack.empty());

        if (stack.top()->is_object()) {
            auto result = stack.top()->as_object().insert({path.back(), value});
            assert(result.second);
            stack.push(&result.first->second);
        } else if (stack.top()->is_array()) {
            if (stack.top()->as_array().size() == 0 && !table) {
                *stack.top() = kora::dynamic_t::object_t{
                    {"type", "array"},
                    {"value", kora::dynamic_t::array_t()}
                };

                kora::dynamic_t *new_array = &stack.top()->as_object()["value"];

                stack.pop();
                stack.push(new_array);
            }

            stack.top()->as_array().push_back(value);
            stack.push(&stack.top()->as_array().back());
        } else {
            assert(false);
        }
    }

    void complete_value() {
        if (stack.top()->is_array() && stack.top()->as_array().size() == 0) {
            *stack.top() = kora::dynamic_t::object_t{
                {"type", "array"},
                {"value", kora::dynamic_t::array_t()}
            };
        }

        stack.pop();

        if (stack.top()->is_object()) {
            path.pop_back();
        }
    }

    void start_document() {
        result = kora::dynamic_t::object_t();
        stack.push(&result);
    }

    void finish_document() { }

    void comment(const std::string &) { }

    void array_table(loltoml::key_iterator_t begin, loltoml::key_iterator_t end) {
        path.assign(begin, end);
        assigned_keys.insert(path);

        while (!stack.empty()) {
            stack.pop();
        }

        kora::dynamic_t *current = &result;
        stack.push(current);

        for (auto it = path.begin(); it + 1 < path.end(); ++it) {
            kora::dynamic_t &next = current->as_object().insert({*it, kora::dynamic_t::object_t()}).first->second;

            if (next.is_object()) {
                current = &next;
                stack.push(current);
            } else if (next.is_array() && !next.as_array().empty() && next.as_array().back().is_object()) {
                current = &next.as_array().back();
                stack.push(&next);
                stack.push(current);
            } else {
                throw std::exception();
            }
        }

        auto it = current->as_object().find(path.back());
        if (it != current->as_object().end()) {
            if (!it->second.is_array() ||
                it->second.as_array().empty() ||
                !it->second.as_array().back().is_object())
            {
                throw std::exception();
            }
        } else {
            it = current->as_object().insert({path.back(), kora::dynamic_t::array_t()}).first;
        }

        it->second.as_array().push_back(kora::dynamic_t::object_t());

        stack.push(&it->second);
        stack.push(&it->second.as_array().back());
    }

    void table(loltoml::key_iterator_t begin, loltoml::key_iterator_t end) {
        path.assign(begin, end);

        if (!assigned_keys.insert(path).second) {
            throw std::exception();
        }

        while (!stack.empty()) {
            stack.pop();
        }

        kora::dynamic_t *current = &result;
        stack.push(current);

        for (auto it = path.begin(); it != path.end(); ++it) {
            kora::dynamic_t &next = current->as_object().insert({*it, kora::dynamic_t::object_t()}).first->second;

            if (next.is_object()) {
                current = &next;
                stack.push(current);
            } else if (next.is_array() && !next.as_array().empty() && next.as_array().back().is_object()) {
                current = &next.as_array().back();
                stack.push(&next);
                stack.push(current);
            } else {
                throw std::exception();
            }
        }
    }

    void key(const std::string &key) {
        path.push_back(key);

        if (path.size() == stack.size()) {
            if (!assigned_keys.insert(path).second) {
                throw std::exception();
            }
        }
    }

    void start_array() {
        initialize_value(kora::dynamic_t::array_t());
    }

    void finish_array(std::size_t) {
        complete_value();
    }

    void start_inline_table() {
        initialize_value(kora::dynamic_t::object_t(), true);
    }

    void finish_inline_table(std::size_t) {
        complete_value();
    }

    void boolean(bool value) {
        std::string value_string = value ? "true" : "false";
        initialize_value(kora::dynamic_t::object_t{{"type", "bool"}, {"value", value_string}});
        complete_value();
    }

    void string(const std::string &value) {
        initialize_value(kora::dynamic_t::object_t{{"type", "string"}, {"value", value}});
        complete_value();
    }

    void datetime(const std::string &value) {
        initialize_value(kora::dynamic_t::object_t{{"type", "datetime"}, {"value", value}});
        complete_value();
    }

    void integer(std::int64_t value) {
        std::string value_string = std::to_string(value);
        initialize_value(kora::dynamic_t::object_t{{"type", "integer"}, {"value", value_string}});
        complete_value();
    }

    void floating_point(double value) {
        std::string value_string = boost::lexical_cast<std::string>(value);
        initialize_value(kora::dynamic_t::object_t{{"type", "float"}, {"value", value_string}});
        complete_value();
    }
};

int main() {
    try {
        handler_t handler;
        loltoml::parse(std::cin, handler);

        kora::write_pretty_json(std::cout, handler.result);
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
