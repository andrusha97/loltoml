#include <loltoml/parse.hpp>

#include <kora/dynamic.hpp>

#include <iostream>
#include <set>
#include <stack>
#include <stdexcept>
#include <vector>

/*
 * This is an example of a DOM-style parser.
 * It uses class kora::dynamic_t from https://github.com/leonidia/util to store the DOM.
 * The only drawback is that it represents datetimes as plain strings because kora::dynamic_t doesn't have
 * a separate type for timestamps or anything like that.
 */


struct handler_t {
    // Since table [a] may appear after [a.b], we should track assigned/unassigned keys separately.
    std::set<std::vector<std::string>> assigned_keys;
    // Path to the current value (only keys from tables). It's used to add paths to assigned_keys.
    std::vector<std::string> path;
    // All the parent objects of the current value including arrays.
    std::stack<kora::dynamic_t*> stack;
    // Here will be the final DOM.
    kora::dynamic_t result;

    void initialize_value(const kora::dynamic_t &value) {
        assert(!stack.empty());

        if (stack.top()->is_object()) {
            auto result = stack.top()->as_object().insert({path.back(), value});
            assert(result.second);
            stack.push(&result.first->second);
        } else if (stack.top()->is_array()) {
            stack.top()->as_array().push_back(value);
            stack.push(&stack.top()->as_array().back());
        } else {
            assert(false);
        }
    }

    void complete_value() {
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
                throw std::runtime_error("Duplicate key");
            }
        }

        auto it = current->as_object().find(path.back());
        if (it != current->as_object().end()) {
            if (!it->second.is_array() ||
                it->second.as_array().empty() ||
                !it->second.as_array().back().is_object())
            {
                throw std::runtime_error("Duplicate key");
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
            throw std::runtime_error("Duplicate key");
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
                throw std::runtime_error("Duplicate key");
            }
        }
    }

    void key(const std::string &key) {
        path.push_back(key);

        // If path.size() != stack.size() then one of the parent objects is an array.
        // Since every element of any array is processed only once,
        // there is no need to track uniqueness of paths comming through arrays.
        if (path.size() == stack.size()) {
            if (!assigned_keys.insert(path).second) {
                throw std::runtime_error("Duplicate key");
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
        initialize_value(kora::dynamic_t::object_t());
    }

    void finish_inline_table(std::size_t) {
        complete_value();
    }

    void boolean(bool value) {
        initialize_value(value);
        complete_value();
    }

    void string(const std::string &value) {
        initialize_value(value);
        complete_value();
    }

    void datetime(const std::string &value) {
        initialize_value(value);
        complete_value();
    }

    void integer(std::int64_t value) {
        initialize_value(value);
        complete_value();
    }

    void floating_point(double value) {
        initialize_value(value);
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
